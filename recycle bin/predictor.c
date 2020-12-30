//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include <math.h> 
#include <string.h>
#include <stdlib.h>
//
// TODO:Student Information
//
const char *studentName = "Wenran Tian";
const char *studentID   = "A59002429";
const char *email       = "w2tian@ucsd.edu";

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
// For common gshare predictor
struct Gshate_Preditor{
	uint32_t history_queue; // we use an integer to cache the history
	uint32_t threshold;     // the largest index of BHT
	uint8_t BHT[0];         // here we do not know the length of this array so that use 0.
} GP;

// For common predictor in alpha 21264
struct Tournament{
	uint32_t history_queue; // we use an integer to cache the history
	uint32_t threshold;     // the largest index of BHT
	uint8_t BHT[0];         // we use one single array, because it has 8 bits which is larger than 2 two-bit preditor and 1 chooser (8 > 2*2+2=6)	
	
} TP;


//struct Gshate_preditor{
//	uint32_t history_queue;
//	uint32_t threshold;
//	uint8_t BHT[0];
//} GP;



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
  	int length = (int) pow(2, ghistoryBits);
  
  	struct Gshate_Preditor *buff1 = ( struct Gshate_Preditor *) malloc(sizeof(struct Gshate_Preditor) + length);  // uint8_t is one byte length
  	GP = *buff1;
  	GP.history_queue = 0;
	GP.threshold = length - 1; 
  
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
  int result;

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:{
    	result = GP.BHT[pc^GP.history_queue];
    	return result>1 ? TAKEN :  NOTTAKEN;
	}
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
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
  int operand;
    switch (bpType) {
    case STATIC:
      return;
    case GSHARE:{
    	operand = GP.BHT[pc^GP.history_queue];
    	operand = (outcome==TAKEN ? operand+1 :  operand-1);
    	operand = (operand >3 ? 3 : operand);
    	operand = (operand <0 ? 0 : operand);
    	GP.BHT[pc^GP.history_queue] = operand;
    	GP.history_queue = (GP.history_queue<<1 + outcome) && GP.threshold; 
    	
    	break;
	}
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
}
