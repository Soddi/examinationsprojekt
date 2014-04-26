/*
 * task1.c
 *
 * Created: 2014-03-17 17:17:46
 *  Author: Soded Alatia & Erik Gustafsson
 */ 

#include <math.h>
#include <asf.h>
#include "PID.h"
#include "semafor.h"
#include "d_a_c.h"
#include "global_variables.h"

int32_t old_error_value = 0;
int32_t sum_error = 0;

double Preg = 0;
double Ireg = 0;
double Dreg = 0;

uint16_t current_sample = 0;

portTickType startTime = 0;

/**
* Task1 with the function that give the smepahore every other second.
*/
/* This function enables and configures the Digital-to-Analog Converter */

void PID_regulation(void *p)
{
	for(;;) {
		startTime = xTaskGetTickCount();
		read_values();
		run_PID_algorithm();
		write_to_dac();
		vTaskDelayUntil(&startTime, ticks_in_milliseconds);
	}
}

void read_values(void)
{
	if((user_P/10) == 0 || user_I == 0 || (user_D/10) == 0) {
		xSemaphoreGive(semafor_signal);
		vTaskDelay(50);
	}
	
	adc_start(ADC);
	while ( (adc_get_status(ADC)) & ((0x1 << 24) == 0) ) {
		//Do nothing since we have no values from the channels
	}
	tank1_value = (adc_get_channel_value(ADC, ADC_CHANNEL_10)) * 33/27; // A8
	tank2_value = (adc_get_channel_value(ADC, ADC_CHANNEL_11)) * 33/27; // A9
}

void run_PID_algorithm(void)
{
	if(user_setpoint == 0) {
		error_value = 0;
		} else {
		error_value = ((int32_t)user_setpoint) - ((int32_t)tank2_value);
	}
	sum_error = (sum_error + error_value);
	
	/*PID*/
	Preg = (double) error_value;
	Ireg = ((SAMPLE_TIME/(double)user_I) * sum_error);
	Dreg =  ((user_D) * ((error_value - old_error_value)/(double)(SAMPLE_TIME))/10);
	output_signal = (uint32_t) (user_P * (Preg + Ireg + Dreg))/10;
}

void write_to_dac(void)
{
	/* control mode */
	if(current_sample < NBR_OF_SAMPLES) {
		if(output_signal > MAX_OUTPUT_VALUE) {
			output_signal = MAX_OUTPUT_VALUE;
		}
		else {
			output_signal = 0;
		}
		current_sample++;
	}
	/*Temp failsafe mode*/
	if(tank1_value > MAX_TANK1_VALUE) {
		output_signal = 0;
	}

	dacc_write_conversion_data(DACC,output_signal);
	old_error_value = error_value;
	/* Makro to give a semaphore. The sempahpore must
	have been created by calling vSemaphoreCreateBinary()
	*/
	/* Every other second task1 signals to task2 */
	xSemaphoreGive(semafor_signal);
	//vTaskDelay(50);
}