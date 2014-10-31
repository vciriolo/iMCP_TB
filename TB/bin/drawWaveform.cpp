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

#include "../src/analysis_tools.cc"
#include "../src/init_tree_H4.cc"
#include "../src/init_Reco_Tree.cc"
#include "../interface/histo_func.h"
#include "../src/MCPMap.cc"

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
    //---Chain
    TChain* chain = new TChain("H4tree");
    InitTree(chain);

    // Definitions
    std::vector<float> digiCh[10];
  
    
      //-----Read raw data tree-----------------------------------------------
      std::string iRun_str = inputFolder+Form("/%d/[0-9]*.root", run);
      chain->Add(iRun_str.c_str());
      std::cout << "\nReading:  " << iRun_str << std::endl;

      //-----Data loop--------------------------------------------------------
      int totEvents = nEvents;
      if(nEvents > chain->GetEntries() || totEvents == -1) totEvents = chain->GetEntries();

      for(int iEntry=0; iEntry<totEvents; iEntry++){
	if(iEntry%100 == 0) std::cout << "read entry: " << iEntry << std::endl;
          //---Read the entry
          chain->GetEntry(iEntry);
	
	  for(int iCh=0; iCh<10; iCh++) digiCh[iCh].clear();

	  //---Read digitizer samples
	  for(unsigned int iSample=0; iSample<nDigiSamples; iSample++){
	    if(iSample > 1024*10 - 1) break;
	    // std::cout << " >>> iSample = " << iSample << std::endl;
	    // std::cout << " >>> digiChannel[iSample] = " << digiChannel[iSample] << std::endl;
	    if (digiGroup[iSample] == 1 && digiChannel[iSample] == 0 && channel == 9) 
	      digiCh[9].push_back(digiSampleValue[iSample]);
	    else
	      digiCh[digiChannel[iSample]].push_back(digiSampleValue[iSample]);
	  }
	  gWF = new TGraph();
	  int i=0;
	  
	  //---loop over MPC's channels                                                                                                               
	  SubtractBaseline(5, 25, &digiCh[channel]);
	  //std::cout << " >>> digiCh[channel].size() = " << digiCh[channel].size() << std::endl;
	  for(unsigned int iSample=0; iSample<digiCh[channel].size(); iSample++){
	    gWF->SetPoint(i, i, digiCh[channel].at(iSample));
	    i++;
	  }

	  gWF->GetXaxis()->SetTitle("sample");
	  mgWF->Add(gWF);
	  // if(iEntry == 0) gWF->Draw("APL");
	  // else gWF->Draw("PL,same");      
      }
      
      mgWF->Draw("apl");
      char plot_name[100];
      sprintf(plot_name, "plots/waveform/run_%d_nEvents_%d_ch_%d.png", run, totEvents, channel);
      c->Print(plot_name, ".png");
      std::cout << plot_name << "created" << std::endl;

      mgWF->Delete();
      chain->Delete();
      
      return 0;
}

        
