/*
 * task1.h
 *
 * Created: 2014-03-17 17:18:06
 *  Author: Soded Alatia och Erik gustafsson
 */ 


#ifndef TASK1_H_
#define TASK1_H_

#define ticks_in_milliseconds 100
#define SAMPLE_TIME (ticks_in_milliseconds/(double)1000)
#define NBR_OF_SAMPLES 2500
#define MAX_OUTPUT_VALUE 4095
#define MAX_TANK1_VALUE 3500

#include <asf.h>

void PID_regulation(void*);
void read_values(void);
void run_PID_algorithm(void);
void write_to_dac(void);

#endif /* TASK1_H_ */