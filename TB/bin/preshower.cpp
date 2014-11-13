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

#include "../src/init_Reco_Tree.cc"
#include "../src/MCPMap.cc"

#define RES_TRIG 85.3
#define RES_TRIG_ERR 0.3

using namespace std;

int main(int argc, char** argv)
{
  gSystem->Load("libTree");
  char *label;
  Fill_MCPList();
  Fill_inverted_MCPList();

  //read input options
  std::ifstream inputCfg (argv[1],ios::in);
  std::string MCP_1X0 = argv[2];
  std::string MCP_3X0 = argv[3];
  label = argv[4];

  int MCPNumber_1X0 = MCPList.at(MCP_1X0);
  int MCPNumber_3X0 = MCPList.at(MCP_3X0);
  int CeF3_1        = MCPList.at("CeF3_1");
  int CeF3_2        = MCPList.at("CeF3_2");
  int CeF3_3        = MCPList.at("CeF3_3");
  int CeF3_4        = MCPList.at("CeF3_4");

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

  //open reco tree
  std::string inFileName = "ntuples/reco_"+string(label)+".root";
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
  std::vector<float> energyStep;
  energyStep.clear();
  std::vector<float> ScanList;
  ScanList.clear();


  //---save list of energy step---
    float prev=-1.;
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++)
      {
	nt->GetEntry(iEntry);
	if (X0>(prev+0.001)||X0<(prev-0.001)) {  //this means: if energy!=prev
	  ScanList.push_back(X0);
	  energyStep.push_back(X0);
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

  TH2F* MCP_vs_CeF3;  
  //-------Runs loop---------------------------------------------------------------------
  for(unsigned int i=0; i<ScanList.size(); i++)
    {
      char h_energy_name[50];
      sprintf(h_energy_name, "h_energy_%f", energyStep.at(i));
      MCP_vs_CeF3 = new TH2F(h_energy_name, h_energy_name, 500,0,10000,500,0,10000);

      sprintf(var_sig, "(charge[%d]+charge[%d]):(charge[%d]+charge[%d]+charge[%d]+charge[%d])>>%s",MCPNumber_1X0,MCPNumber_3X0,CeF3_1,CeF3_2,CeF3_3,CeF3_4,h_energy_name);
      nt->Draw(var_sig, cut_sig && cut_trig0 && cut_tdc, "goff");
    }    

      TCanvas* c2 = new TCanvas();
      c2->cd();
      MCP_vs_CeF3->GetXaxis()->SetTitle("MCP energy");
      MCP_vs_CeF3->GetYaxis()->SetTitle("CeF3 energy");
      MCP_vs_CeF3->SetTitle("correlation plot");
      MCP_vs_CeF3->Draw();
      char CorrelPlotName[200]="";
      sprintf(CorrelPlotName, "plots/efficiency/correlation_plot.pdf");
      c2->Print(CorrelPlotName,"pdf");

      char CorrelRootName[200]="";
      sprintf(CorrelRootName, "plots/efficiency/correlation_plot.root");

      TFile *out = new TFile (TString(CorrelRootName),"RECREATE");
      out->cd();
      MCP_vs_CeF3->Write();

  //  outputFile.close();
  inFile->Close();
  
  return 0;     

}

