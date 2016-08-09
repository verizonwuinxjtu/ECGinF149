#ifndef ECG_PROCESSING_H_
#define ECG_PROCESSING_H_
#include "global.h"
uint InitFlag;
uint heartrate,beats,counter,duration;
int Datain,Dataout,THR,MIN,MAX;


void Filter_50Hz(void);

void HeartRateCal(void);


#endif
