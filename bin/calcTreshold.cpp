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
#include "../interface/init_Reco_Tree.h"
#include "../interface/histo_func.h"
#include "../interface/MCPMap.h"

using namespace std;

//*******MAIN*******************************************************************                                                                                  
int main (int argc, char** argv)
{
  std::cout<<"-----------COMPUTING CHARGE TRESHOLDS FOR EACH CHANNEL------------"<<std::endl;
  std::cout<<"NB: MCP is analyzed only if HV>treshold and PC is ON---------"<<std::endl;

  gSystem->Load("libTree");

  int SBTreshold=1000;
  int nChannels=18;
  int HVtresh=2000;

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
  std::string outputFileName5s = "cfg/treshold_5s.txt";
  std::ofstream outputFile5s ((outputFileName5s).c_str(), std::ofstream::out);

  //-----get the run list and the list of channel to analyze------

  TLegend* leg = new TLegend(0.88,0.45,0.98,0.65);
  TGraph* g_SoB[20];

  TF1* base[18];
  TF1* base2sig[18];

  TCanvas* baseFit[18];
  //------analyze the good channels and compute the charge tresholds--------
  std::cout<<"\n--------------------------------\n-->OK, now computing tresholds:"<<std::endl;
  for (int iCh=0; iCh<nChannels; iCh++)
	{
	  char hSName[100], hBName[100], hBNameFit[100], fitName2sig[100], fitName[100];
	  sprintf(hSName, "hS_%d", iCh);
	  sprintf(hBName, "hB_%d", iCh);
	  sprintf(hBNameFit, "hBFit_%d", iCh);
	  sprintf(fitName, "gausFit_%d", iCh);
	  sprintf(fitName2sig, "gausFit2sig_%d", iCh);

	  TH1F *hS = new TH1F(hSName,hSName, 30000, 0, 30000);
	  TH1F *hB = new TH1F(hBName,hBName, 30000, 0, 30000);
	  TH1F *hBFit = new TH1F(hBNameFit,hBNameFit, 200, -300, 300);

	  base[iCh] = new TF1(fitName,"gaus",-1000,1000);
	      
	  char hSDraw[100], hBDraw[100], hBDrawFit[100];
	  sprintf(hSDraw, "charge[%d]>>%s", iCh, hSName);
	  sprintf(hBDraw, "baseline[%d]>>%s", iCh, hBName);
	  sprintf(hBDrawFit, "baseline[%d]>>%s", iCh, hBNameFit);

	  char cut[300];
	  //	  sprintf(cut, "isPCOn[%d]!=2 && HV[%d]>=%d && tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6", iCh, iCh, HVtresh); 
	  sprintf(cut, "isPCOn[%d]!=2 && HV[%d]>=%d", iCh, iCh, HVtresh); 

	  inputTree->Draw(hSDraw,cut,"goff");
	  inputTree->Draw(hBDraw,cut);
	  inputTree->Draw(hBDrawFit,cut);

	  char canvasName[50];
	  sprintf(canvasName, "plots/baseline/baseFit_%d.pdf", iCh);
	  baseFit[iCh] = new TCanvas();
	  baseFit[iCh]->cd();

	  base[iCh]->SetParameter(1,0);
	  base[iCh]->SetParameter(2,0);
     	  hBFit->Fit(base[iCh],"QN");

	  base2sig[iCh] = new TF1(fitName2sig,"gaus",base[iCh]->GetParameter(1)-2*base[iCh]->GetParameter(2), base[iCh]->GetParameter(1)+2*base[iCh]->GetParameter(2));
	  base2sig[iCh]->SetParameter(1,base[iCh]->GetParameter(1));
	  hBFit->Fit(base2sig[iCh],"Q","",base[iCh]->GetParameter(1)-2*base[iCh]->GetParameter(2), base[iCh]->GetParameter(1)+2*base[iCh]->GetParameter(2));
	  std::cout<<"sigma baseline: "<<base2sig[iCh]->GetParameter(2)<<std::endl;
	  outputFile3s << iCh << "  "<<(int)(base2sig[iCh]->GetParameter(2)*3)<<std::endl;
	  outputFile5s << iCh << "  "<<(int)(base2sig[iCh]->GetParameter(2)*5)<<std::endl;

	  hBFit->Draw();
	  base2sig[iCh]->Draw("same");
	  baseFit[iCh]->Print(canvasName,"pdf");

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
  TCanvas* SoB = new TCanvas("SoB", "SoB", 800, 800);  
  leg->SetFillColor(0);
  SoB->cd();

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
  std::cout<<"\n5sigma:   treshold values printed in "<<outputFileName5s<<std::endl;
  inputFile->Close();
  return 0;
}
