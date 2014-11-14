/*****************************************************************************************
Program to analize scan in preshower configuration

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
#include "TMultiGraph.h"

#include "../src/init_Reco_Tree.cc"
#include "../src/MCPMap.cc"

using namespace std;

int main(int argc, char** argv)
{
  gSystem->Load("libTree");
  char *label_presh, *label_CeF3_only;
  Fill_MCPList();
  Fill_inverted_MCPList();

  //read input options
  std::ifstream inputCfg (argv[1],ios::in);
  std::string MCP_1X0 = argv[2];
  std::string MCP_3X0 = argv[3];
  label_presh = argv[4];
  label_CeF3_only = argv[5];

  int MCPNumber_1X0 = MCPList.at(MCP_1X0);
  int MCPNumber_3X0 = MCPList.at(MCP_3X0);
  int CeF3_1        = MCPList.at("Off1");
  int CeF3_2        = MCPList.at("Off2");
  int CeF3_3        = MCPList.at("Off3");
  int CeF3_4        = MCPList.at("Off4");

  std::cout<<"----START PRESHOWER MODE-------"<<std::endl;
 
  std::map <int,int> treshold;
  int ch, tresh;
  int nChannels=0;
  int trigPos1 = -1, trigPos2 = -1;  //positions of the two trigger chambers in the reco tree
 
  //---open cfg file and fill map with treshold for each channel----
  while(!inputCfg.eof())
      {
	inputCfg >> ch >> tresh;
	treshold.insert(std::make_pair(ch,tresh));
	nChannels++;
      }

  //------------------OPEN 1ST TREE---------------------

  //open reco tree 1 (containing preshower configuration)
  std::string inFileName = "ntuples/reco_"+string(label_presh)+".root";
  TFile *inFile = new TFile (inFileName.c_str());
  TTree* nt = (TTree*)inFile->Get("reco_tree");
  InitRecoTree(nt);
  
  /*  char outputFileName[200]="";
  sprintf(outputFileName, "results/preshower_%s_%s_%s.txt", , doWhat, scanType, label);
  std::ofstream outputFile (outputFileName, std::ofstream::out);
  */
  //------Build TCut and draw variables--------------------------------------------------
  char var_sig[100]="";

  //---open tree and get: run list and corresponding energy-----
  std::vector<float> ScanList;
  ScanList.clear();

  //---save list of energy step---
    float prev=-1.;
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++)
      {
	nt->GetEntry(iEntry);
	if (X0>(prev+0.001)||X0<(prev-0.001)) {  //this means: if energy!=prev
	  ScanList.push_back(X0);
	  prev=X0;
	  if (iEntry==0) {
	    for (int i=0; i<nChannels; i++)  //save trigger position!!
	      {
		if (isTrigger[i]==1)       trigPos1 = i;
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

  char str_cut_sig [200]="";
  char str_cut_trig0 [200]="";
  char str_cut_tdc [200]="";

  //cut strings
  sprintf(str_cut_sig, "charge[%d] > %d && charge[%d] > %d", MCPNumber_1X0, treshold.at(MCPNumber_1X0), MCPNumber_3X0, treshold.at(MCPNumber_3X0));
  sprintf(str_cut_trig0, "charge[%d] > %d", trigPos1, treshold.at(trigPos1));
  sprintf(str_cut_tdc, "tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6");

  //-----construct TCut-----
  TCut cut_sig = str_cut_sig;
  TCut cut_trig0 = str_cut_trig0;
  TCut cut_tdc = str_cut_tdc;

  TFile *out = new TFile ("preshower.root","RECREATE");
  out->cd();

  TH2F* MCP_vs_CeF3;  

  //-------correlation plots---------------------------------------------------------------------
  for(unsigned int i=0; i<ScanList.size(); i++)
    {
      char str_cut_energy [200]="";
      sprintf(str_cut_energy, "X0==%f", ScanList.at(i));
      TCut cut_energy = str_cut_energy;

      char h_energy_name[50];
      sprintf(h_energy_name, "h_correlation_%f", ScanList.at(i));
      MCP_vs_CeF3 = new TH2F(h_energy_name, h_energy_name, 500,0,10000,500,0,10000);

      sprintf(var_sig, "(charge[%d]+charge[%d]):(charge[%d]+charge[%d]+charge[%d]+charge[%d])>>%s",MCPNumber_1X0,MCPNumber_3X0,CeF3_1,CeF3_2,CeF3_3,CeF3_4,h_energy_name);
      nt->Draw(var_sig, cut_sig && cut_trig0 && cut_tdc && cut_energy, "goff");

      MCP_vs_CeF3->GetXaxis()->SetTitle("MCP energy");
      MCP_vs_CeF3->GetYaxis()->SetTitle("CeF3 energy");
      MCP_vs_CeF3->SetTitle("correlation plot");
      MCP_vs_CeF3->Draw();

      MCP_vs_CeF3->Write();
    }    

  //-------resolution---------------------------------------------------------------------
  TGraphErrors* g_res_CeF3 = new TGraphErrors("g_res_CeF3","g_res_CeF3");
  TGraphErrors* g_res_CeF3_presh = new TGraphErrors("g_res_CeF3_presh","g_res_CeF3_presh");
  TMultiGraph* g_res = new TMultiGraph("g_res","g_res");

  for(unsigned int i=0; i<ScanList.size(); i++)
    {
      char str_cut_energy [200]="";
      sprintf(str_cut_energy, "X0==%f", ScanList.at(i));
      TCut cut_energy = str_cut_energy;

      char h_energy_CeF3_name[50], h_energy_CeF3_presh_name[50];
      sprintf(h_energy_CeF3_name, "h_energy_CeF3_%f", ScanList.at(i));
      sprintf(h_energy_CeF3_presh_name, "h_energy_CeF3_presh_%f", ScanList.at(i));

      TH1F* h_energy_CeF3 = new TH1F(h_energy_CeF3_name, h_energy_CeF3_name, 500,0,10000);
      TH1F* h_energy_CeF3_presh = new TH1F(h_energy_CeF3_presh_name, h_energy_CeF3_presh_name, 500,0,10000);

      char var_CeF3[200]="", var_CeF3_presh[200]="";
      sprintf(var_CeF3, "(charge[%d]+charge[%d]+charge[%d]+charge[%d])>>%s",CeF3_1,CeF3_2,CeF3_3,CeF3_4,h_energy_CeF3_name);
      sprintf(var_CeF3_presh, "(charge[%d]+charge[%d])>>%s",MCPNumber_1X0,MCPNumber_3X0,h_energy_CeF3_presh_name);

      nt->Draw(var_CeF3, cut_sig && cut_trig0 && cut_tdc && cut_energy, "goff");
      nt->Draw(var_CeF3_presh, cut_sig && cut_trig0 && cut_tdc && cut_energy, "goff");

      TF1* res_CeF3_fit = new TF1("res_CeF3_fit", "gaus", 0,10000);
      TF1* res_CeF3_presh_fit = new TF1("res_CeF3_presh_fit", "gaus", 0,10000);
      h_energy_CeF3->Fit(res_CeF3_fit);
      h_energy_CeF3_presh->Fit(res_CeF3_presh_fit);

      h_energy_CeF3->GetXaxis()->SetTitle("CeF3 energy");
      h_energy_CeF3->SetTitle("energy");
      h_energy_CeF3->Draw();
      res_CeF3_fit->Draw("same");
      h_energy_CeF3->Write();

      h_energy_CeF3_presh->GetXaxis()->SetTitle("CeF3+MCPs energy");
      h_energy_CeF3_presh->SetTitle("energy");
      h_energy_CeF3_presh->Draw();
      res_CeF3_presh_fit->Draw("same");
      h_energy_CeF3_presh->Write();

      g_res_CeF3->SetPoint(i,ScanList.at(i),res_CeF3_fit->GetParameter(2));
      g_res_CeF3->SetPointError(i,ScanList.at(i),res_CeF3_fit->GetParError(2));
      g_res_CeF3_presh->SetPoint(i,0.,res_CeF3_presh_fit->GetParameter(2));
      g_res_CeF3_presh->SetPointError(i,0.,res_CeF3_presh_fit->GetParError(2));

      delete res_CeF3_fit;
      delete res_CeF3_presh_fit;
    }      

  g_res_CeF3->SetMarkerColor(2);
  g_res_CeF3->GetXaxis()->SetTitle("energy (GeV)");
  g_res_CeF3->GetYaxis()->SetTitle("resolution");

  g_res_CeF3_presh->SetMarkerColor(4);
  g_res_CeF3_presh->GetXaxis()->SetTitle("energy (GeV)");
  g_res_CeF3_presh->GetYaxis()->SetTitle("resolution");

  g_res->Add(g_res_CeF3);
  g_res->Add(g_res_CeF3_presh);

  
  //----------------------OPEN 2ND TREE---------------------------

  //open reco tree 2 (containing only-CeF3 configuration)
  std::string inFileName2 = "ntuples/reco_"+string(label_CeF3_only)+".root";
  TFile *inFile2 = new TFile (inFileName2.c_str());
  TTree* nt2 = (TTree*)inFile2->Get("reco_tree");
  InitRecoTree(nt2);

  //---open tree and get: run list and corresponding energy-----
  std::vector<float> ScanList2;
  ScanList2.clear();

  //---save list of energy step---
    prev=-1.;
    for (int iEntry=0; iEntry<nt2->GetEntries(); iEntry++)
      {
	nt2->GetEntry(iEntry);
	if (X0>(prev+0.001)||X0<(prev-0.001)) {  //this means: if energy!=prev
	  ScanList2.push_back(X0);
	  prev=X0;
	}
      }

  //-------resolution---------------------------------------------------------------------
  TGraphErrors* g_res_CeF3_only = new TGraphErrors("g_res_only","g_res_CeF3_only");

  for(unsigned int i=0; i<ScanList2.size(); i++)
    {
      char str_cut_energy [200]="";
      sprintf(str_cut_energy, "X0==%f", ScanList.at(i));
      TCut cut_energy = str_cut_energy;

      char h_energy_CeF3_name[50];
      sprintf(h_energy_CeF3_name, "h_energy_CeF3_%f", ScanList2.at(i));

      TH1F* h_energy_CeF3 = new TH1F(h_energy_CeF3_name, h_energy_CeF3_name, 500,0,10000);

      char var_CeF3[200]="";
      sprintf(var_CeF3, "(charge[%d]+charge[%d]+charge[%d]+charge[%d])>>%s",CeF3_1,CeF3_2,CeF3_3,CeF3_4,h_energy_CeF3_name);

      nt2->Draw(var_CeF3, cut_tdc && cut_energy, "goff"); 

      TF1* res_CeF3_only_fit = new TF1("res_CeF3_fit", "gaus", 0,10000);
      h_energy_CeF3->Fit(res_CeF3_only_fit);

      h_energy_CeF3->GetXaxis()->SetTitle("CeF3 energy");
      h_energy_CeF3->SetTitle("energy");
      h_energy_CeF3->Draw();
      res_CeF3_only_fit->Draw("same");
      h_energy_CeF3->Write();

      g_res_CeF3_only->SetPoint(i,ScanList2.at(i),res_CeF3_only_fit->GetParameter(2));
      g_res_CeF3_only->SetPointError(i,ScanList2.at(i),res_CeF3_only_fit->GetParError(2));

      delete res_CeF3_only_fit;
    }      

  g_res_CeF3_only->SetMarkerColor(1);
  g_res_CeF3_only->GetXaxis()->SetTitle("energy (GeV)");
  g_res_CeF3_only->GetYaxis()->SetTitle("resolution");

  g_res->Add(g_res_CeF3_only);

  //-----------------SAVE ALL IN A .ROOT--------------

  g_res->Write();

  out->Close();
  inFile->Close();
  
  return 0;     

}

