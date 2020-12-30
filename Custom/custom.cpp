#include<iostream>
#include "predictor.h"
#include <iostream>

#define STRONGLY_NOT_TAKEN 0
#define WEAKLY_NOT_TAKEN 1
#define WEAKLY_TAKEN 2
#define STRONGLY_TAKEN 3

#define STRONGLY_LOCAL 3
#define WEAKLY_LOCAL 2
#define WEAKLY_GLOBAL 1
#define STRONGLY_GLOBAL 0

u_int8_t increaseAfterTaken(u_int8_t currentState){
  if(currentState!=STRONGLY_TAKEN){
    return currentState+1;
  }
  else{
    return currentState;
  }
}

u_int8_t decreaseAfterNotTaken(u_int8_t currentState){
  if(currentState!=STRONGLY_NOT_TAKEN){
    return currentState-1;
  }
  else{
    return currentState;
  }
}


#define GSBits 11
#define GSize (1<<GSBits)
#define GSValidBits (1<<GSBits)-1
u_int8_t GShare_PHT[GSize];
u_int32_t GShare_History;

#define BIMODE_Bits 12
#define BSize (1<<BIMODE_Bits)
#define BMValidBits (BSize-1)
u_int8_t BM_PHT[BSize];
#define SB 0
#define WB 1
#define WG 2
#define SG 3
#define CHOOSER_Bits 11
#define CSize (1<<CHOOSER_Bits)
#define CHOOSERValidBits (CSize-1)
u_int8_t ChooseTable[CSize];


void init_custom_predictor (){
    // init gshare part
    GShare_History = 0;
    for(int i=0; i<GSize; i++){
        GShare_PHT[i] = SN;
    }
    // init Bi-mode part
    for(int i=0; i<BSize; i++){
        BM_PHT[i] = SN;
    }
    //init choice
    for(int i=0; i<CSize; i++){
        ChooseTable[i] = WB;
    }
}


u_int8_t make_custom_prediction(u_int32_t pc){
    u_int8_t gPrediction, bPrediction, prediction;
    u_int32_t gPHTIndex = ((pc>>2)&GSValidBits) ^ GShare_History;
    gPrediction = GShare_PHT[gPHTIndex];
    u_int32_t bPHTIndex = ((pc>>2)&BMValidBits);
    bPrediction = BM_PHT[bPHTIndex];
    u_int32_t choiceIndex = GShare_History&CHOOSERValidBits;
    u_int8_t choice = ChooseTable[choiceIndex];
    prediction = (choice> WB)?gPrediction:bPrediction; 
    return (prediction>WN)?TAKEN:NOTTAKEN;
}


void
train_custom_predictor(uint32_t pc, uint8_t outcome){
    u_int8_t gPrediction, bPrediction, gResult, bResult, choice;
    u_int32_t gPHTIndex = ((pc>>2)&GSValidBits) ^ GShare_History;
    u_int32_t bPHTIndex = ((pc>>2)&BMValidBits);
    u_int32_t choiceIndex = GShare_History&CHOOSERValidBits;
    // update history
    GShare_History = (GShare_History<<1 | outcome) & GSValidBits;
    // train gshare
    gPrediction =  GShare_PHT[gPHTIndex];
    gResult = (gPrediction>WN)?TAKEN:NOTTAKEN;
    GShare_PHT[gPHTIndex] = (outcome==TAKEN)? increaseAfterTaken(gPrediction):decreaseAfterNotTaken(gPrediction);
    // train bi-mode
    bPrediction = BM_PHT[bPHTIndex];
    bResult = (bPrediction>WN)?TAKEN:NOTTAKEN;
    BM_PHT[bPHTIndex] = (outcome==TAKEN)? increaseAfterTaken(bPrediction):decreaseAfterNotTaken(bPrediction);
    // train choice
    if(bResult!=gResult){
        choice =  ChooseTable[choiceIndex];
        ChooseTable[choiceIndex] = (gResult==outcome)?increaseAfterTaken(choice):decreaseAfterNotTaken(choice);
    }
}