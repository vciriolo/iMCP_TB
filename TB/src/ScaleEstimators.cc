#include "../interface/ScaleEstimators.h"
#include "TRandom.h"


void FindTemplateFit(double& scale, double& scaleErr, double& baseL, double& baseLErr, double& xTime, double& xTimeErr, 
		     int& tStart, int& tStop, int& tMax, float& aMax, float& bLine, 
		     TH1F* h_MC, TH1F* h_DA, TF1** f_template, 
		     const bool& verbosity)
{
  if( verbosity )
    std::cout << ">>>>>> FindTemplateFit" << std::endl;
  
  TVirtualFitter::SetDefaultFitter("Fumili2");
  //float xNorm = h_DA->Integral() / h_MC->Integral() * h_DA->GetBinWidth(1) / h_MC->GetBinWidth(1);  

  histoFuncT* templateHistoFunc = new histoFuncT(h_MC, tStart, tStop);
  char funcName[50];
  sprintf(funcName,"f_template");
  
  (*f_template) = new TF1(funcName,templateHistoFunc,30,400,4,"histoFuncT");

  (*f_template)->SetParName(0,"yScale"); 
  (*f_template)->SetParName(1,"xScale"); 
  (*f_template)->SetParName(2,"xShift"); 
  (*f_template)->SetParName(3,"yShift"); 
  (*f_template)->SetLineWidth(1); 
  (*f_template)->SetNpx(10000);
  (*f_template)->SetLineColor(kRed+2);


  // std::cout << " tMax = " << tMax << std::endl;
  // std::cout << " (tStart + (tStop-tStart)/3.) = " << int(tStart + 1.*(tStop-tStart)/3. - 300.) << std::endl;

  if(tStart != 0 && tStop != 0)  (*f_template)->SetParameters(-1. * (aMax - bLine), 1., int(tStart + (tStop-tStart)/3. - 300.), bLine); 
  else  (*f_template)->SetParameters(-1. * (aMax - bLine), 1., tMax, bLine); 
    
  //  (*f_template)->FixParameter(3, bLine);

  //  TFumili::SetPrintLevel(0);  
  TFitResultPtr rp = h_DA->Fit(funcName,"QERLS+");
  //TFitResultPtr rp = h_DA->Fit(funcName,"RQ");
  int fStatus = rp;
  int nTrials = 0;
  while( (fStatus != 0) && (nTrials < 5) )
  {
    rp = h_DA->Fit(funcName,"QNERLS+");
    //rp = h_DA->Fit(funcName,"RQ");
    fStatus = rp;
    if( fStatus == 0 ) break;
    ++nTrials;
  }

  std::cout << " fStatus = " << fStatus << std::endl;
  
  //  (*f_template)->GetParameter(0)
  scale = (*f_template)->GetParameter(0);
  scaleErr = (*f_template)->GetParError(0);
  xTime = (300. / (*f_template)->GetParameter(1)) + (*f_template)->GetParameter(2);
  xTimeErr = (*f_template)->GetParError(2);
  baseL = (*f_template)->GetParameter(3);
  baseLErr = (*f_template)->GetParError(3);

  // std::cout << " S: scale = " << scale << "+/-" << scaleErr << std::endl;
  // std::cout << " S: time = " << xTime << "+/-" << xTimeErr << std::endl;
  // std::cout << " S: baseL = " << baseL << "+/-" << baseLErr << std::endl;


  // TCanvas* c1 = new TCanvas();
  // //  (*f_template)->GetYaxis()->SetRangeUser(-2., 2.);
  // (*f_template)->Draw();
  // c1->Print("pippoS.png", "png");

}

