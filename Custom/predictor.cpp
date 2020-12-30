//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <iostream>
#include <map>
#include<math.h>
#include "predictor.h"
#include "custom.cpp"

//
// TODO:Student Information
//
const char *studentName = "Zhenya Ma";
const char *studentID   = "A59003882";
const char *email       = "z8ma@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;



//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//


typedef struct{
  u_int8_t state = 1;// default value is 1(WEAKLY_NOT_TAKEN or WEAKLY_GLOBAL)
} State;

typedef struct{
  uint32_t BHR;
  std::map<u_int32_t, State> PHT;
}  GsharePredictor;
GsharePredictor gshare; // for part1's gshare 

typedef struct{
  uint32_t GHR;
  std::map<u_int32_t, u_int32_t> localBHT;
  std::map<u_int32_t, State> localPHT;
  std::map<u_int32_t, State> globalPHT;
  std::map<u_int32_t, State> globalChooser;
} TournamentPredictor ;
TournamentPredictor tournament; // tournament's data structure


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//



// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  gshare.BHR = 0;// set history as 0(not taken)
  tournament.GHR = 0;
  init_custom_predictor ();
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //
  int prediction;
  u_int32_t historyIndex, indexPC, branchHistory, PHTindex;
  u_int8_t localPredictorResult,globalPredictorResult, choice;
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      historyIndex = pc & ((int) pow(2, ghistoryBits) - 1);
      PHTindex = historyIndex ^ gshare.BHR;
      prediction = gshare.PHT[PHTindex].state;
      return prediction>WEAKLY_NOT_TAKEN ? TAKEN: NOTTAKEN;
    case TOURNAMENT:
    // local predictor part
      indexPC = pc &((int) pow(2, pcIndexBits) - 1);
      branchHistory = tournament.localBHT[indexPC];
      localPredictorResult = tournament.localPHT[branchHistory].state;
    // global predictor part
      globalPredictorResult = tournament.globalPHT[tournament.GHR].state;
    // chooser
      choice = tournament.globalChooser[tournament.GHR].state;
      prediction = (choice>WEAKLY_GLOBAL) ? localPredictorResult : globalPredictorResult;
      return prediction>WEAKLY_NOT_TAKEN ? TAKEN: NOTTAKEN;

    case CUSTOM:
      return make_custom_prediction(pc);
      break;
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}


void train_gshare_predictor(uint32_t pc, uint8_t outcome){
  int prediction, PHTindex, localResult, globalResult;
  
  uint32_t validBits = (int) pow(2, ghistoryBits) - 1;
  uint32_t historyIndex = pc & (validBits);
  PHTindex = historyIndex ^ gshare.BHR;
  prediction = gshare.PHT[PHTindex].state;
  // update counter state
  gshare.PHT[PHTindex].state = (outcome==TAKEN)? increaseAfterTaken(prediction):decreaseAfterNotTaken(prediction);
  //update BHR
  gshare.BHR = ((gshare.BHR <<1) | outcome) & validBits;

}

void train_tournament_predictor(uint32_t pc, uint8_t outcome){
  int prediction, PHTindex, localResult, globalResult;
  //update local predictor
  uint32_t validLocalHistoryBits = (int) pow(2, lhistoryBits) - 1;
  uint32_t validPCBits = (int) pow(2, pcIndexBits) - 1;
  uint32_t BHTIndex = pc & (validPCBits);
  PHTindex = tournament.localBHT[BHTIndex];
  prediction = tournament.localPHT[PHTindex].state;
  //update counter state
  tournament.localPHT[PHTindex].state = (outcome==TAKEN)? increaseAfterTaken(prediction):decreaseAfterNotTaken(prediction);
  localResult = (prediction>WEAKLY_NOT_TAKEN) ? TAKEN: NOTTAKEN;
  //update BHT
  tournament.localBHT[BHTIndex] = ((PHTindex << 1) | outcome)&validLocalHistoryBits;


  // update global predictor
  uint32_t validGlobalHistoryBits = (int) pow(2, ghistoryBits) - 1;
  PHTindex = tournament.GHR & validGlobalHistoryBits;
  prediction = tournament.globalPHT[PHTindex].state;
  //update counter state
  //std::cout<<"out :"<< (int) outcome<<"before update:"<<(int) tournament.globalPHT[PHTindex].state<<std::endl;
  tournament.globalPHT[PHTindex].state = (outcome==TAKEN)? increaseAfterTaken(prediction):decreaseAfterNotTaken(prediction);
  //std::cout<<"after update:"<<(int)tournament.globalPHT[PHTindex].state<<std::endl;
  globalResult = (prediction>WEAKLY_NOT_TAKEN) ? TAKEN: NOTTAKEN;
  //update BHT
  tournament.GHR = ((tournament.GHR << 1) | outcome)&validGlobalHistoryBits;

  // train the chooser
  if(globalResult != localResult){// if the two predictor get same result, there is no need to train the chooser
    uint8_t choice = tournament.globalChooser[PHTindex].state;
    if(globalResult==outcome){// global predictor is right
      tournament.globalChooser[PHTindex].state = decreaseAfterNotTaken(choice);
    }
    else{// local predictor is right
      tournament.globalChooser[PHTindex].state = increaseAfterTaken(choice);
    }
  }
}
// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  
  //PART1: train Gshare
  train_gshare_predictor(pc, outcome);
  
  //PART2: train tournament
  train_tournament_predictor(pc, outcome);

  //PART3: train custom
  train_custom_predictor(pc, outcome);
}
