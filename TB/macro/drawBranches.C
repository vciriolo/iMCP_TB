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
  TFile *_file0 = TFile::Open(inFile.c_str(), "read");
  TTree* nt = (TTree*)_file0->Get("reco_tree");
  InitRecoTree(nt);

  std::cout << " nt->GetEntries() = " << nt->GetEntries() << std::endl;

  // define histos
  TProfile** p_Tot_amp = new TProfile[5];
  TProfile** p_amp_Tot = new TProfile[5];
  TProfile** p_charge_Amp = new TProfile[5];

//   TProfile** p_Tcf_amp = new TProfile[5];
//   TProfile** p_Tcf_Tot = new TProfile[5];

  TH2F** h2_Tot_amp = new TH2F[5];
  TH2F** h2_amp_Tot = new TH2F[5];
  TH2F** h2_charge_amp = new TH2F[5];
  TH1F** h_amp = new TH1F[5];


  // soglie
  float Tcharge[5] = {126., 93., 90., 145., 89.};

  for(int iCh=0; iCh<5; ++iCh){
    p_Tot_amp[iCh] = new TProfile(Form("p_Tot_amp_%d",iCh), "", 2000, 0., 4000.);
    p_amp_Tot[iCh] = new TProfile(Form("p_amp_Tot_%d",iCh), "", 1000, 0., 1000.);
    p_charge_Amp[iCh] = new TProfile(Form("p_charge_Amp_%d", iCh), "", 2000, 0., 4000.);

//     p_Tcf_amp[iCh] = new TProfile(Form("p_Tcf_amp_%d",iCh),  "", 300, 0., 300.);
//     p_Tcf_Tot[iCh] = new TProfile(Form("p_Tcf_Tof_%d",iCh),  "", 300, 0., 300.);

    h2_Tot_amp[iCh] = new TH2F(Form("h2_Tot_amp_%d", iCh), "", 2000, 0., 4000., 1000, 0., 1000.);
    h2_amp_Tot[iCh] = new TH2F(Form("h2_amp_Tot_%d", iCh), "", 1000, 0., 1000., 2000, 0., 4000.);
    h2_charge_amp[iCh] = new TH2F(Form("h2_charge_amp_%d",iCh), "", 2000, 0., 4000., 10000, 0., 10000.);
    h_amp[iCh] = new TH1F(Form("h_amp_%d",iCh), "", 2000, 0., 4000.);
  }

  std::cout << " start loop over Entries " << std::endl;

  for(int iEntry=0; iEntry<nt->GetEntries(); ++iEntry){
    nt->GetEntry(iEntry);
    
    for(int iCh=0; iCh<5; ++iCh){
      //      if(tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6 && charge[iCh] > Tcharge[iCh]){
      // std::cout << " amp_max[iCh] = " << amp_max[iCh] << std::endl;
      // std::cout << " time_OT[iCh] = " << time_OT[iCh] << std::endl;
      // std::cout << " charge[iCh] = " << charge[iCh] << std::endl;

	p_Tot_amp[iCh]->Fill(amp_max[iCh], time_OT[iCh]);
	p_amp_Tot[iCh]->Fill(time_OT[iCh], amp_max[iCh]);
	p_charge_Amp[iCh]->Fill(amp_max[iCh], charge[iCh]);

	h2_Tot_amp[iCh]->Fill(amp_max[iCh], time_OT[iCh]);
	h2_amp_Tot[iCh]->Fill(time_OT[iCh], amp_max[iCh]);
	h2_charge_amp[iCh]->Fill(amp_max[iCh], charge[iCh]);
	h_amp[iCh]->Fill(amp_max[iCh]);

      
	/*
	p_Tot_amp[iCh]->Fill(amp_max_corr[iCh], time_OT[iCh]);
	p_amp_Tot[iCh]->Fill(time_OT[iCh], amp_max_corr[iCh]);
	p_charge_Amp[iCh]->Fill(amp_max_corr[iCh], charge_corr[iCh]);

	h2_Tot_amp[iCh]->Fill(amp_max_corr[iCh], time_OT[iCh]);
	h_amp[iCh]->Fill(amp_max_corr[iCh]);
	h2_charge_amp[iCh]->Fill(amp_max_corr[iCh], charge_corr[iCh]);

	p_Tcf_amp[iCh]->Fill(amp_max[iCh], time_CF[iCh]);
	p_Tcf_Tot[iCh]->Fill(time_OT[iCh], time_CF[iCh]);
	*/
	//   }
    }
  }
  std::cout << " stop loop over Entries " << std::endl;
  
  // DRAW

  TCanvas** c_pro_TvA = new TCanvas*[5];
  TCanvas** c_pro_AvT = new TCanvas*[5];
  TCanvas** c_pro_CvA = new TCanvas*[5];

  TCanvas** c_h2_TvA = new TCanvas*[5];
  TCanvas** c_h2_AvT = new TCanvas*[5];
  TCanvas** c_h2_CvA = new TCanvas*[5];

  TCanvas** c_h1_A = new TCanvas*[5];


  for(int iCh=0; iCh<5; ++iCh){

//     c_pro_CvA[iCh] = new TCanvas();
//     c_pro_CvA[iCh]->cd();
//     p_charge_Amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
//     p_charge_Amp[iCh]->GetYaxis()->SetTitle("charge");
//     p_charge_Amp[iCh]->Draw("");

//     c_h2_CvA[iCh] = new TCanvas();
//     c_h2_CvA[iCh]->cd();
//     h2_charge_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
//     h2_charge_amp[iCh]->GetYaxis()->SetTitle(Form("charge ", iCh));
//     h2_charge_amp[iCh]->Draw("colz");

    c_h2_TvA[iCh] = new TCanvas();
    c_h2_TvA[iCh]->cd();
    h2_Tot_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
    h2_Tot_amp[iCh]->GetYaxis()->SetTitle("time_OT");
    h2_Tot_amp[iCh]->Draw("colz");
    

    c_h2_AvT[iCh] = new TCanvas();
    c_h2_AvT[iCh]->cd();
    h2_amp_Tot[iCh]->GetXaxis()->SetTitle(Form("time_OT ch%d", iCh));
    h2_amp_Tot[iCh]->GetYaxis()->SetTitle("amp");
    h2_amp_Tot[iCh]->Draw("hist");


//     c_pTcf1[iCh] = new TCanvas();
//     c_pTcf1[iCh]->cd();
//     p_Tcf_Tot[iCh]->GetXaxis()->SetTitle(Form("time_OT ch%d", iCh));
//     p_Tcf_Tot[iCh]->GetYaxis()->SetTitle("Tcf");
//     p_Tcf_Tot[iCh]->Draw("hist");

//     c_pTcf2[iCh] = new TCanvas();
//     c_pTcf2[iCh]->cd();
//     p_Tcf_amp[iCh]->GetXaxis()->SetTitle(Form("time_OT ch%d", iCh));
//     p_Tcf_amp[iCh]->GetYaxis()->SetTitle("amp");
//     p_Tcf_amp[iCh]->Draw("hist");




    
//     c_h1_A[iCh] = new TCanvas();
//     c_h1_A[iCh]->cd();
//     h_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
//     h_amp[iCh]->Draw("hist");

    /*
    c_h2C[iCh] = new TCanvas();
     c_h2C[iCh]->cd();
    c_h2C[iCh] = new TCanvas();
     c_h2C[iCh]->cd();
    h2_charge_amp[iCh]->GetXaxis()->SetTitle(Form("amp ch%d", iCh));
    h2_charge_amp[iCh]->GetYaxis()->SetTitle("charge");
    h2_charge_amp[iCh]->Draw("colz");
  */

  }

  TFile outF("drawBranches.root", "recreate");
  outF.cd();
  for(int iCh=0; iCh<5; ++iCh) {
    //    p_charge_Amp[iCh]->Write();
//     p_Tcf_amp[iCh]->Write();
//     p_Tcf_Tot[iCh]->Write();

    p_Tot_amp[iCh]->Write();
    p_amp_Tot[iCh]->Write();
    p_charge_Amp[iCh]->Write();

    h2_Tot_amp[iCh]->Write();
    h2_amp_Tot[iCh]->Write();
    h2_charge_amp[iCh]->Write();

    h_amp[iCh]->Write();

  }
  outF.Close();

  //  return;

}
