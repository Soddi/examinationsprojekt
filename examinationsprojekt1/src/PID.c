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

uint16_t h1;
uint16_t h2;
int32_t u;
int32_t e;
int32_t wk = 0;
uint16_t t;

uint16_t K = 0;
uint16_t uMax = 100;
uint16_t uMin = 0;
uint16_t Kp = 0;
uint16_t Ki = 500;
uint16_t Kd = 85;
uint16_t Ti = 0;
uint16_t Td = 0;
int32_t e_old_value = 0;
uint16_t setpoint = 0;
int16_t ticks_in_milliseconds = 100;

double Preg = 0;
double Ireg = 0;
double Dreg = 0;

uint16_t tank1_value = 0;
uint16_t tank2_value = 0;
int32_t error_value = 0;
uint16_t output_signal = 0;

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
		
		if(Kp == 0 || Ti == 0 || Td == 0) {
			xSemaphoreGive(semafor_signal);
			vTaskDelay(50);
		}
		
		Kp = user_P;
		Ti = user_I;
		Td = user_D;
		setpoint = user_setpoint; //To rescale 0-1000.
		K = Kp;
		
		adc_start(ADC);
		while ( (adc_get_status(ADC)) & ((0x1 << 24) == 0) ) {
			//Do nothing since we have no values from the channels
		}
		h1 = (adc_get_channel_value(ADC, ADC_CHANNEL_10)); // A8
		h2 = (adc_get_channel_value(ADC, ADC_CHANNEL_11)); // A9
		
		h1 = h1 * 33/27; //faster than 3.3/2.7
		h2 = h2 * 33/27;
		
		tank1_value = h1;
		tank2_value = h2;
		
		if(setpoint == 0) {
			e = 0;
		} else {
			e = ((int32_t)setpoint) - ((int32_t)h2);
			error_value = e;
		}
		//printf("Tank2: %"PRIu16"\n", h2);
		//printf("Error:%"PRId32"\n", e);
		wk = (wk + e);
		//printf("Error sum:%"PRIu32"\n", wk);
		
		/*PID*/
		Preg = (double) e;
		Ireg = ((sample_time/(double)Ti) * wk);
		Dreg =  (Td * ((e - e_old_value)/(double)(sample_time)));
		u = (int16_t) (K * (Preg + Ireg + Dreg));
// 		printf("Preg: %d\n", (uint16_t)(Preg));
// 		printf("IReg: %d\n", (int16_t)(Ireg));
// 		printf("Dreg: %d\n", (int16_t)(Dreg));
		
		/* Failsafe mode */
		if(u > 4095) {
			u = 4095;
		} else if(u < 0) {
			u = 0;
		}
		output_signal = u;
		
		/*Temp failsafe mode*/
		if(h1 > 3000) {
			u = 0;
			//printf("REACHED FAILSAFE MODE!!!!!!!!!!\n");
		}
		//printf("Styrsignal: %"PRId32"\n", u);
		dacc_write_conversion_data(DACC,u);
		e_old_value = e;

		/* Makro to give a semaphore. The sempahpore must
		have been created by calling vSemaphoreCreateBinary()
		*/
		/* Every other second task1 signals to task2 */
		xSemaphoreGive(semafor_signal);
		//vTaskDelay(50);
		vTaskDelayUntil(&startTime, ticks_in_milliseconds);
	}
}