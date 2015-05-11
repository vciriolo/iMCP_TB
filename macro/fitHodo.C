#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include "TFile.h"
#include "TLatex.h"
#include "TLine.h"
#include "TColor.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TString.h"
#include "TMath.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TMarker.h"
#include "TStyle.h"
#include "TFitter.h"
#include "TLorentzVector.h"
#include "TPaveLabel.h"
#include "RooGlobalFunc.h"
#include "RooGenericPdf.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooPolynomial.h"
#include "RooDataHist.h"
#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooArgSet.h"
#include "RooArgList.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooNLLVar.h"
#include "RooChi2Var.h"
#include "RooMinuit.h"
using namespace RooFit;
using namespace std;

//Usage: root -b -q .x fitHodo.C+\(\"../plots/HODO/efficiency_MiB3_eff_HODO_HVScan1.root\",false,\"\"\)
void fitHodo( TString NameFile = "../plots/HODO/efficiency_MiB3_eff_HODO_HVScan1.root", bool Original = false , TString Dir = "" ){

  cout<<"Starting fitHodo..."<<endl;
  //Open Files
  TCanvas* myc1 = new TCanvas("myc1", "CMS", 600, 600);
  TFile* File = TFile::Open( NameFile.Data() );
  if( !File ) cout<<"WARNING: File "<<NameFile.Data()<<" does not exist"<<endl;
  myc1->cd(); gStyle->SetPalette(1);
  TString Comm = "mkdir -p ../plots/HODO/FIT" + Dir;
  system( Comm.Data() );
  TString ori = Original ? "_c" : "";
  //Getting histos
  TH1F * h_XDiff_Hodo1TDC = (TH1F*) File->Get(("h_XDiff_Hodo1TDC"+ori).Data());
  if( !h_XDiff_Hodo1TDC )  cout<<"WARNING: h_XDiff_Hodo1TDC does not exist"<<endl;
  TH1F * h_YDiff_Hodo1TDC = (TH1F*) File->Get(("h_YDiff_Hodo1TDC"+ori).Data());
  if( !h_YDiff_Hodo1TDC )  cout<<"WARNING: h_YDiff_Hodo1TDC does not exist"<<endl;
  TH1F * h_XDiff_Hodo2TDC = (TH1F*) File->Get(("h_XDiff_Hodo2TDC"+ori).Data());
  if( !h_XDiff_Hodo2TDC )  cout<<"WARNING: h_XDiff_Hodo2TDC does not exist"<<endl;
  TH1F * h_YDiff_Hodo2TDC = (TH1F*) File->Get(("h_YDiff_Hodo2TDC"+ori).Data());
  if( !h_YDiff_Hodo2TDC )  cout<<"WARNING: h_YDiff_Hodo2TDC does not exist"<<endl;
  TH1F * h_XDiff_Hodo12 = (TH1F*) File->Get(("h_XDiff_Hodo12"+ori).Data());
  if( !h_XDiff_Hodo12 )    cout<<"WARNING: h_XDiff_Hodo12 does not exist"<<endl;
  TH1F * h_YDiff_Hodo12 = (TH1F*) File->Get(("h_XDiff_Hodo12"+ori).Data());
  if( !h_YDiff_Hodo12 )    cout<<"WARNING: h_YDiff_Hodo12 does not exist"<<endl;

  //Now lt's go...
  cout<<"Files and hisots loaded... let's start to fit."<<endl;  
  bool UseBkg=true; //Add Cheb. function to the fit
  float Coordinates[6] = {0.};
  for(int nToFit=0; nToFit<6; nToFit++){
    cout<<"------------------------- FIT: "<<nToFit<<" ------------------------------"<<endl;
    TH1F *histoToFit = h_XDiff_Hodo1TDC;
    float integ = -1., nBins = -1.; 
    if(nToFit==0){ histoToFit = h_XDiff_Hodo1TDC; integ = h_XDiff_Hodo1TDC->Integral(); nBins = h_XDiff_Hodo1TDC->GetNbinsX(); }
    if(nToFit==1){ histoToFit = h_YDiff_Hodo1TDC; integ = h_YDiff_Hodo1TDC->Integral(); nBins = h_XDiff_Hodo1TDC->GetNbinsX(); }
    if(nToFit==2){ histoToFit = h_XDiff_Hodo2TDC; integ = h_XDiff_Hodo2TDC->Integral(); nBins = h_XDiff_Hodo1TDC->GetNbinsX(); } 
    if(nToFit==3){ histoToFit = h_YDiff_Hodo2TDC; integ = h_YDiff_Hodo2TDC->Integral(); nBins = h_XDiff_Hodo1TDC->GetNbinsX(); }
    if(nToFit==4){ histoToFit = h_XDiff_Hodo12; integ = h_XDiff_Hodo12->Integral(); nBins = h_XDiff_Hodo12->GetNbinsX(); }
    if(nToFit==5){ histoToFit = h_YDiff_Hodo12; integ = h_YDiff_Hodo12->Integral(); nBins = h_YDiff_Hodo12->GetNbinsX(); }
    if( integ == -1. || nBins == -1) cout<<"WARNING: integ and nBins not defined!"<<endl;
    RooRealVar  x("x","HODO fibers", -31., 31., "");
    RooDataHist dh("dh","HODO fibers",RooArgList(x), histoToFit);
    RooRealVar  mean("mean","#gamma#gamma peak position", 0., -25., 25.,"GeV/c^{2}"); mean.setRange( -25., 25. ); mean.setVal( 0. );
    RooRealVar  sigma("sigma","#gamma#gamma sigma", 3., 0.01, 10.,""); sigma.setRange( 0.01, 10. ); sigma.setVal( 4. );
    RooGaussian gaus("gaus","Core Gaussian",x, mean,sigma);
    RooRealVar  Nsig("Nsig","Total yields", integ, integ*0.7, integ*1.3);
    Nsig.setRange( integ*0.2, integ*100 ); Nsig.setVal( integ );
    RooRealVar cb0("cb0","cb0", 0.2, -1.,1.);
    RooRealVar cb1("cb1","cb1",-0.1, -1.,1.);
    RooRealVar cb2("cb2","cb2", 0.1, -1.,1.);
    RooArgList cbpars(cb0,cb1,cb2);
    RooChebychev bkg("bkg","bkg model", x, cbpars );
    RooRealVar Nbkg("Nbkg","Bkg yealds", integ*0.1, 0., integ*0.5);
    Nbkg.setRange( 0., integ*0.5 ); Nsig.setVal( integ*0.1 );
    RooAbsPdf* model=0;
    RooAddPdf model1("model","CB only", RooArgList(gaus) ,RooArgList(Nsig));
    RooAddPdf model2("model","CB + bkg",RooArgList(gaus,bkg),RooArgList(Nsig,Nbkg));
    if(!UseBkg) model = &model1;
    else        model = &model2;
    RooNLLVar nll("nll","log likelihood var", *model, dh );
    RooMinuit m(nll);
    m.setVerbose(kFALSE);
    m.migrad();
    RooPlot*  xframe = x.frame( nBins );
    if(nToFit==0) xframe->SetTitle( "X: HODO1 - TDC [mm]" );
    if(nToFit==1) xframe->SetTitle( "Y: HODO1 - TDC [mm]" );
    if(nToFit==2) xframe->SetTitle( "X: HODO2 - TDC [mm]" );
    if(nToFit==3) xframe->SetTitle( "Y: HODO2 - TDC [mm]" );
    if(nToFit==4) xframe->SetTitle( "X: HODO1 - HODO2 [mm]" );
    if(nToFit==5) xframe->SetTitle( "Y: HODO1 - HODO2 [mm]" );
    dh.plotOn(xframe);
    model->plotOn(xframe,Components(gaus),LineStyle(kDashed), LineColor(kRed));
    model->plotOn(xframe);
    myc1->cd(); xframe->Draw();
    TLatex lat; lat.SetNDC(); lat.SetTextSize(0.030); lat.SetTextColor(1);
    char line[300];
    float xmin(0.55), yhi(0.80), ypass(0.05);
    sprintf(line,"MEAN: %.2f #pm %.2f GeV", mean.getVal(), mean.getError() );
    lat.DrawLatex(xmin,yhi, line);
    sprintf(line,"Sigma: %.2f #pm %.2f GeV", sigma.getVal(), sigma.getError() );
    lat.DrawLatex(xmin,yhi-ypass, line);
    Coordinates[ nToFit ] = mean.getVal();
    TString name = "";
    if(nToFit==0) name = "../plots/HODO/FIT" + Dir + "/FIT_HODOX1TDC" + ori + ".png";
    if(nToFit==1) name = "../plots/HODO/FIT" + Dir + "/FIT_HODOY1TDC" + ori + ".png";
    if(nToFit==2) name = "../plots/HODO/FIT" + Dir + "/FIT_HODOX2TDC" + ori + ".png";
    if(nToFit==3) name = "../plots/HODO/FIT" + Dir + "/FIT_HODOY2TDC" + ori + ".png";
    if(nToFit==4) name = "../plots/HODO/FIT" + Dir + "/FIT_HODOX1HODOX2" + ori + ".png";
    if(nToFit==5) name = "../plots/HODO/FIT" + Dir + "/FIT_HODOY1HODOY2" + ori + ".png";
    myc1->SaveAs( name.Data() );
  }
  cout<<"----------"<<endl;
  cout<<"Considerign the fits we have the following X0 Y0:"<<endl;
  cout<<"float HODOX1TDC = "<<Coordinates[0]<<", HODOY1TDC = "<<Coordinates[1]<<";"<<endl;
  cout<<"float HODOX2TDC = "<<Coordinates[2]<<", HODOY2TDC = "<<Coordinates[3]<<";"<<endl;
  cout<<"float HODOX1HODOX2 = "<<Coordinates[4]<<", HODOY1HODOY2 = "<<Coordinates[5]<<";"<<endl;
  cout<<"----------"<<endl;
  cout<<"THE END... thanks for choosing fitHodo.C"<<endl;
}
