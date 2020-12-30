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
int counter = 10;
// For common gshare predictor
struct Gshate_Preditor{
	uint32_t history_queue; // we use an integer to cache the history
	uint32_t threshold;     // the largest index of BHT
//	uint32_t prediction_index_buff;
	uint8_t *BHT;         // 
} GP;


// For common predictor in alpha 21264
// index and parttern of local history
struct Local_Predictor_Patten{
	uint32_t threshold;     // the largest index of Partten_Table (cut out pc number)
	uint32_t *Partten_Table;     // we cannot decide how long the table is for now  
	
} LPPa;

// Predition of local history
struct Local_Predictor_Predictor{
	uint32_t threshold;     // the largest index of prediction table 
	uint8_t *LHT;         // 	
} LPPr;


// !! In fact, we can use the same data structure as Gshate_Preditor here.  
struct Global_History_Predictor{
//	uint32_t buff[3]; // save some important midvalues for training
	uint32_t history_queue; // we use an integer to cache the history
	uint32_t threshold;     // the largest index of BHT
	// we use one single array, because it has 8 bits which is larger than 2 two-bit preditor and 1 chooser (8 > 2+2=4)	
	// lower two bits are predictor, higher two bits are chooser
	uint32_t *Predictor_Chooser; 
} GHP;


// custom
struct Custom{
    float weight[4096][3];
} CU;




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
    if(bpType == CUSTOM){
    	ghistoryBits = 12;
		pcIndexBits = 10;
		lhistoryBits = 10;
	}
  
  
  	int i,j;
  	int length = (int) pow(2, ghistoryBits);
  	GP.history_queue = 0;
	GP.threshold = length - 1; 
	GP.BHT = (uint8_t*)  malloc(sizeof(uint8_t)*length);
	for(i=0;i<length;i++) GP.BHT[i] = 1;
	
	
	length = (int) pow(2, pcIndexBits);
	LPPa.threshold = length -1;
	LPPa.Partten_Table = (uint32_t*) malloc(sizeof(uint32_t)*length);
	
	
	length = (int) pow(2, lhistoryBits);
	LPPr.threshold = length - 1;
	LPPr.LHT = (uint8_t*) malloc(sizeof(uint8_t)*length);
	for(i=0;i<length;i++) LPPr.LHT[i] = 1;
	
	
	length = (int) pow(2, ghistoryBits);
  	GHP.history_queue = 0;
	GHP.threshold = length - 1; 
  	GHP.Predictor_Chooser = (uint32_t*)  malloc(sizeof(uint32_t)*length);
  	for(i=0;i<length;i++) GHP.Predictor_Chooser[i] = 5;    // 0101
  	
  	
  	for(i = 0; i<4096;i++){
  		CU.weight[i][0] = (float)1/3;
	  	CU.weight[i][1] = (float)1/3;
	  	CU.weight[i][2] = (float)1/3;
  		
	  }

  	
  	
}

