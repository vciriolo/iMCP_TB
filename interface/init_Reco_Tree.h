#ifndef __init_Reco_Tree__
#define __init_Reco_Tree__

#include "TTree.h"
#include "TChain.h"

// Declaration of leaf types 
extern int                 event;
extern float                 time_CF[8];
extern float                 time_CF_corr[8];
extern float                 time_CF30[8];
extern float                 time_OT[8];
extern float                 time_start[8];
extern float                 time_stop[8];
extern float                 time_start_1000[8];
extern float                 time_stop_1000[8];
extern float                 time_start_150[8];
extern float                 time_stop_150[8];
extern float                 time_start_200[8];
extern float                 time_stop_200[8];
extern float                 time_start_500[8];
extern float                 time_stop_500[8];
extern float                 time_start_300[8];
extern float                 time_stop_300[8];
extern float                 time_Max[8];
extern float                 amp_max[8];
extern float                 amp_max_corr[8];
extern float                 amp_max_time[8];
extern float                 charge[8];
extern float                 charge_corr[8];
extern float                 baseline[8];
extern int                   run_id;
extern int                   isPCOn[8];
extern int                   HV[8];
extern int                   HV2[8];
extern int                   isTrigger[8];
extern float                 X0;
// extern float tdcX;
// extern float tdcY;
extern bool hodoX[32];
extern bool hodoY[32];
extern int hodoXpos;
extern int hodoYpos;
extern int nhodoX1;
extern int nhodoY1;
extern int                   sci_front_adc;
extern int                   bgo_back_adc;

// List of branches                 
extern TBranch        *b_event;
extern TBranch        *b_time_CF;   //!                                            
extern TBranch        *b_time_CF_corr;   //!                                   
extern TBranch        *b_time_CF30;   //!                                            
extern TBranch        *b_time_OT;   //!     
extern TBranch        *b_time_start;                       
extern TBranch        *b_time_stop;       
extern TBranch        *b_time_start_1000;                       
extern TBranch        *b_time_stop_1000;       
extern TBranch        *b_time_start_150;                       
extern TBranch        *b_time_stop_150;       
extern TBranch        *b_time_start_200;                       
extern TBranch        *b_time_stop_200;       
extern TBranch        *b_time_start_500;                       
extern TBranch        *b_time_stop_500;       
extern TBranch        *b_time_start_300;                       
extern TBranch        *b_time_stop_300;       
extern TBranch        *b_time_Max;   //!                                   
extern TBranch        *b_amp_max;   //!                                        
extern TBranch        *b_amp_max_corr;   //!                                                          
extern TBranch        *b_amp_max_time;   //!                                                          
extern TBranch        *b_baseline;   //!                                      
extern TBranch        *b_charge;   //!                                     
extern TBranch        *b_charge_corr;   //!                              
extern TBranch        *b_run_id;   //!                              
extern TBranch        *b_isPCOn;   //!  
extern TBranch        *b_HV;   //!     
extern TBranch        *b_HV2;   //!     
extern TBranch        *b_isTrigger;
extern TBranch        *b_X0;
// extern TBranch *b_tdcX;
// extern TBranch *b_tdcY;
extern TBranch *b_hodoX;
extern TBranch *b_hodoY;
extern TBranch *b_hodoXpos;
extern TBranch *b_hodoYpos;
extern TBranch *b_nhodoX;
extern TBranch *b_nhodoY;
extern TBranch *b_sci_front_adc;
extern TBranch *b_bgo_back_adc;

void InitRecoTree(TTree* nt);

void init();

void SetOutTree(TTree* outTree);

#endif
