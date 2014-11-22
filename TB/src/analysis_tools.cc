#include "../interface/analysis_tools.h"

using namespace std;

////////////////////////////////
float get_amp_max_from_time_OT(int iCh, float ix, int PC, int run) {

  TF1* func = new TF1("func", "[0] + [1] * x + [2] * x*x + [3] * x*x*x + [4] * x*x*x*x", 0., 1000.);

  if(iCh == 4) func->SetParameters(1000, -52.94, 305.443, -152.454, 41.9803);
  if(iCh == 5) 
    {
      if (PC==1)       func->SetParameters(1000, 73.6578, 99.83, -36.9063, 33.7668);
      else             func->SetParameters(1000, -38.154, 233.842, -80.6531, 12.8032);
    }
  if(iCh == 6) 
    {
      if (run<814)     func->SetParameters(1000, 229.573, -238.61, 225.771, -21.8069);
      else             func->SetParameters(1000, 199.071, 80.5514, 98.8197, 13.8348);
    }
  if(iCh == 7) 
    {
      if (PC==1)                   func->SetParameters(1000, 83.5196, -6.21453, 20.6892, 8.57441);
      else if (PC==0 && run<727)   func->SetParameters(1000, -70.3132, 297.532, -128.166, 22.7222);
      else                         func->SetParameters(1000, -29.4327, 222.072, -82.052, 16.54);
    }
  if(iCh == 9) 
    {
      if (PC==1)       func->SetParameters(1000, 83.0385, 102.869, -57.5386, 33.8739);
      else             func->SetParameters(1000, 67.7006, 192.817, -87.4701, 22.9615);
    }

  float y=func->Eval(ix);
  delete func;
  
  if(iCh < 4 || iCh == 8) return -1;
  return y;
}

//////////////////////////////////////////
float get_time_CF_from_time_OT(int iCh, float ix, int PC, int run, float t_start) {

  TF1* func = new TF1("func", "[0] + [1] * x ", 0., 10000.);

  if(iCh == 4) func->SetParameters(-1.02056+t_start, 0.411869);
  if(iCh == 5) 
    {
      if (PC==1)       func->SetParameters(-0.974679+t_start, 0.450735);
      else             func->SetParameters(-0.941685+t_start, 0.289897);
    }
  if(iCh == 6) 
    {
      if (run<814)     func->SetParameters(-0.805311+t_start, 0.404084);
      else             func->SetParameters(-0.558887+t_start, 0.35465);
    }
  if(iCh == 7) 
    {
      if (PC==1)                   func->SetParameters(-1.03487+t_start, 0.390875);
      else if (PC==0 && run<727)   func->SetParameters(-1.37413+t_start, 0.428468);
      else                         func->SetParameters(-1.01822+t_start, 0.346153);
    }
  if(iCh == 9) 
    {
      if (PC==1)       func->SetParameters(-1.09973+t_start, 0.476794);
      else             func->SetParameters(-1.05614+t_start, 0.40859);
    }

  float y=func->Eval(ix);
  delete func;
  
  if(iCh < 4 || iCh == 8) return -1;
  return y;
}

//////////////////////////////////////////
float get_charge_from_amp_max(int iCh, float ix, int PC, int run) {

  TF1* func = new TF1("func", "[0] + [1] * x ", 0., 10000.);

  if(iCh == 4) func->SetParameters(0, 12.2536);
  if(iCh == 5) 
    {
      if (PC==1)       func->SetParameters(0, 10.9988);
      else             func->SetParameters(0, 14.5069);
    }
  if(iCh == 6) 
    {
      if (run<814)     func->SetParameters(0, 10.4325);
      else             func->SetParameters(-2913.14, 9.49739);
    }
  if(iCh == 7) 
    {
      if (PC==1)                   func->SetParameters(0, 13.5061);
      else if (PC==0 && run<727)   func->SetParameters(0, 15.008);
      else                         func->SetParameters(0, 14.3031);
    }
  if(iCh == 9) 
    {
      if (PC==1)       func->SetParameters(0, 11.4501);
      else             func->SetParameters(0, 12.8507);
    }

  float y=func->Eval(ix);
  delete func;
  
  if(iCh < 4 || iCh == 8) return -1;
  return y;
}

