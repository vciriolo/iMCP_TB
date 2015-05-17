/*****************************************************************************************
    This program draws time resolution plots

****************************************************************************************/
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
#include "TH2F.h"
#include "TProfile.h"
#include "TF1.h"
#include "TString.h"
#include "TCut.h"
#include "TMath.h"
#include "TApplication.h"
#include "TError.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPad.h"
#include "TStyle.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TPaveLabel.h"
#include "TPaveText.h"

using namespace std;

void banner4Plot (){
  TPaveText* pt ;
  pt = new TPaveText(.14,0.91,.25,.94,"NDC");
  pt->AddText("Preliminary");
  pt->SetFillColor(0);
  pt->SetTextSize(0.035);
  pt->SetFillStyle(0);
  pt->SetLineColor(0);
  pt->SetLineWidth(0);
  pt->SetMargin(0);
  pt->SetShadowColor(0);
  pt->Draw();
}

int main(int argc, char** argv)
{

  std::string plot_type = argv[1];

  std::cout<<plot_type<<std::endl;

  TFile* inF_GaAsEm;
  //TFile* inF_GaAsEm_OFF;
  TFile* inF_MultiAlkEm;
  //TFile* inF_MultiAlkEm_OFF;
  TFile* inF_Double9090;
  TFile* inF_Double9040;

  TFile* inF_MiB3;
  TFile* inF_ZStack1;
  TFile* inF_ZStack2;

  TMultiGraph *mg = new TMultiGraph();

  TLegend *legC;

  if(plot_type == "HV12"){
    legC = new TLegend(0.15,0.60,0.35,0.85,NULL,"brNDC");

    inF_MultiAlkEm = TFile::Open("plots/resCFD_studies/MultiAlkEm_HV12_HVScan12_MultiAlkEm.root");
    inF_GaAsEm = TFile::Open("plots/resCFD_studies/GaAsEm_HV12_HVScan12_GaAsEm.root");
    //inF_GaAsEm_OFF = TFile::Open("plots/efficiency_studies/GaAsEm_HV_HVScan7.root");
    inF_Double9090 = TFile::Open("plots/resCFD_studies/Double9090_HV12_HVScan12_Double9090.root");
    inF_Double9040 = TFile::Open("plots/resCFD_studies/Double9040_HV12_HVScan12_Double9040.root");    
    TGraphErrors* eff_GaAsEm = (TGraphErrors*)inF_GaAsEm->Get("resCFD");
    TGraphErrors* eff_MultiAlkEm = (TGraphErrors*)inF_MultiAlkEm->Get("resCFD");
    TGraphErrors* eff_Double9040 = (TGraphErrors*)inF_Double9040->Get("resCFD");
    TGraphErrors* eff_Double9090 = (TGraphErrors*)inF_Double9090->Get("resCFD");

    //settings
    eff_GaAsEm->SetMarkerColor(kGreen+1);
    eff_GaAsEm->SetLineColor(kGreen+1);
    //eff_GaAsEm_OFF->SetMarkerColor(kGreen+1);
    //eff_GaAsEm_OFF->SetLineColor(kGreen+1);
    eff_Double9040->SetMarkerColor(kBlue);
    eff_Double9040->SetLineColor(kBlue);
    eff_Double9090->SetMarkerColor(kRed);
    eff_Double9090->SetLineColor(kRed);
    eff_MultiAlkEm->SetMarkerColor(1);
    eff_MultiAlkEm->SetLineColor(1);
  //
    eff_GaAsEm->SetMarkerStyle(20);
    eff_GaAsEm->SetLineWidth(2);
    eff_GaAsEm->SetMarkerSize(0.9);
//    eff_GaAsEm_OFF->SetMarkerStyle(22);
//    eff_GaAsEm_OFF->SetLineWidth(2);
    eff_Double9040->SetMarkerStyle(20);
    eff_Double9040->SetLineWidth(2);
    eff_Double9040->SetMarkerSize(0.9);
    eff_Double9090->SetMarkerStyle(20);
    eff_Double9090->SetLineWidth(2);
    eff_Double9090->SetMarkerSize(0.9);
    eff_MultiAlkEm->SetMarkerStyle(22);
    eff_MultiAlkEm->SetLineWidth(2);

    legC->SetTextFont(42);
    legC->SetTextSize(0.034);
    legC->SetFillColor(kWhite);
    legC->SetLineColor(kWhite);
    legC->SetShadowColor(kWhite);
    legC->AddEntry(eff_MultiAlkEm, "MultiAlk. emitt. ON: #DeltaV_{12} = 300 V", "p");
    legC->AddEntry(eff_GaAsEm, "GaAs emitt. ON: #DeltaV_{12} = 300 V", "p");
//    legC->AddEntry(eff_GaAsEm_OFF, "GaAs emitt. - iMCP mode", "p");
    legC->AddEntry(eff_Double9040, "Double9040 HV_{1} = HV_{2}", "p");
    legC->AddEntry(eff_Double9090, "Double9090 HV_{1} = HV_{2}", "p");
  
    mg->Add(eff_GaAsEm);
//    mg->Add(eff_GaAsEm_OFF);
    mg->Add(eff_Double9090);
    mg->Add(eff_Double9040);
    mg->Add(eff_MultiAlkEm);

    TCanvas* c = new TCanvas();
    gPad->SetTicks();
    char plot_name[100];
    std::string command = "if [ ! -e final_plots/ ] ; then mkdir final_plots ; fi";
    system(command.c_str());
    sprintf(plot_name, "final_plots/timeResCFD_%s.pdf", plot_type.c_str());

    mg->Draw("AP");
    mg->SetTitle("Electron Beam 450 MeV");
    mg->GetXaxis()->SetRangeUser(1400,4000);
    mg->GetXaxis()->SetTitle("HV_{1} (V)");
    mg->GetXaxis()->SetTitleSize(0.048);
    mg->GetYaxis()->SetTitle("Spread on the time difference (ps)");
    mg->GetYaxis()->SetTitleSize(0.048);
    mg->SetMaximum(160);
    mg->SetMinimum(0);
    mg->Draw("AP");  
    legC->Draw("same");
    banner4Plot();
    c->Update();

    c->Print(plot_name, "pdf");
    sprintf(plot_name, "final_plots/timeResCFD_%s.png", plot_type.c_str());
    c->Print(plot_name, "png");

    sprintf(plot_name, "final_plots/timeResCFD_%s.root", plot_type.c_str());
    c->SaveAs(plot_name, "root");
    sprintf(plot_name, "final_plots/timeResCFD_%s.C", plot_type.c_str());
    c->SaveAs(plot_name, "C");
  }

  if(plot_type == "HV1"){
    legC = new TLegend(0.15,0.65,0.35,0.85,NULL,"brNDC");

    inF_GaAsEm = TFile::Open("plots/resCFD_studies/GaAsEm_HV1_HVScan1_GaAsEm.root");
    //inF_GaAsEm_OFF = TFile::Open("plots/efficiency_studies/GaAsEm_HV_HVScan7.root");
    inF_MultiAlkEm = TFile::Open("plots/resCFD_studies/MultiAlkEm_HV1_HVScan1_MultiAlkEm.root");
    inF_Double9090 = TFile::Open("plots/resCFD_studies/Double9090_HV1_HVScan1_Double9090.root");
    inF_Double9040 = TFile::Open("plots/resCFD_studies/Double9040_HV1_HVScan1_Double9040.root");    
    TGraphErrors* eff_GaAsEm = (TGraphErrors*)inF_GaAsEm->Get("eff");
    TGraphErrors* eff_Double9090 = (TGraphErrors*)inF_Double9090->Get("eff");
    TGraphErrors* eff_MultiAlkEm = (TGraphErrors*)inF_MultiAlkEm->Get("eff");
    TGraphErrors* eff_Double9040 = (TGraphErrors*)inF_Double9040->Get("eff");

    //settings
    eff_GaAsEm->SetMarkerColor(kGreen+1);
    eff_GaAsEm->SetLineColor(kGreen+1);
    //eff_GaAsEm_OFF->SetMarkerColor(kGreen+1);
    //eff_GaAsEm_OFF->SetLineColor(kGreen+1);
    eff_Double9040->SetMarkerColor(kBlue);
    eff_Double9040->SetLineColor(kBlue);
    eff_Double9090->SetMarkerColor(kRed);
    eff_Double9090->SetLineColor(kRed);
    eff_MultiAlkEm->SetMarkerColor(1);
    eff_MultiAlkEm->SetLineColor(1);
  //
    eff_GaAsEm->SetMarkerStyle(20);
    eff_GaAsEm->SetLineWidth(2);
    eff_GaAsEm->SetMarkerSize(0.9);
//    eff_GaAsEm_OFF->SetMarkerStyle(22);
//    eff_GaAsEm_OFF->SetLineWidth(2);
    eff_Double9040->SetMarkerStyle(20);
    eff_Double9040->SetLineWidth(2);
    eff_Double9040->SetMarkerSize(0.9);
    eff_Double9090->SetMarkerStyle(20);
    eff_Double9090->SetLineWidth(2);
    eff_Double9090->SetMarkerSize(0.9);
    eff_MultiAlkEm->SetMarkerStyle(22);
    eff_MultiAlkEm->SetLineWidth(2);

    legC->SetTextFont(42);
    legC->SetTextSize(0.034);
    legC->SetFillColor(kWhite);
    legC->SetLineColor(kWhite);
    legC->SetShadowColor(kWhite);
    legC->AddEntry(eff_MultiAlkEm, "MultiAlk emitt. HV_{2} = 2700 (V)", "p");
    legC->AddEntry(eff_GaAsEm, "GaAs emitt. HV{2} = 2700 (V)", "p");
//    legC->AddEntry(eff_GaAsEm_OFF, "GaAs emitt. - iMCP mode", "p");
    legC->AddEntry(eff_Double9040, "Double9040 HV_{2} = 2700 (V)", "p");
    legC->AddEntry(eff_Double9090, "Double9090 HV_{2} = 2700 (V)", "p");
  
    mg->Add(eff_GaAsEm);
//    mg->Add(eff_GaAsEm_OFF);
    mg->Add(eff_Double9090);
    mg->Add(eff_Double9040);
    mg->Add(eff_MultiAlkEm);

    TCanvas* c = new TCanvas();
    gPad->SetTicks();
    char plot_name[100];
    std::string command = "if [ ! -e final_plots/ ] ; then mkdir final_plots ; fi";
    system(command.c_str());
    sprintf(plot_name, "final_plots/timeResCFD_%s.pdf", plot_type.c_str());

    mg->Draw("AP");
    mg->SetTitle("Electron Beam 450 MeV");
    mg->GetXaxis()->SetRangeUser(1400,3400);
    mg->GetXaxis()->SetTitle("HV_{1} (V)");
    mg->GetXaxis()->SetTitleSize(0.048);
    mg->GetYaxis()->SetTitle("Spread on the time difference (ps)");
    mg->GetYaxis()->SetTitleSize(0.048);
    mg->SetMaximum(100);
    mg->SetMinimum(0);
    mg->Draw("AP");  
    legC->Draw("same");
    banner4Plot();
    c->Update();

    c->Print(plot_name, "pdf");
    sprintf(plot_name, "final_plots/timeResCFD_%s.png", plot_type.c_str());
    c->Print(plot_name, "png");

    sprintf(plot_name, "final_plots/timeResCFD_%s.root", plot_type.c_str());
    c->SaveAs(plot_name, "root");
    sprintf(plot_name, "final_plots/timeResCFD_%s.C", plot_type.c_str());
    c->SaveAs(plot_name, "C");
  }

  else {
    if(plot_type == "X0"){
    legC = new TLegend(0.4,0.15,0.25,0.42,NULL,"brNDC");

    inF_GaAsEm = TFile::Open("plots/resCFD_studies/GaAsEm_X0_X0Scan12_GaAsEm.root");
    //inF_GaAsEm_OFF = TFile::Open("plots/efficiency_studies/GaAsEm_HV_HVScan7.root");
    inF_MultiAlkEm = TFile::Open("plots/resCFD_studies/MultiAlkEm_X0_X0Scan12_MultiAlkEm.root");
    inF_Double9090 = TFile::Open("plots/resCFD_studies/Double9090_X0_X0Scan12_Double9090.root");
    inF_Double9040 = TFile::Open("plots/resCFD_studies/Double9040_X0_X0Scan12_Double9040.root");    

    TGraphErrors* eff_GaAsEm;
    TGraphErrors* eff_MultiAlkEm;
    TGraphErrors* eff_Double9090;
    TGraphErrors* eff_Double9040;
    
    
    eff_GaAsEm = (TGraphErrors*)inF_GaAsEm->Get("eff");
    eff_MultiAlkEm = (TGraphErrors*)inF_MultiAlkEm->Get("eff");
    eff_Double9090 = (TGraphErrors*)inF_Double9090->Get("eff");
    eff_Double9040 = (TGraphErrors*)inF_Double9040->Get("eff");
    
    //settings
    eff_GaAsEm->SetMarkerColor(kGreen+1);
    eff_GaAsEm->SetLineColor(kGreen+1);
    eff_Double9040->SetMarkerColor(kBlue);
    eff_Double9040->SetLineColor(kBlue);
    eff_Double9090->SetMarkerColor(kRed);
    eff_Double9090->SetLineColor(kRed);
    eff_MultiAlkEm->SetMarkerColor(1);
    eff_MultiAlkEm->SetLineColor(1);
    eff_GaAsEm->SetMarkerStyle(20);
    eff_GaAsEm->SetLineWidth(2);
    eff_GaAsEm->SetMarkerSize(0.9);  
    eff_Double9040->SetMarkerStyle(20);
    eff_Double9040->SetLineWidth(2);
    eff_Double9040->SetMarkerSize(0.9);
    eff_Double9090->SetMarkerStyle(20);
    eff_Double9090->SetLineWidth(2);
    eff_Double9090->SetMarkerSize(0.9);
    eff_MultiAlkEm->SetMarkerStyle(20);
    eff_MultiAlkEm->SetLineWidth(2);
    eff_MultiAlkEm->SetMarkerSize(0.9);
    
    legC->SetTextFont(42);
    legC->SetTextSize(0.037);
    legC->SetFillColor(kWhite);
    legC->SetLineColor(kWhite);
    legC->SetShadowColor(kWhite);

    mg->Add(eff_GaAsEm);
    mg->Add(eff_Double9090);
    mg->Add(eff_MultiAlkEm);
    mg->Add(eff_Double9040);


    TCanvas* c = new TCanvas();
    gPad->SetTicks();
    char plot_name[100];
    std::string command = "if [ ! -e final_plots/ ] ; then mkdir final_plots ; fi";
    system(command.c_str());
    sprintf(plot_name, "final_plots/timeResCFD_%s.pdf", plot_type.c_str());

   mg->Draw("AP");
    mg->SetTitle("Electron Beam 450 MeV");
    mg->GetXaxis()->SetRangeUser(-0.1,5.1);
    mg->GetXaxis()->SetTitle("Number of X_{0}");
    mg->GetXaxis()->SetTitleSize(0.046);
    mg->GetYaxis()->SetTitle("Efficiency");
    mg->GetYaxis()->SetTitleSize(0.046);
    mg->SetMaximum(1);
    mg->SetMinimum(0);
     
    mg->Draw("AP");  
    legC->Draw("same");
    banner4Plot();
    c->Update();

    c->Print(plot_name, "pdf");
    sprintf(plot_name, "final_plots/timeResCFD_%s.png", plot_type.c_str());
    c->Print(plot_name, "png");

    sprintf(plot_name, "final_plots/timeResCFD_%s.root", plot_type.c_str());
    c->SaveAs(plot_name, "root");
    sprintf(plot_name, "final_plots/timeResCFD_%s.C", plot_type.c_str());
    c->SaveAs(plot_name, "C");
  }
}

  return 0;
}

