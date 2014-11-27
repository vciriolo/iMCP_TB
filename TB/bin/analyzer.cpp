/*****************************************************************************************
    This program compute charge or efficiency for all the run in a selected Scan for
    selected MCP under test.

    compile with ---> c++ -o analyzer bin/analyzer.cpp `root-config --cflags --glibs --libs`
    run with:      ./analyzer cfg/treshold.txt MiB3 all eff HVScan Scan2
          where the arguments are: 1) cfg file with the tresholds 2) MCP to analyze 4) analysis type (eff,time) 5) Scan type (HV or X0) 6) suffix of reco file in input

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

#include "../interface/init_Reco_Tree.h"
#include "../interface/MCPMap.h"

#define RES_TRIG 85.3
#define RES_TRIG_ERR 0.3

using namespace std;

int main(int argc, char** argv)
{
  gSystem->Load("libTree");
  char *label, *doWhat, *scanType;
  Fill_MCPList();
  Fill_inverted_MCPList();

  //read input options
  std::ifstream inputCfg (argv[1],ios::in);
  std::string MCP = argv[2];
  doWhat = argv[3];
  scanType = argv[4];
  label = argv[5];

  std::cout<<"----START ANALYZER: analyzing MCP "<<MCP<<", mode: "<<scanType<<", scan: "<<label<<"-------"<<std::endl;
 
  int MCPNumber = MCPList.at(MCP);
  std::map <int,int> treshold;
  int ch, tresh;
  int nChannels=0;
  int trigPos1 = -1;  //positions of the two trigger chambers in the reco tree
 
  //---open cfg file and fill map with treshold for each channel----
  while(!inputCfg.eof())
      {
	inputCfg >> ch >> tresh;
	treshold.insert(std::make_pair(ch,tresh));
	nChannels++;
      }

  //open reco tree
  std::string inFileName = "ntuples/reco_"+string(label)+".root";
  TFile *inFile = new TFile (inFileName.c_str());
  TTree* nt = (TTree*)inFile->Get("reco_tree");
  InitRecoTree(nt);
  
  char outputFileName[200]="";
  sprintf(outputFileName, "results/%s_%s_%s_%s.txt", MCP.c_str(), doWhat, scanType, label);
  std::ofstream outputFile (outputFileName, std::ofstream::out);

  //------Build TCut and draw variables--------------------------------------------------
  char str_cut_sig[200]="";
  char str_cut_trig0[200]="";
  char str_cut_tdc[200]="";
  char str_cut_saturated[200]="";

  char var_sig[100]="";
  char var_dt_vs_ampM[100]="";
  char var_dtStart_vs_ampM[100] = "";
  char var_dtStart_vs_Tot[100] = "";
  char var_fracSaturated[100]="";
  char var_evtAll[100]="";

  //  char var_base[100]="";
  char var_time[100]="";
  char var_timeLED[100] = "";
  char var_trig0[100]="";

  //---open tree and get: run list and corresponding HV/X0-----
  std::vector<int> HVVal;
  HVVal.clear();
  std::vector<float> X0Step;
  X0Step.clear();
  std::vector<float> ScanList;
  ScanList.clear();


  //---save list of HV (or X0) step---
  if (strcmp(scanType,"HV")==0) {
    int prev=0;  
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++)
      {
	nt->GetEntry(iEntry);
	if (HV[MCPNumber]!=prev) {
	  ScanList.push_back((float)HV[MCPNumber]);
	  HVVal.push_back(HV[MCPNumber]);
	  X0Step.push_back(X0);
	  prev=HV[MCPNumber];
	  if (iEntry==0) {
	    for (int i=0; i<nChannels; i++)  //save trigger position!
	      {
		if (isTrigger[i]==1)       trigPos1 = i;
	      }
	  }
	}
      }
  }

  else {
    float prev=-1.;
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++)
      {
	nt->GetEntry(iEntry);
	if (X0>(prev+0.001)||X0<(prev-0.001)) {
	  ScanList.push_back(X0);
	  HVVal.push_back(HV[MCPNumber]);
	  X0Step.push_back(X0);
	  prev=X0;
	  if (iEntry==0) {
	    for (int i=0; i<nChannels; i++)  //save trigger position!!
	      {
		if (isTrigger[i]==1)       trigPos1 = i;
	      }
	  }

	}
      }
  }

  if (trigPos1==-1) {
    std::cout<<"ERROR!!! trigger not found!!!"<<std::endl;
    return -1;
  }
  else
    std::cout<<"TRIGGER INFO: --> \ntrigger 1 = "<<inverted_MCPList.at(trigPos1)<<"\n----------"<<std::endl;

  //cut strings
  sprintf(str_cut_sig, "charge_corr[%d] > %d", MCPNumber, treshold.at(MCPNumber));
  sprintf(str_cut_trig0, "charge_corr[%d] > %d", trigPos1, treshold.at(trigPos1));
  sprintf(str_cut_tdc, "tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6");
  sprintf(str_cut_saturated, "amp_max[%d] > 3450", MCPNumber);

  //-----construct TCut-----
  TCut cut_sig = str_cut_sig;
  TCut cut_trig0 = str_cut_trig0;
  TCut cut_tdc = str_cut_tdc;
  TCut cut_saturated = str_cut_saturated;

  TGraphErrors *g_eff = new TGraphErrors(ScanList.size());
  g_eff->SetName("eff");

  TGraphErrors *g_resoLED = new TGraphErrors(ScanList.size());
  g_resoLED->SetName("resoLED");

  TGraphErrors* g_frac_saturated = new TGraphErrors(ScanList.size());
  g_frac_saturated->SetName("frac_saturated");

  //  TH2F* ampM_vs_dt = new TH2F("ampM_vs_dt", "", 1000, -5., 5., 10000, 0., 10000.);

  //-------Runs loop---------------------------------------------------------------------
  for(unsigned int i=0; i<ScanList.size(); i++)
    {
      //define histos
      char h_sig_name[20], h_trig0_name[20], h_time_name[20], h_timeLED_name[20];
      char h_dt_vs_ampM_name[20], h_dtStart_vs_ampM_name[20], h_dtStart_vs_Tot_name[20];
      char h_fracSat_name[20], h_evtAll_name[20];
      char res_func_name[20], res_func_name_2sig[20], resLED_func_name[20];

      sprintf(h_sig_name, "h_sig_%d", i);
      sprintf(h_trig0_name, "h_trig0_%d", i);
      sprintf(h_time_name, "h_time_%d", i);
      sprintf(h_timeLED_name, "h_timeLED_%d", i);
      sprintf(h_dt_vs_ampM_name, "h_dt_vs_ampM_%d", i);
      sprintf(h_dtStart_vs_ampM_name, "h_dtStart_vs_ampM_%d", i);
      sprintf(h_dtStart_vs_Tot_name, "h_dtStart_vs_Tot_%d", i);

      sprintf(h_fracSat_name, "h_fracSat_name_%d", i);
      sprintf(h_evtAll_name, "h_evtAll_name_%d", i);

      sprintf(res_func_name, "res_func_%d", i);
      sprintf(resLED_func_name, "resLED_func_%d", i);
      sprintf(res_func_name_2sig, "res_func_2sig_%d", i);

      //
      TH1F* h_sig= new TH1F(h_sig_name, h_sig_name, 500, -5000, 25000);
      TH1F* h_trig0 = new TH1F(h_trig0_name, h_trig0_name, 500, -5000, 25000);
      TH1F* h_time = new TH1F(h_time_name, h_time_name, 1000, -2, 1);
      if (MCPList.at(MCP)==4) h_time = new TH1F(h_time_name, h_time_name, 1000, -5, -2);
      TH1F* h_timeLED = new TH1F(h_timeLED_name, h_timeLED_name, 1250, -2., 1.);
      if (MCPList.at(MCP)==4) h_timeLED = new TH1F(h_timeLED_name, h_timeLED_name, 1250, -2., 1.);

      TH2F* h_dt_vs_ampM = new TH2F(h_dt_vs_ampM_name, h_dt_vs_ampM_name, 10000, 0., 10000., 1000, -5., 5.);
      TH2F* h_dtStart_vs_ampM = new TH2F(h_dtStart_vs_ampM_name, h_dtStart_vs_ampM_name, 10000, 0., 10000., 100, -10., 10.);
      TH2F* h_dtStart_vs_Tot = new TH2F(h_dtStart_vs_Tot_name, h_dtStart_vs_Tot_name, 1000, 0., 50., 100, -10., 10.);

      TH1F* h_fracSat = new TH1F(h_fracSat_name, h_fracSat_name, 100, 0., 100.);
      TH1F* h_evtAll = new TH1F(h_evtAll_name, h_evtAll_name, 100, 0., 100.);
      TF1* res_func = new TF1(res_func_name, "gausn", -10, 10);
      TF1* res_func_2sig = new TF1(res_func_name_2sig, "gausn", -10, 10);
      TF1* resLED_func = new TF1(resLED_func_name, "gausn", -10, 10);

      //-----Draw variables-----
      sprintf(var_sig, "charge_corr[%d]>>%s", MCPNumber, h_sig_name);
      sprintf(var_time, "(time_CF_corr[%d]-time_CF_corr[%d])>>%s", MCPNumber, trigPos1, h_time_name);
      sprintf(var_timeLED, "((time_start[%d]-time_start[%d])+0.5*(time_OT[%d]-time_OT[%d]))>>%s",MCPNumber,trigPos1,MCPNumber,trigPos1,h_timeLED_name);
      sprintf(var_trig0, "charge_corr[%d]>>%s", trigPos1, h_trig0_name);

      sprintf(var_dt_vs_ampM, "(time_CF_corr[%d]-time_CF_corr[%d]):amp_max_corr[%d]>>%s", MCPNumber, trigPos1, MCPNumber, h_dt_vs_ampM_name);
      sprintf(var_dtStart_vs_ampM, "(time_start[%d]-time_start[%d]):amp_max_corr[%d]>>%s", MCPNumber, trigPos1, MCPNumber, h_dtStart_vs_ampM_name);
      sprintf(var_dtStart_vs_Tot, "(time_start[%d]-time_start[%d]):time_OT[%d]>>%s", MCPNumber, trigPos1, MCPNumber, h_dtStart_vs_Tot_name);

      sprintf(var_fracSaturated, "time_CF[%d] >> %s", MCPNumber, h_fracSat_name); 
      sprintf(var_evtAll, "time_CF[%d] >> %s", MCPNumber, h_evtAll_name); 

      char cut_scan[100];
      if (strcmp(scanType,"HV")==0)  sprintf(cut_scan, "HV[%d] == %d", MCPNumber, HVVal.at(i));
      else  sprintf(cut_scan, "X0 > %f && X0 < %f", X0Step.at(i)-0.0001, X0Step.at(i)+0.0001); //"X0==%f" does not work, don't know why 

      if(MCPNumber == 2) sprintf(str_cut_saturated, "run_id > 796 && amp_max[%d] > 4000", MCPNumber);

      //-----Draw and print infos-----
      nt->Draw(var_sig, cut_trig0 && cut_sig && cut_scan && cut_tdc, "goff");
      nt->Draw(var_trig0,cut_trig0 && cut_scan && cut_tdc, "goff");

      //      int acc = 1*(float(h_base->GetEntries())/2000.); //estimated from pedestal run
      //            std::cout<<h_sig->GetEntries()<<" "<<h_trig0->GetEntries()<<" "<<h_base->GetEntries()<<std::endl;

      float eff = h_sig->GetEntries()/h_trig0->GetEntries();
      float e_eff = TMath::Sqrt((TMath::Abs(eff*(1-eff)))/h_trig0->GetEntries());
      
      //      float eff = (h_sig->GetEntries()-h_base->GetEntries()*h_trig1->GetEntries()/h_trig0->GetEntries())/h_trig1->GetEntries();
      //     float e_eff = TMath::Sqrt((TMath::Abs(eff*(1-eff)))/h_trig1->GetEntries());

      if(eff < 0)   eff = 0;
      char var_name[3] = "X0";
      if(TString(scanType).Contains("HV") == 1)    sprintf(var_name, "HV");
      //---Eff study
      if(strcmp(doWhat,"eff") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\teff\te_%s\te_eff\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    if(TString(scanType).Contains("HV") == 1) {
	      printf("%d\t%.3f\t%.3f\t%.3f\n", HVVal.at(i), eff, 0., e_eff);
	      outputFile << HVVal.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
	      g_eff->SetPoint(i, HVVal.at(i), eff);
	      g_eff->SetPointError(i, 0, e_eff);
	    }
	    else {
	      printf("%.3f\t%.3f\t%.3f\t%.3f\n", X0Step.at(i), eff, 0., e_eff);
	      outputFile << X0Step.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
	      g_eff->SetPoint(i, X0Step.at(i), eff);
	      g_eff->SetPointError(i, 0, e_eff);
	    }
	    if(i == (ScanList.size()-1))    
		printf("-----------------------------\n");
	}
      //---Charge study
      else if(strcmp(doWhat,"Q") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\tQ\te_%s\te_Q\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    nt->Draw(var_sig, cut_scan);
	    if(TString(scanType).Contains("HV") == 1) {
	      printf("%d\t%.0f\t%.0f\t%.0f\n", HVVal.at(i), h_sig->GetMean(), 0., h_sig->GetMeanError());
	      outputFile << HVVal.at(i)<<"\t"<<h_sig->GetMean()<<"\t 0.\t"<<h_sig->GetMeanError()<<std::endl;
	      g_eff->SetPoint(i, HVVal.at(i), h_sig->GetMean());
	      g_eff->SetPoint(i, 0, h_sig->GetMeanError());
	    }
	    else {
	      printf("%.3f\t%.0f\t%.0f\t%.0f\n", X0Step.at(i), h_sig->GetMean(), 0., h_sig->GetMeanError());
	      outputFile << X0Step.at(i)<<"\t"<<h_sig->GetMean()<<"\t 0.\t"<<h_sig->GetMeanError()<<std::endl;
	      g_eff->SetPoint(i, 0, h_sig->GetMeanError());
	    }
	    if(i == (ScanList.size()-1))    
		printf("-----------------------------\n");
	}
      //---Time study 
      else if(strcmp(doWhat,"time") == 0)
	{
	  nt->Draw(var_dt_vs_ampM, cut_trig0 && cut_sig && cut_scan);
	  nt->Draw(var_dtStart_vs_ampM, cut_trig0 && cut_sig && cut_scan);
	  nt->Draw(var_dtStart_vs_Tot, cut_trig0 && cut_sig && cut_scan);

	  nt->Draw(var_fracSaturated, cut_trig0 && cut_sig && cut_scan && cut_saturated);
	  nt->Draw(var_evtAll, cut_trig0 && cut_sig && cut_scan);

	  //time resolution with LED
	  nt->Draw(var_timeLED, cut_trig0 && cut_sig && cut_scan);
	  resLED_func->SetParameters(h_timeLED->GetEntries(), h_timeLED->GetMean(), h_timeLED->GetRMS()/2.);
          resLED_func->SetParLimits(0, 0, h_timeLED->GetEntries()*2);
	  resLED_func->SetParLimits(2, 0, h_timeLED->GetRMS());
          resLED_func->SetRange(h_timeLED->GetMean()-2*h_timeLED->GetRMS(), h_timeLED->GetMean()+2*h_timeLED->GetRMS());
          h_timeLED->Fit(resLED_func, "QR+");

	  //   	    float t_res = sqrt(pow(res_func_2sig->GetParameter(2)*1000, 2) - pow(float(RES_TRIG), 2));
	  //	    float e_t_res = sqrt(pow(err_time*res_func_2sig->GetParameter(2)*1000, 2) + pow(float(RES_TRIG_ERR)*RES_TRIG, 2))/t_res;	  
	  float t_res = resLED_func->GetParameter(2)*1000;///sqrt(2);
	  float err_time = resLED_func->GetParError(2)*1000.;
	  float e_t_res = err_time*resLED_func->GetParameter(2)*1000/(t_res);//*sqrt(2));
	  //	  float prob = res_func->GetProb();

	  if(TString(scanType).Contains("HV") == 1) g_resoLED->SetPoint(i,  HVVal.at(i), t_res);
	  else g_resoLED->SetPoint(i, X0Step.at(i), t_res);
	  g_resoLED->SetPointError(i, 0, e_t_res);

	  //time resolution with CFD
	  nt->Draw(var_time, cut_trig0 && cut_sig && cut_scan);
	  res_func->SetParameters(h_time->GetEntries()/2, h_time->GetMean(), h_time->GetRMS()/2);
	  res_func->SetParLimits(0, 0, h_time->GetEntries()*2);
	  res_func->SetParLimits(2, 0, h_time->GetRMS());
	  res_func->SetRange(h_time->GetMean()-2*h_time->GetRMS(), h_time->GetMean()+2*h_time->GetRMS());
	  h_time->Fit(res_func, "QRN+");
	  
	  res_func_2sig->SetParameter(0,res_func->GetParameter(0));
	  res_func_2sig->SetParameter(1,res_func->GetParameter(1));
	  res_func_2sig->SetParameter(2,res_func->GetParameter(2));
	  res_func_2sig->SetRange(res_func->GetParameter(1)-2*res_func->GetParameter(2), res_func->GetParameter(1)+2*res_func->GetParameter(2));
	  h_time->Fit(res_func_2sig, "QR+");
	  
	  err_time = res_func_2sig->GetParError(2)*1000.;
	  t_res = res_func_2sig->GetParameter(2)*1000;///sqrt(2);
	  e_t_res = err_time*res_func_2sig->GetParameter(2)*1000/(t_res);//*sqrt(2));
	  float prob = res_func_2sig->GetProb();
	  if(i == 0)
	    {
	      printf("----------Time Resolution(ps)----------\n");
	      printf(" %s\tt_res\te_%s\te_t_res\tX_prob\n", var_name, var_name);
	      printf("---------------------------------------\n");
	    }
	  if(TString(scanType).Contains("HV") == 1) {
	    printf("%d\t%.1f\t%.0f\t%.1f\t%.3f\n", HVVal.at(i), t_res, 0., e_t_res, prob);
	    outputFile << HVVal.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
	    g_eff->SetPoint(i, HVVal.at(i), t_res);
	    g_eff->SetPointError(i, 0, e_t_res);
	    g_frac_saturated->SetPoint(i, HVVal.at(i), 100.*h_fracSat->GetEntries()/h_evtAll->GetEntries());	
	    //	    g_frac_saturated->SetPoint(i, HVVal.at(i), sqrt(h_fracSat->GetEntries()/h_evtAll->GetEntries());	
	  }
	  else {
	    printf("%.3f\t%.1f\t%.1f\t%.1f\t%.3f\n", X0Step.at(i), t_res, 0., e_t_res, prob);
	    outputFile << X0Step.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
	    g_eff->SetPoint(i, X0Step.at(i), t_res);
	    g_eff->SetPointError(i, 0, e_t_res);
	    g_frac_saturated->SetPoint(i, X0Step.at(i), 100.*h_fracSat->GetEntries()/h_evtAll->GetEntries());	
	  }
	  if(i == (ScanList.size()-1))    
	    printf("---------------------------------------\n");
	  TCanvas* c = new TCanvas();
	  char plot_name[100];
	  std::string command = "if [ ! -e plots/time_resolution/"+string(label)+" ] ; then mkdir plots/time_resolution/"+label+" ; fi";
	  system(command.c_str());
	  
	  sprintf(plot_name, "plots/time_resolution/%s/%s_%d.pdf", label, MCP.c_str(), i);
	  h_time->Draw();
	  res_func_2sig->Draw("same");
	  //	    gStyle->SetOptFit(1111);
	  gStyle->SetOptStat(0000);
	  c->Print(plot_name, "pdf");
	  sprintf(plot_name, "plots/time_resolution/%s/%s_%d.png", label, MCP.c_str(), i);
	  c->Print(plot_name, "png");
	  
	  //	    std::cout << Form("plots/time_resolution/%s/%s_%d.root", label, MCP.c_str(), i) << std::endl;
	  
	  TFile pippi(Form("plots/time_resolution/%s/%s_%d.root", label, MCP.c_str(), i), "recreate");
	  pippi.cd();
	  h_time->Write();
	  h_timeLED->Write();
// 	  h_fracSat->Write();
// 	  h_evtAll->Write();
	  h_dt_vs_ampM->Write();
	  h_dtStart_vs_Tot->Write();
	  h_dtStart_vs_ampM->Write();
	  res_func_2sig->Write("same");
	  resLED_func->Write();
	  pippi.Close();
	}
    }    
  
  //do efficiency plot!
  if(strcmp(doWhat,"time") != 0)
    {
      TCanvas* c2 = new TCanvas();
      c2->cd();
      g_eff->GetXaxis()->SetTitle(scanType);
      g_eff->GetYaxis()->SetTitle("Efficiency");
      g_eff->SetTitle(MCP.c_str());
      g_eff->SetMarkerStyle(20);
      g_eff->SetMarkerSize(0.9);
      g_eff->SetMarkerColor(4);
      gPad->SetGrid();
      g_eff->Draw("AP");
      g_eff->GetXaxis()->SetRangeUser(1100,3500);
      g_eff->Draw("AP");
      char effPlotName[200]="";
      sprintf(effPlotName, "plots/efficiency/efficiency_%s_%s_%s_%s.pdf", MCP.c_str(), doWhat, scanType, label);
      c2->Print(effPlotName,"pdf");
    }

  char effRootName[200]="";
  sprintf(effRootName, "plots/efficiency/efficiency_%s_%s_%s_%s.root", MCP.c_str(), doWhat, scanType, label);
  if(strcmp(doWhat,"time") == 0) sprintf(effRootName, "plots/time_resolution/time_%s_%s_%s_%s.root", MCP.c_str(), doWhat, scanType, label);
  TFile* out = new TFile(TString(effRootName), "recreate");
  out->cd();
  g_eff->Write();
  g_frac_saturated->Write();
  if(strcmp(doWhat,"time") == 0) g_resoLED->Write();
  std::cout << " stampati .root" << std::endl;
  out->Close();



  std::cout<<"results printed in results/"<<std::endl;
  //  outputFile.close();
  inFile->Close();
  
  return 0;     

}

