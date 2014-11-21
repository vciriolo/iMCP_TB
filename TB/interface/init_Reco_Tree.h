#ifndef __init_Reco_Tree__
#define __init_Reco_Tree__

#include "TTree.h"
#include "TChain.h"

// Declaration of leaf types                                                                                                                                      
extern float                 time_CF[10];
extern float                 time_CF_corr[10];
extern float                 time_OT[10];
extern float                 time_start[10];
extern float                 time_stop[10];
extern float                 time_Max[10];
extern float                 amp_max[10];
extern float                 amp_max_corr[10];
extern float                 amp_max_time[10];
extern float                 charge[10];
extern float                 charge_corr[10];
extern float                 baseline[10];
extern int                   fibreX[8];
extern int                   fibreY[8];
extern int                   sci_front_adc;
extern int                   run_id;
extern int                   isPCOn[10];
extern int                   HV[10];
extern int                   isTrigger[10];
extern float                 X0;
extern float tdcX;
extern float tdcY;

// List of branches                                                                                                                                               
extern TBranch        *b_time_CF;   //!                                            
extern TBranch        *b_time_CF_corr;   //!                                   
extern TBranch        *b_time_OT;   //!     
extern TBranch        *b_time_start;                       
extern TBranch        *b_time_stop;                       
extern TBranch        *b_time_Max;   //!                                   
extern TBranch        *b_amp_max;   //!                                        
extern TBranch        *b_amp_max_corr;   //!                                                          
extern TBranch        *b_amp_max_time;   //!                                                          
extern TBranch        *b_baseline;   //!                                      
extern TBranch        *b_charge;   //!                                     
extern TBranch        *b_charge_corr;   //!                              
extern TBranch        *b_fibreX;   //!                                    
extern TBranch        *b_fibreY;   //!                                        
extern TBranch        *b_sci_front_adc;   //!                               
extern TBranch        *b_run_id;   //!                              
extern TBranch        *b_isPCOn;   //!  
extern TBranch        *b_HV;   //!     
extern TBranch        *b_isTrigger;
extern TBranch        *b_X0;
extern TBranch *b_tdcX;
extern TBranch *b_tdcY;

void InitRecoTree(TTree* nt);

void init();

void SetOutTree(TTree* outTree);

#endif
