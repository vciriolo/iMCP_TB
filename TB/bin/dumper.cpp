/*******************************************************************************
THIS PROGRAM READ THE RAW DATA AND PRODUCE THE RECO FILE USED FOR THE ANALYSIS
-
    compile with --> c++ -o dumper bin/dumper.cpp `root-config --cflags --glibs`
    run with --> ./dumper cfg/example_cfg_file.cfg /gwteray/users/marzocchi/iMCP/dataTrees 9 prova
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
#include "TApplication.h"

#include "../src/analysis_tools.cc"
#include "../src/init_tree_H4.cc"
#include "../src/init_Reco_Tree.cc"
//#include "../interface/histo_func.h"
#include "../src/MCPMap.cc"

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  
    std::cout<<"--------DUMPER: READ RAW DATA AND PRODUCE RECO TREE--------"<<std::endl;

    //-----this map tells how the MCPs will be order in the output tree. The names should match the cfg file----
    Fill_MCPList();  //look into the MCPMap.h class
    
    //--------Read Options--------------------------------
    ifstream inputCfg (argv[1], ios::in);

    std::string inputFolder = argv[2];
    int nChannels = atoi (argv[3]);
    std::string outputFile = argv[4];

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
      float timeCF[10];
      float timeOT[10];
      float intBase[10], intSignal[10], ampMax[10];
            int goodEvt=1;
      ///int fibreX[8], hodoYchannels[8];
      //---Chain
      TChain* chain = new TChain("H4tree");
      InitTree(chain);
      //-----Read raw data tree-----------------------------------------------
      char iRun_str[40];
      sprintf(iRun_str, "%s/%d/[0-9]*.root", (inputFolder).c_str(), run);
      chain->Add(iRun_str);
      cout << "\nReading:  "<<iRun_str << endl;
      //-----Data loop--------------------------------------------------------
      for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++){
	//	for(int iEntry=0; iEntry<10; iEntry++){    //RA
	    if(iEntry % 1000 == 0)
		cout << "read entry: " << iEntry << endl;
            //-----Unpack data--------------------------------------------------
            for(int iCh=0; iCh<nChannels; iCh++)
            {
                digiCh[iCh].clear();
            }
            //---Read the entry
            chain->GetEntry(iEntry);
	    //---DAQ bug workaround
	    //	    	    if(run < 145) goodEvt = 10;
	    // 	    else goodEvt = 1;
	    //   if(evtNumber % goodEvt == 0) 
	    //   {
            //---Read SciFront ADC value and set the e- multiplicity 

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
	    

		//---Read digitizer samples
             for(unsigned int iSample=0; iSample<nDigiSamples; iSample++)
	       {
		 if (digiGroup[iSample]==1 && digiChannel[iSample]==0)
                        digiCh[9].push_back(digiSampleValue[iSample]);
		 else
                        digiCh[digiChannel[iSample]].push_back(digiSampleValue[iSample]);
	       }

                //---loop over MPC's channels
             for(int iCh=0; iCh<nChannels; iCh++)
                {
                    SubtractBaseline(5, 25, &digiCh[iCh]);
                    timeCF[iCh]=TimeConstFrac(47, 500, &digiCh[iCh], 0.5);
		    timeOT[iCh]=TimeOverThreshold(210, 500, &digiCh[iCh], -20.);
                    int t1 = (int)(timeCF[iCh]/0.2) - 3;
                    int t2 = (int)(timeCF[iCh]/0.2) + 17;
                    intBase[iCh] = ComputeIntegral(26, 46, &digiCh[iCh]);
                    if(t1 > 50 && t1 < 1024 && t2 > 50 && t2 < 1024)
                    {
                        ampMax[iCh] = AmpMax(t1, t2, &digiCh[iCh]);
                        intSignal[iCh] = ComputeIntegral(t1, t2, &digiCh[iCh]);
			//			intSignalCF[iCh] = ComputeIntegral((int)(timeAM[iCh]/0.2)-5, (int)(timeAM[iCh]/0.2), &digiCh[iCh]);
                    }
                    else
                    {
                        ampMax[iCh] = AmpMax(0, 1024, &digiCh[iCh]);
                        intSignal[iCh] = ComputeIntegral(50, 70, &digiCh[iCh]);
			//			intSignalCF[iCh] = 2000;
                    }
                }

	     //--------dump ntuple - impulses are negative, invert the sign
	     for (int iCh=0; iCh<nChannels; iCh++)
		  {
		    time_CF[MCPList.at(MCPName.at(iCh))]   = timeCF[iCh];
		    time_OT[MCPList.at(MCPName.at(iCh))]   = timeOT[iCh];
		    amp_max[MCPList.at(MCPName.at(iCh))]   = -ampMax[iCh];
		    charge[MCPList.at(MCPName.at(iCh))]    = -intSignal[iCh];
		    baseline[MCPList.at(MCPName.at(iCh))]  = -intBase[iCh];

		    isPCOn[MCPList.at(MCPName.at(iCh))]      = PCOn.at(iCh);
		    HV[MCPList.at(MCPName.at(iCh))]          = HVVal.at(iCh);
		    if (strcmp((MCPName.at(iCh)).c_str(),trig1.c_str())==0)          isTrigger[MCPList.at(MCPName.at(iCh))] = 1;
		    //		    else if (strcmp((MCPName.at(iCh)).c_str(),trig2.c_str())==0)     isTrigger[MCPList.at(MCPName.at(iCh))] = 2;
		    else                                           isTrigger[MCPList.at(MCPName.at(iCh))] = 0;
		      
		  }

      	     run_id = run;
	     X0     = X0temp;
	     outTree->Fill();    
	     //  }
	}     
        //---Get ready for next run
        chain->Delete();
    }

    //-----close everything-----------------------------------------------------
    outTree->Write();
    outROOT->Close();
    
//---------Done-----------------------------------------------------------------
}

        
