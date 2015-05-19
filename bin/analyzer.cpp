/*****************************************************************************************
    This program compute charge or efficiency for all the run in a selected Scan for
    selected MCP under test.

NEW - FOR BTF:
- options "HV1" means HV1 varying while HV2 fixed
- options "HV2" means HV2 varying while HV1 fixed
- options "HV12" means both HV varying (use HV1-HV2 as variable)

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
    int trigPos1 = -1;  //position of the trigger chamber in the reco tree 
    int trigPos2 = -1;  //position of 2nd the trigger chamber in the reco tree 
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
    std::vector<int> HV2Val;
    HV2Val.clear();
    std::vector<float> X0Step;
    X0Step.clear();
    std::vector<float> ScanList;
    ScanList.clear();

  //---save list of HV (or X0) step---
  if (strcmp(scanType,"HV1")==0 || strcmp(scanType,"HV2")==0 || strcmp(scanType,"HV12")==0) {
    int prev=0, prev2=0;  
    for (int iEntry=0; iEntry<nt->GetEntries(); iEntry++)
      {
	nt->GetEntry(iEntry);
	if (HV[MCPNumber]!=prev || HV2[MCPNumber]!=prev2) {
	  ScanList.push_back((float)HV[MCPNumber]);
	  HVVal.push_back(HV[MCPNumber]);
	  HV2Val.push_back(HV2[MCPNumber]);
	  X0Step.push_back(X0);
	  prev=HV[MCPNumber];
	  prev2=HV2[MCPNumber];
	  if (iEntry==0) {
	    for (int i=0; i<nChannels; i++)  //save trigger position!
	      {
		if (isTrigger[i]==1)       trigPos1 = i;
		if (isTrigger[i]==2)       trigPos2 = i;
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
	  HV2Val.push_back(HV2[MCPNumber]);
	  X0Step.push_back(X0);
	  prev=X0;
	  if (iEntry==0) {
	    for (int i=0; i<nChannels; i++)  //save trigger position!!
	      {
		if (isTrigger[i]==1)       trigPos1 = i;
		if (isTrigger[i]==2)       trigPos2 = i;
	      }
	  }
	}
      }
  }
  
  clockPos1 = MCPList.at("clock1");
  clockPos2 = MCPList.at("clock2");

  //  if (trigPos1==-1 || trigPos2==-1 || clockPos1==-1 || clockPos2==-1) {
  if (trigPos1==-1) {
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
    char str_cut_multiplicity[500]="";
    char str_cut_noisePeak[500]="";
    //---Define Cuts---
    //    sprintf(str_cut_sig, "charge_corr[%d] > %d", MCPNumber, treshold.at(MCPNumber));
    //    sprintf(str_cut_trig0, "charge_corr[%d] > %d", trigPos1, treshold.at(trigPos1));
    //    float cutTDCX_min = -5.+ShiftX, cutTDCX_max = 3.+ShiftX, cutTDCY_min = -2.+ShiftY, cutTDCY_max = 6.+ShiftY; 
    //    sprintf(str_cut_tdc, "tdcX > %f && tdcX < %f && tdcY >%f && tdcY < %f", cutTDCX_min, cutTDCX_max, cutTDCY_min, cutTDCY_max);
    //    sprintf(str_cut_saturated, "amp_max[%d] > 3450", MCPNumber);
    //    sprintf(str_cut_nFibers, "nhodoX<=3 && nhodoY<=3 && ");
    //    sprintf(str_cut_trig_not_sat, "amp_max[%d] < 3450", trigPos1); 
    //    sprintf(str_cut_bad_timeCFD, "time_start_150[%d] != -20", MCPNumber);

    sprintf(str_cut_sig, "charge[%d] > %d", MCPNumber, treshold.at(MCPNumber));
    //    std::cout<<"DEBUG: "<<str_cut_sig<<std::endl;
    sprintf(str_cut_trig0, "charge[%d] > %d && charge[%d] > %d", trigPos1, treshold.at(trigPos1), trigPos2, treshold.at(trigPos2));
    if(strcmp(scanType, "X0") == 0) 
      sprintf(str_cut_tdc, "hodoXpos>10 && hodoXpos<15 && hodoYpos>10 && hodoYpos<15"); 
    else
       sprintf(str_cut_tdc, "1==1"); //selection OFF because there are some runs with bad hodo position -> need to check this!!!!!!!
    sprintf(str_cut_saturated, "amp_max[%d] > 3450", MCPNumber);
    //sprintf(str_cut_nFibers, "1==1"); //selection OFF
    sprintf(str_cut_trig_not_sat, "amp_max[%d] < 3450", trigPos1); 
    sprintf(str_cut_bad_timeCFD, "time_start_150[%d] != -20", MCPNumber);
    //    sprintf(str_cut_sci, "sci_front_adc > 400 && sci_front_adc <550");
    if (TString(MCP).Contains("Double") == 1)
		sprintf(str_cut_multiplicity, "amp_max[1]<20 && amp_max[2]<20 && sci_front_adc > 80 && sci_front_adc < 350");// && bgo_back_adc > 420 && bgo_back_adc < 640");
    else
		sprintf(str_cut_multiplicity, "amp_max[3]<20 && amp_max[4]<20 && sci_front_adc > 80 && sci_front_adc < 350");// && bgo_back_adc > 420 && bgo_back_adc < 640");
    int notMCPnumber = 2;
    if(notMCPnumber == MCPNumber) notMCPnumber = notMCPnumber + 1;
    sprintf(str_cut_noisePeak, "amp_max[%d] < 30", notMCPnumber);
    //sprintf(str_cut_multiplicity, "1==1");// && bgo_back_adc > 420 && bgo_back_adc < 640");

    /*    if (MCPList.at(MCP)==4)
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
    */
    //---construct TCut
    TCut cut_sig = str_cut_sig;
    TCut cut_trig0 = str_cut_trig0;
    TCut cut_tdc = str_cut_tdc;
    TCut cut_saturated = str_cut_saturated;
    TCut cut_nFibers = str_cut_nFibers;
    TCut cut_trig_not_sat = str_cut_trig_not_sat;
    TCut cut_bad_timeCFD = str_cut_bad_timeCFD;
    TCut cut_bad_timeLED = str_cut_bad_timeLED;
    TCut cut_multiplicity = str_cut_multiplicity;
    TCut cut_noisePeak = str_cut_noisePeak;