////////////////////////////////////////
double getAmplitude_fromTot(int iCh, float ix){

  TF1* func = new TF1("func", "[0] + [1] * x + [2] * x*x + [3] * x*x*x + [4] * x*x*x*x", 0., 1000.);

  if(iCh == 4) func->SetParameters(1.12648e+03, -2.56113e+01, 1.63616e+00, -3.15637e-02, 2.29793e-04);
  if(iCh == 5) func->SetParameters(1.02036e+03, -4.70789e+00, 7.15330e-01, -1.71378e-02, 1.84110e-04);
  if(iCh == 6) func->SetParameters(1.01040e+03, -2.92853e+00, 5.80857e-01, -1.14420e-02, 1.28716e-04);
  if(iCh == 7) func->SetParameters(1.08410e+03, -1.66636e+01, 1.11720e+00, -1.83232e-02, 1.06718e-04);
  if(iCh == 9) func->SetParameters(1.08692e+03, -1.92588e+01, 1.30588e+00, -2.32895e-02, 1.57087e-04);

  float y=func->Eval(ix);
  delete func;
  
  if(iCh < 4 || iCh == 8) return -1;
  return y;
}


double getSignal_fromAmplitude(int iCh, float ix){

  TF1* func = new TF1("func", "[0] + [1] * x ", 0., 10000.);

  if(iCh == 4) func->SetParameters(-3.62382e+01, 1.21567e+01);
  if(iCh == 5) func->SetParameters(-3.93046e+01, 1.33656e+01);
  if(iCh == 6) func->SetParameters(-2.57033e+01, 1.06569e+01);
  if(iCh == 7) func->SetParameters(-3.48622e+01, 1.34139e+01);
  if(iCh == 9) func->SetParameters(-4.96490e+01, 1.26266e+01);

  float y=func->Eval(ix);
  delete func;

  if(iCh < 4 || iCh == 8) return -1;
  return y;
}

//----------------------------------------------------------------------------------------
void DFT_lowCut(vector<float>* samples, float f_cut)
{
    float* a = (float*)malloc(sizeof(float)*samples->size());
    float* b = (float*)malloc(sizeof(float)*samples->size());
    for (unsigned int k=0; k<samples->size(); k++) 
    { 
        a[k] = 0;
        b[k] = 0; 
        for (unsigned int t=0; t<samples->size(); t++) 
        { 
            float angle = 2 * M_PI * t * k / samples->size(); 
            a[k] += samples->at(t) * cos(angle); 
            b[k] += samples->at(t) * sin(angle); 
        } 
    }
}

