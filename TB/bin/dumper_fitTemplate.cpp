/*******************************************************************************
THIS PROGRAM READ THE RAW DATA AND PRODUCE THE RECO FILE USED FOR THE ANALYSIS
-
    compile with --> c++ -o dumper_fitTemplate bin/dumper_fitTemplate.cpp `root-config --cflags --glibs`
    run with --> ./dumper_fitTemplate cfg/example_cfg_file.cfg /gwteray/users/marzocchi/iMCP/dataTrees 9 prova
                    where arguments are:  -cfg_file  -inputFolder  -# of channels  -suffix for the output File

BE CAREFUL: the number of channels MUST coincide with the number of channel in the cfg file

*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream> 
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <ctime>
#include <map>
#include <math.h>

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TClass.h"
#include "TCanvas.h"
#include "TApplication.h"

#include "../interface/ScaleEstimators.h"
#include "../interface/analysis_tools.h"
#include "../interface/init_tree_H4.h"
#include "../interface/init_Reco_Tree.h"
//#include "../interface/histo_func.h"
#include "../interface/MCPMap.h"

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  
    std::cout<<"--------DUMPER TEMPLATE FIT: READ RAW DATA AND PRODUCE RECO TREE--------"<<std::endl;

    //-----this map tells how the MCPs will be order in the output tree. The names should match the cfg file----
    Fill_MCPList();  //look into the MCPMap.h class
    
    //--------Read Options--------------------------------
    ifstream inputCfg (argv[1], ios::in);

    std::string inputFolder = argv[2];
    int nChannels = atoi (argv[3]);
    std::string outputFile = argv[4];
    std::string ProMedioFile = argv[5];

    TProfile** wf_promed = new TProfile*[10];
    TH1F** templateHisto = new TH1F*[10];
    double pro_medio_CF[10] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    double pro_medio_Charge[10] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    TFile* inF = TFile::Open(ProMedioFile.c_str(),"read");                                                                
    for(int iCh=0; iCh<10; ++iCh) {
      wf_promed[iCh] = (TProfile*)inF->Get(Form("wf_promed_%d",iCh));                                                        
      wf_promed[iCh]->SetDirectory(0);                                                                                       
    }
    inF->Close();
    inF->Delete();                       
    for(int iCh=0; iCh<10; ++iCh) {                                                                                       
      templateHisto[iCh] = new TH1F(Form("templateHisto_%d",iCh), "", 1024, 0., 1024.);                                      
      for(int iBins=1; iBins<=1024; ++iBins) {
	if(iBins <= 100) templateHisto[iCh]->SetBinContent(iBins, wf_promed[iCh]->GetBinContent(iBins+150));  
	else if(iBins > 100 && iBins < 250) templateHisto[iCh]->SetBinContent(iBins, templateHisto[iCh]->GetBinContent(iBins-100));  
	else templateHisto[iCh]->SetBinContent(iBins, wf_promed[iCh]->GetBinContent(iBins));
      }
      double amp = 0.5;
      TimeConstFrac_ProMedio(templateHisto[iCh], amp, pro_medio_CF[iCh]);
      pro_medio_Charge[iCh] = templateHisto[iCh]->Integral(pro_medio_CF[iCh]-5, pro_medio_CF[iCh]+20);
      wf_promed[iCh]->Delete();                                                                                              
      //      std::cout << " iCh = " << iCh << " pro_medio_CF[iCh] = " << pro_medio_CF[iCh] << " pro_medio_Charge[iCh] = " << pro_medio_Charge[iCh] << std::endl;
    }                                                                                                                     


    // for(int iCh=0; iCh<10; ++iCh) std::cout << " templateHisto[iCh]->GetEntries() = " << templateHisto[iCh]->GetEntries() << std::endl;
    //    return 500;                                                                                            


    //---------output tree----------------
    TFile* outROOT = TFile::Open(("ntuples/reco_"+outputFile+".root").c_str(),"recreate");  
    outROOT->cd();
    
    TTree* outTree = new TTree("reco_tree", "reco_tree");
    outTree->SetDirectory(0);
    SetOutTree(outTree);
    
   
    int run=0, chNumber=0, HVtemp=0, PC=0;
    float X0temp=0.;
    std::string name, trig1, trig2;

    //---------definitions-----------
    std::map<int, int> PCOn;
    std::map<int, int> HVVal; 
    std::map<int, std::string> MCPName; 

    int start=0;
    //-------start to read the cfg file--------
    while(!inputCfg.eof())  
    {
      PCOn.clear();
      HVVal.clear();
      MCPName.clear();

      if (start==0) {   //read trigger chambers
	inputCfg >> trig1 >> trig2;
	start=1;
      }

      //-----fill maps--------
      for (int count=0; count<nChannels; count++)   //read exactly nChannels lines of the cfg file -> be careful to give the right number in input!!!!
	{
	  inputCfg >> run >> chNumber >> HVtemp >> X0temp >> PC >> name;

	  PCOn.insert(std::make_pair(chNumber,PC)); 
	  HVVal.insert(std::make_pair(chNumber,HVtemp)); 
	  MCPName.insert(std::make_pair(chNumber,name)); 
	}

      //-----Definitions
      vector<float> digiCh[10];
      float timeCF[10], timeCFcorr[10];
      float timeOT[10];
      float timeMax[10];
      float intBase[10], intSignal[10], intSignalcorr[10], ampMax[10];
      ///int fibreX[8], hodoYchannels[8];
      
      TH1F** wfHisto = new TH1F*[10];
      TF1** f_templateFit = new TF1*[10];      

      //--reading wire chamber from other tree --
      TChain* t1 = new TChain("outputTree");
      InitTree2(t1);

      //---Chain
      TChain* chain = new TChain("H4tree");
      InitTree(chain);

      char command1[300];
      sprintf(command1, "find  %s/%d/*/dqmPlotstotal.root > listTemp.txt", (inputFolder).c_str(), run);
      system(command1);
      char command2[300];
      sprintf(command2, "find  %s/%d/[0-9]*.root > listTemp2.txt", (inputFolder).c_str(), run);
      system(command2);

      ifstream rootList ("listTemp.txt");
      ifstream rootList2 ("listTemp2.txt");

      while (!rootList.eof() && !rootList2.eof())
	{
	  char iRun_tW[70];
	  rootList >> iRun_tW;
	  char iRun_str[70];
	  rootList2 >> iRun_str;
	  
	  TChain* tTemp = new TChain("outputTree");
	  tTemp->Add(iRun_tW);
	  TChain* tTempH4 = new TChain("H4tree");
	  tTempH4->Add(iRun_str);
	  
	  if (tTemp->GetEntries() == tTempH4->GetEntries())
	    {
	      t1->Add(iRun_tW);	
	      chain->Add(iRun_str);	
	    }
	  else
	    std::cout<<"Bad spill found.. Skipped"<<std::endl;
	  tTemp->Delete();
	  tTempH4->Delete();
	}
      
      system("rm listTemp.txt");
      system("rm listTemp2.txt");
      
      std::cout<<"start reading run: "<<run<<std::endl;
      
      //-----Data loop--------------------------------------------------------
      for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++){
	//      for(int iEntry=7; iEntry<8; iEntry++){    //RA
	if(iEntry % 1000 == 0)	cout << "read entry: " << iEntry << endl;
            //-----Unpack data--------------------------------------------------
            for(int iCh=0; iCh<nChannels; iCh++)
            {
                digiCh[iCh].clear();
		wfHisto[iCh] = new TH1F(Form("wfHisto_%d", iCh), "", 1024, 0., 1024.);
            }

            //---Read the entry
            chain->GetEntry(iEntry);
 
	    unsigned int spill=spillNumber;
	    unsigned int event=evtNumber;

	    for(unsigned int iCh=0; iCh<nAdcChannels; iCh++)
		{
                    if(adcBoard[iCh] == 1 && adcChannel[iCh] == 0) 
                        sci_front_adc = adcData[iCh];
		    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOX_ADC_START_CHANNEL &&
		       adcChannel[iCh] <= HODOX_ADC_END_CHANNEL)
			fibreX[(adcChannel[iCh]-HODOX_ADC_START_CHANNEL)] = adcData[iCh];
		    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOY_ADC_START_CHANNEL &&
		       adcChannel[iCh] <= HODOY_ADC_END_CHANNEL)
			fibreY[(adcChannel[iCh]-HODOY_ADC_START_CHANNEL)] = adcData[iCh];
		}

	    int tStart[10] = {0,0,0,0,0,0,0,0,0,0};
	    int tStop[10] = {0,0,0,0,0,0,0,0,0,0};
	    int tMax[10] = {0,0,0,0,0,0,0,0,0,0};
	    float aMax[10] = {10000., 10000., 10000., 10000., 10000., 10000., 10000., 10000., 10000., 10000.};
	    float bLine[10] = {0.,0.,0.,0.,0., 0.,0.,0.,0.,0.};
	    //---Read digitizer samples
	    for(unsigned int iSample=0; iSample<nDigiSamples; iSample++){
	      if(iSample > 1024*10 - 1) break;
	      if (digiGroup[iSample] == 1 && digiChannel[iSample] == 0){
		digiCh[9].push_back(digiSampleValue[iSample]);
		wfHisto[9]->SetBinContent((iSample%1024) + 1, digiSampleValue[iSample]);

		int iBin = (iSample%1024) + 1;
		float value = digiSampleValue[iSample];
		if((iBin >= 10 && iBin < 30) || (iBin >= 500 && iBin < 1000)) bLine[9] += value/520;
		//		if(digiChannel[iSample] == 9) std::cout << " value = " << value << std::endl;
		if(value < aMax[9]) { aMax[9] = value; tMax[9] = iBin; }
		if(iBin > 50 && value < 500. && tStart[9] == 0) tStart[9] = iBin;
		if(iBin > 50 && value > 500. && tStart[9] != 0 && tStop[9] == 0) tStop[9] = iBin;		
	      }
	      else{
		digiCh[digiChannel[iSample]].push_back(digiSampleValue[iSample]);
		wfHisto[digiChannel[iSample]]->SetBinContent((iSample%1024) + 1, digiSampleValue[iSample]); 

		int iBin = (iSample%1024) + 1;
		float value = digiSampleValue[iSample];
		if((iBin >= 10 && iBin < 30) || (iBin >= 500 && iBin < 1000)) bLine[digiChannel[iSample]] += value/520;
		if(digiChannel[iSample] == 9) std::cout << " value = " << value << std::endl;
		if(value < aMax[digiChannel[iSample]]) { aMax[digiChannel[iSample]] = value; tMax[digiChannel[iSample]] = iBin;}
		if(iBin > 50 && value < 500. && tStart[digiChannel[iSample]] == 0) tStart[digiChannel[iSample]] = iBin;
		if(iBin > 50 && value > 500. && tStart[digiChannel[iSample]] != 0 && 
		   tStop[digiChannel[iSample]] == 0) tStop[digiChannel[iSample]] = iBin;
	      }
	    }

	    //---loop over MPC's channels  
            for(int iCh=0; iCh<nChannels; iCh++){
	      if(iCh < 4) continue;
	      if(iCh == 8) continue;

	      // std::cout << " loop over MCP iCh = " << iCh  << std::endl;
	      // std::cout << " bLine[iCh] = " << bLine[iCh] << std::endl;
	      // std::cout << " aMax[iCh] = " << aMax[iCh] << std::endl;
	      // std::cout << " tMax[iCh] = " << tMax[iCh] << std::endl;
	      // std::cout << " tStart[iCh] = " << tStart[iCh] << std::endl;
	      // std::cout << " tStop[iCh] = " << tStop[iCh] << std::endl;

	      tMax[iCh] -= 300;

	      double scale = 0.;
	      double scaleErr = 0.;
	      double baseL = 0.;
	      double baseLErr = 0.;
	      double xTime = 0.;
	      double xTimeErr = 0.;
	      
	      FindTemplateFit(scale, scaleErr, baseL, baseLErr, xTime, xTimeErr, tStart[iCh], tStop[iCh], tMax[iCh], aMax[iCh], bLine[iCh], templateHisto[iCh], wfHisto[iCh], &(f_templateFit[iCh]));

	      float par0 = f_templateFit[iCh]->GetParameter(0);
	      float par1 = f_templateFit[iCh]->GetParameter(1);
	      float par2 = f_templateFit[iCh]->GetParameter(2);
	      float par3 = f_templateFit[iCh]->GetParameter(3);

	      ampMax[iCh] = par0;
	      //tMax
	      timeMax[iCh] = (300. / par1) + par2;
	      timeCF[iCh] = (pro_medio_CF[iCh] / par1) + par2;
	      intSignal[iCh] = pro_medio_Charge[iCh] * par0/par1 + par3/par1 * 25;
	      intBase[iCh] = par3;

	      timeOT[iCh] = par2;
	      intSignalcorr[iCh] = par1;
	      timeCFcorr[iCh] = timeCF[iCh];

	      // std::cout << " >> timeMax[iCh] = " << timeMax[iCh] << std::endl;
	      // std::cout << " >> timeCF[iCh] = " << timeCF[iCh] << std::endl;
	      // std::cout << " >> f_templateFit[iCh]->GetParameter(0) = " << f_templateFit[iCh]->GetParameter(0) << std::endl;

	      /*
	      if(iCh == 4){
		TCanvas* c1 = new TCanvas();
		templateHisto[iCh]->Draw();
		f_templateFit[iCh]->Draw("same");
		c1->Print("pippo.png", "png");

		TCanvas* c2 = new TCanvas();
		wfHisto[iCh]->Draw();
		f_templateFit[iCh]->Draw("same");
		c2->Print("pippo2.png", "png");
		
		TFile outFile_template("outFile_template.root", "recreate");
		outFile_template.cd();
		iCh = 4;
		templateHisto[iCh]->Write(Form("promedio_%d", iCh));
		wfHisto[iCh]->Write(Form("wfHisto_%d", iCh));
		f_templateFit[iCh]->Write(Form("func_%d", iCh));
		outFile_template.Close();
	      }
	      */
	    }

	    /*	    
	    TFile outFile_template("outFile_template.root", "recreate");
	    outFile_template.cd();
	    for(int iCh=0; iCh<nChannels; iCh++){
	      if(iCh < 4) continue;
	      if(iCh == 8) continue;

	      templateHisto[iCh]->Write(Form("promedio_%d", iCh));
	      wfHisto[iCh]->Write(Form("wfHisto_%d", iCh));
	      f_templateFit[iCh]->Write(Form("func_%d", iCh));
	    }
	    outFile_template.Close();
	    return 100;
	    */

	    //--------dump ntuple - impulses are negative, invert the sign
	    for (int iCh=0; iCh<nChannels; iCh++)
	      {
		    time_CF[MCPList.at(MCPName.at(iCh))]   = timeCF[iCh];
		    time_CF_corr[MCPList.at(MCPName.at(iCh))]   = timeCFcorr[iCh];
		    time_OT[MCPList.at(MCPName.at(iCh))]   = timeOT[iCh];
		    time_Max[MCPList.at(MCPName.at(iCh))]   = timeMax[iCh];
		    amp_max[MCPList.at(MCPName.at(iCh))]   = ampMax[iCh];
		    amp_max_corr[MCPList.at(MCPName.at(iCh))]   = ampMax[iCh];
		    charge[MCPList.at(MCPName.at(iCh))]    = intSignal[iCh];
		    charge_corr[MCPList.at(MCPName.at(iCh))]    = intSignalcorr[iCh];
		    baseline[MCPList.at(MCPName.at(iCh))]  = intBase[iCh];

		    isPCOn[MCPList.at(MCPName.at(iCh))]      = PCOn.at(iCh);
		    HV[MCPList.at(MCPName.at(iCh))]          = HVVal.at(iCh);
		    if (strcmp((MCPName.at(iCh)).c_str(),trig1.c_str())==0)          isTrigger[MCPList.at(MCPName.at(iCh))] = 1;
		    //		    else if (strcmp((MCPName.at(iCh)).c_str(),trig2.c_str())==0)     isTrigger[MCPList.at(MCPName.at(iCh))] = 2;
		    else                                           isTrigger[MCPList.at(MCPName.at(iCh))] = 0;
		      
		    wfHisto[iCh]->Delete();
		  }
	    
      	     run_id = run;
	     X0     = X0temp;

	     t1->GetEntry(iEntry);
	     tdcX = (*TDCreco)[0];
	     tdcY = (*TDCreco)[1];

	     if (spill!=spillNumber || event!=evtNumber) {
	       std::cout<<"PROBLEM: non-coherent read"<<std::endl;
	       continue;
	     }

	     outTree->Fill();    
	     
      }
 
       //---Get ready for next run
        chain->Delete();
	t1->Delete();
    }
    
    //-----close everything-----------------------------------------------------
    //    for(int iCh=0; iCh<nChannels; iCh++) wf_promed[iCh]->Write();

    std::cout << " ciao pippo " << std::endl;

    outTree->Write();
    outROOT->Close();
    
//---------Done-----------------------------------------------------------------
}

        
