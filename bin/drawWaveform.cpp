/*******************************************************************************
THIS PROGRAM READ THE RAW DATA AND DRAW 10 WAVEFORMS
-
    compile with --> c++ -o drawWaveform bin/drawWaveform.cpp `root-config --cflags --glibs`
    run with --> ./drawWaveform /gwteray/users/marzocchi/iMCP/dataTrees 9 251 0 0

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
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TLine.h"

#include "../interface/analysis_tools.h"
#include "../interface/init_tree_H4.h"
#include "../interface/init_Reco_Tree.h"
#include "../interface/histo_func.h"
#include "../interface/MCPMap.h"

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  
    std::cout<<"--------DUMPER: READ RAW DATA AND DRAW WAVEFORM--------"<<std::endl;
    
    //--------Read Options--------------------------------
    std::string inputFolder = argv[1];
    int run = atoi (argv[2]);
    int channel = atoi (argv[3]);
    int firstEntry = atoi (argv[4]);
    int nEvents = atoi (argv[5]);
    int trigPos = atoi(argv[6]);
    nEvents += firstEntry;

    std::cout << " >>> run = " << run << std::endl;
    std::cout << " >>> channel = " << channel << std::endl;
    std::cout << " >>> firstEntry = " << firstEntry << std::endl;
    std::cout << " >>> nEvents = " << nEvents << std::endl;

    // TGraph* gWF_single;
    // TCanvas* c_single;
    TMultiGraph *mgWF = new TMultiGraph();
    TGraph* gWF;
    TCanvas* c = new TCanvas();
    c->cd();

    // Definitions
    std::vector<float> digiCh[18];

    int triggerTime;
      //--reading wire chamber from other tree --
    //      TChain* tC = new TChain("outputTree");
    //      InitTree2(tC);

      //---Chain
      TChain* chain = new TChain("H4tree");
      InitTree(chain);

      //      char command1[300];
      //      sprintf(command1, "find  %s/%d/*/dqmPlotstotal.root > listTemp.txt", (inputFolder).c_str(), run);
      //      system(command1);
      char command2[300];
      sprintf(command2, "find  %s/%d/[0-9]*.root > listTemp2.txt", (inputFolder).c_str(), run);
      system(command2);

      //    ifstream rootList ("listTemp.txt");
    ifstream rootList2 ("listTemp2.txt");

    //    while (!rootList.eof() && !rootList2.eof())
    while (!rootList2.eof())
      {
	//	char iRun_tW[70];
	//	rootList >> iRun_tW;
	char iRun_str[70];
	rootList2 >> iRun_str;

	//	TChain* tTemp = new TChain("outputTree");
	//	tTemp->Add(iRun_tW);
	TChain* tTempH4 = new TChain("H4tree");
	tTempH4->Add(iRun_str);

	//	if (tTemp->GetEntries() == tTempH4->GetEntries())
	if (tTempH4->GetEntries())
	  {
	    //	    tC->Add(iRun_tW);	
	    chain->Add(iRun_str);	
	  }
	else
	  std::cout<<"Bad spill found.. Skipped"<<std::endl;
      }

    //    system("rm listTemp.txt");
    system("rm listTemp2.txt");

    int ampMaxTimeTemp;
    int timeCF,t1,t2;
    float tStart, tStop;
    TLine *line;    
    TLine *line2;
    TLine *line3;    
    TLine *line4;
    TLine *line5;    
    TLine *line6;
    TLine *line7;
      //-----Read raw data tree-----------------------------------------------
      std::string iRun_str = inputFolder+Form("/%d/[0-9]*.root", run);
      chain->Add(iRun_str.c_str());
      std::cout << "\nReading:  " << run << std::endl;

      //-----Data loop--------------------------------------------------------
      int totEvents = nEvents;
      if(nEvents > chain->GetEntries() || totEvents == -1) totEvents = chain->GetEntries();

      for(int iEntry=0; iEntry<totEvents; iEntry++){
	if(iEntry%100 == 0) std::cout << "read entry: " << iEntry << std::endl;
          //---Read the entry
          chain->GetEntry(iEntry);
	
	  for(int iCh=0; iCh<18; iCh++) digiCh[iCh].clear();

	  //---Read digitizer samples
	    for(unsigned int iSample=0; iSample<nDigiSamples; iSample++){
	      digiCh[digiGroup[iSample]*9+digiChannel[iSample]].push_back(digiSampleValue[iSample]);
	    //	    if(iSample > 1024*10 - 1) break;
	    // std::cout << " >>> iSample = " << iSample << std::endl;
	    // std::cout << " >>> digiChannel[iSample] = " << digiChannel[iSample] << std::endl;
	  }
	  gWF = new TGraph();
	  int i=0;

	  //---loop over MPC's channels                                                                                                               
	  triggerTime=400;

	  if (iEntry>=firstEntry) {

	    SubtractBaseline(5, 25, &digiCh[trigPos]);
	    triggerTime=int(TimeConstFrac(triggerTime, 600, &digiCh[trigPos], 1.)/0.2);
	    if (triggerTime<100 || triggerTime >800)  continue;
		int ampMaxTimeTemp = TimeConstFrac(triggerTime-50, triggerTime+50, &digiCh[channel], 1)/0.2;

		  t1 = ampMaxTimeTemp-13;
		  t2 = ampMaxTimeTemp+12;

		  //		if (channel!=trigPos)  SubtractBaseline(t1-27, t1-7, &digiCh[channel]);
		if (channel!=trigPos)  SubtractBaseline(25, 50, &digiCh[channel]);
		  if(t1 > 50 && t1 < 1024 && t2 > 50 && t2 < 1024) ampMaxTimeTemp = AmpMax(t1, t2, &digiCh[channel]);
		  ampMaxTimeTemp = AmpMax(47, 500, &digiCh[channel]);

		  timeCF = TimeConstFracAbs(t1-10, t2+10, &digiCh[channel], 0.5, ampMaxTimeTemp);		    
		    //		    if (-AmpMax(5, 25, &digiCh[channel])<-200)  std::cout<<iEntry<<" "<<-AmpMax(5, 25, &digiCh[channel])<<std::endl;
		if (channel==8 || channel==17) {
		  TimeOverThreshold(200, 1000, &digiCh[channel], -150., tStart, tStop);
		}
		    
		//	  for(unsigned int iSample=0; iSample<digiCh[channel].size(); iSample++){
	  for(unsigned int iSample=0; iSample<1000; iSample++){
	    gWF->SetPoint(i, i, digiCh[channel].at(iSample));
	    i++;
	  }

	  gWF->GetXaxis()->SetTitle("sample");
	  mgWF->Add(gWF);
	  //	  std::cout<<triggerTime/0.2<<std::endl;
	  }
	  // if(iEntry == 0) gWF->Draw("APL");
	  // else gWF->Draw("PL,same");      
      }

      if (channel!=trigPos) {
      line = new TLine(t1, -2000, t1, 200);
      line2 = new TLine(t2, -2000, t2, 200);
      line3 = new TLine(t1-7, -2000, t1-7, 200);
      line4 = new TLine(t1-27, -2000, t1-27, 200);
      line5 = new TLine(triggerTime-50, -2000, triggerTime-50, 200);
      line6 = new TLine(triggerTime+50, -2000, triggerTime+50, 200);
      line7 = new TLine(timeCF, -2000, timeCF, 200);
      
      mgWF->Draw("apl");
      line->DrawLine(t1, -2000, t1, 200);
      line->SetLineColor(2);
      line->Draw("same");
      line2->DrawLine(t2, -2000, t2, 200);
      line2->SetLineColor(2);
      line2->Draw("same");
      line3->DrawLine(t1-7, -2000, t1-7, 200);
      line3->SetLineColor(4);
      line3->Draw("same");
      line4->DrawLine(t1-27, -2000, t1-27, 200);
      line4->SetLineColor(4);
      line4->Draw("same");
      line7->DrawLine(timeCF, -2000, timeCF, 200);
      line7->SetLineColor(8);
      line7->Draw("same");
      /*      line5->DrawLine(triggerTime-50, -2000, triggerTime-50, 200);
      line5->SetLineColor(1);
      line5->Draw("same");
      line6->DrawLine(triggerTime+50, -2000, triggerTime+50, 200);
      line6->SetLineColor(1);
      line6->Draw("same");
      */
      }

      else {
      line = new TLine(t1, -2000, t1, 200);
      line2 = new TLine(t2, -2000, t2, 200);
      line3 = new TLine(5, -2000, 5, 200);
      line4 = new TLine(25, -2000, 25, 200);
      line5 = new TLine(triggerTime-50, -2000, triggerTime-50, 200);
      line6 = new TLine(triggerTime+50, -2000, triggerTime+50, 200);
      
      mgWF->Draw("apl");
      /*      line->DrawLine(ampMaxTimeTemp-13, -2000, ampMaxTimeTemp-13, 200);
      line->SetLineColor(2);
      line->Draw("same");
      line2->DrawLine(ampMaxTimeTemp+12, -2000, ampMaxTimeTemp+12, 200);
      line2->SetLineColor(2);
      line2->Draw("same");
      line3->DrawLine(5, -2000, 5, 200);
      line3->SetLineColor(4);
      line3->Draw("same");
      line4->DrawLine(25, -2000, 25, 200);
      line4->SetLineColor(4);
      line4->Draw("same");
      */      line5->DrawLine(triggerTime-50, -2000, triggerTime-50, 200);
      line5->SetLineColor(1);
      line5->Draw("same");
      line6->DrawLine(triggerTime+50, -2000, triggerTime+50, 200);
      line6->SetLineColor(1);
      line6->Draw("same");

      }
      char plot_name[100];
      sprintf(plot_name, "plots/waveform/run_%d_nEvents_%d_ch_%d.png", run, totEvents, channel);
      c->Print(plot_name, ".png");
      std::cout << plot_name << "created" << std::endl;

      mgWF->Delete();
      chain->Delete();
      
      return 0;
}

        