//---------------------------------------------------------------------------------------
//---estimate the baseline in a given range and then subtract it from the signal 
void SubtractBaseline(int tb1, int tb2, vector<float>* samples)
{
    float baseline=0;
    //---compute baseline
    for(int iSample=tb1; iSample<tb2; iSample++)
    {
        baseline += samples->at(iSample);
    }
    baseline = baseline/(float)(tb2-tb1);
    //---subtract baseline
    for(unsigned int iSample=0; iSample<samples->size(); iSample++)
    {
        samples->at(iSample) = samples->at(iSample) - baseline;
    }
}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
void TimeConstFrac_ProMedio(TH1F* samples, double& AmpFraction, double& tCF, 
			    float step, int Nsamples)
{
    float xx= 0.;
    float xy= 0.;
    float Sx = 0.;
    float Sy = 0.;
    float Sxx = 0.;
    float Sxy = 0.;
    float Chi2 = 0.;
    int minSample = 1;
    float minValue = -1.;
    int cfSample = 1; // first sample over AmpMax*CF 

    for(int iS=0; iS<samples->GetNbinsX(); ++iS){
      if(samples->GetBinContent(iS+1) < samples->GetBinContent(minSample+1)) minSample = iS;
    }
    minValue = samples->GetBinContent(minSample+1);
    tCF = minSample;
    if(AmpFraction == 1) return;
    //    std::cout << " >>> minValue = " << minValue << " minSample = " << minSample << std::endl;

    for(int iS=minSample; iS>0; --iS){
      //      std::cout << " CF cfSample = " << cfSample << " samples->at(iSample) = " << samples->GetBinContent(iS+1) << std::endl;
      if(samples->GetBinContent(iS+1) > minValue * AmpFraction){
	cfSample = iS;	
	break;
      }
    }
    //    std::cout << " >>> cfSample = " << cfSample << std::endl;

    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++){
      if(cfSample+n<0) continue;
      xx = (cfSample+n)*(cfSample+n);
      xy = (cfSample+n)*(samples->GetBinContent(cfSample+n+1));
      Sx = Sx + (cfSample+n);
      Sy = Sy + samples->GetBinContent(cfSample+n+1);
      Sxx = Sxx + xx;
      Sxy = Sxy + xy;
    }

    float Delta = Nsamples*Sxx - Sx*Sx;
    float A = (Sxx*Sy - Sx*Sxy) / Delta;
    float B = (Nsamples*Sxy - Sx*Sy) / Delta;
    
    float sigma2 = pow(1./sqrt(12)*B,2);
    
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
      {
        if(cfSample+n<0) continue;
        Chi2 = Chi2 + pow(samples->GetBinContent(cfSample+n+1) - A - B*((cfSample+n)),2)/sigma2;
      } 
    // A+Bx = AmpFraction * amp
    float interpolation = (samples->GetBinContent(minSample+1) * AmpFraction - A) / B;
    //    std::cout << " >>> interp = " << interpolation << " cfSample = " << cfSample << " tMax = " << tMax << std::endl;
    tCF = interpolation;
    return;
    //    return ;

}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeConstFrac(int t1, int t2, const vector<float>* samples, float AmpFraction, 
                    float step, int Nsamples)
{
    float xx= 0.;
    float xy= 0.;
    float Sx = 0.;
    float Sy = 0.;
    float Sxx = 0.;
    float Sxy = 0.;
    float Chi2 = 0.;
    int minSample=t1;
    int cfSample=t1; // first sample over AmpMax*CF 
    float minValue=0;

    for(int iSample=t1; iSample<t2; iSample++)
    {
        if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
	//	std::cout<<"CF: "<<iSample<<" "<<samples->at(iSample)<<" "<<minSample<<std::endl;	
    }
    //    getchar();

    minValue = samples->at(minSample);
    if(AmpFraction == 1) 
        return minSample*step;
    for(int iSample=minSample; iSample>t1; iSample--)
    {
      if(samples->at(iSample) > minValue*AmpFraction) 
        {
            cfSample = iSample;
	    //	      	    std::cout << " CF cfSample = " << cfSample << " samples->at(iSample) = " << samples->at(iSample) << std::endl;
            break;
        }
    }
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        xx = (cfSample+n)*(cfSample+n)*step*step;
        xy = (cfSample+n)*step*(samples->at(cfSample+n));
        Sx = Sx + (cfSample+n)*step;
        Sy = Sy + samples->at(cfSample+n);
        Sxx = Sxx + xx;
        Sxy = Sxy + xy;
    }

    float Delta = Nsamples*Sxx - Sx*Sx;
    float A = (Sxx*Sy - Sx*Sxy) / Delta;
    float B = (Nsamples*Sxy - Sx*Sy) / Delta;

    //    std::cout<<Nsamples<<" "<<Sxx<<" "<<Sx<<" "<<Sy<<" "<<Delta<<" "<<step<<std::endl;

    float sigma2 = pow(step/sqrt(12)*B,2);
 
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        Chi2 = Chi2 + pow(samples->at(cfSample+n) - A - B*((cfSample+n)*step),2)/sigma2;
    } 
    // A+Bx = AmpFraction * amp
    float interpolation = (samples->at(minSample) * 1. - A) / B;
    //            std::cout << " >>> interp = " << interpolation << " A = " << A << " B = " << B << std::endl;
    return interpolation;

}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeConstFracAbs(int t1, int t2, const vector<float>* samples, float AmpFraction, float AmpMax, 
                    float step, int Nsamples)
{
    float xx= 0.;
    float xy= 0.;
    float Sx = 0.;
    float Sy = 0.;
    float Sxx = 0.;
    float Sxy = 0.;
    float Chi2 = 0.;
    int minSample=t1;
    int cfSample=t1; // first sample over AmpMax*CF 
    float minValue = AmpFraction * AmpMax;

    for(int iSample=t1; iSample<t2; iSample++){
      if(samples->at(iSample) < minValue) minSample = iSample;
      //        std::cout<<"CF: "<<iSample<<" "<<samples->at(iSample)<<" "<<minSample<<std::endl;
	//      break;
    }
    //    std::cout<<"minSample: "<<minSample<<std::endl;

    for(int iSample=minSample; iSample>t1; iSample--)
    {
        if(samples->at(iSample) > minValue) 
        {
            cfSample = iSample;
	    //            	    std::cout << " CF cfSample = " << cfSample << " samples->at(iSample) = " << samples->at(iSample) << std::endl;
            break;
        }
    }
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        xx = (cfSample+n)*(cfSample+n)*step*step;
        xy = (cfSample+n)*step*(samples->at(cfSample+n));
        Sx = Sx + (cfSample+n)*step;
        Sy = Sy + samples->at(cfSample+n);
        Sxx = Sxx + xx;
        Sxy = Sxy + xy;
    }

    float Delta = Nsamples*Sxx - Sx*Sx;
    float A = (Sxx*Sy - Sx*Sxy) / Delta;
    float B = (Nsamples*Sxy - Sx*Sy) / Delta;
    //      std::cout<<Nsamples<<" "<<Sxx<<" "<<Sx<<" "<<Sy<<" "<<Delta<<" "<<step<<std::endl;

    float sigma2 = pow(step/sqrt(12)*B,2);
 
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        Chi2 = Chi2 + pow(samples->at(cfSample+n) - A - B*((cfSample+n)*step),2)/sigma2;
    } 
    // A+Bx = AmpFraction * amp
    float interpolation = (minValue - A) / B /step;
    //        std::cout << " >>> interp = " << interpolation << "minValue = "<<minValue<<" A = " << A << " B = " << B << std::endl;
    return interpolation;

}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeOverThreshold(int t1, int t2, const vector<float>* samples, float threshold, float &start, float &stop, 
			float step, int Nsamples){

  float xx= 0.;
  float xy= 0.;
  float Sx = 0.;
  float Sy = 0.;
  float Sxx = 0.;
  float Sxy = 0.;
  //  float Chi2 = 0.;
  int startSample = t1;
  int minSample = t1;

  for(int iSample=t1; iSample<t2; iSample++){
    //    if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
        if(samples->at(iSample) < threshold) minSample = iSample;
	//    std::cout<<"OT: "<<iSample<<" "<<samples->at(iSample)<<" "<<minSample<<std::endl;
  }

  for(int iSample=minSample; iSample>t1; --iSample){
    //      std::cout << " iSample = " << iSample << " samples->at(iSample) = " << samples->at(iSample) << " threshold = " << threshold << std::endl;
    if(samples->at(iSample) > threshold){
      startSample = iSample;
      //        	    std::cout << " OT startSample = " << startSample << " samples->at(iSample) = " << samples->at(iSample) << std::endl;
      break;
    }
  }

  //interpolation
  for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
      if(startSample+n<0) continue;
      xx = (startSample+n)*(startSample+n)*step*step;
      xy = (startSample+n)*step*(samples->at(startSample+n));
      Sx = Sx + (startSample+n)*step;
      Sy = Sy + samples->at(startSample+n);
      Sxx = Sxx + xx;
      Sxy = Sxy + xy;
    }

  float Delta = Nsamples*Sxx - Sx*Sx;
  float A = (Sxx*Sy - Sx*Sxy) / Delta;
  float B = (Nsamples*Sxy - Sx*Sy) / Delta;
  //    std::cout<<Nsamples<<" "<<Sxx<<" "<<Sx<<" "<<Sy<<" "<<Delta<<" "<<step<<std::endl;

  //  float sigma2 = pow(step/sqrt(12)*B,2);

  // for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
  //   {
  //     if(startSample+n<0) continue;
  //     Chi2 = Chi2 + pow(samples->at(startSample+n) - A - B*((startSample+n)*step),2)/sigma2;
  //   }

  // A+Bx = threshold

  //  float tStart = startSample;
  float tStart_int = (threshold*1. - A) / B /step;

  //  std::cout << " >>> interpStart = " << tStart_int << "minValue = "<<threshold*1.<<" A = " << A << " B = " << B << std::endl;
  //  std::cout << " >>> tStart = " << tStart << " tStart_int = " << tStart_int << std::endl;
  //  std::cout << " >>> A = " << A << " B = " << B << std::endl;
  
  ///////compute tStop
  xx = 0.;
  xy = 0.;
  Sx = 0.;
  Sy = 0.;
  Sxx = 0.;
  Sxy = 0.;
  //  Chi2 = 0.;

  float stopSample = t2;
  
  for(int iSample=minSample; iSample<t2; ++iSample){
    if(samples->at(iSample) > threshold){
      stopSample = iSample;
      break;
    }
  }

  for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
      if(stopSample+n<0) continue;
      xx = (stopSample+n)*(stopSample+n)*step*step;
      xy = (stopSample+n)*step*(samples->at(stopSample+n));
      Sx = Sx + (stopSample+n)*step;
      Sy = Sy + samples->at(stopSample+n);
      Sxx = Sxx + xx;
      Sxy = Sxy + xy;
    }

  Delta = Nsamples*Sxx - Sx*Sx;
  A = (Sxx*Sy - Sx*Sxy) / Delta;
  B = (Nsamples*Sxy - Sx*Sy) / Delta;

  // sigma2 = pow(step/sqrt(12)*B,2);

  // for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
  //   {
  //     if(stopSample+n<0) continue;
  //     Chi2 = Chi2 + pow(samples->at(stopSample+n) - A - B*((stopSample+n)*step),2)/sigma2;
  //   }

  // A+Bx = thresh
  //  float tStop = stopSample;
  float tStop_int = (threshold*1. - A) / B / step;
  //  std::cout << " >>> tStart = " << tStop << " tStop_int = " << tStop_int << std::endl;
  // std::cout << " >>> tStop = " << tStop << std::endl;
  // std::cout << " >>> A = " << A << " B = " << B << std::endl;
  ///    std::cout << " >>> DT = " << tStop - tStart << std::endl;
  //  return (tStop - tStart);
  start=tStart_int; //SAVE T_START
  stop =tStop_int; //SAVE T_STOP

  return (tStop_int - tStart_int);
}


