#ifndef histoFuncT_h
#define histoFuncT_h

#include "TH1.h"
#include <cstdio>

class histoFuncT
{
 public:
  
  
  //! ctor
 histoFuncT(TH1F* histo, int& tStart, int& tStop):
  tStart_p(tStart), tStop_p(tStop)
  {
    histo_p = histo;
  };
  
  
  //! dtor
  ~histoFuncT()
    {};
  
  
  //! operator()
  double operator()(double* x, double* par)
  {
    double xx = (x[0]);
    
    double xMin = histo_p -> GetBinCenter(1);
    double xMax = histo_p -> GetBinCenter(histo_p -> GetNbinsX());
    
    
    
    if( (xx < xMin) || (xx >= xMax) )
      return 1.e-10;

    //    std::cout << " tStart_p = " << tStart_p << " tStop_p = " << tStop_p <<  std::endl;
    //std::cout << " xx-par[2] = " << xx-par[2] << std::endl;

    if (tStart_p != 0 && tStop_p != 0 && xx > tStart_p && (xx) < tStop_p) { 
      //      std::cout << " rejected xx = " << xx  << std::endl;
      TF1::RejectPoint(); 
      return 0; 
    } 
    
    else
    {  
      int bin = histo_p -> FindBin(xx);
      int bin1 = 0;
      int bin2 = 0;
      
      if(xx >= histo_p -> GetBinCenter(bin))
      {
        bin1 = bin;
        bin2 = bin+1;
      }
      
      else
      {
        bin1 = bin-1;
        bin2 = bin;
      }
      
      
      double x1 = histo_p->GetBinCenter(bin1);
      double y1 = histo_p->GetBinContent(bin1);
      
      double x2 = histo_p->GetBinCenter(bin2);
      double y2 = histo_p->GetBinContent(bin2);

      double x1T = histo_p->GetBinCenter(bin1 - par[2]);
      double y1T = histo_p->GetBinContent(par[1]*(bin1 - par[2]));

      double x2T = histo_p->GetBinCenter(bin2 - par[2]);
      double y2T = histo_p->GetBinContent(par[1]*(bin2 - par[2]));
      
      double m = 1. * (y2T - y1T) / par[1] / (x2 - x1);
      
      
      /*
      if( (y1 + m * (xx - x1)) < 1.e-10)
        return 1.e-10;
      */
      
      return par[0] * (y1T + m * par[1] *(xx - x1)) + par[3];
    }
    
    return 1.e-10;  
  }
  
  
  
 private:
  
  TH1F* histo_p;
  int tStart_p;
  int tStop_p;
};

#endif
