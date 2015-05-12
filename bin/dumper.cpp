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
    cout << CFG << endl;

    string inputFolder = CFG.GetOpt<string>("global", "inputDir");
    string outputFile = CFG.GetOpt<string>("global", "outputFile");
    int nRuns = CFG.GetOpt<int>("global", "nRuns");
    int nCh = CFG.GetOpt<int>("global", "nChannels");
    int trigPos = CFG.GetOpt<int>("global", "trigPos");

    //---------output tree----------------
    TFile* outROOT = TFile::Open(outputFile.c_str(), "recreate");  
    outROOT->cd();

    TProfile** wf_promed = new TProfile*[18];
    for(int iCh=0; iCh<18; ++iCh) 
        wf_promed[iCh] = new TProfile(Form("wf_promed_%d",iCh), "", 102400, 0., 1024.);

    TTree* outTree = new TTree("reco_tree", "reco_tree");
    outTree->SetDirectory(0);
    SetOutTree(outTree);

    int iRun=0;
    int start=0;
    //-------runs loop    
    while(iRun < nRuns) 
    {
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
        TChain* positionTree = new TChain("outputTree");
        InitTree2(positionTree);

        //---Chain
        TChain* chain = new TChain("H4tree");
        InitTree(chain);

        // char command1[300];
        // sprintf(command1, "find  %s/%d/*/dqmPlotstotal.root > ntuples/listTemp_%d.txt", (inputFolder).c_str(), run, run);
        // system(command1);
        char command2[300];
        sprintf(command2, "find  %s/%d/[0-9]*.root > ntuples/listTemp2_%d.txt", (inputFolder).c_str(), run, run);
        system(command2);

        // char list1[200];
        char list2[200];
        // sprintf (list1, "ntuples/listTemp_%d.txt", run);
        sprintf (list2, "ntuples/listTemp2_%d.txt", run);

        // ifstream rootList (list1);
        ifstream rootList2 (list2);

        // while (!rootList.eof() && !rootList2.eof())
	// {
        //     char iRun_tW[70];
        //     rootList >> iRun_tW;
        //     char iRun_str[70];
        //     rootList2 >> iRun_str;

        //     TChain* tTemp = new TChain("outputTree");
        //     tTemp->Add(iRun_tW);
        //     TChain* tTempH4 = new TChain("H4tree");
        //     tTempH4->Add(iRun_str);
	  
        //     if (tTemp->GetEntries() == tTempH4->GetEntries())
	//     {
        //         positionTree->Add(iRun_tW);	
        //         chain->Add(iRun_str);	
	//     }
        //     else
        //         cout<<"Bad spill found.. Skipped"<<endl;
        //     tTemp->Delete();
        //     tTempH4->Delete();
	// }

        while(!rootList2.eof())
	{
            char iRun_str[70];
            rootList2 >> iRun_str;
            
            chain->Add(iRun_str);	
	}

        // char command3[300];
        // sprintf(command3, "rm ntuples/listTemp_%d.txt", run);
        char command4[300];
        sprintf(command4, "rm ntuples/listTemp2_%d.txt", run);
      
        // system(command3);
        system(command4);
      
        cout << "start reading run: " << run << endl;

        //-----Data loop--------------------------------------------------------
        for(int iEntry=0; iEntry<chain->GetEntries(); iEntry++)
        {
	    if(iEntry % 1000 == 0)
		cout << "read entry: " << iEntry << " / " << chain->GetEntriesFast() << endl;
            //-----Unpack data--------------------------------------------------
            for(int iCh=0; iCh<18; iCh++)
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
                if(adcBoard[iCh] == 100728833 && adcChannel[iCh] == 0) 
                    sci_front_adc = adcData[iCh];                
                if(adcBoard[iCh] == 100728833 && adcChannel[iCh] == 1) 
                    bgo_back_adc = adcData[iCh];
                /*		    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOX_ADC_START_CHANNEL &&
                                    adcChannel[iCh] <= HODOX_ADC_END_CHANNEL)
                                    fibreX[(adcChannel[iCh]-HODOX_ADC_START_CHANNEL)] = adcData[iCh];
                                    if(adcBoard[iCh] == 1 && adcChannel[iCh] >= HODOY_ADC_START_CHANNEL &&
                                    adcChannel[iCh] <= HODOY_ADC_END_CHANNEL)
                                    fibreY[(adcChannel[iCh]-HODOY_ADC_START_CHANNEL)] = adcData[iCh];
                */
            }

	    //---Read digitizer samples
	    for(unsigned int iSample=0; iSample<nDigiSamples; iSample++)
                digiCh[digiGroup[iSample]*9+digiChannel[iSample]].push_back(digiSampleValue[iSample]);

	    int triggerTime=400;                  //DON'T CHANGE THIS!!!!!
	    SubtractBaseline(5, 25, &digiCh[trigPos]);  //trigger baseline subtraction
	    triggerTime=int(TimeConstFrac(triggerTime, 600, &digiCh[trigPos], 1.)/0.2); //trigger
	    if (triggerTime<100 || triggerTime >800)  
                continue;

	    //---loop over MPC's channels
	    for(int jCh=0; jCh<nCh; jCh++)
            {
                string currentMCP = CFG.GetOpt<string>("global", "MCPs", jCh);
                int iCh = CFG.GetOpt<int>(currentMCP, "digiChannel");

		if(currentMCP.find("clock") != string::npos) 
                { 
                    //---clock digitization info
                    SubtractBaseline(5, 25, &digiCh[iCh]);  
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
		else 
                {
                    int ampMaxTimeTemp = TimeConstFrac(triggerTime-50, triggerTime+50, &digiCh[iCh], 1)/0.2; 
                    //time of max sample (window's coincidence:-50,50)
                    //window for charge calculation
                    int t1 = ampMaxTimeTemp-13;
                    int t2 = ampMaxTimeTemp+12;
                    ampMaxT[iCh]=ampMaxTimeTemp;
		
                    //subtract baseline immediately before pulse		
                    if(iCh!=trigPos) 
                        SubtractBaseline(t1-27, t1-7, &digiCh[iCh]);  
                    intBase[iCh] = ComputeIntegral(26, 50, &digiCh[iCh]);

                    if(t1 > 50 && t1 < 1024 && t2 > 50 && t2 < 1024){
                        ampMax[iCh] = AmpMax(t1, t2, &digiCh[iCh]);
                        intSignal[iCh] = ComputeIntegral(t1, t2, &digiCh[iCh]);
                    }
                    else
                        ampMax[iCh] = AmpMax(47, 500, &digiCh[iCh]);
                    //---time OT
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

                    timeOT[iCh] = TimeOverThreshold(t1-10, t2+10, &digiCh[iCh], -1000., tStart, tStop);
                    timeStart[iCh]=tStart;
                    timeStop[iCh]=tStop;

                    timeCF[iCh] = TimeConstFracAbs(t1-10, t2+10, &digiCh[iCh], 0.5, ampMax[iCh]);
                    timeCF30[iCh] = TimeConstFracAbs(t1-10, t2+10, &digiCh[iCh], 0.3, ampMax[iCh]);

                    if(ampMax[iCh] < -3300.) 
                    {
                        ampMaxcorr[iCh] =
                            get_amp_max_from_time_OT(iCh, timeOT[iCh]*0.2, CFG.GetOpt<int>(currentMCP, "isPCON"), run);
                        intSignalcorr[iCh] =
                            get_charge_from_amp_max(iCh, ampMaxcorr[iCh], CFG.GetOpt<int>(currentMCP, "isPCON"), run);
                        timeCFcorr[iCh] =
                            get_time_CF_from_time_OT(iCh, timeOT[iCh]*0.2, CFG.GetOpt<int>(currentMCP, "isPCON"),
                                                     run, tStart*0.2)/0.2; 
                    }
                    else 
                    {
                        ampMaxcorr[iCh] = -1.*ampMax[iCh];
                        intSignalcorr[iCh] = -1.*intSignal[iCh];
                        timeCFcorr[iCh] = timeCF[iCh];
                    }
		}

                //---fill the MCP reco variables
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
                HV[MCPList.at(currentMCP)] = CFG.GetOpt<float>(currentMCP, "HV", iRun);
                HV2[MCPList.at(currentMCP)] = CFG.GetOpt<float>(currentMCP, "HV2", iRun);
                isTrigger[MCPList.at(currentMCP)] = CFG.GetOpt<int>(currentMCP, "isTrigger");            
            }

            run_id = run;
            X0     = CFG.GetOpt<float>("global", "nX0");

            // positionTree->GetEntry(iEntry);
            // tdcX = (*TDCreco)[0];
            // tdcY = (*TDCreco)[1];
            // // event = evtNumber;

            // nhodoX1=0;
            // nhodoX2=0;
            // nhodoY1=0;
            // nhodoY2=0;

            // for (int i=0; i<64; i++)
            // {
            //     hodoX1[i] = (*HODOX1)[i];		 
            //     hodoX2[i] = (*HODOX2)[i];
            //     hodoY1[i] = (*HODOY1)[i];
            //     hodoY2[i] = (*HODOY2)[i];		      
            //     //cout<<(*HODOX1)[i]<<" "<<hodoX1[i]<<endl;	  //DEBUG    
            //     if (hodoX1[i]==true)   nhodoX1++;
            //     if (hodoX2[i]==true)   nhodoX2++;
            //     if (hodoY1[i]==true)   nhodoY1++;
            //     if (hodoY2[i]==true)   nhodoY2++;
            // }
            outTree->Fill();    
	}     
        //---Get ready for next run
        chain->Delete();
	// positionTree->Delete();
        ++iRun;
    }
    //-----close everything-----------------------------------------------------
    for(int iCh=0; iCh<nCh; iCh++) 
        wf_promed[iCh]->Write();

    outTree->Write();
    outROOT->Close();
    
//---------Done-----------------------------------------------------------------
}

        
