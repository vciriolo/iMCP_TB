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
#include "TCanvas.h"

#include "../src/analysis_tools.cc"
#include "../src/init_tree_BTF.cc"
#include "../src/init_Reco_Tree.cc"
#include "../interface/histo_func.h"
#include "../src/MCPMap.cc"

//*******MAIN*******************************************************************
int main (int argc, char** argv)
{  
    std::cout<<"--------DUMPER: READ RAW DATA AND DRAW WAVEFORM--------"<<std::endl;
    
    //--------Read Options--------------------------------
    std::string inputFolder = argv[1];
    int nChannels = atoi (argv[2]);
    int run = atoi (argv[3]);
    int channel = atoi (argv[4]);
    int firstEntry = atoi (argv[5]);

    TGraph* gWF[10];
    TCanvas* c[10];

      //---Chain
      TChain* chain = new TChain("eventRawData");
      InitTree(chain);
      //-----Read raw data tree-----------------------------------------------
      char iRun_str[40];
      sprintf(iRun_str, "%s/run_IMCP_%d_*.root", (inputFolder).c_str(), run);
      chain->Add(iRun_str);
      cout << "\nReading:  "<<iRun_str << endl;
      //-----Data loop--------------------------------------------------------
      for(int iEntry=firstEntry; iEntry<(firstEntry+10); iEntry++)
        {
	  cout << "read entry: " << iEntry << endl;
          //---Read the entry
          chain->GetEntry(iEntry);

	  gWF[iEntry-firstEntry] = new TGraph();
	  c[iEntry-firstEntry]  = new TCanvas();
	  c[iEntry-firstEntry]->cd();
	  int i=0;
		//---Read digitizer samples
	  for(unsigned int iSample=0; iSample<nDigiSamples; iSample++)
	      {
		if(digiChannel[iSample] == channel)
		  {
		    gWF[iEntry-firstEntry]->SetPoint(iSample, i, -digiSampleValue[iSample]);
		    i++;
		  }
	      }

	  gWF[iEntry-firstEntry]->GetXaxis()->SetTitle("sample");
	  gWF[iEntry-firstEntry]->Draw("APL");
	  char plot_name[100];
	  sprintf(plot_name, "plots/waveform/run_%d_entry_%d_ch_%d.pdf", run, iEntry, channel);
	  c[iEntry-firstEntry]->Print(plot_name, "pdf");
	}     

        chain->Delete();
    
return 0;
}

        
