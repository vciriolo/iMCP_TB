#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <vector>
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
#include "../src/analysis_tools.cc"


void computeTimeOT(int nToys, std::string inFile)
{
  srand (time (NULL));

  TFile *_file0 = TFile::Open(inFile.c_str(), "read");

  TProfile**  wf_promed = new TProfile*[5];
  wf_promed[0] = (TProfile*)_file0->Get("wf_promed_4");
  wf_promed[1] = (TProfile*)_file0->Get("wf_promed_5");
  wf_promed[2] = (TProfile*)_file0->Get("wf_promed_6");
  wf_promed[3] = (TProfile*)_file0->Get("wf_promed_7");
  wf_promed[4] = (TProfile*)_file0->Get("wf_promed_9");

  wf_promed[0]->SetDirectory(0);
  wf_promed[1]->SetDirectory(0);
  wf_promed[2]->SetDirectory(0);
  wf_promed[3]->SetDirectory(0);
  wf_promed[4]->SetDirectory(0);
  _file0->Delete();

  //  TH2F** correlation_TvA = new TH2F*[5];
  //  TH2F** correlation_AvT = new TH2F*[5];
  TProfile** correlationTvA = new TProfile*[5];
  TProfile** correlationAvT = new TProfile*[5];
  for(int iCh=0; iCh<5; ++iCh) {
    //    correlation_TvA[iCh] = new TH2F(Form("correlation_TvA_%d",iCh), "", 10000, 0., 10000., 10000., 0., 1000.);
    //    correlation_AvT[iCh] = new TH2F(Form("correlation_AvT_%d",iCh), "", 10000, 0., 1000., 10000., 0., 10000.);
    correlationTvA[iCh] = new TProfile(Form("correlationTvA_%d",iCh), "", 10000, 0., 10000.);
    correlationAvT[iCh] = new TProfile(Form("correlationAvT_%d",iCh), "", 10000, 0., 1000.);
  }

  std::vector<float> digiCh[5];
  float ampMax[5];
  float sampleMax[5];

  //  std::cout << " >>> nToys = " << nToys << std::endl;
    
  for(int iToy=0.; iToy<nToys; ++iToy){
    //for(int iToy=0; iToy<1; ++iToy){
    if(iToy % 100 == 0)     std::cout << " >>> iToy = " << iToy << std::endl;

    for(int iCh=0; iCh<5; ++iCh){
      digiCh[iCh].clear();
      ampMax[iCh] = 0.;
      sampleMax[iCh] = 0.;
    

      float randN = int( 10000. * rand() / RAND_MAX);
      //std::cout << " randN = " << randN << std::endl;    
      
      for(int iSample=0; iSample<wf_promed[0]->GetNbinsX(); ++iSample){
	float value = wf_promed[iCh]->GetBinContent(iSample+1);
	// std::cout << " wf_promed[iCh]->GetBinContent(iSample+1) = " << wf_promed[iCh]->GetBinContent(iSample+1) << std::endl;    
	digiCh[iCh].push_back(value*randN);
	if(value < ampMax[iCh]) ampMax[iCh] = value;
      }
    
      float tOT = TimeOverThreshold(20, 800, &(digiCh[iCh]), -1000.);
      //      float tOT = 1;
      //      correlation_TvA[iCh]->Fill(-1. * ampMax[iCh] * randN, tOT);
      //      correlation_AvT[iCh]->Fill(tOT, -1. * ampMax[iCh] * randN);
      correlationTvA[iCh]->Fill(-1. * ampMax[iCh] * randN, tOT);
      correlationAvT[iCh]->Fill(tOT, -1. * ampMax[iCh] * randN);
    }
  }

  ////  TF1 pippo("pippo", "15 - exp(1 - 0.0005 * (x-2000))", 0, 10000);
  //  TF1* trend = new TF1("trend", "[0] - exp([1] - [2] * (x-[3]))", 2000, 10000);
  ////  TF1* trend = new TF1("trend", "log(x-[0])", 1000, 10000);
  ////  trend->SetParameters(15, 1, 0.0005, 2000);
  //trend->SetParameters(20, 0.1, 0.005, 2000);

  /*
  TCanvas* c1 = new TCanvas();
  c1->cd();
  correlation[0]->Draw("colz");
  correlation[0]->Fit("trend", "R");
  */

  TFile outF("correlation_timeOT_ampM.root", "recreate");
  outF.cd();
  for(int iCh=0; iCh<5; ++iCh) {
    //    correlation_TvA[iCh]->Write();
    //    correlation_AvT[iCh]->Write();
    correlationTvA[iCh]->Write();
    correlationAvT[iCh]->Write();
  }
  outF.Close();
  
  //  return;
}
