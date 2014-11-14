/* #ifndef ScaleEstimators_h */
/* #define ScaleEstimators_h */

#ifndef __ScaleEstimators__
#define __ScaleEstimators__

#include "histoFuncT.h"
//#include "histo_func.h"

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

#include "TH1F.h"
#include "TF1.h"
#include "TVirtualFitter.h"


     
void FindTemplateFit(double& scale, double& scaleErr, double& baseL, double& baseLErr, double& xTime, double& xTimeErr, 
		     int& tStart, int& tStop, int& tMax, float& aMax, float& bLine, 
		     TH1F* h_MC, TH1F* h_DA, TF1** f_template, 
		     const bool& verbosity = false);


#endif
