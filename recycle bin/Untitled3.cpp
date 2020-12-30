#include <stdio.h>
#include <math.h> 
#include <stdint.h>
#include <stdlib.h>

struct Local_Predictor_Patten{
	uint32_t threshold;     // the largest index of Partten_Table (cut out pc number)
	uint32_t *Partten_Table;     // we cannot decide how long the table is for now  
	
} LPPa;

main(){
	
	LPPa.Partten_Table = (uint32_t*) malloc(sizeof(uint32_t)*65536);
	LPPa.Partten_Table[65535] = 10;
	LPPa.Partten_Table[65534] = 77;
	uint8_t a = 1; 
	
	uint32_t b;
	int length = (int) pow(2, 5);
	b = length - 1; 
	
	printf("%d", 1 << 2*3);
	 
}
