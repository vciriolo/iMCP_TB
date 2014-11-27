#include "../interface/init_Reco_Tree.h"

float                 time_CF[10];
float                 time_CF_corr[10];
float                 time_OT[10];
float                 time_start[10];
float                 time_stop[10];
float                 time_start_100[10];
float                 time_stop_100[10];
float                 time_start_150[10];
float                 time_stop_150[10];
float                 time_start_200[10];
float                 time_stop_200[10];
float                 time_start_250[10];
float                 time_stop_250[10];
float                 time_start_300[10];
float                 time_stop_300[10];
float                 time_Max[10];
float                 amp_max[10];
float                 amp_max_corr[10];
float                 amp_max_time[10];
float                 charge[10];
float                 charge_corr[10];
float                 baseline[10];
int                   fibreX[8];
int                   fibreY[8];
int                   sci_front_adc;
int                   run_id;
int                   isPCOn[10];
int                   HV[10];
int                   isTrigger[10];
float                 X0;
float tdcX;
float tdcY;
bool hodoX1[64];
bool hodoY1[64];
bool hodoX2[64];
bool hodoY2[64];

// List of branches                     
TBranch        *b_time_CF;   //! 
TBranch        *b_time_CF_corr;   //!   
TBranch        *b_time_OT;   //!     
TBranch        *b_time_start;                                                                                                                         
TBranch        *b_time_stop;                                
TBranch        *b_time_start_100; 
TBranch        *b_time_stop_100;                                
TBranch        *b_time_start_150;                                                                                                                         
TBranch        *b_time_stop_150;                                
TBranch        *b_time_start_200;                                                                                                                         
TBranch        *b_time_stop_200;                                
TBranch        *b_time_start_250;                                                                                                                         
TBranch        *b_time_stop_250;
TBranch        *b_time_start_300;                                                                                                                         
TBranch        *b_time_stop_300;
TBranch        *b_time_Max;   //! 
TBranch        *b_amp_max;   //!    
TBranch        *b_amp_max_corr;   //!   
TBranch        *b_amp_max_time;   //!  
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
TBranch *b_hodoX1;
TBranch *b_hodoY1;
TBranch *b_hodoX2;
TBranch *b_hodoY2;

void init()
{
}

