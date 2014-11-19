
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

  int SBTreshold=1000;
  int nChannels=9;
  int HVtresh=2000;
  //  int doPlot=0;

  std::string inputFileName = argv[1];  
  std::cout<<"\nReading file: "<<inputFileName<<std::endl;

  if (argc > 2) {
    SBTreshold = atoi(argv[2]);  //desired treshold (e.g. S/B=1000)
    nChannels = atoi(argv[3]);  //nChannels in the reco tree
    HVtresh = atoi(argv[4]);   //HV treshold
  }

  std::cout << " SBTreshold = " << SBTreshold << std::endl;
  std::string outputFileName = "cfg/treshold_SB1000_ft.txt";
  std::ofstream outputFile ((outputFileName).c_str(), std::ofstream::out);
  std::string outputFileName3s = "cfg/treshold_3s_ft.txt";
  std::ofstream outputFile3s ((outputFileName3s).c_str(), std::ofstream::out);
  std::string outputFileName5s = "cfg/treshold_5s_ft.txt";
  std::ofstream outputFile5s ((outputFileName5s).c_str(), std::ofstream::out);

  //-----get the run list and the list of channel to analyze------

  TLegend* leg = new TLegend(0.88,0.45,0.98,0.65);
  TGraph* g_SoB[20];

  TF1* base[10];
  TF1* baseAmp[10];

  TCanvas* baseFit[10];


  ////////////// pedestal
  TFile *inputFilePed = new TFile ("/gwteray/users/brianza/ntuple_reco_iMCP_H4_Template/reco_pedestal.root"); 
  TTree *inputTreePed = (TTree*)inputFilePed->Get("reco_tree");
  InitRecoTree(inputTreePed);  
  inputTreePed->SetDirectory(0);
  inputFilePed->Close();
  inputFilePed->Delete();
  std::cout << " inputTreePed->GetEntries() = " << inputTreePed->GetEntries() << std::endl;

  ////////////// signal
  TFile *inputFile = new TFile ((inputFileName).c_str()); 
  TTree *inputTree = (TTree*)inputFile->Get("reco_tree");
  InitRecoTree(inputTree);  
  std::cout << " inputTree->GetEntries() = " << inputTree->GetEntries() << std::endl;

  Fill_inverted_MCPList();
  std::cout << " inverted_MCPList.size() = " << inverted_MCPList.size() << std::endl;

  TH1F** hS = new TH1F*[10];
  TH1F** hB = new TH1F*[10];
  TH1F** hBFit = new TH1F*[10];

  TH1F** hAmpS = new TH1F*[10];
  TH1F** hAmpB = new TH1F*[10];
  TGraph** gAmpSoB = new TGraph*[10];

  //------analyze the good channels and compute the charge tresholds--------
  std::cout<<"\n--------------------------------\n-->OK, now computing tresholds:"<<std::endl;
  for (int iCh=0; iCh<nChannels; iCh++)
	{
	  char hSName[100], hBName[100], hBNameFit[100], fitName[100];
	  sprintf(hSName, "hS_%d", iCh);
	  sprintf(hBName, "hB_%d", iCh);
	  sprintf(hBNameFit, "hBFit_%d", iCh);
	  sprintf(fitName, "gausFit_%d", iCh);

	  hS[iCh] = new TH1F(hSName,hSName, 1000, -5000, 50000);
	  hB[iCh] = new TH1F(hBName,hBName, 1000, -5000, 50000);
	  hBFit[iCh] = new TH1F(hBNameFit,hBNameFit, 1000, -5000, 50000);

	  base[iCh] = new TF1(fitName,"gaus",-1000,1000);
	      
	  char hSDraw[100], hBDraw[100], hBDrawFit[100];
	  sprintf(hSDraw, "(charge[%d]+(baseline[%d]*25)) >>%s", iCh, iCh,  hSName);
	  sprintf(hBDraw, "(charge[%d]+(baseline[%d]*25))>>%s", iCh, iCh, hBName);
	  sprintf(hBDrawFit, "(charge[%d]+(baseline[%d]*25.))>>%s", iCh, iCh, hBNameFit);

	  char cut[300];
	  //	  if(inverted_MCPList.at(iCh) == "enSEE") HVtresh = 2000;
	  //	  sprintf(cut, "isPCOn[%d]!=2 && HV[%d]>=%d && tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6 && time_OT[%d] < 0.", iCh, iCh, HVtresh, iCh); 
	  sprintf(cut, "isPCOn[%d]!=2 && HV[%d]>=%d && tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6", iCh, iCh, HVtresh); 

	  char cutPed[300];
	  //	  sprintf(cut, "isPCOn[%d]!=2 && tdcX > -8 && tdcX < 0 && tdcY >-2 && tdcY < 6 && time_OT[iCh] < 0.", iCh, iCh); 
	  sprintf(cutPed, "time_OT[%d] < 0. && charge_corr[%d] > -1.1 && charge_corr[%d] < -0.9", iCh, iCh, iCh); 

	  inputTree->Draw(hSDraw, cutPed);
// 	  inputTreePed->Draw(hBDraw,cut);
// 	  inputTreePed->Draw(hBDrawFit,cut);
 	  inputTreePed->Draw(hBDraw, cutPed);
 	  inputTreePed->Draw(hBDrawFit, cutPed);
	  //	  inputTreePed->Draw(hBDraw, cutPed);      
	  //	  inputTreePed->Draw(hBDrawFit, cutPed);   



	  baseAmp[iCh] = new TF1(Form("baseAmp_%d",iCh),"gaus",-30.,30.);
	  hAmpS[iCh] = new TH1F(Form("hAmpS_%d",iCh), "", 2500, -1000, 10000);
	  hAmpB[iCh] = new TH1F(Form("hAmpB_%d",iCh), "", 2500, -1000, 10000);

	  inputTree->Draw(Form("amp_max[%d]>>%s",iCh,Form("hAmpS_%d",iCh)), cutPed);
	  inputTreePed->Draw(Form("amp_max[%d]>>%s",iCh,Form("hAmpB_%d",iCh)), cutPed);


	  std::cout << " iCh = " << iCh << std::endl;
	  std::cout << " hAmpS->GetEntries() = " << hAmpS[iCh]->GetEntries() << std::endl;
	  std::cout << " hAmpB->GetEntries() = " << hAmpB[iCh]->GetEntries() << std::endl;

	  char canvasName[50];
	  sprintf(canvasName, "plots/baseline/baseFit_%d.pdf", iCh);
	  baseFit[iCh] = new TCanvas();
	  baseFit[iCh]->cd();

	  base[iCh]->SetParameters(hBFit[iCh]->GetMean(), hBFit[iCh]->GetRMS());
     	  hBFit[iCh]->Fit(base[iCh]);
	  std::cout << "Charge sigma == " << base[iCh]->GetParameter(2) << std::endl;
	  outputFile3s << iCh << "  " << (int)(base[iCh]->GetParameter(2)*3) << std::endl;
	  outputFile5s << iCh << "  " << (int)(base[iCh]->GetParameter(2)*5) << std::endl;

	  hBFit[iCh]->Draw();
	  base[iCh]->Draw("same");
	  baseFit[iCh]->Print(canvasName,"pdf");
	  sprintf(canvasName, "plots/baseline/baseFit_%d.png", iCh);
	  baseFit[iCh]->Print(canvasName,"png");

	  //	  baseAmp[iCh]->SetParameters(hAmpB[iCh]->GetMean(),hAmpB[iCh]->GetRMS());
	  baseAmp[iCh]->SetParameters(0., hAmpB[iCh]->GetRMS());
	  hAmpB[iCh]->Fit(baseAmp[iCh], "R");
	  float sigmaAmp = baseAmp[iCh]->GetParameter(2);
	  std::cout << "Ampl sigma == " << sigmaAmp << std::endl;


	  g_SoB[iCh] = new TGraph();
	  gAmpSoB[iCh] = new TGraph();

	  int SBvalue = 0;
	  int nBinsAmpHisto = hAmpS[iCh]->GetNbinsX();
	  int nBinsHisto = hS[iCh]->GetNbinsX();
	  //	  float intTot_Sc = hS[iCh]->Integral(1, nBinsHisto);
	  //	  float intTot_Bc = hB[iCh]->Integral(1, nBinsHisto);
	  float intTot_Sa = hAmpS[iCh]->Integral(1, nBinsAmpHisto);
	  float intTot_Ba = hAmpB[iCh]->Integral(1, nBinsAmpHisto);

	  for (int iBin=1; iBin<=std::max(nBinsHisto, nBinsAmpHisto); ++iBin)
	    {

	      if(iBin <= nBinsHisto){
		double S = hS[iCh]->Integral(iBin, nBinsHisto);
		double B = hB[iCh]->Integral(iBin, nBinsHisto);
		double SoverB = 2000;
		if (B!=0)    SoverB = S/B;
		//		g_SoB[iCh]->SetPoint(iBin,hS[iCh]->GetBinCenter(iBin),B);
		g_SoB[iCh]->SetPoint(iBin,hS[iCh]->GetBinCenter(iBin),SoverB);
	      }

	       if(iBin <= nBinsAmpHisto){
		 double Samp = hAmpS[iCh]->Integral(iBin, nBinsAmpHisto)/intTot_Sa;
		 double Bamp = hAmpB[iCh]->Integral(iBin, nBinsAmpHisto)/intTot_Ba;
		 if(hAmpS[iCh]->GetBinCenter(iBin) == sigmaAmp * 5.) std::cout << " >>>> Samp/Bamp = " << Samp/Bamp << std::endl;
		 double SoBamp = 2000;
		 if(Bamp!=0) SoBamp = Samp/Bamp;
		 gAmpSoB[iCh]->SetPoint(iBin,hAmpS[iCh]->GetBinCenter(iBin),Samp);
		 // gAmpSoB[iCh]->SetPoint(iBin,hAmpS[iCh]->GetBinCenter(iBin),Bamp);
		 gAmpSoB[iCh]->SetPoint(iBin,hAmpS[iCh]->GetBinCenter(iBin),SoBamp);
	       }


// 	       if (SoverB > SBTreshold) 
// 	         {
// 		   std::cout<<" Channel: "<<iCh<<" ("<<inverted_MCPList.at(iCh)<<")   Treshold Value: "<<iBin<<std::endl;
// 		   outputFile<<iCh<<" "<<iBin<<std::endl;
// 		   SBvalue = iBin;
// 	           break;
// 	         }       
	    }

	  
	  g_SoB[iCh]->SetLineColor(iCh+1);
	  g_SoB[iCh]->SetMarkerColor(iCh+1);
	  if (iCh==0)   g_SoB[iCh]->Draw("APL");
      	  else          g_SoB[iCh]->Draw("pl,same");

	  //	    g_SoB[iCh]->GetXaxis()->SetLimits(0,600);
	  //	    g_SoB[iCh]->SetMinimum(0);
	  //	    g_SoB[iCh]->SetMaximum(1001);
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
  g_SoB[0]->Draw("ap");
  for(int iCh=1; iCh<nChannels; ++iCh)g_SoB[iCh]->Draw("p, same");
  leg->Draw("same");
  SoB->Print("plots/SoverB.pdf","pdf");

  TFile outThresh("outThresh.root", "recreate");
  outThresh.cd();
  for(int iCh=0; iCh<nChannels; ++iCh){
    g_SoB[iCh]->Write(Form("gSoB_%d",iCh));
    hS[iCh]->Write(Form("hS_%d",iCh));
    hB[iCh]->Write(Form("hB_%d",iCh));
    hBFit[iCh]->Write(Form("hBFit_%d",iCh));
    hAmpS[iCh]->Write(Form("hAmpS_%d",iCh));
    hAmpB[iCh]->Write(Form("hAmpB_%d",iCh));
    gAmpSoB[iCh]->Write(Form("gAmpSoB_%d",iCh));
  }
  outThresh.Close();  

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