//---------------------------------------------------------------------------------------
//---compute the maximum amplitude for negative signals (range selectable)

float AmpMax(int t1, int t2, const vector<float>* samples)
{
    int minSample=t1;
    for(int iSample=t1; iSample<t2; iSample++)
    {
        if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
    }
    return samples->at(minSample);
}

//---------------------------------------------------------------------------------------
//---compute charge for a given signal in a given samples interval 
float ComputeIntegral(int t1, int t2, const vector<float>* samples)
{
    float integral=0;
    for(int bin=t1; bin<t2; bin++)
      //      if (samples->at(bin)>0)    integral+=0;
                             integral += samples->at(bin);

    return integral;
}

//---------------------------------------------------------------------------------------
//---build signal fitfunc
TF1* GetFitFunc(TString Ch_n, histoFunc* wave, float t1, float t2) 
{
    Ch_n = "fitFunc_"+Ch_n;
    TF1* fitFunc = new TF1(Ch_n, wave, t1, t2, 4, "histoFunc");
    fitFunc->SetParLimits(1, 0.5, 1.5);
    fitFunc->FixParameter(3, 0.);
    fitFunc->SetNpx(100000);
    
    return fitFunc;
}





////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeStart(int t1, int t2, const vector<float>* samples, float threshold, 
		float step, int Nsamples){

  int startSample = t1;
  int minSample = t1;

  for(int iSample=t1; iSample<t2; iSample++){
    if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
  }

  for(int iSample=minSample; iSample>t1; --iSample){
    //      std::cout << " iSample = " << iSample << " samples->at(iSample) = " << samples->at(iSample) << " threshold = " << threshold << std::endl;
    if(samples->at(iSample) > threshold){
      startSample = iSample;
      break;
    }
  }

  //  float tStart = startSample;
  return (startSample * step);
}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeStop(int t1, int t2, const vector<float>* samples, float threshold, 
	       float step, int Nsamples){
  ///////compute tStop
  float stopSample = t2;
  float minSample = t1;

  for(int iSample=t1; iSample<t2; iSample++){
    if(samples->at(iSample) < samples->at(minSample)) minSample = iSample;
  }

  for(int iSample=minSample; iSample<t2; ++iSample){
    if(samples->at(iSample) > threshold){
      stopSample = iSample;
      break;
    }
  }

  return (stopSample * step);
}

