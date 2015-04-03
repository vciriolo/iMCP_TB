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
#include "TProfile.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
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

using namespace std;

int main(int argc, char** argv)
{
    gSystem->Load("libTree");
    char *label, *doWhat, *scanType;
    Fill_MCPList();
    Fill_inverted_MCPList();

    //---Read input options---
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
    int trigPos1 = -1;  //positions of the trigger chamber in the reco tree 
    int clockPos1 = -1, clockPos2 = -1;

    //---Open cfg file and fill map with treshold for each channel---
    while(!inputCfg.eof())
    {
	inputCfg >> ch >> tresh;
	treshold.insert(std::make_pair(ch,tresh));
	nChannels++;
    }

    //-----Create output files-----

    //open reco tree
    std::string inFileName = "ntuples/reco_"+string(label)+".root";
    TFile *inFile = new TFile (inFileName.c_str());
    TTree* nt = (TTree*)inFile->Get("reco_tree");
    InitRecoTree(nt);
    //---txt---
    char outputFileName[200]="";
    sprintf(outputFileName, "results/%s_%s_%s_%s.txt", MCP.c_str(), doWhat, scanType, label);
    std::ofstream outputFile (outputFileName, std::ofstream::out);
  
    //---ROOT---
    char mkdir_command[100];
    TFile* outROOT_eff = NULL;
    TFile* outROOT_Q = NULL;
    TFile* outROOT_CFD = NULL;
    TFile* outROOT_LED = NULL;
    //---efficiency
    if(strcmp(doWhat, "eff") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "if [ ! -e plots/efficiency_studies ] ; then mkdir plots/efficiency_studies ; fi");
        system(mkdir_command);         
        outROOT_eff = TFile::Open(Form("plots/efficiency_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }
    //---charge
    if(strcmp(doWhat, "Q") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "if [ ! -e plots/charge_studies ] ; then mkdir plots/charge_studies ; fi");
        system(mkdir_command);         
        outROOT_Q = TFile::Open(Form("plots/charge_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }    
    //---time resolution CFD
    if(strcmp(doWhat, "timeCFD") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "if [ ! -e plots/resCFD_studies ] ; then mkdir plots/resCFD_studies ; fi");
        system(mkdir_command);         
        outROOT_CFD = TFile::Open(Form("plots/resCFD_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }
    //---time resolution LED
    if(strcmp(doWhat, "timeLED") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "if [ ! -e plots/resLED_studies ] ; then mkdir plots/resLED_studies ; fi");
        system(mkdir_command);         
        outROOT_LED = TFile::Open(Form("plots/resLED_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }


    //---open tree and get: run list and corresponding HV/X0---
    std::vector<int> HVVal;
    HVVal.clear();
    std::vector<float> X0Step;
    X0Step.clear();
    std::vector<float> ScanList;
    ScanList.clear();

  //Output Histos
  TH1F *h_XDiff_Hodo1TDC   = new TH1F( "h_XDiff_Hodo1TDC", "", 60, -60., 60.); h_XDiff_Hodo1TDC->GetXaxis()->SetTitle("HODOX1 - TDCX [mm]");
  TH1F *h_YDiff_Hodo1TDC   = new TH1F( "h_YDiff_Hodo1TDC", "", 60, -60., 60.); h_YDiff_Hodo1TDC->GetXaxis()->SetTitle("HODOY1 - TDCY [mm]");
  TH1F *h_XDiff_Hodo2TDC   = new TH1F( "h_XDiff_Hodo2TDC", "", 60, -60., 60.); h_XDiff_Hodo2TDC->GetXaxis()->SetTitle("HODOX2 - TDCX [mm]");
  TH1F *h_YDiff_Hodo2TDC   = new TH1F( "h_YDiff_Hodo2TDC", "", 60, -60., 60.); h_YDiff_Hodo2TDC->GetXaxis()->SetTitle("HODOY2 - TDCY [mm]");
  TH1F *h_XDiff_Hodo12     = new TH1F( "h_XDiff_Hodo12", "", 60, -15., 15.); h_XDiff_Hodo12->GetXaxis()->SetTitle("HODOX1 - HODOX2 [mm]");
  TH1F *h_YDiff_Hodo12     = new TH1F( "h_YDiff_Hodo12", "", 60, -15., 15.); h_YDiff_Hodo12->GetXaxis()->SetTitle("HODOY1 - HODOY2 [mm]");
  TH1F *h_XDiff_Hodo1TDC_c = new TH1F( "h_XDiff_Hodo1TDC_c", "", 20, -10., 10.); h_XDiff_Hodo1TDC_c->GetXaxis()->SetTitle("HODOX1 - TDCX [mm]");
  TH1F *h_YDiff_Hodo1TDC_c = new TH1F( "h_YDiff_Hodo1TDC_c", "", 20, -10., 10.); h_YDiff_Hodo1TDC_c->GetXaxis()->SetTitle("HODOY1 - TDCY [mm]");
  TH1F *h_XDiff_Hodo2TDC_c = new TH1F( "h_XDiff_Hodo2TDC_c", "", 20, -10., 10.); h_XDiff_Hodo2TDC_c->GetXaxis()->SetTitle("HODOX2 - TDCX [mm]");
  TH1F *h_YDiff_Hodo2TDC_c = new TH1F( "h_YDiff_Hodo2TDC_c", "", 20, -10., 10.); h_YDiff_Hodo2TDC_c->GetXaxis()->SetTitle("HODOY2 - TDCY [mm]");
  TH1F *h_XDiff_Hodo12_c   = new TH1F( "h_XDiff_Hodo12_c", "", 20, -10., 10.); h_XDiff_Hodo12_c->GetXaxis()->SetTitle("HODOX1 - HODOX2 [mm]");
  TH1F *h_YDiff_Hodo12_c   = new TH1F( "h_YDiff_Hodo12_c", "", 20, -10., 10.); h_YDiff_Hodo12_c->GetXaxis()->SetTitle("HODOY1 - HODOY2 [mm]");
  TH1F *h_nX1              = new TH1F( "h_nX1", "", 50, 0., 50.); h_nX1->GetXaxis()->SetTitle("N Fibers X1");
  TH1F *h_nY1              = new TH1F( "h_nY1", "", 50, 0., 50.); h_nY1->GetXaxis()->SetTitle("NFibers Y1");
  TH1F *h_nX2              = new TH1F( "h_nX2", "", 50, 0., 50.); h_nX2->GetXaxis()->SetTitle("N Fibers X2");
  TH1F *h_nY2              = new TH1F( "h_nY2", "", 50, 0., 50.); h_nY2->GetXaxis()->SetTitle("N Fibers Y2");
  TH1F *h_pX1              = new TH1F( "h_pX1", "", 60, -30., 30.); h_pX1->GetXaxis()->SetTitle("HODO1 X [mm]");
  TH1F *h_pY1              = new TH1F( "h_pY1", "", 60, -30., 30.); h_pY1->GetXaxis()->SetTitle("HODO1 Y [mm]");
  TH1F *h_TDCX             = new TH1F( "h_TDCX", "", 60, -30., 30.); h_TDCX->GetXaxis()->SetTitle("TDC X [mm]");
  TH1F *h_TDCY             = new TH1F( "h_TDCY", "", 60, -30., 30.); h_TDCY->GetXaxis()->SetTitle("TDC Y [mm]");
  TH2F *h_Corr_TDC_HODO1_X = new TH2F( "h_Corr_TDC_HODO1_X", "", 60, -30., 30.,60, -30., 30.); h_Corr_TDC_HODO1_X->GetXaxis()->SetTitle("HODO1 X [mm]"); h_Corr_TDC_HODO1_X->GetYaxis()->SetTitle("TDC X [mm]");
  TH2F *h_Corr_TDC_HODO1_Y = new TH2F( "h_Corr_TDC_HODO1_Y", "", 60, -30., 30.,60, -30., 30.); h_Corr_TDC_HODO1_Y->GetXaxis()->SetTitle("HODO1 Y [mm]"); h_Corr_TDC_HODO1_X->GetYaxis()->SetTitle("TDC Y [mm]");
  TH2F *h_TriggM2          = new TH2F( "h_TriggM2", "", 32, -16., 16., 32, -16., 16.); h_TriggM2->GetXaxis()->SetTitle("HODO2 X [mm]"); h_TriggM2->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_TriggM3          = new TH2F( "h_TriggM3", "", 32, -16., 16., 32, -16., 16.); h_TriggM3->GetXaxis()->SetTitle("HODO2 X [mm]"); h_TriggM3->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_TriggZ1          = new TH2F( "h_TriggZ1", "", 32, -16., 16., 32, -16., 16.); h_TriggZ1->GetXaxis()->SetTitle("HODO2 X [mm]"); h_TriggZ1->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_TriggZ2          = new TH2F( "h_TriggZ2", "", 32, -16., 16., 32, -16., 16.); h_TriggZ2->GetXaxis()->SetTitle("HODO2 X [mm]"); h_TriggZ2->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_TriggSe          = new TH2F( "h_TriggSe", "", 32, -16., 16., 32, -16., 16.); h_TriggSe->GetXaxis()->SetTitle("HODO2 X [mm]"); h_TriggSe->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_NumEff_M2        = new TH2F( "h_NumEff_M2", "", 32, -16, 16, 32, -16, 16); h_NumEff_M2->GetXaxis()->SetTitle("HODO2 X [mm]"); h_NumEff_M2->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_NumEff_M3        = new TH2F( "h_NumEff_M3", "", 32, -16, 16, 32, -16, 16); h_NumEff_M3->GetXaxis()->SetTitle("HODO2 X [mm]"); h_NumEff_M3->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_NumEff_Z1        = new TH2F( "h_NumEff_Z1", "", 32, -16, 16, 32, -16, 16); h_NumEff_Z1->GetXaxis()->SetTitle("HODO2 X [mm]"); h_NumEff_Z1->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_NumEff_Z2        = new TH2F( "h_NumEff_Z2", "", 32, -16, 16, 32, -16, 16); h_NumEff_Z2->GetXaxis()->SetTitle("HODO2 X [mm]"); h_NumEff_Z2->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_NumEff_Se        = new TH2F( "h_NumEff_Se", "", 32, -16, 16, 32, -16, 16); h_NumEff_Se->GetXaxis()->SetTitle("HODO2 X [mm]"); h_NumEff_Se->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_Eff_vsH2_M2      = new TH2F( "h_Eff_vsH2_M2", "", 32, -16, 16, 32, -16, 16); h_Eff_vsH2_M2->GetXaxis()->SetTitle("HODO2 X [mm]"); h_Eff_vsH2_M2->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_Eff_vsH2_M3      = new TH2F( "h_Eff_vsH2_M3", "", 32, -16, 16, 32, -16, 16); h_Eff_vsH2_M3->GetXaxis()->SetTitle("HODO2 X [mm]"); h_Eff_vsH2_M3->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_Eff_vsH2_Z1      = new TH2F( "h_Eff_vsH2_Z1", "", 32, -16, 16, 32, -16, 16); h_Eff_vsH2_Z1->GetXaxis()->SetTitle("HODO2 X [mm]"); h_Eff_vsH2_Z1->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_Eff_vsH2_Z2      = new TH2F( "h_Eff_vsH2_Z2", "", 32, -16, 16, 32, -16, 16); h_Eff_vsH2_Z2->GetXaxis()->SetTitle("HODO2 X [mm]"); h_Eff_vsH2_Z2->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TH2F *h_Eff_vsH2_Se      = new TH2F( "h_Eff_vsH2_Se", "", 32, -16, 16, 32, -16, 16); h_Eff_vsH2_Se->GetXaxis()->SetTitle("HODO2 X [mm]"); h_Eff_vsH2_Se->GetYaxis()->SetTitle("HODO2 Y [mm]");
  TGraphAsymmErrors *TG_effHODO2_X, *TG_effHODO2_Y, *TG_effHODO1_X, *TG_effHODO1_Y, * TG_effTDC_X, * TG_effTDC_Y;
  TH1F * h_N_Trig1_vsX       = new TH1F( "h_N_Trig1_vsX", "", 16, -16, 16 );
  TH1F * h_N_Trig1_hodo2_vsX = new TH1F( "h_N_Trig1_hodo2_vsX", "", 16, -16, 16 );
  TH1F * h_N_Trig1_vsY       = new TH1F( "h_N_Trig1_vsY", "", 16, -16, 16 );
  TH1F * h_N_Trig1_hodo2_vsY = new TH1F( "h_N_Trig1_hodo2_vsY", "", 16, -16, 16 );
  TH1F * h_N_Trig2_vsX       = new TH1F( "h_N_Trig2_vsX", "", 16, -16, 16 );
  TH1F * h_N_Trig2_hodo1_vsX = new TH1F( "h_N_Trig2_hodo1_vsX", "", 16, -16, 16 );
  TH1F * h_N_Trig2_vsY       = new TH1F( "h_N_Trig2_vsY", "", 16, -16, 16 );
  TH1F * h_N_Trig2_hodo1_vsY = new TH1F( "h_N_Trig2_hodo1_vsY", "", 16, -16, 16 );
  TH1F * h_N_Trig1_TDC_vsX   = new TH1F( "h_N_Trig1_TDC_vsX", "", 16, -16, 16 );
  TH1F * h_N_Trig1Tdc_vsX    = new TH1F( "h_N_Trig1Tdc_vsX", "", 16, -16, 16 );
  TH1F * h_N_Trig1_TDC_vsY   = new TH1F( "h_N_Trig1_TDC_vsY", "", 16, -16, 16 );
  TH1F * h_N_Trig1Tdc_vsY    = new TH1F( "h_N_Trig1Tdc_vsY", "", 16, -16, 16 );
  float HODOX1TDC = 0.283591, HODOY1TDC = 2.45504;
  float HODOX2TDC = 2.80849, HODOY2TDC = 2.52657;
  float ShiftX = -4.749, ShiftY = -2.948;
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
  else if (strcmp(scanType,"HODO")==0) {
    int prev=0;
    //Trigger stuff
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++){
	nt->GetEntry(iEntry);
	if (HV[MCPNumber]!=prev) {
	  ScanList.push_back((float)HV[MCPNumber]);
	  HVVal.push_back(HV[MCPNumber]);
	  X0Step.push_back(X0);
	  prev=HV[MCPNumber];
	  if (iEntry==0) {
	    for (int i=0; i<nChannels; i++){  //save trigger position!
		if (isTrigger[i]==1)       trigPos1 = i;
	    }
	  }
	}
    }
    float N_Trig1(0.), N_Trig1wc(0.), N_Trig1_hodo2(0.), N_Trig1_WC(0.), N_Trig2(0.), N_Trig2_hodo1(0.);
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++){
	nt->GetEntry(iEntry);
	//HODO position from fibers
	int   nX1(0), nY1(0), nX2(0), nY2(0);
	float pX1(0.), pY1(0.), pX2(0.), pY2(0.);
	int f_minX1=-1, f_maxX1=-1, f_minY1=-1, f_maxY1=-1, f_minX2=-1, f_maxX2=-1, f_minY2=-1, f_maxY2=-1;
	int iMINX1 = 99, iMAXX1 = -99, iMINY1 = 99, iMAXY1 = -99, iMINX2 = 99, iMAXX2 = -99, iMINY2 = 99, iMAXY2 = -99;
	for(int i=0; i<64; i++){
	  if( hodoX1[i] ){ pX1 +=i; nX1 += hodoX1[i]; if(i<iMINX1){ f_minX1 = i; iMINX1 = -99.; } if(i>iMAXX1){ f_maxX1 = i; iMAXX1 = i; } }
	  if( hodoY1[i] ){ pY1 +=i; nY1 += hodoY1[i]; if(i<iMINY1){ f_minY1 = i; iMINY1 = -99.; } if(i>iMAXY1){ f_maxY1 = i; iMAXY1 = i; } }
	  if( hodoX2[i] ){ pX2 +=i; nX2 += hodoX2[i]; if(i<iMINX2){ f_minX2 = i; iMINX2 = -99.; } if(i>iMAXX2){ f_maxX2 = i; iMAXX2 = i; } }
	  if( hodoY2[i] ){ pY2 +=i; nY2 += hodoY2[i]; if(i<iMINY2){ f_minY2 = i; iMINY2 = -99.; } if(i>iMAXY2){ f_maxY2 = i; iMAXY2 = i; } }
	}
	h_nX1->Fill( nX1 ); h_nY1->Fill( nY1 ); h_nX2->Fill( nX2 ); h_nY2->Fill( nY2 );
	//Selection
	bool sele ( nX1<4 && nY1<4 && nX2<4 && nY2<4 && nX1>1 && nY1>1 && nX2>1 && nY2>1 );
	bool ClusterShape = ( f_maxX1-f_minX1==nX1-1 && f_maxY1-f_minY1==nY1-1 && f_maxX2-f_minX2==nX2-1 && f_maxY2-f_minY2==nY2-1 ); //Fibers all close each others
	sele &= (charge_corr[trigPos1] > treshold.at(trigPos1) );
	sele &= ClusterShape;
	//Differences in position
	pX1 /= nX1; pY1 /= nY1; pX2 /= nX2; pY2 /= nY2; //HODO pos
	pX1 -= 32.; pY1 -= 32.; pX2 -= 32.; pY2 -= 32.; //Centered to 0
	tdcY *= -1; //righ sign
	pX1 /= 2; pY1 /= 2; pX2 /= 2; pY2 /= 2; //conversion to mm (1 fiber is 0.5)
	if(nX1==0) pX1=-99.; if(nY1==0) pY1=-99.; if(nX2==0) pX2=-99.; if(nY2==0) pY2=-99.; //If no hit position -99
	float pX1_c = pX1-HODOX1TDC-ShiftX, pY1_c = pY1-HODOY1TDC-ShiftY, pX2_c = pX2-HODOX2TDC-ShiftX, pY2_c = pY2-HODOY2TDC-ShiftY, tdcX_c = tdcX-ShiftX, tdcY_c = tdcY-ShiftY; //Times centered in 0
	if( sele ){
	  //Correl
	  h_pX1->Fill(pX1);
	  h_pY1->Fill(pY1);
	  h_TDCX->Fill(tdcX);
	  h_TDCY->Fill(tdcY);
	  h_Corr_TDC_HODO1_X->Fill(pX1,tdcX);
	  h_Corr_TDC_HODO1_Y->Fill(pY1,tdcY);
	  //Diff
	  h_XDiff_Hodo1TDC->Fill( pX1-tdcX );
	  h_YDiff_Hodo1TDC->Fill( pY1-tdcY );
	  h_XDiff_Hodo2TDC->Fill( pX2-tdcX );
	  h_YDiff_Hodo2TDC->Fill( pY2-tdcY );
	  h_XDiff_Hodo12->Fill( pX1-pX2 );
	  h_YDiff_Hodo12->Fill( pY1-pY2 );
	  h_XDiff_Hodo1TDC_c->Fill( pX1_c-tdcX_c );
	  h_YDiff_Hodo1TDC_c->Fill( pY1_c-tdcY_c );
	  h_XDiff_Hodo2TDC_c->Fill( pX2_c-tdcX_c );
	  h_YDiff_Hodo2TDC_c->Fill( pY2_c-tdcY_c );
	  h_XDiff_Hodo12_c->Fill( pX1_c-pX2_c );
	  h_YDiff_Hodo12_c->Fill( pY1_c-pY2_c );
	  //Eff as a function of HODO2 position
	  int binX = int( (16.+pX2_c) );
	  int binY = int( (16.+pY2_c) );
	  bool HV1_MiB2( TString(label).Contains("HVScan1") && HV[MCPList.at("MiB2")]==2750 );
	  bool HV1_MiB3( TString(label).Contains("HVScan1") && HV[MCPList.at("MiB3")]==3100 );
	  bool HV1_MiZ1( TString(label).Contains("HVScan1") && HV[MCPList.at("ZStack1")]==3150 );
	  bool HV1_MiZ2( TString(label).Contains("HVScan1") && HV[MCPList.at("ZStack2")]==3300 );
	  bool HV1_MiSe( TString(label).Contains("HVScan1") && HV[MCPList.at("enSEE")]==2200 );
	  bool HV2_MiB2( TString(label).Contains("HVScan2") && HV[MCPList.at("MiB2")]==2750 );
	  bool HV2_MiB3( TString(label).Contains("HVScan2") && HV[MCPList.at("MiB3")]==3100 );
	  bool HV2_MiZ1( TString(label).Contains("HVScan2") && HV[MCPList.at("ZStack1")]==3300 );
	  bool HV2_MiZ2( TString(label).Contains("HVScan2") && HV[MCPList.at("ZStack2")]==3100 );
	  bool HV2_MiSe( TString(label).Contains("HVScan2") && HV[MCPList.at("enSEE")]==2200 );
	  bool HV68_MiB2( (TString(label).Contains("HVScan6") || TString(label).Contains("HVScan8") ) && HV[MCPList.at("MiB2")]==2750 );
	  bool HV68_MiB3( (TString(label).Contains("HVScan6") || TString(label).Contains("HVScan8") ) && HV[MCPList.at("MiB3")]==3100 );
	  bool HV68_MiZ1( (TString(label).Contains("HVScan6") || TString(label).Contains("HVScan8") ) && HV[MCPList.at("ZStack1")]==3300 );
	  bool HV68_MiZ2( (TString(label).Contains("HVScan6") || TString(label).Contains("HVScan8") ) && HV[MCPList.at("ZStack2")]==3300 );
	  bool HV68_MiSe( (TString(label).Contains("HVScan6") || TString(label).Contains("HVScan8") ) && HV[MCPList.at("enSEE")]==2200 );
	  bool X0_0( X0==0 );
	  if( HV1_MiB2 || HV2_MiB2 || HV68_MiB2 || X0_0 ){
	    h_TriggM2->SetBinContent( binX+1, binY+1, h_TriggM2->GetBinContent(binX+1, binY+1) + 1 );
	    if( (charge_corr[ MCPList.at("MiB2") ] > treshold.at( MCPList.at("MiB2") ) ) ){
	      h_NumEff_M2->SetBinContent( binX+1, binY+1, h_NumEff_M2->GetBinContent(binX+1, binY+1) + 1 );
	    }
	  }
	  if( HV1_MiB3 || HV2_MiB3 || HV68_MiB3 || X0_0 ){
	    h_TriggM3->SetBinContent( binX+1, binY+1, h_TriggM3->GetBinContent(binX+1, binY+1) + 1 );
	    if( (charge_corr[ MCPList.at("MiB3") ] > treshold.at( MCPList.at("MiB3") ) ) ){
	      h_NumEff_M3->SetBinContent( binX+1, binY+1, h_NumEff_M3->GetBinContent(binX+1, binY+1) + 1 );
	    }
	  }
	  if( HV1_MiZ1 || HV2_MiZ1 || HV68_MiZ1 || X0_0 ){
	    h_TriggZ1->SetBinContent( binX+1, binY+1, h_TriggZ1->GetBinContent(binX+1, binY+1) + 1 );
	    if( (charge_corr[ MCPList.at("ZStack1") ] > treshold.at( MCPList.at("ZStack1") ) ) ){
	      h_NumEff_Z1->SetBinContent( binX+1, binY+1, h_NumEff_Z1->GetBinContent(binX+1, binY+1) + 1 );
	    }
	  }
	  if( HV1_MiZ2 || HV2_MiZ2 || HV68_MiZ2 || X0_0 ){
	    h_TriggZ2->SetBinContent( binX+1, binY+1, h_TriggZ2->GetBinContent(binX+1, binY+1) + 1 );
	    if( (charge_corr[ MCPList.at("ZStack2") ] > treshold.at( MCPList.at("ZStack2") ) ) ){
	      h_NumEff_Z2->SetBinContent( binX+1, binY+1, h_NumEff_Z2->GetBinContent(binX+1, binY+1) + 1 );
	    }
	  }
	  if( HV1_MiSe || HV2_MiSe || HV68_MiSe || X0_0 ){
	    h_TriggSe->SetBinContent( binX+1, binY+1, h_TriggSe->GetBinContent(binX+1, binY+1) + 1 );
	    if( (charge_corr[ MCPList.at("enSEE") ] > treshold.at( MCPList.at("enSEE") ) ) ){
	      h_NumEff_Se->SetBinContent( binX+1, binY+1, h_NumEff_Se->GetBinContent(binX+1, binY+1) + 1 );
	    }
	  }
	}
	//Efficiensies for HODOS
	bool Trigger_HODO1 = ( f_maxX1-f_minX1==nX1-1 && nX1<3 && nY1<3 && nX1>1 && nY1>1 );
	bool Mcp_tri = (charge_corr[trigPos1] > treshold.at(trigPos1) );
	bool hodo_pos = fabs(pX1_c-pX2_c)<=2 && fabs(pY1_c-pY2_c)<=2; 
	if( Trigger_HODO1 && Mcp_tri ){
	  N_Trig1 += 1;
	  int binX = int( (16.+pX1_c)/2. );
	  int binY = int( (16.+pY1_c)/2. );
	  h_N_Trig1_vsX->SetBinContent( binX+1, h_N_Trig1_vsX->GetBinContent(binX+1)+1 );
	  h_N_Trig1_vsY->SetBinContent( binY+1, h_N_Trig1_vsY->GetBinContent(binY+1)+1 );
	  bool sele_HODO2 = ( nX2>=1 && nY2>=1  );
	  if( sele_HODO2 && hodo_pos){
	    N_Trig1_hodo2 += 1;
	    h_N_Trig1_hodo2_vsX->SetBinContent( binX+1, h_N_Trig1_hodo2_vsX->GetBinContent(binX+1)+1 );
	    h_N_Trig1_hodo2_vsY->SetBinContent( binY+1, h_N_Trig1_hodo2_vsY->GetBinContent(binY+1)+1 );
	  }
	}
	bool tdc_pos = ( fabs(pX1_c-tdcX_c)<4 && fabs(pY1_c-tdcY_c)<4 );
	if( Trigger_HODO1 && Mcp_tri ){
	  N_Trig1wc += 1;
	  int binX = int( (16.+pX1_c)/2. );
	  int binY = int( (16.+pY1_c)/2. );
	  h_N_Trig1Tdc_vsX->SetBinContent( binX+1, h_N_Trig1Tdc_vsX->GetBinContent(binX+1)+1 );
	  h_N_Trig1Tdc_vsY->SetBinContent( binY+1, h_N_Trig1Tdc_vsY->GetBinContent(binY+1)+1 );
	  bool sele_WC = tdcX_c > -70 && tdcY_c > -70;
	  if( sele_WC && tdc_pos){
	    N_Trig1_WC += 1;
	    h_N_Trig1_TDC_vsX->SetBinContent( binX+1, h_N_Trig1_TDC_vsX->GetBinContent(binX+1)+1 );
	    h_N_Trig1_TDC_vsY->SetBinContent( binY+1, h_N_Trig1_TDC_vsY->GetBinContent(binY+1)+1 );
	  }
	}
	bool Trigger_HODO2 = ( f_maxX2-f_minX2==nX2-1 && nX2<3 && nY2<3 && nX2>1 && nY2>1 );
	hodo_pos = fabs(pX1_c-pX2_c)<=2 && fabs(pY1_c-pY2_c)<=2;
	if( Trigger_HODO2 && Mcp_tri ){
	  N_Trig2 += 1;
	  int binX = int( (16.+pX2_c)/2. );
	  int binY = int( (16.+pY2_c)/2. );
	  h_N_Trig2_vsX->SetBinContent( binX+1, h_N_Trig2_vsX->GetBinContent(binX+1)+1 );
	  h_N_Trig2_vsY->SetBinContent( binY+1, h_N_Trig2_vsY->GetBinContent(binY+1)+1 );
	  bool sele_HODO1 = ( nX1>=1 && nY1>=1 && hodo_pos );
	  if( sele_HODO1 ){
	    N_Trig2_hodo1 += 1;
	    h_N_Trig2_hodo1_vsX->SetBinContent( binX+1, h_N_Trig2_hodo1_vsX->GetBinContent(binX+1)+1 );
	    h_N_Trig2_hodo1_vsY->SetBinContent( binY+1, h_N_Trig2_hodo1_vsY->GetBinContent(binY+1)+1 );
	  }
	}
    }//iEntry
    //HODOS efficiencies
    if( N_Trig1!=0 ) cout<<"Trigger HODO1: Efficiency on HODO2 is: "<<N_Trig1_hodo2/N_Trig1<<endl;
    if( N_Trig1!=0 ) cout<<"Trigger HODO1: Efficiency on WC is: "<<N_Trig1_WC/N_Trig1wc<<endl;
    if( N_Trig2!=0 ) cout<<"Trigger HODO2: Efficiency on HODO1 is: "<<N_Trig2_hodo1/N_Trig2<<endl;
    for( int i=0; i<16; i++ ){
	if( h_N_Trig1_vsX->GetBinContent(i+1) < 6000 ){ h_N_Trig1_vsX->SetBinContent(i+1,0); h_N_Trig1_hodo2_vsX->SetBinContent(i+1,0); }
	if( h_N_Trig1_vsY->GetBinContent(i+1) < 6000 ){ h_N_Trig1_vsY->SetBinContent(i+1,0); h_N_Trig1_hodo2_vsY->SetBinContent(i+1,0); }
	if( h_N_Trig2_vsX->GetBinContent(i+1) < 6000 ){ h_N_Trig2_vsX->SetBinContent(i+1,0); h_N_Trig2_hodo1_vsX->SetBinContent(i+1,0); }
	if( h_N_Trig2_vsY->GetBinContent(i+1) < 6000 ){ h_N_Trig2_vsY->SetBinContent(i+1,0); h_N_Trig2_hodo1_vsY->SetBinContent(i+1,0); }
	if( h_N_Trig1Tdc_vsX->GetBinContent(i+1) < 6000 ){ h_N_Trig1_TDC_vsX->SetBinContent(i+1,0); h_N_Trig1Tdc_vsX->SetBinContent(i+1,0); }
	if( h_N_Trig1Tdc_vsY->GetBinContent(i+1) < 6000 ){ h_N_Trig1_TDC_vsY->SetBinContent(i+1,0); h_N_Trig1Tdc_vsY->SetBinContent(i+1,0); }
    }
    TG_effHODO2_X = new TGraphAsymmErrors (h_N_Trig1_hodo2_vsX, h_N_Trig1_vsX); TG_effHODO2_X->SetName("TG_effHODO2_X");
    TG_effHODO2_Y = new TGraphAsymmErrors (h_N_Trig1_hodo2_vsY, h_N_Trig1_vsY); TG_effHODO2_Y->SetName("TG_effHODO2_Y");
    TG_effHODO1_X = new TGraphAsymmErrors (h_N_Trig2_hodo1_vsX, h_N_Trig2_vsX); TG_effHODO1_X->SetName("TG_effHODO1_X");
    TG_effHODO1_Y = new TGraphAsymmErrors (h_N_Trig2_hodo1_vsY, h_N_Trig2_vsY); TG_effHODO1_Y->SetName("TG_effHODO1_Y");
    TG_effTDC_X   = new TGraphAsymmErrors (h_N_Trig1_TDC_vsX, h_N_Trig1Tdc_vsX); TG_effTDC_X->SetName("TG_effTDC_X");
    TG_effTDC_Y   = new TGraphAsymmErrors (h_N_Trig1_TDC_vsY, h_N_Trig1Tdc_vsY); TG_effTDC_Y->SetName("TG_effTDC_Y");
  }//if HODO
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
  
  clockPos1 = MCPList.at("trig1");
  clockPos2 = MCPList.at("trig2");

  if (trigPos1==-1 || clockPos1==-1 || clockPos2==-1) {
    std::cout<<"ERROR!!! trigger or clock not found!!!"<<std::endl;
    return -1;
  }
  else
    std::cout<<"TRIGGER INFO: --> \ntrigger 1 = "<<inverted_MCPList.at(trigPos1)<<"\n----------"<<std::endl;

  //  TH2F* ampM_vs_dt = new TH2F("ampM_vs_dt", "", 1000, -5., 5., 10000, 0., 10000.);

    //---Results graphs---
    TGraphErrors *g_eff = new TGraphErrors();
    g_eff->SetName("eff");

    TGraphErrors *g_Q = new TGraphErrors();
    g_Q->SetName("Q");

    TGraphErrors *g_resCFD = new TGraphErrors();
    g_resCFD->SetName("resCFD");

    TGraphErrors *g_resLED = new TGraphErrors();
    g_resLED->SetName("resLED");

    TGraphErrors* g_frac_saturated = new TGraphErrors();
    g_frac_saturated->SetName("frac_saturated");

//------Build TCut and draw variables-----------------------------------------------------
    //---efficiency
    char var_sig[100]="";
    char var_trig0[100]="";
    // char var_dt_vs_ampM[100]="";
    // char var_dtStart_vs_ampM[100] = "";
    // char var_dtStart_vs_Tot[100] = "";
    char var_fracSaturated[100]="";
    char var_evtAll[100]="";
    //---charge
    char var_charge[200]="";
    //---time resolution
    char var_timeCFD[200]="";
    char var_timeCFD_red[200]="";
    char var_timeCFD_vs_TOT[500] = "";
    char var_timeCFD_vs_ampMax[500] = "";
    char var_timeLED[200] = "";
    char var_timeLED_red[200] = "";
    char var_timeLED_vs_TOT[500] = "";
    char var_timeLED_vs_ampMax[500] = "";
    //---cut strings
    char str_cut_sig[500]="";
    char str_cut_trig0[500]="";
    char str_cut_tdc[500]="";
    char str_cut_saturated[500]="";
    char str_cut_nFibers[500]="";
    char str_cut_trig_not_sat[500]="";
    char str_cut_bad_timeCFD[500]="";
    char str_cut_bad_timeLED[500]="";
    //---Define Cuts---
    sprintf(str_cut_sig, "charge_corr[%d] > %d", MCPNumber, treshold.at(MCPNumber));
    sprintf(str_cut_trig0, "charge_corr[%d] > %d", trigPos1, treshold.at(trigPos1));
    float cutTDCX_min = -5.+ShiftX, cutTDCX_max = 3.+ShiftX, cutTDCY_min = -2.+ShiftY, cutTDCY_max = 6.+ShiftY; 
    sprintf(str_cut_tdc, "tdcX > %f && tdcX < %f && tdcY >%f && tdcY < %f", cutTDCX_min, cutTDCX_max, cutTDCY_min, cutTDCY_max);
    sprintf(str_cut_saturated, "amp_max[%d] > 3450", MCPNumber);
    sprintf(str_cut_nFibers, "nhodoX1<=3 && nhodoX2<=3 && nhodoY1<=3 && nhodoY2<=3");
    sprintf(str_cut_trig_not_sat, "amp_max[%d] < 3450", trigPos1); 
    sprintf(str_cut_bad_timeCFD, "time_start_150[%d] != -20", MCPNumber);
    if (MCPList.at(MCP)==4)
    {
        sprintf(str_cut_bad_timeLED, "time_start_150[%d] != 20 && time_start_150[%d] != 20 && "
                "((time_start_150[%d]-time_CF_corr[%d])<-3/5*(time_stop_150[%d]-time_start_150[%d]))",
                MCPNumber, trigPos1, MCPNumber, trigPos1, MCPNumber, MCPNumber);
    }
    else
    {
        sprintf(str_cut_bad_timeLED, "time_start_150[%d] != 20 && time_start_150[%d] != 20 && "
                "((time_start_150[%d]-time_CF_corr[%d])<-1/3*(time_stop_150[%d]-time_start_150[%d])+2)",
                MCPNumber, trigPos1, MCPNumber, trigPos1, MCPNumber, MCPNumber);
    }     
    //---construct TCut
    TCut cut_sig = str_cut_sig;
    TCut cut_trig0 = str_cut_trig0;
    TCut cut_tdc = str_cut_tdc;
    TCut cut_saturated = str_cut_saturated;
    TCut cut_nFibers = str_cut_nFibers;
    TCut cut_trig_not_sat = str_cut_trig_not_sat;
    TCut cut_bad_timeCFD = str_cut_bad_timeCFD;
    TCut cut_bad_timeLED = str_cut_bad_timeLED;


//-------Runs loop------------------------------------------------------------------------
    for(unsigned int i=0; i<ScanList.size(); i++)
    {
        //---Define run dependend cut---
        char cut_scan[100];
        if (strcmp(scanType,"HV")==0)  
            sprintf(cut_scan, "HV[%d] == %d", MCPNumber, HVVal.at(i));
        else  
            sprintf(cut_scan, "X0 == %f", X0Step.at(i));
        if(MCPNumber == 2) 
            sprintf(str_cut_saturated, "run_id > 796 && amp_max[%d] > 4000", MCPNumber);
        //---and print infos
        char var_name[3] = "X0";
        if(TString(scanType).Contains("HV") == 1)    
            sprintf(var_name, "HV");

        //-------define histos------------------------------------------------------------
        //-----create objects names-----
        char h_fracSat_name[20], h_evtAll_name[20];
        char h_sig_name[20], h_trig0_name[20];
        char h_charge_name[20];
        char pr_timeCFD_vs_TOT_name[20], h_resCFD_name[20], f_resCFD_name[20], f_corrCFD_name[20], f_corrCFD2_name[40];
        char pr_timeLED_vs_TOT_name[20], h_resLED_name[20], f_resLED_name[20], f_corrLED_name[20], f_corrLED2_name[40];
	char pr_timeCFD_vs_ampMaxCorr_name[40], pr_timeLED_vs_ampMaxCorr_name[40];
        //-----create objects------
        //---saturated events fraction
        sprintf(h_fracSat_name, "h_fracSat_name_%d", i);
        sprintf(h_evtAll_name, "h_evtAll_name_%d", i);      
        TH1F* h_fracSat = new TH1F(h_fracSat_name, h_fracSat_name, 100, 0., 100.);
        TH1F* h_evtAll = new TH1F(h_evtAll_name, h_evtAll_name, 100, 0., 100.);
        //---efficiency
        sprintf(h_sig_name, "h_sig_%d", i);
        sprintf(h_trig0_name, "h_trig0_%d", i);
        TH1F* h_sig= new TH1F(h_sig_name, h_sig_name, 500, -5000, 25000);
        TH1F* h_trig0 = new TH1F(h_trig0_name, h_trig0_name, 500, -5000, 25000);
        //---TOT        
        char TOT_diff[100];
        sprintf(TOT_diff, "(time_stop_150[%d]-time_start_150[%d])", MCPNumber, MCPNumber);
        //---charge
        sprintf(h_charge_name, "h_charge_%d", i);
        TH1F* h_charge = new TH1F(h_charge_name, "charge distribution", 500, -500, 110000);
        // TH1F* h_charge = new TH1F(h_charge_name, "TOT distribution", 200, 0, 10);
        //---time CFD/TOT corrected                          
        sprintf(pr_timeCFD_vs_TOT_name, "pr_timeCFD_vs_TOT_%d", i);
        sprintf(pr_timeCFD_vs_ampMaxCorr_name, "pr_timeCFD_vs_ampMaxCorr_%d", i);
        sprintf(h_resCFD_name, "h_resCFD_%d", i);  
        sprintf(f_resCFD_name, "f_resCFD_%d", i);    
        sprintf(f_corrCFD_name, "f_corrCFD_%d", i);
        sprintf(f_corrCFD_name, "f_corrCFD2_%d", i);
        TH1F* h_resCFD = new TH1F(h_resCFD_name, "Electron Beam 50 GeV", 250, -1, 1);
	h_resCFD->GetXaxis()->SetTitle("time_{MCP}-time_{TRIG} (ns)");
	h_resCFD->GetYaxis()->SetTitle("Entries");
	h_resCFD->GetXaxis()->SetTitleSize(0.045);
	h_resCFD->GetYaxis()->SetTitleSize(0.045);
	h_resCFD->GetXaxis()->SetTitleOffset(1.06);
	h_resCFD->GetYaxis()->SetTitleOffset(1.06);
        TF1* f_resCFD = new TF1(f_resCFD_name, "gausn", -1, 1);
        TProfile* pr_timeCFD_vs_TOT;
        TProfile* pr_timeCFD_vs_ampMaxCorr;
        TF1* f_corrCFD;
        TF1* f_corrCFD2;
        if(strcmp(scanType, "HV") == 0)
        {                              
            pr_timeCFD_vs_TOT = new TProfile(pr_timeCFD_vs_TOT_name, "timeCF vs TOT difference",
                                          25, 0, 5, -5, 2);
            f_corrCFD = new TF1(f_corrCFD_name, "pol2", 0, 5);

            pr_timeCFD_vs_ampMaxCorr = new TProfile(pr_timeCFD_vs_ampMaxCorr_name, "timeCF vs ampMaxCorr",
						    20, 0, 8000, -2, 2);
            f_corrCFD2 = new TF1(f_corrCFD2_name, "pol4", 0, 8000);
        }
        else
        {
            pr_timeCFD_vs_TOT = new TProfile(pr_timeCFD_vs_TOT_name, "timeCF vs TOT difference",
                                          30, 0, 10, -5, 2);
            f_corrCFD = new TF1(f_corrCFD_name, "pol2", 0, 6);

            pr_timeCFD_vs_ampMaxCorr = new TProfile(pr_timeCFD_vs_ampMaxCorr_name, "timeCF vs ampMaxCorr",
                                          20, 0, 8000, -2, 2);
            f_corrCFD2 = new TF1(f_corrCFD2_name, "pol4", 0, 8000);
        }
        pr_timeCFD_vs_TOT->SetMarkerStyle(20);
        pr_timeCFD_vs_TOT->SetMarkerSize(0.7);
        pr_timeCFD_vs_ampMaxCorr->SetMarkerStyle(20);
        pr_timeCFD_vs_ampMaxCorr->SetMarkerSize(0.7);
        //---time LED/TOT corrected                          
        sprintf(pr_timeLED_vs_TOT_name, "pr_timeLED_vs_TOT_%d", i);
        sprintf(pr_timeLED_vs_ampMaxCorr_name, "pr_timeLED_vs_ampMaxCorr_%d", i);
        sprintf(h_resLED_name, "h_resLED_%d", i);  
        sprintf(f_resLED_name, "f_resLED_%d", i);    
        sprintf(f_corrLED_name, "f_corrLED_%d", i);
        TH1F* h_resLED = new TH1F(h_resLED_name, "time res with LED method", 250, -1, 1);
        TF1* f_resLED = new TF1(f_resLED_name, "gausn", -1, 1);
        TProfile* pr_timeLED_vs_TOT;
        TProfile* pr_timeLED_vs_ampMaxCorr;
        TF1* f_corrLED;
        TF1* f_corrLED2;
        if(strcmp(scanType, "HV") == 0)
        {                              
            pr_timeLED_vs_TOT = new TProfile(pr_timeLED_vs_TOT_name, "time vs TOT difference",
                                          25, 0, 5, -5, 2);
            f_corrLED = new TF1(f_corrLED_name, "pol2", 0, 5);

            pr_timeLED_vs_ampMaxCorr = new TProfile(pr_timeLED_vs_ampMaxCorr_name, "timeLED vs ampMaxCorr",
						    20, 0, 8000, -2, 2);
            f_corrLED2 = new TF1(f_corrLED2_name, "pol4", 0, 8000);
        }
        else
        {
            pr_timeLED_vs_TOT = new TProfile(pr_timeLED_vs_TOT_name, "time vs TOT difference",
                                          30, 0, 10, -5, 2);
            f_corrLED = new TF1(f_corrLED_name, "pol2", 0, 6);

            pr_timeLED_vs_ampMaxCorr = new TProfile(pr_timeLED_vs_ampMaxCorr_name, "timeLED vs ampMaxCorr",
						    20, 0, 8000, -2, 2);
            f_corrLED2 = new TF1(f_corrLED2_name, "pol4", 0, 8000);
        }
        pr_timeLED_vs_TOT->SetMarkerStyle(20);
        pr_timeLED_vs_TOT->SetMarkerSize(0.7);
        pr_timeLED_vs_ampMaxCorr->SetMarkerStyle(20);
        pr_timeLED_vs_ampMaxCorr->SetMarkerSize(0.7);

        //-----Saturated event computation----
        sprintf(var_fracSaturated, "time_CF[%d] >> %s", MCPNumber, h_fracSat_name); 
        sprintf(var_evtAll, "time_CF[%d] >> %s", MCPNumber, h_evtAll_name); 
        nt->Draw(var_fracSaturated, cut_trig0 && cut_sig && cut_scan && cut_saturated && cut_tdc && cut_nFibers); 
        nt->Draw(var_evtAll, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers);
        if(TString(scanType).Contains("HV") == 1) 
        {
            if(h_evtAll->GetEntries() != 0)
                g_frac_saturated->SetPoint(i, HVVal.at(i), 100.*h_fracSat->GetEntries()/h_evtAll->GetEntries());	
            else
                g_frac_saturated->SetPoint(i, HVVal.at(i), 0);	
        }
        else
        {
            if(h_evtAll->GetEntries() != 0)
                g_frac_saturated->SetPoint(i, X0Step.at(i), 100.*h_fracSat->GetEntries()/h_evtAll->GetEntries());
            else
                g_frac_saturated->SetPoint(i, X0Step.at(i), 0);	
        }
        //-----Efficiency study-----
        if(strcmp(doWhat, "eff") == 0 || strcmp(doWhat, "all") == 0)
	{
            sprintf(var_sig, "charge_corr[%d]>>%s", MCPNumber, h_sig_name);
            sprintf(var_trig0, "charge_corr[%d]>>%s", trigPos1, h_trig0_name);
            nt->Draw(var_sig, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers, "goff");
            nt->Draw(var_trig0, cut_trig0 && cut_scan && cut_tdc && cut_nFibers, "goff");
            float eff = h_sig->GetEntries()/h_trig0->GetEntries();
            float e_eff = TMath::Sqrt((TMath::Abs(eff*(1-eff)))/h_trig0->GetEntries());
            if(eff < 0)   
                eff = 0;
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\teff\te_%s\te_eff\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    if(TString(scanType).Contains("HV") == 1) 
            {
                printf("%d\t%.3f\t%.3f\t%.3f\n", HVVal.at(i), eff, 0., e_eff);
                outputFile << HVVal.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
                g_eff->SetPoint(i, HVVal.at(i), eff);
                g_eff->SetPointError(i, 0, e_eff);
	    }
	    else 
            {
	      if (TString(label).Contains("scanX0_HVHigh50GeV")) { //correct efficiencies at 0X0
		if (TString(MCP).Contains("ZStack2") && X0Step.at(i)==0.) {
		  eff = 0.6635633;
		  e_eff = 0.009062424;
		}
		else if (TString(MCP).Contains("enSEE") && X0Step.at(i)==0.) {
		  eff = 0.6846591;
		  e_eff = 0.009360672;
		}
		else if (TString(MCP).Contains("ZStack1") && X0Step.at(i)==0.) {
		  eff = 0.6325325;
		  e_eff = 0.009567964;
		}
		else if (TString(MCP).Contains("MiB3") && X0Step.at(i)==0.) {
		  eff = 0.5150362;
		  e_eff = 0.009750873;
		}
	      } 
	      if (TString(label).Contains("AngScan_HVHigh")) { //correct efficiencies at 0 degrees for angular scan
		if (TString(MCP).Contains("enSEE") ) {
		  eff -= 0.08;
		  //		  e_eff = 0.009360672;
		}
		else if (TString(MCP).Contains("ZStack1") ) {
		  eff -= 0.034;
		  //		  e_eff = 0.009567964;
		}
		else if (TString(MCP).Contains("MiB3") ) {
		  eff -= 0.08;
		  if (X0Step.at(i)==0.) eff-=0.012;
		  //		  e_eff = 0.009750873;
		}
	      }
	      printf("%.3f\t%.3f\t%.3f\t%.3f\n", X0Step.at(i), eff, 0., e_eff);
	      outputFile << X0Step.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
	      g_eff->SetPoint(i, X0Step.at(i), eff);
	      g_eff->SetPointError(i, 0, e_eff);
	    }
	    if(i == (ScanList.size()-1))    
		printf("-----------------------------\n");
	}
        //-----Charge study-----
        if(strcmp(doWhat, "Q") == 0)// || strcmp(doWhat, "all") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\tQ\te_%s\te_Q\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
            // sprintf(var_charge, "%s>>%s", TOT_diff, h_charge_name);
            sprintf(var_charge, "charge_corr[%d]>>%s", MCPNumber, h_charge_name);
            nt->Draw(var_charge, cut_scan && cut_tdc && cut_nFibers, "goff");
	    if(TString(scanType).Contains("HV") == 1) 
            {
                printf("%d\t%.0f\t%.0f\t%.0f\n", HVVal.at(i), h_charge->GetMean(), 0., h_charge->GetMeanError());
                outputFile << HVVal.at(i)<<"\t"<<h_charge->GetMean()<<"\t 0.\t"<<h_charge->GetMeanError()<<std::endl;
                g_Q->SetPoint(i, HVVal.at(i), h_charge->GetMean());
                g_Q->SetPoint(i, 0, h_charge->GetMeanError());
	    }
	    else 
            {
                printf("%.0f\t%.3f\t%.0f\t%.3f\n", X0Step.at(i), h_charge->GetMean(), 0., h_charge->GetMeanError());
                outputFile << X0Step.at(i)<<"\t"<<h_charge->GetMean()<<"\t 0.\t"<<h_charge->GetMeanError()<<std::endl;
                g_Q->SetPoint(i, X0Step.at(i), h_charge->GetMean());
                g_Q->SetPoint(i, 0, h_charge->GetMeanError());
	    }
	    if(i == (ScanList.size()-1))    
		printf("-----------------------------\n");
	}
        //-----TIME RESOLUTION STUDY----- 
        //---time resolution with CFD---
        if(strcmp(doWhat, "timeCFD") == 0 || strcmp(doWhat, "all") == 0)
	{
            //---print banner
            if(i == 0)
	    {
                printf("----------Time Resolution(ps)----------\n");
                printf(" #\t%s\tt_res\te_%s\te_t_res\tX_prob\n", var_name, var_name);
                printf("---------------------------------------\n");
	    }
            //---change TOT for X0 runs
            if(strcmp(scanType, "X0") == 0 && X0Step.at(i) != 0 && MCPNumber < 3) 
                sprintf(TOT_diff, "(time_stop_500[%d]-time_start_500[%d])", MCPNumber, MCPNumber);
            //---create variables
            char t_CF_diff[100];
	    if (strcmp((inverted_MCPList.at(MCPNumber)).c_str(),"MiB3")==0)
	      sprintf(t_CF_diff, "(time_CF_corr[%d]-(time_CF_corr[%d]-time_CF_corr[%d])-time_CF_corr[%d])", MCPNumber, clockPos2, clockPos1, trigPos1);
	    else
	      sprintf(t_CF_diff, "(time_CF_corr[%d]-time_CF_corr[%d])", MCPNumber, trigPos1);
	    sprintf(var_timeCFD_vs_TOT, "%s:%s>>%s", t_CF_diff, TOT_diff, pr_timeCFD_vs_TOT_name);
            //---correction
            nt->Draw(var_timeCFD_vs_TOT, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_timeCFD, "goff");
            //---skip run with low stat
            if(pr_timeCFD_vs_TOT->GetEntries() < 200)
                h_resCFD->Rebin(2);
            if(pr_timeCFD_vs_TOT->GetEntries() < 50) 
                continue;         
            pr_timeCFD_vs_TOT->Fit(f_corrCFD, "QR");    

	    //---draw res histo with corrections
	    sprintf(var_timeCFD, "%s-(%f + %f*%s + %f*%s*%s)>>%s",
		    t_CF_diff, f_corrCFD->GetParameter(0), f_corrCFD->GetParameter(1), TOT_diff,
		    f_corrCFD->GetParameter(2), TOT_diff, TOT_diff, h_resCFD_name);
                    //f_corrCFD->GetParameter(3), TOT_diff, TOT_diff, TOT_diff, h_resCFD_name);
            nt->Draw(var_timeCFD, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat && cut_bad_timeCFD, "goff");  
	    
	    //correction vs ampMax
	    sprintf(var_timeCFD_red, "(%s-(%f + %f*%s + %f*%s*%s))",
		    t_CF_diff, f_corrCFD->GetParameter(0), f_corrCFD->GetParameter(1), TOT_diff,
		    f_corrCFD->GetParameter(2), TOT_diff, TOT_diff);	    
            sprintf(var_timeCFD_vs_ampMax, "%s:amp_max_corr[%d]>>%s", var_timeCFD_red, MCPNumber, pr_timeCFD_vs_ampMaxCorr_name);
            nt->Draw(var_timeCFD_vs_ampMax, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_timeCFD, "goff");
	    //	    pr_timeCFD_vs_ampMaxCorr->Fit(f_corrCFD2, "QR");    

            //---draw res histo with corrections
	    /*	    sprintf(var_timeCFD, "%s-(%f + %f*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d])>>%s",
		    var_timeCFD_red, f_corrCFD2->GetParameter(0), f_corrCFD2->GetParameter(1), MCPNumber,
		    f_corrCFD2->GetParameter(2), MCPNumber, MCPNumber,
                    f_corrCFD2->GetParameter(3), MCPNumber, MCPNumber, MCPNumber,
		    f_corrCFD2->GetParameter(4), MCPNumber, MCPNumber, MCPNumber, MCPNumber, h_resCFD_name);
            nt->Draw(var_timeCFD, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat && cut_bad_timeCFD, "goff");  
	    */
            //---fit coincidence peak
            f_resCFD->SetParameters(h_resCFD->GetEntries(), h_resCFD->GetMean(), h_resCFD->GetRMS());
            f_resCFD->SetParLimits(1, -0.05, 0.05);
            f_resCFD->SetParLimits(2, 0.01, 0.5);
            h_resCFD->Fit(f_resCFD, "QRB");
            h_resCFD->Fit(f_resCFD, "QRBIM", "", f_resCFD->GetParameter(1)-2*f_resCFD->GetParameter(2),
                          f_resCFD->GetParameter(1)+2*f_resCFD->GetParameter(2));
            //---get resolution
            float e_t_res = f_resCFD->GetParError(2)*1000.;
            float t_res = f_resCFD->GetParameter(2)*1000.;
            float prob = f_resCFD->GetProb();
            //---print results + graph
            if(TString(scanType).Contains("HV") == 1) 
            {
                printf("%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i), t_res, 0., e_t_res, prob);
                outputFile << HVVal.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resCFD->SetPoint(g_resCFD->GetN(),  HVVal.at(i), t_res);
                g_resCFD->SetPointError(g_resCFD->GetN()-1, 0, e_t_res);
            } 
            else
            {  
                printf("%d\t%.3f\t%.1f\t%.1f\t%.1f\t%.3f\n", i, X0Step.at(i), t_res, 0., e_t_res, prob);
                outputFile << X0Step.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resCFD->SetPoint(g_resCFD->GetN(), X0Step.at(i), t_res);
                g_resCFD->SetPointError(g_resCFD->GetN()-1, 0, e_t_res);
            }
            if(i == (ScanList.size()-1))
                printf("---------------------------------------\n");
            //---write plots in the out file
            outROOT_CFD->cd();
            pr_timeCFD_vs_TOT->Write();
            pr_timeCFD_vs_ampMaxCorr->Write();
	    gStyle->SetOptStat(0000);
	    gStyle->SetOptFit(1111);
            h_resCFD->Write();
            pr_timeCFD_vs_TOT->Delete();
            pr_timeCFD_vs_ampMaxCorr->Delete();
            h_resCFD->Delete();
        }
        //---Time resolution with LED and TOT correction---
        if(strcmp(doWhat, "timeLED") == 0 || strcmp(doWhat, "all") == 0)
        {
            //---print banner
            if(i == 0)
            {        
                printf("----------Time Resolution(ps)----------\n");
                printf(" #\t%s\tt_res\te_%s\te_t_res\tX_prob\n", var_name, var_name);
                printf("---------------------------------------\n");
            }
            //---create variables
            char t_start_diff[100];
	    if (strcmp((inverted_MCPList.at(MCPNumber)).c_str(),"MiB3")==0)
	      sprintf(t_start_diff, "(time_start_150[%d]-(time_start_150[%d]-time_start_150[%d])-time_CF_corr[%d])", MCPNumber, clockPos2, clockPos1, trigPos1);
	    else
	      sprintf(t_start_diff, "(time_start_150[%d]-time_CF_corr[%d])", MCPNumber, trigPos1);
            //---change TOT for X0 runs
            if(strcmp(scanType, "X0") == 0 && X0Step.at(i) != 0 && MCPNumber < 3) 
            {
                sprintf(TOT_diff, "(time_stop_500[%d]-time_start_500[%d])", MCPNumber, MCPNumber);
		if (strcmp((inverted_MCPList.at(MCPNumber)).c_str(),"MiB3")==0)
		  sprintf(t_start_diff, "(time_start_500[%d]-(time_start_500[%d]-time_start_500[%d])-time_CF_corr[%d])", MCPNumber,clockPos2,clockPos1,trigPos1);
		else
		  sprintf(t_start_diff, "(time_start_500[%d]-time_CF_corr[%d])", MCPNumber,trigPos1);
            }
            sprintf(var_timeLED_vs_TOT, "%s:%s>>%s", t_start_diff, TOT_diff, pr_timeLED_vs_TOT_name);
            //---correction
            nt->Draw(var_timeLED_vs_TOT, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_timeLED, "goff");
            //---skip run with low stat
            if(pr_timeLED_vs_TOT->GetEntries() < 200)
                h_resLED->Rebin(2);
            if(pr_timeLED_vs_TOT->GetEntries() < 50)
                continue;         
            pr_timeLED_vs_TOT->Fit(f_corrLED, "QR");    
            
	    //---draw res histo with corrections
	    sprintf(var_timeLED, "%s-(%f + %f*%s + %f*%s*%s)>>%s",
                    t_start_diff, f_corrLED->GetParameter(0), f_corrLED->GetParameter(1), TOT_diff,
                    f_corrLED->GetParameter(2), TOT_diff, TOT_diff, h_resLED_name);
            nt->Draw(var_timeLED, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat && cut_bad_timeLED, "goff");  
	    
            sprintf(var_timeLED_red, "(%s-(%f + %f*%s + %f*%s*%s))",
                    t_start_diff, f_corrLED->GetParameter(0), f_corrLED->GetParameter(1), TOT_diff,
                    f_corrLED->GetParameter(2), TOT_diff, TOT_diff);
            sprintf(var_timeLED_vs_ampMax, "%s:amp_max_corr[%d]>>%s", var_timeLED_red, MCPNumber, pr_timeLED_vs_ampMaxCorr_name);
            nt->Draw(var_timeLED_vs_ampMax, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_timeLED, "goff");
	    //	    pr_timeLED_vs_ampMaxCorr->Fit(f_corrLED2, "QR");    

            //---draw res histo with corrections
	    /*	    sprintf(var_timeLED, "%s-(%f + %f*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d])>>%s",
		    var_timeLED_red, f_corrLED2->GetParameter(0), f_corrLED2->GetParameter(1), MCPNumber,
		    f_corrLED2->GetParameter(2), MCPNumber, MCPNumber,
                    f_corrLED2->GetParameter(3), MCPNumber, MCPNumber, MCPNumber, 
		    f_corrLED2->GetParameter(4), MCPNumber, MCPNumber, MCPNumber, MCPNumber, h_resLED_name);
            nt->Draw(var_timeLED, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat && cut_bad_timeLED, "goff");  
	    */
            //---fit coincidence peak
            f_resLED->SetParameters(h_resLED->GetEntries(), h_resLED->GetMean(), h_resLED->GetRMS());
            f_resLED->SetParLimits(1, -0.05, 0.05);
            f_resLED->SetParLimits(2, 0.02, 0.5);
            h_resLED->Fit(f_resLED, "QRB"); 
            f_resLED->SetParLimits(2, 0.01, 0.4);
            h_resLED->Fit(f_resLED, "QRBIM", "", f_resLED->GetParameter(1)-2*f_resLED->GetParameter(2),
                          f_resLED->GetParameter(1)+2*f_resLED->GetParameter(2));
            //---get resolution
            float e_t_res = f_resLED->GetParError(2)*1000.;
            float t_res = f_resLED->GetParameter(2)*1000.;
            float prob = f_resLED->GetProb();
            //---print results + graph
            if(TString(scanType).Contains("HV") == 1) 
            {
                printf("%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i), t_res, 0., e_t_res, prob);
                outputFile << HVVal.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resLED->SetPoint(g_resLED->GetN(),  HVVal.at(i), t_res);
                g_resLED->SetPointError(g_resLED->GetN()-1, 0, e_t_res);
            } 
            else
            {  
                printf("%d\t%.3f\t%.1f\t%.1f\t%.1f\t%.3f\n", i, X0Step.at(i), t_res, 0., e_t_res, prob);
                outputFile << X0Step.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resLED->SetPoint(g_resLED->GetN(), X0Step.at(i), t_res);
                g_resLED->SetPointError(g_resLED->GetN()-1, 0, e_t_res);
            }
            if(i == (ScanList.size()-1))
                printf("---------------------------------------\n");
            //---write plots in the out file
            outROOT_LED->cd();
            pr_timeLED_vs_TOT->Write();
            pr_timeLED_vs_ampMaxCorr->Write();
	    gStyle->SetOptStat(0000);
	    gStyle->SetOptFit(1111);
            h_resLED->Write();
            pr_timeLED_vs_TOT->Delete();
            pr_timeLED_vs_ampMaxCorr->Delete();
            h_resLED->Delete();
        }
    }

    //---Save global scan graph---
    //---efficiency
    if(outROOT_eff)
    {
        outROOT_eff->cd();
        g_eff->Write();
        g_frac_saturated->Write();
        outROOT_eff->Close();
    }
    //---charge
    if(outROOT_Q)
    {
        outROOT_Q->cd();
        g_Q->Write();
        g_frac_saturated->Write();
        outROOT_Q->Close();
    }
    //---time resolution CFD
    if(outROOT_CFD)
    {
        outROOT_CFD->cd();
        g_resCFD->Write();
        g_frac_saturated->Write();
        outROOT_CFD->Close();
    }
    //---time resolution LED
    if(outROOT_LED)
    {
        outROOT_LED->cd();
        g_resLED->Write();
        g_frac_saturated->Write();
        outROOT_LED->Close();
    }
    
 //
      std::cout<<"results printed in results/"<<std::endl;
      //  outputFile.close();
  if (strcmp(scanType,"HODO")==0){
     h_XDiff_Hodo1TDC->Write();
     h_YDiff_Hodo1TDC->Write();
     h_XDiff_Hodo2TDC->Write();
     h_YDiff_Hodo2TDC->Write();
     h_XDiff_Hodo12->Write();
     h_YDiff_Hodo12->Write();
     h_XDiff_Hodo1TDC_c->Write();
     h_YDiff_Hodo1TDC_c->Write();
     h_XDiff_Hodo2TDC_c->Write();
     h_YDiff_Hodo2TDC_c->Write();
     h_XDiff_Hodo12_c->Write();
     h_YDiff_Hodo12_c->Write();
     h_nX1->Write();
     h_nY1->Write();
     h_nX2->Write();
     h_nY2->Write();
     h_pX1->Write();
     h_pY1->Write();
     h_TDCX->Write();
     h_TDCY->Write();
     h_Corr_TDC_HODO1_X->Write();
     h_Corr_TDC_HODO1_Y->Write();
     //Eff Hodos
     h_N_Trig1_hodo2_vsX->Write(); h_N_Trig1_vsX->Write();
     h_N_Trig1_hodo2_vsY->Write(); h_N_Trig1_vsY->Write();
     h_N_Trig2_hodo1_vsX->Write(); h_N_Trig2_vsX->Write();
     h_N_Trig2_hodo1_vsY->Write(); h_N_Trig2_vsY->Write();
     h_N_Trig1_TDC_vsX->Write();   h_N_Trig1Tdc_vsX->Write();
     h_N_Trig1_TDC_vsY->Write();   h_N_Trig1Tdc_vsY->Write();
     TG_effHODO2_X->GetXaxis()->SetTitle("HODO2 X [mm]"); TG_effHODO2_X->GetYaxis()->SetTitle("Eff."); TG_effHODO2_X->SetMinimum(0); TG_effHODO2_X->SetMaximum(1);
     TG_effHODO2_Y->GetXaxis()->SetTitle("HODO2 Y [mm]"); TG_effHODO2_Y->GetYaxis()->SetTitle("Eff."); TG_effHODO2_Y->SetMinimum(0); TG_effHODO2_Y->SetMaximum(1);
     TG_effHODO1_X->GetXaxis()->SetTitle("HODO1 X [mm]"); TG_effHODO1_X->GetYaxis()->SetTitle("Eff."); TG_effHODO1_X->SetMinimum(0); TG_effHODO1_X->SetMaximum(1);
     TG_effHODO1_Y->GetXaxis()->SetTitle("HODO1 Y [mm]"); TG_effHODO1_Y->GetYaxis()->SetTitle("Eff."); TG_effHODO1_Y->SetMinimum(0); TG_effHODO1_Y->SetMaximum(1);
     TG_effTDC_X->GetXaxis()->SetTitle("TDC X [mm]"); TG_effTDC_X->GetYaxis()->SetTitle("Eff."); TG_effTDC_X->SetMinimum(0); TG_effTDC_X->SetMaximum(1);
     TG_effTDC_Y->GetXaxis()->SetTitle("TDC Y [mm]"); TG_effTDC_Y->GetYaxis()->SetTitle("Eff."); TG_effTDC_Y->SetMinimum(0); TG_effTDC_Y->SetMaximum(1);
     TG_effHODO2_X->Write();
     TG_effHODO2_Y->Write();
     TG_effHODO1_X->Write();
     TG_effHODO1_Y->Write();
     TG_effTDC_X->Write();
     TG_effTDC_Y->Write();
     //Eff MCP vs hodo2
     h_TriggM2->Write();
     h_TriggM3->Write();
     h_TriggZ1->Write();
     h_TriggZ2->Write();
     h_TriggSe->Write();
     h_NumEff_M2->Write();
     h_NumEff_M3->Write();
     h_NumEff_Z1->Write();
     h_NumEff_Z2->Write();
     h_NumEff_Se->Write();
     for(int i=0; i<32; i++){
	for(int j=0; j<32; j++){
//cout<<"ECCO: "<<h_NumEff_M3->GetBinContent(i+1, j+1)  <<" / "<< h_TriggM2->GetBinContent(i+1, j+1)<<endl;
	  if( h_TriggM2->GetBinContent(i+1, j+1) > 10 ) h_Eff_vsH2_M2->SetBinContent( i+1, j+1, h_NumEff_M2->GetBinContent(i+1, j+1)  / h_TriggM2->GetBinContent(i+1, j+1) );
	  if( h_TriggM3->GetBinContent(i+1, j+1) > 10 ) h_Eff_vsH2_M3->SetBinContent( i+1, j+1, h_NumEff_M3->GetBinContent(i+1, j+1)  / h_TriggM3->GetBinContent(i+1, j+1) );
	  if( h_TriggZ1->GetBinContent(i+1, j+1) > 10 ) h_Eff_vsH2_Z1->SetBinContent( i+1, j+1, h_NumEff_Z1->GetBinContent(i+1, j+1)  / h_TriggZ1->GetBinContent(i+1, j+1) );
	  if( h_TriggZ2->GetBinContent(i+1, j+1) > 10 ) h_Eff_vsH2_Z2->SetBinContent( i+1, j+1, h_NumEff_Z2->GetBinContent(i+1, j+1)  / h_TriggZ2->GetBinContent(i+1, j+1) );
	  if( h_TriggSe->GetBinContent(i+1, j+1) > 10 ) h_Eff_vsH2_Se->SetBinContent( i+1, j+1, h_NumEff_Se->GetBinContent(i+1, j+1)  / h_TriggSe->GetBinContent(i+1, j+1) );
	}
     }
     h_Eff_vsH2_M2->Write();
     h_Eff_vsH2_M3->Write();
     h_Eff_vsH2_Z1->Write();
     h_Eff_vsH2_Z2->Write();
     h_Eff_vsH2_Se->Write();
  }


      inFile->Close();
  	
      return 0;     
}

