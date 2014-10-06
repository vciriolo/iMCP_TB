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

  std::string outputFileName = "cfg/treshold.txt";
  std::ofstream outputFile ((outputFileName).c_str(), std::ofstream::out);

  //-----get the run list and the list of channel to analyze------
  std::vector<int> goodCh;
  goodCh.clear();

  std::cout<<"Channels to analyze: ";
  inputTree->GetEntry(1);
    {
      for (int iCh=0; iCh<nChannels; iCh++)  //scan all the channels -> save only the channels with PC on and HV>treshold
        {
          if (isPCOn[iCh]!=0 && HV[iCh]>=HVtresh) {
	    goodCh.push_back(iCh);
	    std::cout<<iCh<<" ";
          }
        }
    }

  TCanvas* SoB = new TCanvas("SoB", "SoB", 800, 800);  
  TGraph* g_SoB[20];
  TLegend* leg = new TLegend(0.88,0.45,0.98,0.65);
  leg->SetFillColor(0);
  SoB->cd();

  //------analyze the good channels and compute the charge tresholds--------
  std::cout<<"\n--------------------------------\n-->OK, now computing tresholds:"<<std::endl;
  for (int iCh=0; iCh<nChannels; iCh++)
	{
	  //---is it a good channel? check: if not, go to next channel----
	  std::vector<int>::iterator it;
	  it = find (goodCh.begin(), goodCh.end(), iCh);
	  if (it == goodCh.end())    continue;

	  char hSName[100], hBName[100];
	  sprintf(hSName, "hS_%d", iCh);
	  sprintf(hBName, "hB_%d", iCh);

	  TH1F *hS = new TH1F(hSName,hSName, 30000, 0, 30000);
	  TH1F *hB = new TH1F(hBName,hBName, 30000, 0, 30000);
	      
	  char hSDraw[100], hBDraw[100];
	  sprintf(hSDraw, "charge[%d]>>%s", iCh, hSName);
	  sprintf(hBDraw, "baseline[%d]>>%s", iCh, hBName);

	  char cut[300];
	  sprintf(cut, "sci_front_adc>500 && sci_front_adc<1500 && isPCOn[0]!=0 && HV>=%d", HVtresh); 

	  inputTree->Draw(hSDraw,cut,"goff");
	  inputTree->Draw(hBDraw,cut,"goff");

	  char gName[50];
	  sprintf(gName, "g_SoB_%d", iCh);
	  g_SoB[iCh] = new TGraph();

	  int SBvalue=0;

	  for (int iBin=1; iBin<30000; iBin++)
	    {
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
	  if (iCh==0) 
	    g_SoB[iCh]->Draw("APL");
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
  SoB->Print("prova.pdf","pdf");

  std::cout<<"\nTreshold values printed in "<<outputFileName<<std::endl;
  inputFile->Close();
  return 0;
}
