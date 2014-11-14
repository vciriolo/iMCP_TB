#ifndef __init_Reco_Tree__
#define __init_Reco_Tree__

#include "TTree.h"
#include "TChain.h"

    // Declaration of leaf types
float                 time_CF[10]={};   
float                 time_CF_corr[10]={};   
float                 time_OT[10]={};   
float                 time_Max[10]={};   
float                 amp_max[10]={};   
float                 amp_max_corr[10]={};   
float                 charge[10]={};   
float                 charge_corr[10]={};   
float                 baseline[10]={};   
int                   fibreX[8]={};   
int                   fibreY[8]={};   
int                   sci_front_adc=0;   
int                   run_id=0;   
int                   isPCOn[10]={};   
int                   HV[10]={};   
int                   isTrigger[10]={};
float                 X0=0.; 
float tdcX=-999.;
float tdcY=-999.;

    // List of branches
    TBranch        *b_time_CF;   //!
    TBranch        *b_time_CF_corr;   //!
    TBranch        *b_time_OT;   //!
    TBranch        *b_time_Max;   //!
    TBranch        *b_amp_max;   //!
    TBranch        *b_amp_max_corr;   //!
    TBranch        *b_baseline;   //!
    TBranch        *b_charge;   //!
    TBranch        *b_charge_corr;   //!
    TBranch        *b_fibreX;   //!
    TBranch        *b_fibreY;   //!
    TBranch        *b_sci_front_adc;   //!
    TBranch        *b_run_id;   //!
    TBranch        *b_isPCOn;   //!
    TBranch        *b_HV;   //!
    TBranch        *b_isTrigger;
    TBranch        *b_X0;
TBranch *b_tdcX;
TBranch *b_tdcY;
   
void InitRecoTree(TTree* nt);

void init();

void SetOutTree(TTree* outTree);

#endif