int rangeRestrict(int a);

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
  uint32_t result, val1, val2, index;
  float cu_result;
  
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:{
    	pc = pc & GP.threshold;
    	result = GP.BHT[pc^GP.history_queue];
//    	if(result<0) printf("!!");
    	return result>1 ? TAKEN :  NOTTAKEN;
	}
    case TOURNAMENT:{
    	pc = pc & LPPa.threshold;
    	
//		printf("%d\n",pc);
    	
    	val1 = LPPa.Partten_Table[pc];
    	val1 = LPPr.LHT[val1];
    	
    	val2 = GHP.history_queue;
    	val2 = GHP.Predictor_Chooser[val2];
//    	
    	// 00 01 for predition from GHP, 10 11 for LHP
    	result = (val2 >> 2) > 1 ? val1 : (val2 & 3); // LP : GP
    	
//    	return val1>1;
//    	return (val2 & 3)>1;
    	return result>1;
    	
    	
//		return TAKEN;
    	
	}
    	
    case CUSTOM:{
		index = pc & GP.threshold;
    	result = GP.BHT[index];
    	result = result > 1 ? 1: 0;
    	
    	pc = pc & LPPa.threshold;
    	
    	
    	val1 = LPPa.Partten_Table[pc];
    	val1 = LPPr.LHT[val1];
    	val1 = val1 > 1 ? 1: 0;
    	
    	
    	val2 = GHP.history_queue;
    	val2 = GHP.Predictor_Chooser[val2] & 3;  // low bits for prediction
    	val2 = val2 > 1 ? 1: 0;
//    	
    	
//    	return val1>1;
//    	return (val2 & 3)>1;
//		printf("%f ",CU.weight[pc][0]);
		cu_result = CU.weight[GHP.history_queue][0]*result+CU.weight[GHP.history_queue][1]*val1+CU.weight[GHP.history_queue][2]*val2;
		return cu_result>0.5;

//    	return (result+val1+val2)>1;
		
	}
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
  uint32_t operand, buff,buff1, index,r1,r2,r3,i,j;
  float alpha, midvalue;
    switch (bpType) {
    case STATIC:
      return;
    case GSHARE:{
    	pc = pc & GP.threshold;     // keep the pc and history with same length;
    	index = pc^GP.history_queue;
    	operand = GP.BHT[index];
    	operand = (outcome==TAKEN ? operand+1 :  operand-1);
		operand = rangeRestrict(operand);
    	GP.BHT[index] = operand;
    	GP.history_queue = ((GP.history_queue<<1) + outcome) & GP.threshold; 
    	
    	return;
	}
    case TOURNAMENT:{
    	// train the local history predictor
    	pc = pc & LPPa.threshold;
		index =  LPPa.Partten_Table[pc];
		operand = LPPr.LHT[index];
		r1 = operand>1 ? 1 : 0;
		operand = (outcome==TAKEN ? operand+1 :  operand-1);
		LPPr.LHT[index] = rangeRestrict(operand);
		index = ((index<<1 )+ outcome) & LPPr.threshold;
		LPPa.Partten_Table[pc] = index;
		
		// train the global history predictor
		buff = GHP.Predictor_Chooser[GHP.history_queue];
		operand = buff & 3;
		r2 = operand>1 ? 1 : 0;
		operand = (outcome==TAKEN ? operand+1 :  operand-1);
		operand = rangeRestrict(operand);
		GHP.Predictor_Chooser[GHP.history_queue] = buff - (buff&3) + operand;
		buff1 = GHP.history_queue;
		GHP.history_queue = ((GHP.history_queue<<1 )+ outcome) & GHP.threshold ;
		 
		// train the chooser
		if(r1 == r2) break;
		
		
		if(outcome == r1) index = 1;
		else index = - 1;
		operand = (buff >> 2) + index;
		operand = rangeRestrict(operand);
		GHP.Predictor_Chooser[buff1] = (GHP.Predictor_Chooser[buff1]&3) + (operand << 2);
		return;
		
	}
    case CUSTOM:{

		buff1 = pc;
		pc = pc & GP.threshold;     // keep the pc and history with same length;
    	index = pc^GP.history_queue;
    	operand = GP.BHT[index];
    	r1 = operand>1? 1 : 0 ;
    	operand = (outcome==TAKEN ? operand+1 :  operand-1);
		operand = rangeRestrict(operand);
    	GP.BHT[index] = operand;
    	GP.history_queue = ((GP.history_queue<<1) + outcome) & GP.threshold; 
    	
    	pc = buff1;
    	pc = pc & LPPa.threshold;
		index =  LPPa.Partten_Table[pc];
		operand = LPPr.LHT[index];
		r2 = operand>1? 1 : 0 ;
		operand = (outcome==TAKEN ? operand+1 :  operand-1);
		LPPr.LHT[index] = rangeRestrict(operand);
		index = ((index<<1 )+ outcome) & LPPr.threshold;
		LPPa.Partten_Table[pc] = index;
		
		// train the global history predictor
		buff = GHP.Predictor_Chooser[GHP.history_queue];
		operand = buff & 3;
		r3 = operand>1? 1 : 0;
		operand = (outcome==TAKEN ? operand+1 :  operand-1);
		operand = rangeRestrict(operand);
		GHP.Predictor_Chooser[GHP.history_queue] = buff - (buff&3) + operand;
		GHP.history_queue = ((GHP.history_queue<<1 )+ outcome) & GHP.threshold;
		buff1 = GHP.history_queue;
		
		if(r1 == r2 && r3 == r2) break;
		
		alpha = 0.03;
//		printf("%f",CU.weight[0]);
		if(r1 == outcome) CU.weight[buff1][0]+= alpha;
		if(r2 == outcome) CU.weight[buff1][1]+= alpha;
		if(r3 == outcome) CU.weight[buff1][2]+= alpha;
		midvalue = CU.weight[buff1][0]+CU.weight[buff1][1]+CU.weight[buff1][2];
//		printf("%f",midvalue);
		for(i = 0; i<3;i++) CU.weight[buff1][i]=CU.weight[buff1][i]/midvalue;



	}
    default:
      break;
  }
}

int rangeRestrict(int a){
	if(a > 3) a =3;
	else if(a<0) a=0;
	return a;
}
