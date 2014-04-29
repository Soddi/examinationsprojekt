/*
 * PID.h
 *
 * Created: 2014-03-17 17:18:06
 *  Author: Soded Alatia och Erik gustafsson
 */ 


#ifndef PID
#define PID

#include <asf.h>

#define ticks_in_milliseconds 100
#define SAMPLE_TIME (ticks_in_milliseconds/(double)1000)
#define NBR_OF_SAMPLES 4000
#define MAX_OUTPUT_VALUE 4095
#define MAX_TANK1_VALUE 3500

void PID_regulation(void*);

#endif /* PID */