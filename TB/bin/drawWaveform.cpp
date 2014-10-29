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

      //-----Read raw data tree-----------------------------------------------
      char iRun_str[40];
      sprintf(iRun_str, "%s/%d/[0-9]*.root", (inputFolder).c_str(), run);
      chain->Add(iRun_str);
      cout << "\nReading:  "<<iRun_str << endl;

      //-----Data loop--------------------------------------------------------
      int totEvents = nEvents;
      if(nEvents > chain->GetEntries()) totEvents = chain->GetEntries();

      for(int iEntry=0; iEntry<totEvents; iEntry++){
	if(iEntry%100 == 0) std::cout << "read entry: " << iEntry << std::endl;
          //---Read the entry
          chain->GetEntry(iEntry);

	  gWF = new TGraph();
	  int i=0;
		//---Read digitizer samples
	  for(unsigned int iSample=0; iSample<nDigiSamples; iSample++){
		if( (digiGroup[iSample]==0 && digiChannel[iSample] == channel) ||
		    (digiGroup[iSample]==1 && digiChannel[iSample] == 0 && channel == 9) )
		  {
		    gWF->SetPoint(iSample, i, -digiSampleValue[iSample]);
		    i++;
		  }
	      }

	  gWF->GetXaxis()->SetTitle("sample");
	  mgWF->Add(gWF);
	  // if(iEntry == 0) gWF->Draw("APL");
	  // else gWF->Draw("PL,same");      
      }     
      mgWF->Draw("apl");
      char plot_name[100];
      sprintf(plot_name, "plots/waveform/run_%d_nEvents_%d_ch_%d.png", run, totEvents, channel);
      c->Print(plot_name, "png");

      chain->Delete();
    
      return 0;
}

        
