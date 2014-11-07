#include "../interface/analysis_tools.h"

using namespace std;

double getAmplitude_fromTot(int iCh, float ix){

  TF1* func = new TF1("func", "[0] + [1] * x + [2] * x*x + [3] * x*x*x + [4] * x*x*x*x", 0., 1000.);

  if(iCh == 4) func->SetParameters(1.12648e+03, -2.56113e+01, 1.63616e+00, -3.15637e-02, 2.29793e-04);
  if(iCh == 5) func->SetParameters(1.02036e+03, -4.70789e+00, 7.15330e-01, -1.71378e-02, 1.84110e-04);
  if(iCh == 6) func->SetParameters(1.01040e+03, -2.92853e+00, 5.80857e-01, -1.14420e-02, 1.28716e-04);
  if(iCh == 7) func->SetParameters(1.08410e+03, -1.66636e+01, 1.11720e+00, -1.83232e-02, 1.06718e-04);
  if(iCh == 9) func->SetParameters(1.08692e+03, -1.92588e+01, 1.30588e+00, -2.32895e-02, 1.57087e-04);
  if(iCh < 4 || iCh == 8) return -1;
  return func->Eval(ix);
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
    }
    minValue = samples->at(minSample);
    if(AmpFraction == 1) 
        return minSample*step;
    for(int iSample=minSample; iSample>t1; iSample--)
    {
        if(samples->at(iSample) > minValue*AmpFraction) 
        {
            cfSample = iSample;
	    //	    std::cout << " CF cfSample = " << cfSample << " samples->at(iSample) = " << samples->at(iSample) << std::endl;
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

    float sigma2 = pow(step/sqrt(12)*B,2);
 
    for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
    {
        if(cfSample+n<0) continue;
        Chi2 = Chi2 + pow(samples->at(cfSample+n) - A - B*((cfSample+n)*step),2)/sigma2;
    } 
    // A+Bx = AmpFraction * amp
    float interpolation = (samples->at(minSample) * AmpFraction - A) / B;
    //    std::cout << " >>> interp = " << interpolation << " A = " << A << " B = " << B << std::endl;
    return interpolation;

}

//---------------------------------------------------------------------------------------
//---estimate time (ns) with CFD, samples must be a negative signal and baseline subtracted
float TimeOverThreshold(int t1, int t2, const vector<float>* samples, float threshold, 
			float step, int Nsamples){

  float xx= 0.;
  float xy= 0.;
  float Sx = 0.;
  float Sy = 0.;
  float Sxx = 0.;
  float Sxy = 0.;
  float Chi2 = 0.;
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

  //  float sigma2 = pow(step/sqrt(12)*B,2);

  // for(int n=-(Nsamples-1)/2; n<=(Nsamples-1)/2; n++)
  //   {
  //     if(startSample+n<0) continue;
  //     Chi2 = Chi2 + pow(samples->at(startSample+n) - A - B*((startSample+n)*step),2)/sigma2;
  //   }

  // A+Bx = threshold

  float tStart = startSample;
  float tStart_int = (samples->at(startSample) - A) / B / step;
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
  float tStop = stopSample;
  float tStop_int = (samples->at(stopSample) - A) / B / step;
  //  std::cout << " >>> tStart = " << tStop << " tStop_int = " << tStop_int << std::endl;
  // std::cout << " >>> tStop = " << tStop << std::endl;
  // std::cout << " >>> A = " << A << " B = " << B << std::endl;
  ///    std::cout << " >>> DT = " << tStop - tStart << std::endl;
  //  return (tStop - tStart);
  return (tStop_int - tStart_int) / step;
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
      if (samples->at(bin)>0)    integral+=0;
      else                       integral += samples->at(bin);

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

  float tStart = startSample;
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

