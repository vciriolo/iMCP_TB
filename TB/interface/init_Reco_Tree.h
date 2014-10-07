#ifndef __init_Reco_Tree__
#define __init_Reco_Tree__

#include "TTree.h"
#include "TChain.h"

    // Declaration of leaf types
float                 time_CF[10]={};   
float                 amp_max[10]={};   
float                 charge[10]={};   
float                 baseline[10]={};   
int                   fibreX[8]={};   
int                   fibreY[8]={};   
int                   sci_front_adc=0;   
int                   run_id=0;   
int                   isPCOn[10]={};   
int                   HV[10]={};   
int                   isTrigger[10]={};
float                 X0=0.; 

    // List of branches
    TBranch        *b_time_CF;   //!
    TBranch        *b_amp_max;   //!
    TBranch        *b_baseline;   //!
    TBranch        *b_charge;   //!
    TBranch        *b_fibreX;   //!
    TBranch        *b_fibreY;   //!
    TBranch        *b_sci_front_adc;   //!
    TBranch        *b_run_id;   //!
    TBranch        *b_isPCOn;   //!
    TBranch        *b_HV;   //!
    TBranch        *b_isTrigger;
    TBranch        *b_X0;
   
void InitRecoTree(TTree* nt);

void init();

void SetOutTree(TTree* outTree);

#endif
