/*******************************************************************************
THIS PROGRAM READ THE RAW DATA AND PRODUCE THE RECO FILE USED FOR THE ANALYSIS
-

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
#include "TApplication.h"

#include "../interface/analysis_tools.h"
#include "../interface/init_tree_H4.h"
#include "../interface/init_Reco_Tree.h"
#include "../interface/MCPMap.h"
#include "../interface/CfgManager.h"

using namespace std;

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  
    cout<<"--------DUMPER: READ RAW DATA AND PRODUCE RECO TREE--------"<<endl;

    //-----this map tells how the MCPs will be order in the output tree. The names should match the cfg file----
    Fill_MCPList();   //look into the MCPMap.h class
  
    //--------Read Options--------------------------------
    CfgManager CFG(argv[1]);

    string inputFolder = CFG.GetOpt<string>("global", "inputDir");
    string outputFile = CFG.GetOpt<string>("global", "outputFile");
    int nRuns = CFG.GetOpt<int>("global", "nRuns");
    int nChannels = CFG.GetOpt<int>("global", "nCh");
    int trigPos = CFG.GetOpt<int>("global", "trigPos");
    
    //---------output tree----------------
    TFile* outROOT = TFile::Open(outputFile.c_str(), "recreate");  
    outROOT->cd();

    TProfile** wf_promed = new TProfile*[18];
    for(int iCh=0; iCh<18; ++iCh) wf_promed[iCh] = new TProfile(Form("wf_promed_%d",iCh), "", 102400, 0., 1024.);

    TTree* outTree = new TTree("reco_tree", "reco_tree");
    outTree->SetDirectory(0);
    SetOutTree(outTree);

    int iRun=-1;
    int start=0;
    //-------runs loop    
    while(iRun < nRuns) 
    {
        ++iRun;
        int run = CFG.GetOpt<int>("global", "runs", iRun);
        //-----Definitions
        vector<float> digiCh[18];
        float timeCF[18], timeCFcorr[18], timeCF30[18];
        float timeOT[18], timeStart[18], timeStop[18], ampMaxT[18];
        float timeStart_1000[18], timeStop_1000[18], timeStart_150[18], timeStop_150[18];
        float timeStart_200[18], timeStop_200[18], timeStart_500[18], timeStop_500[18], timeStart_300[18], timeStop_300[18];
        float intBase[18], intSignal[18], intSignalcorr[18], ampMax[18], ampMaxcorr[18];
        ///int fibreX[8], hodoYchannels[8];
        float tStart, tStop;

        //--reading wire chamber from other tree --
        TChain* t1 = new TChain("outputTree");
        InitTree2(t1);

        //---Chain
        TChain* chain = new TChain("H4tree");
        InitTree(chain);

        char command1[300];
        sprintf(command1, "find  %s/%d/*/dqmPlotstotal.root > listTemp_%s_%d.txt", (inputFolder).c_str(), run, outputFile.c_str(), run);
        system(command1);
        char command2[300];
        sprintf(command2, "find  %s/%d/[0-9]*.root > listTemp2_%s_%d.txt", (inputFolder).c_str(), run, outputFile.c_str(), run);
        system(command2);

        char list1[200];
        char list2[200];
        sprintf (list1, "listTemp_%s_%d.txt", outputFile.c_str(), run);
        sprintf (list2, "listTemp2_%s_%d.txt", outputFile.c_str(), run);

        ifstream rootList (list1);
        ifstream rootList2 (list2);

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
                cout<<"Bad spill found.. Skipped"<<endl;
            tTemp->Delete();
            tTempH4->Delete();
	}

        char command3[300];
        sprintf(command3, "rm listTemp_%s_%d.txt", outputFile.c_str(), run);
        char command4[300];
        sprintf(command4, "rm listTemp2_%s_%d.txt", outputFile.c_str(), run);
      
        system(command3);
        system(command4);
      
        cout<<"start reading run: "<<run<<endl;
      
        /*      //-----Read raw data tree-----------------------------------------------
                char iRun_str[70];
                sprintf(iRun_str, "%s/%d/[0-9]*.root", (inputFolder).c_str(), run);
                chain->Add(iRun_str);
                cout << "\nReading:  "<<iRun_str << endl;
        */
        //-----Data loop--------------------------------------------------------
        for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++)
        {
	    if(iEntry % 1000 == 0)
		cout << "read entry: " << iEntry << endl;
            //-----Unpack data--------------------------------------------------
            for(int iCh=0; iCh<nChannels; iCh++)
            {
                digiCh[iCh].clear();

            }
            //---Read the entry
            chain->GetEntry(iEntry);
 
	    unsigned int spill=spillNumber;
	    event=evtNumber;

	    if (spill!=spillNumber || event!=evtNumber) {
                cout<<"PROBLEM: non-coherent read"<<endl;
                continue;
	    }

	    for(unsigned int iCh=0; iCh<nAdcChannels; iCh++)
            {
                if(adcBoard[iCh] == 1 && adcChannel[iCh] == 0) 
                    sci_front_adc = adcData[iCh];
                /*		    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOX_ADC_START_CHANNEL &&
                                    adcChannel[iCh] <= HODOX_ADC_END_CHANNEL)
                                    fibreX[(adcChannel[iCh]-HODOX_ADC_START_CHANNEL)] = adcData[iCh];
                                    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOY_ADC_START_CHANNEL &&
                                    adcChannel[iCh] <= HODOY_ADC_END_CHANNEL)
                                    fibreY[(adcChannel[iCh]-HODOY_ADC_START_CHANNEL)] = adcData[iCh];
                */
            }
	    	    

	    //---Read digitizer samples
	    //	    cout << " nDigiSamples = " << nDigiSamples << endl;
	    for(unsigned int iSample=0; iSample<nDigiSamples; iSample++){
                digiCh[digiGroup[iSample]*9+digiChannel[iSample]].push_back(digiSampleValue[iSample]);
	    }

	    int triggerTime=100;                  //DON'T CHANGE THIS!!!!!
	    //SubtractBaseline(5, 25, &digiCh[MCPList.at(trig1)]);  //trigger baseline subtraction
	    //	    triggerTime=int(TimeConstFrac(triggerTime, 300, &digiCh[MCPList.at(trig1)], 1.)/0.2); //trigger
	    SubtractBaseline(5, 25, &digiCh[trigPos]);  //trigger baseline subtraction
	    triggerTime=int(TimeConstFrac(triggerTime, 300, &digiCh[trigPos], 1.)/0.2); //trigger
	    if (triggerTime<100 || triggerTime >800)  continue;

	    //---loop over MPC's channels
	    for(int iCh=0; iCh<nChannels; iCh++)
            {
                string currentMCP = CFG.GetOpt<string>("global", "MCPs", iCh);
		if (iCh==8 || iCh==17) { //clock digitization info
                    SubtractBaseline(5, 25, &digiCh[iCh]);  //baseline subtraction
                    ampMax[iCh] = AmpMax(51, 1000, &digiCh[iCh]);
                    intBase[iCh] = ComputeIntegral(26, 50, &digiCh[iCh]);
                    intSignal[iCh] = ComputeIntegral(51, 1000, &digiCh[iCh]);
                    timeCF[iCh] = TimeConstFracAbs(51, 1000, &digiCh[iCh], 0.5, ampMax[iCh]);

                    TimeOverThreshold(51, 1000, &digiCh[iCh], -150., tStart, tStop);		  
                    timeStart_150[iCh]=tStart;
                    timeStop_150[iCh]=tStop;

                    TimeOverThreshold(51, 1000, &digiCh[iCh], -200., tStart, tStop);
                    timeStart_200[iCh]=tStart;
                    timeStop_200[iCh]=tStop;
		  
                    TimeOverThreshold(51, 1000, &digiCh[iCh], -300., tStart, tStop);
                    timeStart_300[iCh]=tStart;
                    timeStop_300[iCh]=tStop;

                    TimeOverThreshold(51, 1000, &digiCh[iCh], -500., tStart, tStop);
                    timeStart_500[iCh]=tStart;
                    timeStop_500[iCh]=tStop;

                    TimeOverThreshold(51, 1000, &digiCh[iCh], -1000., tStart, tStop);
                    timeStart_1000[iCh]=tStart;
                    timeStop_1000[iCh]=tStop;

                    timeOT[iCh] = TimeOverThreshold(51, 1000, &digiCh[iCh], -1000., tStart, tStop);
                    timeStart[iCh]=tStart;
                    timeStop[iCh]=tStop;

                    ampMaxcorr[iCh] = -1.*ampMax[iCh];
                    intSignalcorr[iCh] = -1.*intSignal[iCh];
                    timeCFcorr[iCh] = timeCF[iCh];
                }
		//save infos for MCPs
		else {

                    int ampMaxTimeTemp = TimeConstFrac(triggerTime-50, triggerTime+50, &digiCh[iCh], 1)/0.2; //time of max sample (window's coincidence:-50,50)
                    //window for charge calculation
                    int t1 = ampMaxTimeTemp-13;
                    int t2 = ampMaxTimeTemp+12;
                    ampMaxT[iCh]=ampMaxTimeTemp;
		
                    if (iCh!=trigPos) SubtractBaseline(t1-27, t1-7, &digiCh[iCh]);  //subtract baseline immediately before pulse		
                    intBase[iCh] = ComputeIntegral(26, 50, &digiCh[iCh]);

                    //fill pro-medio
                    if(t1 > 50 && t1 < 1024 && t2 > 50 && t2 < 1024) ampMax[iCh] = AmpMax(t1, t2, &digiCh[iCh]);
                    ampMax[iCh] = AmpMax(47, 500, &digiCh[iCh]);
                    if(ampMax[iCh] < -30. && ampMax[iCh] > -3000.){			
                        double timeCFpm = TimeConstFracAbs(47, 800, &digiCh[iCh], 0.5, ampMax[iCh]);
                        for(unsigned int iSample=0; iSample<digiCh[iCh].size(); ++iSample) {
                            if(timeCFpm > 80. && timeCFpm < 220.) wf_promed[iCh]->Fill( (iSample + 300 - timeCFpm) , (-1.*digiCh[iCh].at(iSample)/ampMax[iCh]));
                        }
                    }//unsaturated
		
                    if(t1 > 50 && t1 < 1024 && t2 > 50 && t2 < 1024){
                        ampMax[iCh] = AmpMax(t1, t2, &digiCh[iCh]);
                        intSignal[iCh] = ComputeIntegral(t1, t2, &digiCh[iCh]);
                    }
                    /*		  else{    //A COSA SERVE????
                                  intSignal[iCh] = ComputeIntegral(50, 70, &digiCh[iCh]);	
                                  ampMax[iCh] = AmpMax(0, 1024, &digiCh[iCh]);
                                  }*/

                    //time OT
                    TimeOverThreshold(t1-10, t2+30, &digiCh[iCh], -150., tStart, tStop);
                    timeStart_150[iCh]=tStart;
                    timeStop_150[iCh]=tStop;

                    TimeOverThreshold(t1-10, t2+30, &digiCh[iCh], -200., tStart, tStop);
                    timeStart_200[iCh]=tStart;
                    timeStop_200[iCh]=tStop;
		  
                    TimeOverThreshold(t1-10, t2+30, &digiCh[iCh], -300., tStart, tStop);
                    timeStart_300[iCh]=tStart;
                    timeStop_300[iCh]=tStop;

                    TimeOverThreshold(t1-10, t2+30, &digiCh[iCh], -500., tStart, tStop);
                    timeStart_500[iCh]=tStart;
                    timeStop_500[iCh]=tStop;

                    TimeOverThreshold(t1-10, t2+30, &digiCh[iCh], -1000., tStart, tStop);
                    timeStart_1000[iCh]=tStart;
                    timeStop_1000[iCh]=tStop;

                    /*
                      if( (tStart < 0. || tStop < 0.) && tStart != -100){
                      cout << "iEntry =  " << iEntry << endl;
                      cout << " channel iCh = " << iCh << " tStart = " << tStart 
                      << " tStop = " << tStop 
                      << " ampMax[iCh] = " << ampMax[iCh] << endl;

                      TH1F* histo = new TH1F("histo", "", 1024, 0., 1024.);
                      for(int iBinni=0; iBinni<1024; ++iBinni)
		      histo->SetBinContent(iBinni+1, digiCh[iCh].at(iBinni));
                      TFile outF("outF.root", "recreate");
                      outF.cd();
                      histo->Write();
                      outF.Close();
                      return 1000;
                      }
                    */

                    timeOT[iCh] = TimeOverThreshold(t1-10, t2+10, &digiCh[iCh], -1000., tStart, tStop);
                    timeStart[iCh]=tStart;
                    timeStop[iCh]=tStop;

                    timeCF[iCh] = TimeConstFracAbs(t1-10, t2+10, &digiCh[iCh], 0.5, ampMax[iCh]);
                    timeCF30[iCh] = TimeConstFracAbs(t1-10, t2+10, &digiCh[iCh], 0.3, ampMax[iCh]);

                    if(ampMax[iCh] < -3300.) {
                        ampMaxcorr[iCh] =
                            get_amp_max_from_time_OT(iCh, timeOT[iCh]*0.2, CFG.GetOpt<int>(currentMCP, "isPCON"), run);
                        intSignalcorr[iCh] =
                            get_charge_from_amp_max(iCh, ampMaxcorr[iCh], CFG.GetOpt<int>(currentMCP, "isPCON"), run);
                        timeCFcorr[iCh] =
                            get_time_CF_from_time_OT(iCh, timeOT[iCh]*0.2, CFG.GetOpt<int>(currentMCP, "isPCON"),
                                                     run, tStart*0.2)/0.2; 
                    }
                    else {
                        ampMaxcorr[iCh] = -1.*ampMax[iCh];
                        intSignalcorr[iCh] = -1.*intSignal[iCh];
                        timeCFcorr[iCh] = timeCF[iCh];
                    }

                    //		  cout<<iEntry<<" "<<iCh<<" "<<t1<<" "<<t2<<" "<<ampMax[iCh]<<" "<<timeCF[iCh]<<" "<<timeStart[iCh]<<endl;
                    //		  if (timeCF[iCh]<0 || timeCF[iCh]>1024) {  cout<<iEntry<<" "<<iCh<<" "<<timeCF[iCh]<<endl; getchar();}
                    //		  if (ampMax[iCh]<-1000.)  {cout<<amp_max[iCh]<<endl; getchar(); }
                    //		  		  if(iCh==7 && ampMax[iCh]<-1000.) { cout<<timeCF[iCh]<<" "<<tStart<<" "<<endl; getchar();}
		  		  
                    /*		  if(ampMax[iCh] < -1000.){
                                  if(ampMaxcorr[iCh]*0.5 > -6000.) timeCFcorr[iCh] = TimeConstFracAbs(t1-10, t2+10, &digiCh[iCh], 0.5, ampMaxcorr[iCh]);
                                  else timeCFcorr[iCh] = -999.;
                                  }
		  
                                  else timeCFcorr[iCh] = timeCF[iCh];
                    */
		}
                
                time_CF[MCPList.at(currentMCP)]   = timeCF[iCh]*0.2;
                time_CF_corr[MCPList.at(currentMCP)]   = timeCFcorr[iCh]*0.2;
                time_CF30[MCPList.at(currentMCP)]   = timeCF30[iCh]*0.2;
                time_OT[MCPList.at(currentMCP)]   = timeOT[iCh]*0.2;
                time_start[MCPList.at(currentMCP)]   = timeStart[iCh]*0.2;
                time_stop[MCPList.at(currentMCP)]   = timeStop[iCh]*0.2;
                time_start_150[MCPList.at(currentMCP)]   = timeStart_150[iCh]*0.2;
                time_stop_150[MCPList.at(currentMCP)]   = timeStop_150[iCh]*0.2;
                time_start_200[MCPList.at(currentMCP)]   = timeStart_200[iCh]*0.2;
                time_stop_200[MCPList.at(currentMCP)]   = timeStop_200[iCh]*0.2;
                time_start_300[MCPList.at(currentMCP)]   = timeStart_300[iCh]*0.2;
                time_stop_300[MCPList.at(currentMCP)]   = timeStop_300[iCh]*0.2;
                time_start_500[MCPList.at(currentMCP)]   = timeStart_500[iCh]*0.2;
                time_stop_500[MCPList.at(currentMCP)]   = timeStop_500[iCh]*0.2;
                time_start_1000[MCPList.at(currentMCP)]   = timeStart_1000[iCh]*0.2;
                time_stop_1000[MCPList.at(currentMCP)]   = timeStop_1000[iCh]*0.2;
                amp_max[MCPList.at(currentMCP)]   = -ampMax[iCh];
                amp_max_corr[MCPList.at(currentMCP)]   = ampMaxcorr[iCh];
                amp_max_time[MCPList.at(currentMCP)]   = ampMaxT[iCh]*0.2;
                charge[MCPList.at(currentMCP)]    = -intSignal[iCh];
                charge_corr[MCPList.at(currentMCP)]    = intSignalcorr[iCh];
                baseline[MCPList.at(currentMCP)]  = -intBase[iCh];

                isPCOn[MCPList.at(currentMCP)] = CFG.GetOpt<int>(currentMCP, "isPCON");
                HV[MCPList.at(currentMCP)] = CFG.GetOpt<float>(currentMCP, "HV", run);
                isTrigger[MCPList.at(currentMCP)] = CFG.GetOpt<int>(currentMCP, "isTrigger");
            }

            run_id = run;
            X0     = CFG.GetOpt<float>("global", "nX0");

            t1->GetEntry(iEntry);
            tdcX = (*TDCreco)[0];
            tdcY = (*TDCreco)[1];
            // event = evtNumber;

            nhodoX1=0;
            nhodoX2=0;
            nhodoY1=0;
            nhodoY2=0;

            for (int i=0; i<64; i++)
            {
                hodoX1[i] = (*HODOX1)[i];		 
                hodoX2[i] = (*HODOX2)[i];
                hodoY1[i] = (*HODOY1)[i];
                hodoY2[i] = (*HODOY2)[i];		      
                //cout<<(*HODOX1)[i]<<" "<<hodoX1[i]<<endl;	  //DEBUG    
                if (hodoX1[i]==true)   nhodoX1++;
                if (hodoX2[i]==true)   nhodoX2++;
                if (hodoY1[i]==true)   nhodoY1++;
                if (hodoY2[i]==true)   nhodoY2++;
            }

            outTree->Fill();    

            //  }
	}     
        //---Get ready for next run
        chain->Delete();
	t1->Delete();
    }
    //-----close everything-----------------------------------------------------
    for(int iCh=0; iCh<nChannels; iCh++) wf_promed[iCh]->Write();

    outTree->Write();
    outROOT->Close();
    
//---------Done-----------------------------------------------------------------
}

        
