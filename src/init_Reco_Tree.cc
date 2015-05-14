#include "../interface/init_Reco_Tree.h"

int event;
float                 time_CF[18];
float                 time_CF_corr[18];
float                 time_CF30[18];
float                 time_OT[18];
float                 time_start[18];
float                 time_stop[18];
float                 time_start_1000[18];
float                 time_stop_1000[18];
float                 time_start_150[18];
float                 time_stop_150[18];
float                 time_start_200[18];
float                 time_stop_200[18];
float                 time_start_500[18];
float                 time_stop_500[18];
float                 time_start_300[18];
float                 time_stop_300[18];
float                 time_Max[18];
float                 amp_max[18];
float                 amp_max_corr[18];
float                 amp_max_time[18];
float                 charge[18];
float                 charge_corr[18];
float                 baseline[18];
int                   sci_front_adc;
int                   bgo_back_adc;
int                   run_id;
int                   isPCOn[18];
int                   HV[18];
int                   HV2[18];
int                   isTrigger[18];
float                 X0;
float tdcX;
float tdcY;
bool hodoX[32];
bool hodoY[32];
int hodoXpos;
int hodoYpos;
int nhodoX;
int nhodoY;

// List of branches                     
TBranch        *b_event;
TBranch        *b_time_CF;   //! 
TBranch        *b_time_CF_corr;   //!   
TBranch        *b_time_CF30;   //! 
TBranch        *b_time_OT;   //!     
TBranch        *b_time_start;                                                                                                                         
TBranch        *b_time_stop;                                
TBranch        *b_time_start_1000; 
TBranch        *b_time_stop_1000;                                
TBranch        *b_time_start_150;                                                                                                                         
TBranch        *b_time_stop_150;                                
TBranch        *b_time_start_200;                                                                                                                         
TBranch        *b_time_stop_200;                                
TBranch        *b_time_start_500;                                                                                                                         
TBranch        *b_time_stop_500;
TBranch        *b_time_start_300;                                                                                                                         
TBranch        *b_time_stop_300;
TBranch        *b_time_Max;   //! 
TBranch        *b_amp_max;   //!    
TBranch        *b_amp_max_corr;   //!   
TBranch        *b_amp_max_time;   //!  
TBranch        *b_baseline;   //!   
TBranch        *b_charge;   //!    
TBranch        *b_charge_corr;   //!  
TBranch        *b_sci_front_adc;   //!
TBranch        *b_bgo_back_adc;   //! 
/*
TBranch        *b_fibreX;   //!  
TBranch        *b_fibreY;   //!  
*/
TBranch        *b_run_id;   //!   
TBranch        *b_isPCOn;   //! 
TBranch        *b_HV;   //!     
TBranch        *b_HV2;   //!     
TBranch        *b_isTrigger;
TBranch        *b_X0;
TBranch *b_tdcX;
TBranch *b_tdcY;
TBranch *b_hodoX;
TBranch *b_hodoY;
TBranch *b_hodoXpos;
TBranch *b_hodoYpos;
TBranch *b_nhodoX;
TBranch *b_nhodoY;

void init()
{
}

