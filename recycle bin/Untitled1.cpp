#include <stdio.h>
#include <stdint.h>
#include <math.h> 
#include <string.h>
#include <stdlib.h>

//typedef struct
//{
//    int a;
//    char data[];
//}st,*pst;

struct Gshate_Preditor{
	int history_queue;
	double threshold;
	long BHT[0];
} GP;

struct Gshate_Preditor1{
	int history_queue;
	int threshold;
	int BHT[10];
} GP1;



main(){
	int buff[int(pow(2,2))];
	long s = 1;
	uint8_t ss = 1;
	int table[10];
//	buff_st_1 *buff1 = (buff_st_1 *)malloc(sizeof(buff_st_1) + sizeof(student_st));
//	GP = (struct Gshate_Preditor *)malloc(sizeof(GP)+10);
	Gshate_Preditor *buff1 = (Gshate_Preditor *) malloc(sizeof(Gshate_Preditor));
//	printf("%i\n", sizeof(struct Gshate_Preditor));
//	buff1->BHT[9] = 5;
	GP = *buff1;
	free(buff1);
	GP.BHT[7] = 5;
//	
	s = 1 > 2 ? 5+1 : 7+1;
	printf("%i", s);
	
	
	
	
}