//-------Runs loop------------------------------------------------------------------------
    for(unsigned int i=0; i<ScanList.size(); i++)
    {
        //---Define run dependend cut---
        char cut_scan[100];
	if (strcmp(scanType,"HV1")==0 || strcmp(scanType,"HV2")==0 || strcmp(scanType,"HV12")==0) 
	  sprintf(cut_scan, "HV[%d] == %d && HV2[%d] == %d", MCPNumber, HVVal.at(i), MCPNumber, HV2Val.at(i));
        else  
            sprintf(cut_scan, "X0 == %f", X0Step.at(i));
	//        if(MCPNumber == 2) 
	//            sprintf(str_cut_saturated, "run_id > 796 && amp_max[%d] > 4000", MCPNumber);
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
        TH1F* h_sig= new TH1F(h_sig_name, h_sig_name, 500, -5000, 40000);
        TH1F* h_trig0 = new TH1F(h_trig0_name, h_trig0_name, 500, -5000, 40000);
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
        TH1F* h_resCFD = new TH1F(h_resCFD_name, "Electron Beam 50 GeV", 250, -0.5, 0.5);
	h_resCFD->GetXaxis()->SetTitle("time_{MCP}-time_{TRIG} (ns)");
	h_resCFD->GetYaxis()->SetTitle("Entries");
	h_resCFD->GetXaxis()->SetTitleSize(0.05);
	h_resCFD->GetYaxis()->SetTitleSize(0.05);
	h_resCFD->GetXaxis()->SetTitleOffset(0.9);
	h_resCFD->GetYaxis()->SetTitleOffset(0.9);
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
            sprintf(var_sig, "charge[%d]>>%s", MCPNumber, h_sig_name);
            sprintf(var_trig0, "charge[%d]>>%s", trigPos1, h_trig0_name);
	    //	    nt->Draw(var_sig, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers && cut_multiplicity, "goff");
	    //	    nt->Draw(var_trig0, cut_trig0 && cut_scan && cut_tdc && cut_nFibers && cut_tdc && cut_multiplicity, "goff");
	    //nt->Draw(var_sig, cut_trig0 && cut_sig && cut_scan && cut_multiplicity && cut_tdc, "goff");
            //nt->Draw(var_trig0, cut_trig0 && cut_scan && cut_multiplicity && cut_tdc, "goff");
	    nt->Draw(var_sig, cut_trig0 && cut_sig && cut_scan && cut_multiplicity && cut_tdc && cut_noisePeak, "goff");
            nt->Draw(var_trig0, cut_trig0 && cut_scan && cut_multiplicity && cut_tdc && cut_noisePeak, "goff");
	    //	    std::cout<<"DEBUG - sign: "<<h_sig->Integral(0, h_sig->GetNbinsX()+1)<<" - trig: "<<h_trig0->Integral(0, h_trig0->GetNbinsX()+1)<<std::endl;
	    
            float eff = h_sig->GetEntries()/h_trig0->GetEntries();
	    float e_eff = TMath::Sqrt((TMath::Abs(eff*(1-eff)))/h_trig0->Integral(0, h_trig0->GetNbinsX()+1)); //BUG
            // float e_eff = eff*TMath::Sqrt(1/(float)h_sig->GetEntries()+1)+1/(float)h_trig0->GetEntries();
            if(eff < 0)   
                eff = 0;
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\teff\te_%s\te_eff\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
	    if(TString(scanType).Contains("HV12") == 1) 
            {
	      //	      printf("%d\t%.3f\t%.3f\t%.3f\n", HVVal.at(i) - HV2Val.at(i), eff, 0., e_eff);
	      printf("%d\t%d\t%.3f\t%.3f\t%.3f\n", HVVal.at(i), HV2Val.at(i), eff, 0., e_eff);
                outputFile << HVVal.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
		//                g_eff->SetPoint(i, - HVVal.at(i) + HV2Val.at(i), eff);
                g_eff->SetPoint(i, HVVal.at(i), eff);
                g_eff->SetPointError(i, 0, e_eff);
	    }
	    else if(TString(scanType).Contains("HV1") == 1) 
            {
                printf("%d\t%.3f\t%.3f\t%.3f\n", HVVal.at(i), eff, 0., e_eff);
                outputFile << HVVal.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
                g_eff->SetPoint(i, HVVal.at(i), eff);
                g_eff->SetPointError(i, 0, e_eff);
	    }
	    else if(TString(scanType).Contains("HV2") == 1) 
            {
                printf("%d\t%d\t%.3f\t%.3f\n", HV2Val.at(i), eff, 0., e_eff);
                outputFile << HV2Val.at(i)<<"\t"<<eff<<"\t 0.\t"<<e_eff<<std::endl;
                g_eff->SetPoint(i, HV2Val.at(i), eff);
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
	/*        if(strcmp(doWhat, "Q") == 0)// || strcmp(doWhat, "all") == 0)
	{
	    if(i == 0)
	    {
		printf("---------Efficiency----------\n");
		printf(" %s\tQ\te_%s\te_Q\n", var_name, var_name);
		printf("-----------------------------\n");
	    }
            // sprintf(var_charge, "%s>>%s", TOT_diff, h_charge_name);
            sprintf(var_charge, "charge[%d]>>%s", MCPNumber, h_charge_name);
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
		} */
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
	    //	    if (strcmp((inverted_MCPList.at(MCPNumber)).c_str(),"MiB3")==0)
	    //   sprintf(t_CF_diff, "(time_CF_corr[%d]-(time_CF_corr[%d]-time_CF_corr[%d])-time_CF_corr[%d])", MCPNumber, clockPos2, clockPos1, trigPos1);
	    // else
	    sprintf(t_CF_diff, "(time_CF[%d]-time_CF[%d])", MCPNumber, trigPos1);
	    sprintf(var_timeCFD_vs_TOT, "%s:%s>>%s", t_CF_diff, TOT_diff, pr_timeCFD_vs_TOT_name);
            //---correction
            nt->Draw(var_timeCFD_vs_TOT, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_timeCFD && cut_multiplicity, "goff");
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
                     && cut_trig_not_sat && cut_bad_timeCFD && cut_multiplicity, "goff");  
	    
	    //correction vs ampMax
	    sprintf(var_timeCFD_red, "(%s-(%f + %f*%s + %f*%s*%s))",
		    t_CF_diff, f_corrCFD->GetParameter(0), f_corrCFD->GetParameter(1), TOT_diff,
		    f_corrCFD->GetParameter(2), TOT_diff, TOT_diff);	    
            sprintf(var_timeCFD_vs_ampMax, "%s:amp_max_corr[%d]>>%s", var_timeCFD_red, MCPNumber, pr_timeCFD_vs_ampMaxCorr_name);
            nt->Draw(var_timeCFD_vs_ampMax, cut_trig0 && cut_sig && cut_scan && cut_nFibers
                     && cut_tdc && cut_trig_not_sat && cut_bad_timeCFD && cut_multiplicity, "goff");
	    //	    pr_timeCFD_vs_ampMaxCorr->Fit(f_corrCFD2, "QR");    

            //---draw res histo with corrections
	    /*	    sprintf(var_timeCFD, "%s-(%f + %f*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d] + %f*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d]*amp_max_corr[%d])>>%s",
		    var_timeCFD_red, f_corrCFD2->GetParameter(0), f_corrCFD2->GetParameter(1), MCPNumber,
		    f_corrCFD2->GetParameter(2), MCPNumber, MCPNumber,
                    f_corrCFD2->GetParameter(3), MCPNumber, MCPNumber, MCPNumber,
		    f_corrCFD2->GetParameter(4), MCPNumber, MCPNumber, MCPNumber, MCPNumber, h_resCFD_name);
            nt->Draw(var_timeCFD, cut_trig0 && cut_sig && cut_scan && cut_tdc && cut_nFibers
                     && cut_trig_not_sat && cut_bad_timeCFD && cut_multiplicity, "goff");  
	    */
            //---fit coincidence peak
            f_resCFD->SetParameters(h_resCFD->GetEntries(), h_resCFD->GetMean(), h_resCFD->GetRMS());
            f_resCFD->SetParLimits(1, -1, 1);
            f_resCFD->SetParLimits(2, 0.01, 0.5);
            h_resCFD->Fit(f_resCFD, "QRB");
            h_resCFD->Fit(f_resCFD, "QRBIM", "", f_resCFD->GetParameter(1)-2*f_resCFD->GetParameter(2),
                          f_resCFD->GetParameter(1)+2*f_resCFD->GetParameter(2));
            //---get resolution
            float e_t_res = f_resCFD->GetParError(2)*1000.;
            float t_res = f_resCFD->GetParameter(2)*1000.;
            float prob = f_resCFD->GetProb();
            //---print results + graph
            if(TString(scanType).Contains("HV1") == 1) 
            {
                printf("%d\t%d\t%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i),i, HV2Val.at(i), t_res, 0., e_t_res, prob);
                outputFile << HVVal.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resCFD->SetPoint(g_resCFD->GetN(),  HVVal.at(i), t_res);
                g_resCFD->SetPointError(g_resCFD->GetN()-1, 0, e_t_res);
            } 
            else if(TString(scanType).Contains("HV2") == 1) 
            {
                printf("%d\t%d\t%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i),i, HV2Val.at(i), t_res, 0., e_t_res, prob);
                outputFile << HV2Val.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resCFD->SetPoint(g_resCFD->GetN(),  HV2Val.at(i), t_res);
                g_resCFD->SetPointError(g_resCFD->GetN()-1, 0, e_t_res);
            } 
            else if(TString(scanType).Contains("HV12") == 1) 
            {
                printf("%d\t%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i),HV2Val.at(i), t_res, 0., e_t_res, prob);
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
	    //	    if (strcmp((inverted_MCPList.at(MCPNumber)).c_str(),"MiB3")==0)
	    //  sprintf(t_start_diff, "(time_start_150[%d]-(time_start_150[%d]-time_start_150[%d])-time_CF_corr[%d])", MCPNumber, clockPos2, clockPos1, trigPos1);
	    //else
	    sprintf(t_start_diff, "(time_start_150[%d]-time_CF_corr[%d])", MCPNumber, trigPos1);
            //---change TOT for X0 runs
            if(strcmp(scanType, "X0") == 0 && X0Step.at(i) != 0 && MCPNumber < 3) 
            {
                sprintf(TOT_diff, "(time_stop_500[%d]-time_start_500[%d])", MCPNumber, MCPNumber);
		//	if (strcmp((inverted_MCPList.at(MCPNumber)).c_str(),"MiB3")==0)
		//  sprintf(t_start_diff, "(time_start_500[%d]-(time_start_500[%d]-time_start_500[%d])-time_CF_corr[%d])", MCPNumber,clockPos2,clockPos1,trigPos1);
		//else
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
            if(TString(scanType).Contains("HV1") == 1) 
            {
                printf("%d\t%d\t%%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i),i, HV2Val.at(i), t_res, 0., e_t_res, prob);
                outputFile << HVVal.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resLED->SetPoint(g_resLED->GetN(),  HVVal.at(i), t_res);
                g_resLED->SetPointError(g_resLED->GetN()-1, 0, e_t_res);
            } 
            else if(TString(scanType).Contains("HV12") == 1) 
            {
                printf("%d\t%d\t%%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i),i, HV2Val.at(i), t_res, 0., e_t_res, prob);
                outputFile << HV2Val.at(i)<<"\t"<<t_res<<"\t 0.\t"<<e_t_res<<std::endl;
                g_resLED->SetPoint(g_resLED->GetN(),  HV2Val.at(i), t_res);
                g_resLED->SetPointError(g_resLED->GetN()-1, 0, e_t_res);
            } 
            else if(TString(scanType).Contains("HV12") == 1) 
            {
                printf("%d\t%d\t%%d\t%d\t%.1f\t%.0f\t%.1f\t%.3f\n", i, HVVal.at(i),i, HV2Val.at(i), t_res, 0., e_t_res, prob);
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

      inFile->Close();
  	
      return 0;     
}

