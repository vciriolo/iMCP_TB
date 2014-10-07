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

using namespace std;

//*******MAIN*******************************************************************                                                                                  
int main (int argc, char** argv)
{

  std::string outputFileName = "cfg/Scan4.cfg";
  std::ofstream outputFile ((outputFileName).c_str(), std::ofstream::out);
  
  int run,v1,v2,v3;

  for (int i=0; i<13; i++)
    {
      std::cout<<"insert: "<<std::endl;
      std::cin>>run;
      std::cin>>v1;
      std::cin>>v2;
      std::cin>>v3;
      outputFile<<run<<" "<<0<<" "<<2500<<"   "<<0.1<<"   "<<1<<"   "<<"Trig1"<<"\n";
      outputFile<<run<<" "<<1<<" "<<v1<<"   "<<0.1<<"   "<<0<<"   "<<"MiB2"<<"\n";
      outputFile<<run<<" "<<2<<" "<<1000<<"   "<<0.1<<"   "<<0<<"   "<<"Off1"<<"\n";
      outputFile<<run<<" "<<3<<" "<<v3<<"   "<<0.1<<"   "<<0<<"   "<<"Planacon"<<"\n";
      outputFile<<run<<" "<<4<<" "<<v2<<"   "<<0.1<<"   "<<0<<"   "<<"MiB3"<<"\n";
      outputFile<<run<<" "<<5<<" "<<2750<<"   "<<0.1<<"   "<<1<<"   "<<"Trig2"<<"\n";
      outputFile<<run<<" "<<6<<" "<<1000<<"   "<<0.1<<"   "<<0<<"   "<<"Off2"<<"\n";
      outputFile<<run<<" "<<7<<" "<<1000<<"   "<<0.1<<"   "<<0<<"   "<<"Off3"<<"\n";
      outputFile<<run<<" "<<8<<" "<<1000<<"   "<<0.1<<"   "<<0<<"   "<<"Off4"<<"\n";	
    }

  return 0;
}
