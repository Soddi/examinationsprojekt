/*
 * task1.c
 *
 * Created: 2014-03-17 17:17:46
 *  Author: Soded Alatia & Erik Gustafsson
 */ 

#include <math.h>
#include "PID.h"
#include "semafor.h"
#include "d_a_c.h"
#include "global_variables.h"
#include <asf.h>
#include <inttypes.h>

int32_t old_error_value = 0;
int32_t sum_error = 0;
int16_t ticks_in_milliseconds = 100;

double Preg = 0;
double Ireg = 0;
double Dreg = 0;

int32_t output_tmp = 0;
int i = 0;

portTickType startTime = 0;

/**
* Task1 with the function that give the smepahore every other second.
*/
/* This function enables and configures the Digital-to-Analog Converter */
void pid_reg(void *p)
{
	double sample_time = (ticks_in_milliseconds/(double)1000);
	for(;;)
	{
		startTime = xTaskGetTickCount();
		
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

		if(user_setpoint == 0) {
			error_value = 0;
		} else {
			error_value = ((int32_t)user_setpoint) - ((int32_t)tank2_value);
		}
		sum_error = (sum_error + error_value);
		
		/*PID*/
		Preg = (double) error_value;
		Ireg = ((sample_time/(double)user_I) * sum_error);
		Dreg =  ((user_D) * ((error_value - old_error_value)/(double)(sample_time))/10);
		output_tmp = (uint32_t) (user_P * (Preg + Ireg + Dreg))/10;
		
		/* control mode */
		if(output_tmp > 4095) {
			output_tmp = 4095;
		} else if(output_tmp < 0) {
			output_tmp = 0;
		}

		if(i < 2500) {
			output_signal = output_tmp;
			i++;
		} else {
			output_signal = 0;
		}
		
		/*Temp failsafe mode*/
		if(tank1_value > 3500) {
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
		vTaskDelayUntil(&startTime, ticks_in_milliseconds);
	}
}