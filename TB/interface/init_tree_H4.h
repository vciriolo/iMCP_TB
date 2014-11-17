#ifndef __init_tree_H4__
#define __init_tree_H4__

#include "TTree.h"
#include "TChain.h"

    // Declaration of leaf types
extern    unsigned int          evtNumber;
extern     unsigned int          evtTimeDist;
extern     unsigned int          evtTimeStart;
//    unsigned int          evtTime;
//    unsigned int          boardTriggerBit;
//    unsigned int          triggerWord;
extern     unsigned int          nAdcChannels;
extern    unsigned int          adcBoard[100];   
extern     unsigned int          adcChannel[100];   
extern     unsigned int          adcData[100];   
extern    unsigned int          nTdcChannels;
extern    unsigned int          tdcBoard[10];   
extern    unsigned int          tdcChannel[10];  
extern    unsigned int          tdcData[10];  
extern    unsigned int          nDigiSamples;
extern    unsigned int          digiGroup[100000];   
extern    unsigned int          digiChannel[100000];   
extern    unsigned int          digiSampleIndex[100000];   
extern    float                 digiSampleValue[100000];   

extern std::vector<float>   *TDCreco;
extern unsigned int spillNumber;

    // List of branches
extern    TBranch        *b_evtNumber;   //!
extern    TBranch        *b_evtTimeDist;   //!
extern    TBranch        *b_evtTimeStart;   //!
//    TBranch        *b_evtTime;   //!
//    TBranch        *b_boardTriggerBit;   //!
//    TBranch        *b_triggerWord;   //!
extern    TBranch        *b_nAdcChannels;   //!
extern    TBranch        *b_adcBoard;   //!
extern    TBranch        *b_adcChannel;   //!
extern    TBranch        *b_adcData;   //!
extern    TBranch        *b_nTdcChannels;   //!
extern    TBranch        *b_tdcBoard;   //!
extern    TBranch        *b_tdcChannel;   //!
extern    TBranch        *b_tdcData;   //!
extern    TBranch        *b_nDigiSamples;   //!
extern    TBranch        *b_digiGroup;   //!
extern    TBranch        *b_digiChannel;   //!
extern    TBranch        *b_digiSampleIndex;   //!
extern    TBranch        *b_digiSampleValue;   //!

extern TBranch  *b_TDCreco;
extern TBranch  *b_spillNumber;
   
void InitTree(TChain* nt);

void InitTree2(TChain* nt2);

#endif
