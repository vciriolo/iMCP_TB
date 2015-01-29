#ifndef __init_Reco_Tree__
#define __init_Reco_Tree__

#include "TTree.h"
#include "TChain.h"

// Declaration of leaf types 
extern int                 event;
extern float                 time_CF[11];
extern float                 time_CF_corr[11];
extern float                 time_CF30[11];
extern float                 time_OT[11];
extern float                 time_start[11];
extern float                 time_stop[11];
extern float                 time_start_1000[11];
extern float                 time_stop_1000[11];
extern float                 time_start_150[11];
extern float                 time_stop_150[11];
extern float                 time_start_200[11];
extern float                 time_stop_200[11];
extern float                 time_start_500[11];
extern float                 time_stop_500[11];
extern float                 time_start_300[11];
extern float                 time_stop_300[11];
extern float                 time_Max[11];
extern float                 amp_max[11];
extern float                 amp_max_corr[11];
extern float                 amp_max_time[11];
extern float                 charge[11];
extern float                 charge_corr[11];
extern float                 baseline[11];
extern int                   run_id;
extern int                   isPCOn[11];
extern int                   HV[11];
extern int                   isTrigger[11];
extern float                 X0;
extern float tdcX;
extern float tdcY;
extern bool hodoX1[64];
extern bool hodoY1[64];
extern bool hodoX2[64];
extern bool hodoY2[64];
extern int nhodoX1;
extern int nhodoX2;
extern int nhodoY1;
extern int nhodoY2;
/*
extern int                   fibreX[8];
extern int                   fibreY[8];
extern int                   sci_front_adc;
*/

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
extern TBranch        *b_isTrigger;
extern TBranch        *b_X0;
extern TBranch *b_tdcX;
extern TBranch *b_tdcY;
extern TBranch *b_hodoX1;
extern TBranch *b_hodoY1;
extern TBranch *b_hodoX2;
extern TBranch *b_hodoY2;
extern TBranch *b_nhodoX1;
extern TBranch *b_nhodoY1;
extern TBranch *b_nhodoX2;
extern TBranch *b_nhodoY2;
/*
extern TBranch        *b_fibreX;   //!                                    
extern TBranch        *b_fibreY;   //!                                        
extern TBranch        *b_sci_front_adc;   //!                               
*/

void InitRecoTree(TTree* nt);

void init();

void SetOutTree(TTree* outTree);

#endif
