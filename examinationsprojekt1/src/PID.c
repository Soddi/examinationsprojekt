/*
 * PID.c
 *
 * Created: 2014-03-17 17:17:46
 *  Author: Soded Alatia & Erik Gustafsson
 * 
 * This is the classic PID-regulation.
 * Reads the tank values, calculates the output value and writes to dacc channel.
 */ 

#include <math.h>
#include <asf.h>
#include "PID.h"
#include "semafor.h"
#include "d_a_c.h"
#include "global_variables.h"

static void read_values(void);
static void run_PID_algorithm(void);
static void write_to_dac(void);

/* This function is executed every 100ms
*	Reads the analog tank values, does the regulation and writes to DAC */
void PID_regulation(void *p)
{
	portTickType startTime = xTaskGetTickCount();
	for(;;) {
		read_values();
		run_PID_algorithm();
		write_to_dac();
		vTaskDelayUntil(&startTime, ticks_in_milliseconds);
	}
}

/*	reads the tank values.
*	The values are then rescaled to compensate for the 2,7V output value from the CA3240
*	33/27 is calculated faster than 3.3/2.7 which is essentially the same thing...
*	It should be noted that we have forgotten to typecast the value*/
static void read_values(void)
{
	if((user_P) == 0 || user_I == 0 || (user_D) == 0) {
		xSemaphoreGive(semafor_signal);
		vTaskDelay(50);
	}
	
	adc_start(ADC);
	while ((adc_get_status(ADC) & (0x1 << 24)) == 0);
	tank1_value = adc_get_channel_value(ADC, ADC_CHANNEL_10) * (double)33/27; // A8
	tank2_value = adc_get_channel_value(ADC, ADC_CHANNEL_11) * (double)33/27; // A9
}

/* calculates both the error and the output value using PID */
void run_PID_algorithm(void)
{
	static double old_error_value = 0;
	double sum_error = 0;
	double Preg = 0;
	double Ireg = 0;
	double Dreg = 0;
	
	if(user_setpoint == 0) {
		error_value = 0;
	} else {
		error_value = (double)user_setpoint - tank2_value;
	}
	sum_error = (sum_error + error_value);
	
	/*PID*/
	Preg = error_value;
	Ireg = ((SAMPLE_TIME/user_I) * sum_error);
	Dreg = (user_D / 10) * (error_value - old_error_value) / SAMPLE_TIME;
	output_value = (uint32_t) (user_P/10 * (Preg + Ireg + Dreg));
	old_error_value = error_value;
}

static void write_to_dac(void)
{
	static uint16_t current_sample = 0;
	
	/* control mode */
	if(current_sample < NBR_OF_SAMPLES) {
		if(output_value > MAX_OUTPUT_VALUE) {
			output_value = MAX_OUTPUT_VALUE;
		}
		current_sample++;
	} else {
		output_value = 0;
	}
	
	/*failsafe mode*/
	if(tank1_value > MAX_TANK1_VALUE) {
		output_value = 0;
	}
	
	dacc_write_conversion_data(DACC,output_value);

	xSemaphoreGive(semafor_signal);
}