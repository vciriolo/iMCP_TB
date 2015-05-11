/*****************************************************************************************
    This program draws efficiency plots

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
    //"HVScan";
  //    std::string plot_type = "AngScanHigh";
  //  std::string plot_type = "scanX0_HVLow_50";
  //  std::string plot_type = "scanX0_HVLow_20";
  std::cout<<plot_type<<std::endl;

  TFile* inF_GaAsEm_ON;
  TFile* inF_GaAsEm_OFF;
  TFile* inF_MultiAlkEm;
  TFile* inF_Double9090_ON;
  TFile* inF_Double9090_OFF;
  TFile* inF_Double9040_ON;
  TFile* inF_Double9040_OFF;

  TFile* inF_GaAsEm;
  TFile* inF_Double9090;
  TFile* inF_Double9040;

  TMultiGraph *mg = new TMultiGraph();

  TLegend *legC;

  if(plot_type == "HVScan"){
    legC = new TLegend(0.12,0.62,0.40,0.87,NULL,"brNDC");

    inF_GaAsEm_ON = TFile::Open("plots/efficiency_studies/GaAsEm_HV_HVScan2.root");
    inF_GaAsEm_OFF = TFile::Open("plots/efficiency_studies/GaAsEm_HV_HVScan7.root");
    inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_HV_HVScan5.root");
    inF_Double9090_ON = TFile::Open("plots/efficiency_studies/Double9090_HV_HVScan1.root");
    inF_Double9090_OFF = TFile::Open("plots/efficiency_studies/Double9090_HV_HVScan8.root");
    inF_Double9040_ON = TFile::Open("plots/efficiency_studies/Double9040_HV_HVScan2.root");
    inF_Double9040_OFF = TFile::Open("plots/efficiency_studies/Double9040_HV_HVScan6.root");

    TGraphErrors* eff_GaAsEm_ON = (TGraphErrors*)inF_GaAsEm_ON->Get("eff");
    TGraphErrors* eff_GaAsEm_OFF = (TGraphErrors*)inF_GaAsEm_OFF->Get("eff");
    TGraphErrors* eff_MultiAlkEm = (TGraphErrors*)inF_MultiAlkEm->Get("eff");
    TGraphErrors* eff_Double9090_ON = (TGraphErrors*)inF_Double9090_ON->Get("eff");
    TGraphErrors* eff_Double9090_OFF = (TGraphErrors*)inF_Double9090_OFF->Get("eff");
    TGraphErrors* eff_Double9040_ON = (TGraphErrors*)inF_Double9040_ON->Get("eff");
    TGraphErrors* eff_Double9040_OFF = (TGraphErrors*)inF_Double9040_OFF->Get("eff");

    //settings
    eff_GaAsEm_ON->SetMarkerColor(kGreen+1);
    eff_GaAsEm_ON->SetLineColor(kGreen+1);
    eff_GaAsEm_OFF->SetMarkerColor(kGreen+1);
    eff_GaAsEm_OFF->SetLineColor(kGreen+1);
    eff_Double9040_ON->SetMarkerColor(kBlue);
    eff_Double9040_ON->SetLineColor(kBlue);
    eff_Double9090_ON->SetMarkerColor(kRed);
    eff_Double9090_ON->SetLineColor(kRed);
    eff_Double9040_OFF->SetMarkerColor(kBlue);
    eff_Double9040_OFF->SetLineColor(kBlue);
    eff_Double9090_OFF->SetMarkerColor(kRed);
    eff_Double9090_OFF->SetLineColor(kRed);
    eff_MultiAlkEm->SetMarkerColor(1);
    eff_MultiAlkEm->SetLineColor(1);
  //
    eff_GaAsEm_ON->SetMarkerStyle(20);
    eff_GaAsEm_ON->SetLineWidth(2);
    eff_GaAsEm_ON->SetMarkerSize(0.9);
    eff_GaAsEm_OFF->SetMarkerStyle(22);
    eff_GaAsEm_OFF->SetLineWidth(2);
    eff_Double9040_ON->SetMarkerStyle(20);
    eff_Double9040_ON->SetLineWidth(2);
    eff_Double9040_ON->SetMarkerSize(0.9);
    eff_Double9090_ON->SetMarkerStyle(20);
    eff_Double9090_ON->SetLineWidth(2);
    eff_Double9090_ON->SetMarkerSize(0.9);
    eff_Double9040_OFF->SetMarkerStyle(22);
    eff_Double9040_OFF->SetLineWidth(2);
    eff_Double9090_OFF->SetMarkerStyle(22);
    eff_Double9090_OFF->SetLineWidth(2);
    eff_MultiAlkEm->SetMarkerStyle(22);
    eff_MultiAlkEm->SetLineWidth(2);


    legC->SetTextFont(42);
    legC->SetTextSize(0.034);
    legC->SetFillColor(kWhite);
    legC->SetLineColor(kWhite);
    legC->SetShadowColor(kWhite);

    legC->AddEntry(eff_GaAsEm_ON, "GaAs emitt. - PMT-MCP mode", "p");
    legC->AddEntry(eff_GaAsEm_OFF, "GaAs emitt. - iMCP mode", "p");
    legC->AddEntry(eff_Double9040_ON, "Double9040 - PMT-MCP mode", "p");
    legC->AddEntry(eff_Double9040_OFF, "Double9040 - iMCP mode", "p");
    legC->AddEntry(eff_Double9090_ON, "Double9090 PMT-MCP mode", "p");
    legC->AddEntry(eff_Double9090_OFF, "Double9090 - iMCP mode", "p");
    legC->AddEntry(eff_MultiAlkEm, "MultiAlk. Emitt. - iMCP mode", "p");
  
    mg->Add(eff_GaAsEm_ON);
    mg->Add(eff_GaAsEm_OFF);
    mg->Add(eff_Double9090_ON);
    mg->Add(eff_Double9090_OFF);
    mg->Add(eff_Double9040_OFF);
    mg->Add(eff_Double9040_ON);
    mg->Add(eff_Double9040_OFF);
    mg->Add(eff_MultiAlkEm);

    //    TCanvas* c = new TCanvas("cEff","cEff",400,800);
    TCanvas* c = new TCanvas();
    gPad->SetTicks();
    char plot_name[100];
    std::string command = "if [ ! -e final_plots/ ] ; then mkdir final_plots ; fi";
    system(command.c_str());
    sprintf(plot_name, "final_plots/efficiency_%s.pdf", plot_type.c_str());

    mg->Draw("APL");
    mg->SetTitle("Electron Beam 50 GeV");
    mg->GetXaxis()->SetRangeUser(1200,3400);
    mg->GetXaxis()->SetTitle("Bias Voltage (V)");
    mg->GetXaxis()->SetTitleSize(0.05);
    mg->GetYaxis()->SetTitle("Efficiency");
    mg->GetYaxis()->SetTitleSize(0.05);
    mg->SetMaximum(1);
    mg->SetMinimum(0);
    mg->Draw("APL");  
    legC->Draw("same");
    banner4Plot();
    c->Update();

    c->Print(plot_name, "pdf");
    sprintf(plot_name, "final_plots/efficiency_%s.png", plot_type.c_str());
    c->Print(plot_name, "png");

    sprintf(plot_name, "final_plots/efficiency_%s.root", plot_type.c_str());
    c->SaveAs(plot_name, "root");
    sprintf(plot_name, "final_plots/efficiency_%s.C", plot_type.c_str());
    c->SaveAs(plot_name, "C");
  }

  else {

    legC = new TLegend(0.4,0.15,0.65,0.42,NULL,"brNDC");

    TGraphErrors* eff_GaAsEm;
    TGraphErrors* eff_MultiAlkEm;
    TGraphErrors* eff_Double9090;
    TGraphErrors* eff_Double9040;

    if(plot_type == "AngScanHigh"){
      inF_GaAsEm = TFile::Open("plots/efficiency_studies/GaAsEm_X0_AngScan_HVHigh.root");
      inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_X0_AngScan_HVHigh.root");
      inF_Double9090 = TFile::Open("plots/efficiency_studies/Double9090_X0_AngScan_HVHigh.root");
    }
    if(plot_type == "AngScanLow"){
      inF_GaAsEm = TFile::Open("plots/efficiency_studies/GaAsEm_X0_AngScan_HVLow.root");
      inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_X0_AngScan_HVLow.root");
      inF_Double9090 = TFile::Open("plots/efficiency_studies/Double9090_X0_AngScan_HVLow.root");
    }
    
    if(plot_type == "scanX0_HVHigh_50"){
      inF_GaAsEm = TFile::Open("plots/efficiency_studies/GaAsEm_X0_scanX0_HVHigh50GeV.root");
      inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_X0_scanX0_HVHigh50GeV.root");
      inF_Double9040 = TFile::Open("plots/efficiency_studies/Double9040_X0_scanX0_HVHigh50GeV.root");
      inF_Double9090 = TFile::Open("plots/efficiency_studies/Double9090_X0_scanX0_HVHigh50GeV.root");
    }
    if(plot_type == "scanX0_HVHigh_20"){
      inF_GaAsEm = TFile::Open("plots/efficiency_studies/GaAsEm_X0_scanX0_HVHigh20GeV.root");
      inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_X0_scanX0_HVHigh20GeV.root");
      inF_Double9040 = TFile::Open("plots/efficiency_studies/Double9040_X0_scanX0_HVHigh20GeV.root");
      inF_Double9090 = TFile::Open("plots/efficiency_studies/Double9090_X0_scanX0_HVHigh20GeV.root");
    }
    if(plot_type == "scanX0_HVLow_50"){
      inF_GaAsEm = TFile::Open("plots/efficiency_studies/GaAsEm_X0_scanX0_HVLow50GeV.root");
      inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_X0_scanX0_HVLow50GeV.root");
      inF_Double9040 = TFile::Open("plots/efficiency_studies/Double9040_X0_scanX0_HVLow50GeV.root");
      inF_Double9090 = TFile::Open("plots/efficiency_studies/Double9090_X0_scanX0_HVLow50GeV.root");
    }
    if(plot_type == "scanX0_HVLow_20"){
      inF_GaAsEm = TFile::Open("plots/efficiency_studies/GaAsEm_X0_scanX0_HVLow20GeV.root");
      inF_MultiAlkEm = TFile::Open("plots/efficiency_studies/MultiAlkEm_X0_scanX0_HVLow20GeV.root");
      inF_Double9040 = TFile::Open("plots/efficiency_studies/Double9040_X0_scanX0_HVLow20GeV.root");
      inF_Double9090 = TFile::Open("plots/efficiency_studies/Double9090_X0_scanX0_HVLow20GeV.root");
    }
    
    
    eff_GaAsEm = (TGraphErrors*)inF_GaAsEm->Get("eff");
    eff_MultiAlkEm = (TGraphErrors*)inF_MultiAlkEm->Get("eff");
    eff_Double9090 = (TGraphErrors*)inF_Double9090->Get("eff");
    if (plot_type=="scanX0_HVLow_20" || plot_type=="scanX0_HVHigh_20" || plot_type=="scanX0_HVHigh_50" || plot_type=="scanX0_HVLow_50") 
      eff_Double9040 = (TGraphErrors*)inF_Double9040->Get("eff");
    
    //settings
    eff_GaAsEm->SetMarkerColor(kGreen+1);
    eff_GaAsEm->SetLineColor(kGreen+1);
    if (plot_type=="scanX0_HVLow_20" || plot_type=="scanX0_HVHigh_20" || plot_type=="scanX0_HVHigh_50" || plot_type=="scanX0_HVLow_50") {
      eff_Double9040->SetMarkerColor(kBlue);
      eff_Double9040->SetLineColor(kBlue);
    }
    eff_Double9090->SetMarkerColor(kRed);
    eff_Double9090->SetLineColor(kRed);
    eff_MultiAlkEm->SetMarkerColor(1);
    eff_MultiAlkEm->SetLineColor(1);
    //
    eff_GaAsEm->SetMarkerStyle(20);
    eff_GaAsEm->SetLineWidth(2);
    eff_GaAsEm->SetMarkerSize(0.9);
    if (plot_type=="scanX0_HVLow_20" || plot_type=="scanX0_HVHigh_20" || plot_type=="scanX0_HVHigh_50" || plot_type=="scanX0_HVLow_50") {
      eff_Double9040->SetMarkerStyle(20);
      eff_Double9040->SetLineWidth(2);
      eff_Double9040->SetMarkerSize(0.9);
    }
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
        
    if(plot_type == "AngScanLow"){
      legC->AddEntry(eff_GaAsEm, "GaAs emitt. - iMCP mode - 3000 V", "p");
      legC->AddEntry(eff_Double9090, "Double9090 - iMCP mode - 3000 V", "p");
      legC->AddEntry(eff_MultiAlkEm, "MultiAlk. Emitt. - iMCP mode - 1900 V", "p");
    }
    if(plot_type == "AngScanHigh"){
      legC->AddEntry(eff_GaAsEm, "GaAs emitt. - iMCP mode - 3200 V", "p");
      legC->AddEntry(eff_Double9090, "Double9090 - iMCP mode - 3200 V", "p");
      legC->AddEntry(eff_MultiAlkEm, "MultiAlk. Emitt. - iMCP mode - 2200 V", "p");
    }

    if(plot_type == "scanX0_HVHigh_50" || plot_type == "scanX0_HVHigh_20"){
      legC->AddEntry(eff_GaAsEm, "GaAs emitt. - iMCP mode - 3200 V", "p");
      legC->AddEntry(eff_Double9090, "Double9090 - iMCP mode - 3200 V", "p");
      legC->AddEntry(eff_Double9040, "Double9040 - iMCP mode - 3200 V", "p");
      legC->AddEntry(eff_MultiAlkEm, "MultiAlk. Emitt. - iMCP mode - 2200 V", "p");
    }

    if(plot_type == "scanX0_HVLow_50" || plot_type == "scanX0_HVLow_20"){
      legC->AddEntry(eff_GaAsEm, "GaAs emitt. - iMCP mode - 3000 V", "p");
      legC->AddEntry(eff_Double9090, "Double9090 - iMCP mode - 3000 V", "p");
      legC->AddEntry(eff_Double9040, "Double9040 - iMCP mode - 3000 V", "p");
      legC->AddEntry(eff_MultiAlkEm, "MultiAlk. Emitt. - iMCP mode - 1900 V", "p");
    }

    mg->Add(eff_GaAsEm);
    mg->Add(eff_Double9090);
    mg->Add(eff_MultiAlkEm);
    if (plot_type=="scanX0_HVLow_20" || plot_type=="scanX0_HVHigh_20" || plot_type=="scanX0_HVHigh_50" || plot_type=="scanX0_HVLow_50") 
      mg->Add(eff_Double9040);

    TCanvas* c = new TCanvas();
    gPad->SetTicks();
    char plot_name[100];
    std::string command = "if [ ! -e final_plots/ ] ; then mkdir final_plots ; fi";
    system(command.c_str());
    sprintf(plot_name, "final_plots/efficiency_%s.pdf", plot_type.c_str());

    mg->Draw("APL");
    if (plot_type=="scanX0_HVLow_20" || plot_type=="scanX0_HVHigh_20" || plot_type=="scanX0_HVHigh_50" || plot_type=="scanX0_HVLow_50") 
      {
	if (plot_type=="scanX0_HVLow_20" || plot_type=="scanX0_HVHigh_20") {
	  mg->SetTitle("Electron Beam 20 GeV");
	  mg->GetXaxis()->SetRangeUser(-0.1,5.1);
	}
	else {
	  mg->SetTitle("Electron Beam 50 GeV");
	  mg->GetXaxis()->SetRangeUser(-0.1,5.1);
	}
	mg->GetXaxis()->SetTitle("Number of X_{0}");
	mg->GetXaxis()->SetTitleSize(0.046);
	mg->GetYaxis()->SetTitle("Efficiency");
	mg->GetYaxis()->SetTitleSize(0.046);
	//	mg->SetTitle(plot_type.c_str());
	mg->SetMaximum(1);
	mg->SetMinimum(0);
      }
    else
      {
	mg->SetTitle("Electron Beam 50 GeV");
	mg->GetXaxis()->SetRangeUser(-1., 46.);
	mg->GetXaxis()->SetTitle("Angle (degrees)");
	mg->GetXaxis()->SetTitleSize(0.045);
	mg->GetYaxis()->SetTitle("Efficiency");
	mg->GetYaxis()->SetTitleSize(0.045);
	mg->GetYaxis()->SetTitleOffset(1.12); //damned root
	//	mg->SetTitle(plot_type.c_str());
	mg->SetMaximum(0.8);
	mg->SetMinimum(0.4);
	c->SetCanvasSize(500,400);
      }

    mg->Draw("APL");  
    legC->Draw("same");
    banner4Plot();
    c->Update();

    c->Print(plot_name, "pdf");
    sprintf(plot_name, "final_plots/efficiency_%s.png", plot_type.c_str());
    c->Print(plot_name, "png");

    sprintf(plot_name, "final_plots/efficiency_%s.root", plot_type.c_str());
    c->Print(plot_name, "root");
    sprintf(plot_name, "final_plots/efficiency_%s.C", plot_type.c_str());
    c->SaveAs(plot_name, "C");
  }


  return 0;
}