void SetOutTree(TTree* outTree)
{
  //---standard analysis branches
  outTree->Branch("event",&event,"event/I");
  outTree->Branch("time_CF",&time_CF,"time_CF[18]/F");
  outTree->Branch("time_CF_corr",&time_CF_corr,"time_CF_corr[18]/F");
  outTree->Branch("time_CF30",&time_CF30,"time_CF30[18]/F");
  outTree->Branch("time_OT",&time_OT,"time_OT[18]/F");
  outTree->Branch("time_start",&time_start,"time_start[18]/F");
  outTree->Branch("time_stop",&time_stop,"time_stop[18]/F");
  outTree->Branch("time_start_1000",&time_start_1000,"time_start_1000[18]/F");
  outTree->Branch("time_stop_1000",&time_stop_1000,"time_stop_1000[18]/F");
  outTree->Branch("time_start_150",&time_start_150,"time_start_150[18]/F");
  outTree->Branch("time_stop_150",&time_stop_150,"time_stop_150[18]/F");
  outTree->Branch("time_start_200",&time_start_200,"time_start_200[18]/F");
  outTree->Branch("time_stop_200",&time_stop_200,"time_stop_200[18]/F");
  outTree->Branch("time_start_500",&time_start_500,"time_start_500[18]/F");
  outTree->Branch("time_stop_500",&time_stop_500,"time_stop_500[18]/F");
  outTree->Branch("time_start_300",&time_start_300,"time_start_300[18]/F");
  outTree->Branch("time_stop_300",&time_stop_300,"time_stop_300[18]/F");
  outTree->Branch("time_Max",&time_Max,"time_Max[18]/F");
  outTree->Branch("amp_max",&amp_max,"amp_max[18]/F");
  outTree->Branch("amp_max_corr",&amp_max_corr,"amp_max_corr[18]/F");
  outTree->Branch("amp_max_time",&amp_max_time,"amp_max_time[18]/F");
  outTree->Branch("charge",&charge,"charge[18]/F"); 
  outTree->Branch("charge_corr",&charge_corr,"charge_corr[18]/F");
  outTree->Branch("baseline",&baseline,"baseline[18]/F");

  //---global branches    
  outTree->Branch("sci_front_adc",&sci_front_adc,"sci_front_adc/I");
  outTree->Branch("bgo_back_adc",&bgo_back_adc,"bgo_back_adc/I");
  outTree->Branch("run_id",&run_id,"run_id/I");

  //---additional branches
  outTree->Branch("isPCOn",&isPCOn,"isPCOn[18]/I");
  outTree->Branch("HV",&HV,"HV[18]/I");
  outTree->Branch("HV2",&HV2,"HV2[18]/I");
  outTree->Branch("isTrigger",&isTrigger,"isTrigger[18]/I");
  outTree->Branch("X0",&X0,"X0/F");
  // outTree->Branch("tdcX",&tdcX,"tdcX/F");
  // outTree->Branch("tdcY",&tdcY,"tdcY/F");
  outTree->Branch("hodoX",&hodoX,"hodoX[32]/O");
  outTree->Branch("hodoY",&hodoY,"hodoY[32]/O");
  outTree->Branch("hodoXpos",&hodoXpos,"hodoXpos/I");
  outTree->Branch("hodoYpos",&hodoYpos,"hodoYpos/I");
  outTree->Branch("nhodoX",&nhodoX,"nhodoX/I");
  outTree->Branch("nhodoY",&nhodoY,"nhodoY/I");
}

   
void InitRecoTree(TTree* nt)
{
  nt->SetBranchAddress("event", &event, &b_event);
  nt->SetBranchAddress("time_CF", &time_CF, &b_time_CF);
  nt->SetBranchAddress("time_CF_corr", &time_CF_corr, &b_time_CF_corr);
  nt->SetBranchAddress("time_CF30", &time_CF30, &b_time_CF30);
  nt->SetBranchAddress("time_OT", &time_OT, &b_time_OT);
  nt->SetBranchAddress("time_start", &time_start, &b_time_start);
  nt->SetBranchAddress("time_stop", &time_stop, &b_time_stop);
  nt->SetBranchAddress("time_start_1000", &time_start_1000, &b_time_start_1000);
  nt->SetBranchAddress("time_stop_1000", &time_stop_1000, &b_time_stop_1000);
  nt->SetBranchAddress("time_start_150", &time_start_150, &b_time_start_150);
  nt->SetBranchAddress("time_stop_150", &time_stop_150, &b_time_stop_150);
  nt->SetBranchAddress("time_start_200", &time_start_200, &b_time_start_200);
  nt->SetBranchAddress("time_stop_200", &time_stop_200, &b_time_stop_200);
  nt->SetBranchAddress("time_start_500", &time_start_500, &b_time_start_500);
  nt->SetBranchAddress("time_stop_500", &time_stop_500, &b_time_stop_500);
  nt->SetBranchAddress("time_start_300", &time_start_300, &b_time_start_300);
  nt->SetBranchAddress("time_stop_300", &time_stop_300, &b_time_stop_300);
  nt->SetBranchAddress("time_Max", &time_Max, &b_time_Max);
  nt->SetBranchAddress("amp_max", &amp_max, &b_amp_max);
  nt->SetBranchAddress("amp_max_corr", &amp_max_corr, &b_amp_max_corr);
  nt->SetBranchAddress("amp_max_time", &amp_max_time, &b_amp_max_time);
  nt->SetBranchAddress("charge", &charge, &b_charge);
  nt->SetBranchAddress("charge_corr", &charge_corr, &b_charge_corr);
  nt->SetBranchAddress("baseline", &baseline, &b_baseline);
  nt->SetBranchAddress("run_id", &run_id, &b_run_id);
  nt->SetBranchAddress("isPCOn", &isPCOn, &b_isPCOn);
  nt->SetBranchAddress("HV", &HV, &b_HV);
  nt->SetBranchAddress("HV2", &HV2, &b_HV2);
  nt->SetBranchAddress("isTrigger", &isTrigger, &b_isTrigger);
  nt->SetBranchAddress("X0", &X0, &b_X0);
  // nt->SetBranchAddress("tdcX", &tdcX, &b_tdcX);
  // nt->SetBranchAddress("tdcY", &tdcY, &b_tdcY);
  nt->SetBranchAddress("hodoX", &hodoX, &b_hodoX);
  nt->SetBranchAddress("hodoY", &hodoY, &b_hodoY);
  nt->SetBranchAddress("nhodoX", &nhodoX, &b_nhodoX);
  nt->SetBranchAddress("nhodoY", &nhodoY, &b_nhodoY);
  nt->SetBranchAddress("sci_front_adc", &sci_front_adc, &b_sci_front_adc);
  nt->SetBranchAddress("bgo_back_adc", &bgo_back_adc, &b_bgo_back_adc);

  /*
  nt->SetBranchAddress("fibreX", &fibreX, &b_fibreX);
  nt->SetBranchAddress("fibreY", &fibreY, &b_fibreY);
  */
}
