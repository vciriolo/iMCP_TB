/*******************************************************************************                                                                                  
THIS PROGRAM COMPUTES THE TRESHOLDS FOR EACH MCP USING A FIXED S/B VALUE (IN INPUT)
                                                                                                    
    compile with --> c++ -o calcTreshold bin/calcTreshold.cpp `root-config --cflags --glibs`                                                                
    run with --> ./calcTreshold ntuples/reco_prova.root 1000 9 2000  
                          where the arguments are: -inputTree -S/B treshold -number of channels -HV treshold (analyze only MCPs with HV>treshold)
                                                                                                                                                                  
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <ctime>
#include <map>
#include <math.h>

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TClass.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TLine.h"

//#include "../include/analysis_tools.h"
#include "../src/init_Reco_Tree.cc"
#include "../interface/histo_func.h"
#include "../src/MCPMap.cc"

using namespace std;

//*******MAIN*******************************************************************                                                                                  
int main (int argc, char** argv)
{
  std::cout<<"-----------COMPUTING CHARGE TRESHOLDS FOR EACH CHANNEL------------"<<std::endl;
  std::cout<<"NB: MCP is analyzed only if HV>treshold and PC is ON---------"<<std::endl;

  int SBTreshold=1000;
  int nChannels=9;
  int HVtresh=2000;
  int doPlot=0;

  std::string inputFileName = argv[1];  
  std::cout<<"\nReading file: "<<inputFileName<<std::endl;

  if (argc > 2) {
    SBTreshold = atoi(argv[2]);  //desired treshold (e.g. S/B=1000)
    nChannels = atoi(argv[3]);  //nChannels in the reco tree
    HVtresh = atoi(argv[4]);   //HV treshold
  }

  TFile *inputFile = new TFile ((inputFileName).c_str()); 
  TTree *inputTree = (TTree*)inputFile->Get("reco_tree");
  InitRecoTree(inputTree);  

  Fill_inverted_MCPList();

  std::string outputFileName = "cfg/treshold_SB1000.txt";
  std::ofstream outputFile ((outputFileName).c_str(), std::ofstream::out);
  std::string outputFileName3s = "cfg/treshold_3s.txt";
  std::ofstream outputFile3s ((outputFileName3s).c_str(), std::ofstream::out);

  char hBNameFit[100] = "hB_0_Fit";
  TH1F *hBFit = new TH1F(hBNameFit,hBNameFit, 30, -500, 500);
  TF1 *base = new TF1("gaus","gaus",-1000,1000);
  //-----get the run list and the list of channel to analyze------

  TCanvas* SoB = new TCanvas("SoB", "SoB", 800, 800);  
  TGraph* g_SoB[20];
  TLegend* leg = new TLegend(0.88,0.45,0.98,0.65);
  leg->SetFillColor(0);
  SoB->cd();

  //------analyze the good channels and compute the charge tresholds--------
  std::cout<<"\n--------------------------------\n-->OK, now computing tresholds:"<<std::endl;
  for (int iCh=0; iCh<nChannels; iCh++)
	{
	  char hSName[100], hBName[100];
	  sprintf(hSName, "hS_%d", iCh);
	  sprintf(hBName, "hB_%d", iCh);

	  TH1F *hS = new TH1F(hSName,hSName, 30000, 0, 30000);
	  TH1F *hB = new TH1F(hBName,hBName, 30000, 0, 30000);
	      
	  char hSDraw[100], hBDraw[100], hBDrawFit[100];
	  sprintf(hSDraw, "charge[%d]>>%s", iCh, hSName);
	  sprintf(hBDraw, "baseline[%d]>>%s", iCh, hBName);
	  sprintf(hBDrawFit, "baseline[%d]>>%s", iCh, hBNameFit);

	  char cut[300];
	  sprintf(cut, "sci_front_adc>500 && sci_front_adc<1500 && isPCOn[%d]!=0 && HV[%d]>=%d", iCh, iCh, HVtresh); 

	  inputTree->Draw(hSDraw,cut,"goff");
	  inputTree->Draw(hBDraw,cut,"goff");
	  inputTree->Draw(hBDrawFit,cut,"goff");

	  base->SetParameter(1,0);
	  base->SetParameter(2,0);
     	  hBFit->Fit(base,"QN");
	  std::cout<<"sigma baseline: "<<base->GetParameter(2)<<std::endl;
	  outputFile3s << iCh << "  "<<(int)(base->GetParameter(2)*3)<<std::endl;

	  char gName[50];
	  sprintf(gName, "g_SoB_%d", iCh);
	  g_SoB[iCh] = new TGraph();

	  int SBvalue=0;

	  for (int iBin=1; iBin<30000; iBin++)
	    {
	      if (hS->GetEntries()==0) {
		g_SoB[iCh]->SetPoint(0,0,-1);
		std::cout<<" Channel: "<<iCh<<" is PC OFF or HV under treshold!"<<std::endl;
		outputFile<<iCh<<" "<<-1<<std::endl;
		break;
	      }

	       double S = hS->Integral(iBin, 30000);
	       double B = hB->Integral(iBin, 30000);
	       double SoverB=2000;
	       if (B!=0)    SoverB = S/B;
	       g_SoB[iCh]->SetPoint(iBin,iBin,SoverB);

	       if (SoverB > SBTreshold) 
	         {
		   std::cout<<" Channel: "<<iCh<<" ("<<inverted_MCPList.at(iCh)<<")   Treshold Value: "<<iBin<<std::endl;
		   outputFile<<iCh<<" "<<iBin<<std::endl;
		   SBvalue = iBin;
	           break;
	         }       
	    }

	  g_SoB[iCh]->SetLineColor(iCh+1);
	  g_SoB[iCh]->SetMarkerColor(iCh+1);
	  if (iCh==0)   g_SoB[iCh]->Draw("APL");
      	  else          g_SoB[iCh]->Draw("pl,same");

	    g_SoB[iCh]->GetXaxis()->SetLimits(0,600);
	    g_SoB[iCh]->SetMinimum(0);
	    g_SoB[iCh]->SetMaximum(1001);
	    g_SoB[iCh]->GetXaxis()->SetTitle("charge");
	    g_SoB[iCh]->GetYaxis()->SetTitle("S/B");

	    TLine* tl = new TLine(SBvalue, 0, SBvalue, 1000);
	    tl->SetLineColor(iCh+1);
	    tl->Draw("same");

	  leg->AddEntry(g_SoB[iCh],inverted_MCPList.at(iCh).c_str(),"l");

	}

  leg->Draw("same");
  SoB->Print("plots/SoverB.pdf","pdf");
  
  /*  TCanvas* ch_baseline = new TCanvas("ch_baseline", "ch_baseline", 800, 800);  
  ch_baseline->cd();
  hBFit->Draw("");
  base->Draw("same");
  ch_baseline->Print("plots/baseline_ch0.pdf","pdf");
  */  
  std::cout<<"\nS/B=1000: treshold values printed in "<<outputFileName<<std::endl;
  std::cout<<"\n3sigma:   treshold values printed in "<<outputFileName3s<<std::endl;
  inputFile->Close();
  return 0;
}
