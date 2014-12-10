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
    int trigPos1 = -1;  //positions of the two trigger chambers in the reco tree 
    //---Open cfg file and fill map with treshold for each channel---
    while(!inputCfg.eof())
    {
	inputCfg >> ch >> tresh;
	treshold.insert(std::make_pair(ch,tresh));
	nChannels++;
    }
    //---Open reco tree---
    std::string inFileName = "ntuples/reco_"+string(label)+".root";
    TFile *inFile = new TFile (inFileName.c_str());
    TTree* nt = (TTree*)inFile->Get("reco_tree");
    InitRecoTree(nt);

    //-----Create output files-----
    //---txt---
    char outputFileName[200]="";
    sprintf(outputFileName, "results/%s_%s_%s_%s.txt", MCP.c_str(), doWhat, scanType, label);
    std::ofstream outputFile (outputFileName, std::ofstream::out);
    //---ROOT---
    char mkdir_command[50];
    TFile* outROOT_eff = NULL;
    TFile* outROOT_Q = NULL;
    TFile* outROOT_CFD = NULL;
    TFile* outROOT_LED = NULL;
    //---efficiency
    if(strcmp(doWhat, "eff") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "mkdir plots/efficiency_studies");
        system(mkdir_command);         
        outROOT_eff = TFile::Open(Form("plots/efficiency_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }
    //---charge
    if(strcmp(doWhat, "Q") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "mkdir plots/charge_studies");
        system(mkdir_command);         
        outROOT_Q = TFile::Open(Form("plots/charge_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }    
    //---time resolution CFD
    if(strcmp(doWhat, "timeCFD") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "mkdir plots/resCFD_studies");
        system(mkdir_command);         
        outROOT_CFD = TFile::Open(Form("plots/resCFD_studies/%s_%s_%s.root", MCP.c_str(), scanType, label), "recreate");
    }
    //---time resolution LED
    if(strcmp(doWhat, "timeLED") == 0 || strcmp(doWhat, "all") == 0)
    {
        sprintf(mkdir_command, "mkdir plots/resLED_studies");
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

    //---Output Histos HODO---
    TH1F *h_hodoX1   = new TH1F( "h_hodoX1", "", 64, -32., 32.); h_hodoX1->GetXaxis()->SetTitle("HODO X1");
    TH1F *h_hodoY1   = new TH1F( "h_hodoY1", "", 64, -32., 32.); h_hodoY1->GetXaxis()->SetTitle("HODO Y1");
    TH1F *h_hodoX2   = new TH1F( "h_hodoX2", "", 64, -32., 32.); h_hodoX2->GetXaxis()->SetTitle("HODO X2");
    TH1F *h_hodoY2   = new TH1F( "h_hodoY2", "", 64, -32., 32.); h_hodoY2->GetXaxis()->SetTitle("HODO Y2");
    TH1F *h_tdcX     = new TH1F( "h_tdcX", "", 64, -32., 32.); h_tdcX->GetXaxis()->SetTitle("Wire Chamber X");
    TH1F *h_tdcY     = new TH1F( "h_tdcY", "", 64, -32., 32.); h_tdcY->GetXaxis()->SetTitle("Wire Chamber Y");
    TH1F *h_hodoX1_c = new TH1F( "h_hodoX1_c", "", 64, -32., 32.); h_hodoX1_c->GetXaxis()->SetTitle("HODO X1");
    TH1F *h_hodoY1_c = new TH1F( "h_hodoY1_c", "", 64, -32., 32.); h_hodoY1_c->GetXaxis()->SetTitle("HODO Y1");
    TH1F *h_hodoX2_c = new TH1F( "h_hodoX2_c", "", 64, -32., 32.); h_hodoX2_c->GetXaxis()->SetTitle("HODO X2");
    TH1F *h_hodoY2_c = new TH1F( "h_hodoY2_c", "", 64, -32., 32.); h_hodoY2_c->GetXaxis()->SetTitle("HODO Y2");
    TH1F *h_tdcX_c   = new TH1F( "h_tdcX_c", "", 64, -32., 32.); h_tdcX_c->GetXaxis()->SetTitle("Wire Chamber X");
    TH1F *h_tdcY_c   = new TH1F( "h_tdcY_c", "", 64, -32., 32.); h_tdcY_c->GetXaxis()->SetTitle("Wire Chamber Y");

//----------------------------------------------------------------------------------------
//------Get HV / X0 / HODO and MCP channels-----------------------------------------------

    if (strcmp(scanType,"HV")==0) 
    {
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
    else if (strcmp(scanType,"HODO")==0) 
    {
        int prev=0;
        float x0_HODO1 = -7.69582, y0_HODO1 = 0.322465; //From macro/fitHodo.C
        float x0_HODO2 = -2.44197, y0_HODO2 = 0.534708; //From macro/fitHodo.C
        float x0_WC = -4.21425, y0_WC = 2.59668;        //From macro/fitHodo.C
        for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++){
            nt->GetEntry(iEntry);
            bool sele ( true );//run_id==610 );
            if( sele ){
                for(int i=0; i<64; i++){
                    if( (i+1-int(x0_HODO1)) > 1 && (i+1-int(x0_HODO1)) < 64 ) h_hodoX1_c->SetBinContent(i+1-int(x0_HODO1), h_hodoX1_c->GetBinContent(i+1-int(x0_HODO1)) + hodoX1[i] );
                    if( (i+1-int(y0_HODO1)) > 1 && (i+1-int(y0_HODO1)) < 64 ) h_hodoY1_c->SetBinContent(i+1-int(y0_HODO1), h_hodoY1_c->GetBinContent(i+1-int(y0_HODO1)) + hodoY1[i] );
                    if( (i+1-int(x0_HODO2)) > 1 && (i+1-int(x0_HODO2)) < 64 ) h_hodoX2_c->SetBinContent(i+1-int(x0_HODO2), h_hodoX2_c->GetBinContent(i+1-int(x0_HODO2)) + hodoX2[i] );
                    if( (i+1-int(y0_HODO2)) > 1 && (i+1-int(y0_HODO2)) < 64 ) h_hodoY2_c->SetBinContent(i+1-int(y0_HODO2), h_hodoY2_c->GetBinContent(i+1-int(y0_HODO2)) + hodoY2[i] );
                    h_hodoX1->SetBinContent(i+1, h_hodoX1->GetBinContent(i+1) + hodoX1[i] );
                    h_hodoY1->SetBinContent(i+1, h_hodoY1->GetBinContent(i+1) + hodoY1[i] );
                    h_hodoX2->SetBinContent(i+1, h_hodoX2->GetBinContent(i+1) + hodoX2[i] );
                    h_hodoY2->SetBinContent(i+1, h_hodoY2->GetBinContent(i+1) + hodoY2[i] );
                }
                h_tdcX->Fill( tdcX );
                h_tdcY->Fill( tdcY );
                h_tdcX_c->Fill( tdcX-x0_WC );
                h_tdcY_c->Fill( tdcY-y0_WC );
            }
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
    }
    else 
    {
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

    //---Results graphs---
    TGraphErrors *g_eff = new TGraphErrors(ScanList.size());
    g_eff->SetName("eff");

    TGraphErrors *g_Q = new TGraphErrors(ScanList.size());
    g_Q->SetName("Q");

    TGraphErrors *g_resCFD = new TGraphErrors(ScanList.size());
    g_resCFD->SetName("resCFD");

    TGraphErrors *g_resLED = new TGraphErrors(ScanList.size());
    g_resLED->SetName("resLED");

    TGraphErrors* g_frac_saturated = new TGraphErrors(ScanList.size());
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
    //---time resolution
    char var_timeCFD[100]="";
    char var_timeCFD_vs_TOT[1000] = "";
    char var_timeLED[100] = "";
    char var_timeLED_vs_TOT[1000] = "";
    //---cut strings
    char str_cut_sig[200]="";
    char str_cut_trig0[200]="";
    char str_cut_tdc[200]="";
    char str_cut_saturated[200]="";
    char str_cut_nFibers[200]="";
    char str_cut_trig_not_sat[200]="";
    char str_cut_bad_time[200]="";
    //---Define Cuts---
    sprintf(str_cut_sig, "charge_corr[%d] > %d", MCPNumber, treshold.at(MCPNumber));
    sprintf(str_cut_trig0, "charge_corr[%d] > %d", trigPos1, treshold.at(trigPos1));
    sprintf(str_cut_tdc, "tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6");
    sprintf(str_cut_saturated, "amp_max[%d] > 3450", MCPNumber);
    sprintf(str_cut_nFibers, "nhodoX1<=3 && nhodoX2<=3 && nhodoY1<=3 && nhodoY2<=3");
    sprintf(str_cut_trig_not_sat, "amp_max[%d] < 3450", trigPos1); 
    if (MCPList.at(MCP)==4)
    {
        sprintf(str_cut_bad_time, "time_start_150[%d] != 20 && time_start_150[%d] != 20 && "
                "(time_start_150[%d]-time_CF[%d])<-3/5*(time_stop_150[%d]-time_start_150[%d])",
                MCPNumber, trigPos1, MCPNumber, trigPos1, MCPNumber, MCPNumber);
    }
    else
    {
        sprintf(str_cut_bad_time, "time_start_150[%d] != 20 && time_start_150[%d] != 20 && "
                "(time_start_150[%d]-time_CF[%d])<-1/3*(time_stop_150[%d]-time_start_150[%d])+2",
                MCPNumber, trigPos1, MCPNumber, trigPos1, MCPNumber, MCPNumber);
    }     
    //---construct TCut
    TCut cut_sig = str_cut_sig;
    TCut cut_trig0 = str_cut_trig0;
    TCut cut_tdc = str_cut_tdc;
    TCut cut_saturated = str_cut_saturated;
    TCut cut_nFibers = str_cut_nFibers;
    TCut cut_trig_not_sat = str_cut_trig_not_sat;
    TCut cut_bad_time = str_cut_bad_time;

//-------Runs loop------------------------------------------------------------------------
    for(unsigned int i=0; i<ScanList.size(); i++)
    {
        //---Define run dependend cut---
        char cut_scan[100];
        if (strcmp(scanType,"HV")==0)  
            sprintf(cut_scan, "HV[%d] == %d", MCPNumber, HVVal.at(i));
        else  
//            sprintf(cut_scan, "X0 > %f && X0 < %f", X0Step.at(i)-0.0001, X0Step.at(i)+0.0001); //"X0==%f" does not work, don't know why 
            sprintf(cut_scan, "X0 == %f", X0Step.at(i));
        if(MCPNumber == 2) 
            sprintf(str_cut_saturated, "run_id > 796 && amp_max[%d] > 4000", MCPNumber);
        //---and print infos
        char var_name[3] = "X0";
        if(TString(scanType).Contains("HV") == 1)    
            sprintf(var_name, "HV");

        //-------define histos------------------------------------------------------------
        //-----create objects names-----
        char h_sig_name[20], h_trig0_name[20];
        // char h_dt_vs_ampM_name[20], h_dtStart_vs_ampM_name[20], h_dtStart_vs_Tot_name[20];
        char h_fracSat_name[20], h_evtAll_name[20];
        char pr_timeCFD_vs_TOT_name[20], h_resCFD_name[20], f_resCFD_name[20], f_corrCFD_name[20];
        char pr_timeLED_vs_TOT_name[20], h_resLED_name[20], f_resLED_name[20], f_corrLED_name[20];
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
        // sprintf(h_dt_vs_ampM_name, "h_dt_vs_ampM_%d", i);
        // sprintf(h_dtStart_vs_ampM_name, "h_dtStart_vs_ampM_%d", i);
        // sprintf(h_dtStart_vs_Tot_name, "h_dtStart_vs_Tot_%d", i);      
        // TH2F* h_dt_vs_ampM = new TH2F(h_dt_vs_ampM_name, h_dt_vs_ampM_name, 10000, 0., 10000., 1000, -5., 5.);
        // TH2F* h_dtStart_vs_ampM = new TH2F(h_dtStart_vs_ampM_name, h_dtStart_vs_ampM_name, 10000, 0., 10000., 100, -10., 10.);
        // TH2F* h_dtStart_vs_Tot = new TH2F(h_dtStart_vs_Tot_name, h_dtStart_vs_Tot_name, 1000, 0., 50., 100, -10., 10.);
        //---timing---
        //---time CFD/TOT corrected                          
        sprintf(pr_timeCFD_vs_TOT_name, "pr_timeCFD_vs_TOT_%d", i);
        sprintf(h_resCFD_name, "h_resCFD_%d", i);  
        sprintf(f_resCFD_name, "f_resCFD_%d", i);    
        sprintf(f_corrCFD_name, "f_corrCFD_%d", i);
        TH1F* h_resCFD = new TH1F(h_resCFD_name, "time res with CFD method", 250, -1, 1);
        TF1* f_resCFD = new TF1(f_resCFD_name, "gausn", -1, 1);
        TProfile* pr_timeCFD_vs_TOT;
        TF1* f_corrCFD;
        if(strcmp(scanType, "HV") == 0)
        {                              
            pr_timeCFD_vs_TOT = new TProfile(pr_timeCFD_vs_TOT_name, "timeCF vs TOT difference",
                                          25, 0, 5, -5, 2);
            f_corrCFD = new TF1(f_corrCFD_name, "pol2", 0, 5);
        }
        else
        {
            pr_timeCFD_vs_TOT = new TProfile(pr_timeCFD_vs_TOT_name, "timeCF vs TOT difference",
                                          30, 0, 10, -5, 2);
            f_corrCFD = new TF1(f_corrCFD_name, "pol2", 0, 10);
        }
        pr_timeCFD_vs_TOT->SetMarkerStyle(20);
        pr_timeCFD_vs_TOT->SetMarkerSize(0.7);
        //---time LED/TOT corrected                          
        sprintf(pr_timeLED_vs_TOT_name, "pr_timeLED_vs_TOT_%d", i);
        sprintf(h_resLED_name, "h_resLED_%d", i);  
        sprintf(f_resLED_name, "f_resLED_%d", i);    
        sprintf(f_corrLED_name, "f_corrLED_%d", i);
        TH1F* h_resLED = new TH1F(h_resLED_name, "time res with LED method", 250, -1, 1);
        TF1* f_resLED = new TF1(f_resLED_name, "gausn", -1, 1);
        TProfile* pr_timeLED_vs_TOT;
        TF1* f_corrLED;
        if(strcmp(scanType, "HV") == 0)
        {                              
            pr_timeLED_vs_TOT = new TProfile(pr_timeLED_vs_TOT_name, "time vs TOT difference",
                                          25, 0, 5, -5, 2);
            f_corrLED = new TF1(f_corrLED_name, "pol2", 0, 5);
        }
        else
        {
            pr_timeLED_vs_TOT = new TProfile(pr_timeLED_vs_TOT_name, "time vs TOT difference",
                                          30, 0, 10, -5, 2);
            f_corrLED = new TF1(f_corrLED_name, "pol2", 0, 10);
        }
        pr_timeLED_vs_TOT->SetMarkerStyle(20);
        pr_timeLED_vs_TOT->SetMarkerSize(0.7);

        //-----Saturated event computation----
        sprintf(var_fracSaturated, "time_CF[%d] >> %s", MCPNumber, h_fracSat_name); 
        sprintf(var_evtAll, "time_CF[%d] >> %s", MCPNumber, h_evtAll_name); 
        nt->Draw(var_fracSaturated, cut_trig0 && cut_sig && cut_scan && cut_saturated && cut_tdc && cut_nFibers && cut_trig_not_sat);
        nt->Draw(var_evtAll, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers && cut_trig_not_sat);
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
            nt->Draw(var_sig, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers && cut_trig_not_sat, "goff");
            nt->Draw(var_trig0, cut_trig0 && cut_scan && cut_tdc && cut_nFibers && cut_trig_not_sat, "goff");
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
                printf("%.3f\t%.3f\t%.3f\t%.3f\n", X0Step.at(i), eff, 0., e_eff);
                outputFile << X0Step.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
                g_eff->SetPoint(i, X0Step.at(i), eff);
                g_eff->SetPointError(i, 0, e_eff);
	    }
	    if(i == (ScanList.size()-1))    
		printf("-----------------------------\n");
	}
        //-----Charge study-----
        if(strcmp(doWhat, "Q") == 0 || strcmp(doWhat, "all") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\tQ\te_%s\te_Q\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    nt->Draw(var_sig, cut_scan);
	    if(TString(scanType).Contains("HV") == 1) 
            {
                printf("%d\t%.0f\t%.0f\t%.0f\n", HVVal.at(i), h_sig->GetMean(), 0., h_sig->GetMeanError());
                outputFile << HVVal.at(i)<<"\t"<<h_sig->GetMean()<<"\t 0.\t"<<h_sig->GetMeanError()<<std::endl;
                g_Q->SetPoint(i, HVVal.at(i), h_sig->GetMean());
                g_Q->SetPoint(i, 0, h_sig->GetMeanError());
	    }
	    else 
            {
                printf("%.3f\t%.0f\t%.0f\t%.0f\n", X0Step.at(i), h_sig->GetMean(), 0., h_sig->GetMeanError());
                outputFile << X0Step.at(i)<<"\t"<<h_sig->GetMean()<<"\t 0.\t"<<h_sig->GetMeanError()<<std::endl;
                g_Q->SetPoint(i, X0Step.at(i), h_sig->GetMean());
                g_Q->SetPoint(i, 0, h_sig->GetMeanError());
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
                printf(" %s\tt_res\te_%s\te_t_res\tX_prob\n", var_name, var_name);
                printf("---------------------------------------\n");
	    }
            //---create variables
            char t_CF_diff[100], t_OT_diff[100];
            sprintf(t_CF_diff, "(time_CF[%d]-time_CF[%d])", MCPNumber, trigPos1);
            sprintf(t_OT_diff, "(time_stop_150[%d]-time_start_150[%d])", MCPNumber, MCPNumber);        
            sprintf(var_timeCFD_vs_TOT, "%s:%s>>%s", t_CF_diff, t_OT_diff, pr_timeCFD_vs_TOT_name);
            //---correction
            nt->Draw(var_timeCFD_vs_TOT, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat, "goff");
            //---skip run with low stat
            if(pr_timeCFD_vs_TOT->GetEntries() < 100) 
                continue;         
            pr_timeCFD_vs_TOT->Fit(f_corrCFD, "QR");    
            //---draw res histo with corrections
            sprintf(var_timeCFD, "%s-(%f + %f*%s + %f*%s*%s)>>%s",
                    t_CF_diff, f_corrCFD->GetParameter(0), f_corrCFD->GetParameter(1), t_OT_diff,
                    f_corrCFD->GetParameter(2), t_OT_diff, t_OT_diff, h_resCFD_name);
            nt->Draw(var_timeCFD, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat, "goff");  
            //---fit coincidence peak
            f_resCFD->SetParameters(h_resCFD->GetEntries(), 0, 0.05);
            f_resCFD->SetParLimits(1, -0.03, 0.03);
            f_resCFD->SetParLimits(2, 0.01, 0.5);
            h_resCFD->Fit(f_resCFD, "QR");
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
                g_resCFD->SetPoint(i,  HVVal.at(i), t_res);
                g_resCFD->SetPointError(i, 0, e_t_res);
            } 
            else
            {  
                printf("%d\t%.3f\t%.1f\t%.1f\t%.1f\t%.3f\n", i, X0Step.at(i), t_res, 0., e_t_res, prob);
                outputFile << X0Step.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resCFD->SetPoint(i, X0Step.at(i), t_res);
                g_resCFD->SetPointError(i, 0, e_t_res);
            }
            if(i == (ScanList.size()-1))
                printf("---------------------------------------\n");
            //---write plots in the out file
            outROOT_CFD->cd();
            pr_timeCFD_vs_TOT->Write();
            h_resCFD->Write();
            pr_timeCFD_vs_TOT->Delete();
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
            char t_start_diff[100], t_OT_diff[100];
            sprintf(t_start_diff, "(time_start_150[%d]-time_CF[%d])", MCPNumber, trigPos1);
            sprintf(t_OT_diff, "(time_stop_150[%d]-time_start_150[%d])", MCPNumber, MCPNumber);        
            sprintf(var_timeLED_vs_TOT, "%s:%s>>%s", t_start_diff, t_OT_diff, pr_timeLED_vs_TOT_name);
            //---correction
            nt->Draw(var_timeLED_vs_TOT, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_time, "goff");
            //---skip run with low stat
            if(pr_timeLED_vs_TOT->GetEntries() < 100) 
                continue;         
            pr_timeLED_vs_TOT->Fit(f_corrLED, "QR");    
            //---draw res histo with corrections
            sprintf(var_timeLED, "%s-(%f + %f*%s + %f*%s*%s)>>%s",
                    t_start_diff, f_corrLED->GetParameter(0), f_corrLED->GetParameter(1), t_OT_diff,
                    f_corrLED->GetParameter(2), t_OT_diff, t_OT_diff, h_resLED_name);
            nt->Draw(var_timeLED, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat && cut_bad_time, "goff");  
            //---fit coincidence peak
            f_resLED->SetParameters(h_resLED->GetEntries(), 0, 0.05);
            f_resLED->SetParLimits(1, -0.03, 0.03);
            f_resLED->SetParLimits(2, 0.01, 0.5);
            h_resLED->Fit(f_resLED, "QR");
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
                g_resLED->SetPoint(i,  HVVal.at(i), t_res);
                g_resLED->SetPointError(i, 0, e_t_res);
            } 
            else
            {  
                printf("%d\t%.3f\t%.1f\t%.1f\t%.1f\t%.3f\n", i, X0Step.at(i), t_res, 0., e_t_res, prob);
                outputFile << X0Step.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resLED->SetPoint(i, X0Step.at(i), t_res);
                g_resLED->SetPointError(i, 0, e_t_res);
            }
            if(i == (ScanList.size()-1))
                printf("---------------------------------------\n");
            //---write plots in the out file
            outROOT_LED->cd();
            pr_timeLED_vs_TOT->Write();
            h_resLED->Write();
            pr_timeLED_vs_TOT->Delete();
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
    // //---hodo
    // if(strcmp(scanType,"HODO")==0)
    // {
    //     h_hodoX1->Write();
    //     h_hodoY1->Write();
    //     h_hodoX2->Write();
    //     h_hodoY2->Write();
    //     h_tdcX->Write();
    //     h_tdcY->Write();
    //     h_hodoX1_c->Write();
    //     h_hodoY1_c->Write();
    //     h_hodoX2_c->Write();
    //     h_hodoY2_c->Write();
    //     h_tdcX_c->Write();
    //     h_tdcY_c->Write();
    // }
    inFile->Close();  
    return 0;     

}

