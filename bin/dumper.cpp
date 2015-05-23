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

// 2015 hodo mapping
map<int, int> ADC_to_PMT_map;
map<int, int> PMT_to_hodoX_map;
map<int, int> PMT_to_hodoY_map;

void FillMaps()
{  
    // PMT In - PNT Out  [both 0->63]
    ADC_to_PMT_map.insert(pair<int,int>(0,35));
    ADC_to_PMT_map.insert(pair<int,int>(1,41));
    ADC_to_PMT_map.insert(pair<int,int>(2,48));
    ADC_to_PMT_map.insert(pair<int,int>(3,56));
    ADC_to_PMT_map.insert(pair<int,int>(4,57));
    ADC_to_PMT_map.insert(pair<int,int>(5,49));
    ADC_to_PMT_map.insert(pair<int,int>(6,58));
    ADC_to_PMT_map.insert(pair<int,int>(7,50));
    ADC_to_PMT_map.insert(pair<int,int>(8,32));
    ADC_to_PMT_map.insert(pair<int,int>(9,59));
    ADC_to_PMT_map.insert(pair<int,int>(10,33));
    ADC_to_PMT_map.insert(pair<int,int>(11,51));
    ADC_to_PMT_map.insert(pair<int,int>(12,40));
    ADC_to_PMT_map.insert(pair<int,int>(13,60));
    ADC_to_PMT_map.insert(pair<int,int>(14,34));
    ADC_to_PMT_map.insert(pair<int,int>(15,52));
    ADC_to_PMT_map.insert(pair<int,int>(16,42));
    ADC_to_PMT_map.insert(pair<int,int>(17,61));
    ADC_to_PMT_map.insert(pair<int,int>(18,43));
    ADC_to_PMT_map.insert(pair<int,int>(19,53));
    ADC_to_PMT_map.insert(pair<int,int>(20,62));
    ADC_to_PMT_map.insert(pair<int,int>(21,44));
    ADC_to_PMT_map.insert(pair<int,int>(22,63));
    ADC_to_PMT_map.insert(pair<int,int>(23,54));
    ADC_to_PMT_map.insert(pair<int,int>(24,36));
    ADC_to_PMT_map.insert(pair<int,int>(25,55));
    ADC_to_PMT_map.insert(pair<int,int>(26,46));
    ADC_to_PMT_map.insert(pair<int,int>(27,45));
    ADC_to_PMT_map.insert(pair<int,int>(28,47));
    ADC_to_PMT_map.insert(pair<int,int>(29,38));
    ADC_to_PMT_map.insert(pair<int,int>(30,37));
    ADC_to_PMT_map.insert(pair<int,int>(31,39));
    ADC_to_PMT_map.insert(pair<int,int>(32,29));
    ADC_to_PMT_map.insert(pair<int,int>(33,31));
    ADC_to_PMT_map.insert(pair<int,int>(34,21));
    ADC_to_PMT_map.insert(pair<int,int>(35,30));
    ADC_to_PMT_map.insert(pair<int,int>(36,23));
    ADC_to_PMT_map.insert(pair<int,int>(37,28));
    ADC_to_PMT_map.insert(pair<int,int>(38,22));
    ADC_to_PMT_map.insert(pair<int,int>(39,15));
    ADC_to_PMT_map.insert(pair<int,int>(40,20));
    ADC_to_PMT_map.insert(pair<int,int>(41,14));
    ADC_to_PMT_map.insert(pair<int,int>(42,7));
    ADC_to_PMT_map.insert(pair<int,int>(43,27));
    ADC_to_PMT_map.insert(pair<int,int>(44,6));
    ADC_to_PMT_map.insert(pair<int,int>(45,13));
    ADC_to_PMT_map.insert(pair<int,int>(46,3));
    ADC_to_PMT_map.insert(pair<int,int>(47,5));
    ADC_to_PMT_map.insert(pair<int,int>(48,12));
    ADC_to_PMT_map.insert(pair<int,int>(49,4));
    ADC_to_PMT_map.insert(pair<int,int>(50,11));
    ADC_to_PMT_map.insert(pair<int,int>(51,19));
    ADC_to_PMT_map.insert(pair<int,int>(52,10));
    ADC_to_PMT_map.insert(pair<int,int>(53,18));
    ADC_to_PMT_map.insert(pair<int,int>(54,2));
    ADC_to_PMT_map.insert(pair<int,int>(55,17));
    ADC_to_PMT_map.insert(pair<int,int>(56,9));
    ADC_to_PMT_map.insert(pair<int,int>(57,16));
    ADC_to_PMT_map.insert(pair<int,int>(58,1));
    ADC_to_PMT_map.insert(pair<int,int>(59,0));
    ADC_to_PMT_map.insert(pair<int,int>(60,25));
    ADC_to_PMT_map.insert(pair<int,int>(61,8));
    ADC_to_PMT_map.insert(pair<int,int>(62,26));
    ADC_to_PMT_map.insert(pair<int,int>(63,24));

    // PMT Out - Hodo X  [both 0->63]
    PMT_to_hodoX_map.insert(pair<int,int>(0,28));
    PMT_to_hodoX_map.insert(pair<int,int>(1,17));
    PMT_to_hodoX_map.insert(pair<int,int>(2,16));
    PMT_to_hodoX_map.insert(pair<int,int>(3,15));
    PMT_to_hodoX_map.insert(pair<int,int>(4,14));
    PMT_to_hodoX_map.insert(pair<int,int>(5,13));
    PMT_to_hodoX_map.insert(pair<int,int>(6,12));
    PMT_to_hodoX_map.insert(pair<int,int>(7,0));
    PMT_to_hodoX_map.insert(pair<int,int>(8,29));
    PMT_to_hodoX_map.insert(pair<int,int>(9,18));
    PMT_to_hodoX_map.insert(pair<int,int>(10,19));
    PMT_to_hodoX_map.insert(pair<int,int>(11,20));
    PMT_to_hodoX_map.insert(pair<int,int>(12,9));
    PMT_to_hodoX_map.insert(pair<int,int>(13,10));
    PMT_to_hodoX_map.insert(pair<int,int>(14,11));
    PMT_to_hodoX_map.insert(pair<int,int>(15,1));
    PMT_to_hodoX_map.insert(pair<int,int>(16,30));
    PMT_to_hodoX_map.insert(pair<int,int>(17,23));
    PMT_to_hodoX_map.insert(pair<int,int>(18,22));
    PMT_to_hodoX_map.insert(pair<int,int>(19,21));
    PMT_to_hodoX_map.insert(pair<int,int>(20,8));
    PMT_to_hodoX_map.insert(pair<int,int>(21,7));
    PMT_to_hodoX_map.insert(pair<int,int>(22,6));
    PMT_to_hodoX_map.insert(pair<int,int>(23,2));
    PMT_to_hodoX_map.insert(pair<int,int>(24,31));
    PMT_to_hodoX_map.insert(pair<int,int>(25,24));
    PMT_to_hodoX_map.insert(pair<int,int>(26,25));
    PMT_to_hodoX_map.insert(pair<int,int>(27,26));
    PMT_to_hodoX_map.insert(pair<int,int>(28,27));
    PMT_to_hodoX_map.insert(pair<int,int>(29,5));
    PMT_to_hodoX_map.insert(pair<int,int>(30,4));
    PMT_to_hodoX_map.insert(pair<int,int>(31,3));

    // PMT Out - Hodo Y  [both 0->63]
    PMT_to_hodoY_map.insert(pair<int,int>(32,3));
    PMT_to_hodoY_map.insert(pair<int,int>(33,4));
    PMT_to_hodoY_map.insert(pair<int,int>(34,5));
    PMT_to_hodoY_map.insert(pair<int,int>(35,27));
    PMT_to_hodoY_map.insert(pair<int,int>(36,26));
    PMT_to_hodoY_map.insert(pair<int,int>(37,25));
    PMT_to_hodoY_map.insert(pair<int,int>(38,24));
    PMT_to_hodoY_map.insert(pair<int,int>(39,31));
    PMT_to_hodoY_map.insert(pair<int,int>(40,2));
    PMT_to_hodoY_map.insert(pair<int,int>(41,6));
    PMT_to_hodoY_map.insert(pair<int,int>(42,7));
    PMT_to_hodoY_map.insert(pair<int,int>(43,8));
    PMT_to_hodoY_map.insert(pair<int,int>(44,21));
    PMT_to_hodoY_map.insert(pair<int,int>(45,22));
    PMT_to_hodoY_map.insert(pair<int,int>(46,23));
    PMT_to_hodoY_map.insert(pair<int,int>(47,30));
    PMT_to_hodoY_map.insert(pair<int,int>(48,1));
    PMT_to_hodoY_map.insert(pair<int,int>(49,11));
    PMT_to_hodoY_map.insert(pair<int,int>(50,10));
    PMT_to_hodoY_map.insert(pair<int,int>(51,9));
    PMT_to_hodoY_map.insert(pair<int,int>(52,20));
    PMT_to_hodoY_map.insert(pair<int,int>(53,19));
    PMT_to_hodoY_map.insert(pair<int,int>(54,18));
    PMT_to_hodoY_map.insert(pair<int,int>(55,29));
    PMT_to_hodoY_map.insert(pair<int,int>(56,0));
    PMT_to_hodoY_map.insert(pair<int,int>(57,12));
    PMT_to_hodoY_map.insert(pair<int,int>(58,13));
    PMT_to_hodoY_map.insert(pair<int,int>(59,14));
    PMT_to_hodoY_map.insert(pair<int,int>(60,15));
    PMT_to_hodoY_map.insert(pair<int,int>(61,16));
    PMT_to_hodoY_map.insert(pair<int,int>(62,17));
    PMT_to_hodoY_map.insert(pair<int,int>(63,28));
}

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  
    cout<<"--------DUMPER: READ RAW DATA AND PRODUCE RECO TREE--------"<<endl;

    //-----this map tells how the MCPs will be order in the output tree. The names should match the cfg file----
    Fill_MCPList();   //look into the MCPMap.h class
    FillMaps();
  
    //--------Read Options--------------------------------
    CfgManager CFG(argv[1]);
    cout << CFG << endl;

    string inputFolder = CFG.GetOpt<string>("global", "inputDir");
    string outputFile = CFG.GetOpt<string>("global", "outputFile");
    string runType = CFG.GetOpt<string>("global", "runType");
    int nRuns = CFG.GetOpt<int>("global", "nRuns");
    int nCh = CFG.GetOpt<int>("global", "nChannels");
    int trigPos = CFG.GetOpt<int>("global", "trigPos");
    int trigPos2 = CFG.GetOpt<int>("global", "trigPos2");

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
        sprintf(command2, "find  %s/%d.root > ntuples/listTemp2_%d.txt", (inputFolder).c_str(), run, run);
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

            hodoXpos = -1;
            hodoYpos = -1;
	    for(unsigned int iCh=0; iCh<nAdcChannels; iCh++)
            {
                if(adcBoard[iCh] == 100728833 && adcChannel[iCh] == 0) 
                    sci_front_adc = adcData[iCh];                
                if(adcBoard[iCh] == 100728833 && adcChannel[iCh] == 1) 
                    bgo_back_adc = adcData[iCh];
                if(adcBoard[iCh] == 201392129)
                {
                    int tmpPAD = ADC_to_PMT_map[adcChannel[iCh]];
                    if(tmpPAD >= 0 && tmpPAD < 32)
                    {
                        if(adcData[iCh] > 700)
                        {
                            hodoXpos = PMT_to_hodoX_map[tmpPAD];
                            hodoX[PMT_to_hodoX_map[tmpPAD]] = 1;
                        }
                        else
                            hodoX[PMT_to_hodoX_map[tmpPAD]] = 0;
                    }
                    else if(tmpPAD >= 32 && tmpPAD < 64)
                    {
                        if(adcData[iCh] > 700)
                        {
                            hodoYpos = PMT_to_hodoY_map[tmpPAD];
                            hodoY[PMT_to_hodoY_map[tmpPAD]] = 1;
                        }
                        else
                            hodoY[PMT_to_hodoY_map[tmpPAD]] = 0;
                    }
                }
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
		    //                    if(iCh!=trigPos) 
		    //		    SubtractBaseline(t1-27, t1-7, &digiCh[iCh]);  
		    SubtractBaseline(25, 50, &digiCh[iCh]);  
                    intBase[iCh] = ComputeIntegral(50, 75, &digiCh[iCh]);

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
            X0 = -1;
            if(runType == "X0")
                X0 = CFG.GetOpt<float>("global", "nX0", iRun);

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
	outTree->AutoSave();
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

        