void SetOutTree(TTree* outTree)
{
  //---standard analysis branches
  outTree->Branch("time_CF",&time_CF,"time_CF[10]/F");
  outTree->Branch("time_CF_corr",&time_CF_corr,"time_CF_corr[10]/F");
  outTree->Branch("time_OT",&time_OT,"time_OT[10]/F");
  outTree->Branch("time_start",&time_start,"time_start[10]/F");
  outTree->Branch("time_stop",&time_stop,"time_stop[10]/F");
  outTree->Branch("time_start_100",&time_start_100,"time_start_100[10]/F");
  outTree->Branch("time_stop_100",&time_stop_100,"time_stop_100[10]/F");
  outTree->Branch("time_start_150",&time_start_150,"time_start_150[10]/F");
  outTree->Branch("time_stop_150",&time_stop_150,"time_stop_150[10]/F");
  outTree->Branch("time_start_200",&time_start_200,"time_start_200[10]/F");
  outTree->Branch("time_stop_200",&time_stop_200,"time_stop_200[10]/F");
  outTree->Branch("time_start_250",&time_start_250,"time_start_250[10]/F");
  outTree->Branch("time_stop_250",&time_stop_250,"time_stop_250[10]/F");
  outTree->Branch("time_start_300",&time_start_300,"time_start_300[10]/F");
  outTree->Branch("time_stop_300",&time_stop_300,"time_stop_300[10]/F");
  outTree->Branch("time_Max",&time_Max,"time_Max[10]/F");
  outTree->Branch("amp_max",&amp_max,"amp_max[10]/F");
  outTree->Branch("amp_max_corr",&amp_max_corr,"amp_max_corr[10]/F");
  outTree->Branch("amp_max_time",&amp_max_time,"amp_max_time[10]/F");
  outTree->Branch("charge",&charge,"charge[10]/F"); 
  outTree->Branch("charge_corr",&charge_corr,"charge_corr[10]/F");
  outTree->Branch("baseline",&baseline,"baseline[10]/F");
  
  //---hodoscope branches       
  outTree->Branch("fibreX",&fibreX,"fibreX[8]/I");
  outTree->Branch("fibreY",&fibreY,"fibreY[8]/I");
  
  //---global branches    
  outTree->Branch("sci_front_adc",&sci_front_adc,"sci_front_adc/I");
  outTree->Branch("run_id",&run_id,"run_id/I");

  //---additional branches
  outTree->Branch("isPCOn",&isPCOn,"isPCOn[10]/I");
  outTree->Branch("HV",&HV,"HV[10]/I");
  outTree->Branch("isTrigger",&isTrigger,"isTrigger[10]/I");
  outTree->Branch("X0",&X0,"X0/F");

  outTree->Branch("tdcX",&tdcX,"tdcX/F");
  outTree->Branch("tdcY",&tdcY,"tdcY/F");

  outTree->Branch("hodoX1",&hodoX1,"hodoX1[64]/O");
  outTree->Branch("hodoY1",&hodoY1,"hodoY1[64]/O");
  outTree->Branch("hodoX2",&hodoX2,"hodoX2[64]/O");
  outTree->Branch("hodoY2",&hodoY2,"hodoY2[64]/O");
  //    outTree->Branch("MCPName"+nameMCP->at(Ch_1),&baseline_Ch1,"baseline_"+nameMCP->at(Ch_1)+"/F");          
}

   
void InitRecoTree(TTree* nt)
{
  nt->SetBranchAddress("time_CF", &time_CF, &b_time_CF);
  nt->SetBranchAddress("time_CF_corr", &time_CF_corr, &b_time_CF_corr);
  nt->SetBranchAddress("time_OT", &time_OT, &b_time_OT);
  nt->SetBranchAddress("time_start", &time_start, &b_time_start);
  nt->SetBranchAddress("time_stop", &time_stop, &b_time_stop);
  nt->SetBranchAddress("time_start_100", &time_start_100, &b_time_start_100);
  nt->SetBranchAddress("time_stop_100", &time_stop_100, &b_time_stop_100);
  nt->SetBranchAddress("time_start_150", &time_start_150, &b_time_start_150);
  nt->SetBranchAddress("time_stop_150", &time_stop_150, &b_time_stop_150);
  nt->SetBranchAddress("time_start_200", &time_start_200, &b_time_start_200);
  nt->SetBranchAddress("time_stop_200", &time_stop_200, &b_time_stop_200);
  nt->SetBranchAddress("time_start_250", &time_start_250, &b_time_start_250);
  nt->SetBranchAddress("time_stop_250", &time_stop_250, &b_time_stop_250);
  nt->SetBranchAddress("time_start_300", &time_start_300, &b_time_start_300);
  nt->SetBranchAddress("time_stop_300", &time_stop_300, &b_time_stop_300);
  nt->SetBranchAddress("time_Max", &time_Max, &b_time_Max);
  nt->SetBranchAddress("amp_max", &amp_max, &b_amp_max);
  nt->SetBranchAddress("amp_max_corr", &amp_max_corr, &b_amp_max_corr);
  nt->SetBranchAddress("amp_max_time", &amp_max_time, &b_amp_max_time);
  nt->SetBranchAddress("charge", &charge, &b_charge);
  nt->SetBranchAddress("charge_corr", &charge_corr, &b_charge_corr);
  nt->SetBranchAddress("baseline", &baseline, &b_baseline);
  nt->SetBranchAddress("fibreX", &fibreX, &b_fibreX);
  nt->SetBranchAddress("fibreY", &fibreY, &b_fibreY);
  nt->SetBranchAddress("sci_front_adc", &sci_front_adc, &b_sci_front_adc);
  nt->SetBranchAddress("run_id", &run_id, &b_run_id);
  nt->SetBranchAddress("isPCOn", &isPCOn, &b_isPCOn);
  nt->SetBranchAddress("HV", &HV, &b_HV);
  nt->SetBranchAddress("isTrigger", &isTrigger, &b_isTrigger);
  nt->SetBranchAddress("X0", &X0, &b_X0);
  nt->SetBranchAddress("tdcX", &tdcX, &b_tdcX);
  nt->SetBranchAddress("tdcY", &tdcY, &b_tdcY);
  nt->SetBranchAddress("hodoX1", &hodoX1, &b_hodoX1);
  nt->SetBranchAddress("hodoY1", &hodoY1, &b_hodoY1);
  nt->SetBranchAddress("hodoX2", &hodoX2, &b_hodoX2);
  nt->SetBranchAddress("hodoY2", &hodoY2, &b_hodoY2);
}
