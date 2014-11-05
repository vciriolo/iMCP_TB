#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "TROOT.h"
#include "TSystem.h"
#include "TKey.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TString.h"
#include "TCut.h"
#include "TMath.h"
#include "TApplication.h"
#include "TError.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPad.h"

#include "../src/init_Reco_Tree.cc"
#include "../src/MCPMap.cc"


void drawBranches(std::string inFile)
{
  //  TFile *_file0 = TFile::Open("reco_test.root");
  //   TFile *_file0 = TFile::Open("test.root");
  TFile *_file0 = TFile::Open(inFile.c_str(), "read");
  TTree* nt = (TTree*)_file0->Get("reco_tree");
  InitRecoTree(nt);

  std::cout << " nt->GetEntries() = " << nt->GetEntries() << std::endl;

  // define histos
  TProfile** p_Tot_amp = new TProfile[5];
  TH2F** h2_Tot_amp = new TH2F[5];
  TH1F** h_amp = new TH1F[5];
  TProfile** p_amp_Tot = new TProfile[5];

  // soglie
  float Tcharge[5] = {126., 93., 90., 145., 89.};

  for(int iCh=0; iCh<5; ++iCh){
    p_Tot_amp[iCh] = new TProfile(Form("p_Tot_amp_%d",iCh), "", 2000, 0., 4000.);
    h2_Tot_amp[iCh] = new TH2F(Form("h2_Tot_amp_%d", iCh), "", 2000, 0., 4000., 1024, 0., 1024.);
    p_amp_Tot[iCh] = new TProfile(Form("p_amp_Tot_%d",iCh), "", 1024, 0., 1024.);
    h_amp[iCh] = new TH1F(Form("h_amp_%d",iCh), "", 2000, 0., 4000.);
  }

  std::cout << " start loop over Entries " << std::endl;

  for(int iEntry=0; iEntry<nt->GetEntries(); ++iEntry){
    nt->GetEntry(iEntry);
    //    std::cout << " tdcX = " << tdcX << std::endl;
    
    for(int iCh=0; iCh<5; ++iCh){
      if(tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6 && charge[iCh] > Tcharge[iCh]){
	// std::cout << " amp_max[iCh] = " << amp_max[iCh] << std::endl;
	// std::cout << " time_OT[iCh] = " << time_OT[iCh] << std::endl;
	// std::cout << " charge[iCh] = " << charge[iCh] << std::endl;

	p_Tot_amp[iCh]->Fill(amp_max[iCh], time_OT[iCh]);
	h2_Tot_amp[iCh]->Fill(amp_max[iCh], time_OT[iCh]);
	p_amp_Tot[iCh]->Fill(time_OT[iCh], amp_max[iCh]);
	h_amp[iCh]->Fill(amp_max[iCh]);
      }
    }
  }
  std::cout << " stop loop over Entries " << std::endl;
  
  // DRAW

  TCanvas** c_pT = new TCanvas*[5];
  TCanvas** c_hT = new TCanvas*[5];
  TCanvas** c_pA = new TCanvas*[5];
  TCanvas** c_h1A = new TCanvas*[5];

  // int iCh = 0;
  // c_pT[0] = new TCanvas();
  // c_pT[0]->cd();
  // p_Tot_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
  // p_Tot_amp[iCh]->GetYaxis()->SetTitle("time_OT");
  // p_Tot_amp[iCh]->Draw("hist");


  for(int iCh=0; iCh<5; ++iCh){
    c_pT[iCh] = new TCanvas();
    c_pT[iCh]->cd();
    p_Tot_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
    p_Tot_amp[iCh]->GetYaxis()->SetTitle("time_OT");
    p_Tot_amp[iCh]->Draw("hist");

    //    continue;
    c_hT[iCh] = new TCanvas();
    c_hT[iCh]->cd();
    h2_Tot_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
    h2_Tot_amp[iCh]->GetYaxis()->SetTitle("time_OT");
    h2_Tot_amp[iCh]->Draw("colz");

    c_pA[iCh] = new TCanvas();
    c_pA[iCh]->cd();
    p_amp_Tot[iCh]->GetXaxis()->SetTitle(Form("time_OT ch%d", iCh));
    p_amp_Tot[iCh]->GetYaxis()->SetTitle("amp");
    p_amp_Tot[iCh]->Draw("hist");

    c_h1A[iCh] = new TCanvas();
    c_h1A[iCh]->cd();
    h_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
    h_amp[iCh]->Draw("hist");

  }

  //  return;

}
