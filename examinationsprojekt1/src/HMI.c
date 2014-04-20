/*
 * task2.c
 *
 * Created: 2014-03-17 17:18:26
 *  Author: Soded alatia & Erik Gustafsson
 */ 

#include <asf.h>
#include "HMI.h"
#include "semafor.h"

#include <math.h>
#include "my_adc.h"
#include "lcd_shield.h"
#include "global_variables.h"

#include <inttypes.h>

int32_t ASTERIX_VALUE = -1;			/* Asterix_value range between 0 - 3*/

uint16_t user_setpoint = 0;
uint16_t user_P = 0;
uint16_t user_I = 0;
uint16_t user_D = 0;

portTickType HMI_startTime = 0;


/* signal characteristics */
struct {
	uint16_t setpoint;
	uint16_t P;
	uint16_t I;
	uint16_t D;
} reglering;

/**
* Task2 "takes" the semaphore and finishes the execution
*/
void disp_buttons(void *p)
{
	HMI_startTime = xTaskGetTickCount();
	for(;;)
	{
		/* macro to take a semaphore. It must have been
		created with
		vSemaphoreCreateBinary().
		xSempahoreTake reurns pdTRUE if the semaphore was
		recived,
		pdFALSE if xBlockTime timed out before the semaphore
		became availible. */
		task2_LCD();
		vTaskDelayUntil(&HMI_startTime, 250);
	}
}

void handle_input(button btn_id)
{
	/* is one of the buttons pressed? */
	
	if(btn_id == NO_BUTTON) {
	}
	else if (btn_id != NO_BUTTON) {
		switch (btn_id) {
			case RIGHT:	/* Move the asterix to the right, to choose the regulation */
			if(ASTERIX_VALUE == -1) {
				lcd_put_cursor(0, 0);
				lcd_write_str("*");
				ASTERIX_VALUE = 0;
			}
			else if (ASTERIX_VALUE == 0) {
				lcd_put_cursor(0, 0);
				lcd_write_str(" ");
				lcd_put_cursor(0, 4);
				lcd_write_str("*");
				ASTERIX_VALUE = 1;
			}
			else if (ASTERIX_VALUE == 1) {
				lcd_put_cursor(0, 4);
				lcd_write_str(" ");
				lcd_put_cursor(0, 9);
				lcd_write_str("*");
				ASTERIX_VALUE = 2;
			}
			else if (ASTERIX_VALUE == 2) {
				lcd_put_cursor(0, 9);
				lcd_write_str(" ");
				lcd_put_cursor(0, 13);
				lcd_write_str("*");
				ASTERIX_VALUE = 3;
			}
			//printf("----------------------RIGHT----------------------\n");
			break;
			case UP:	/* Increase the value for the specified regulation */
			if (ASTERIX_VALUE == 0 && reglering.setpoint < SETPOINT_MAX_VALUE) {
				reglering.setpoint += 5;
			}
			else if (ASTERIX_VALUE == 1 && reglering.P < P_MAX_VALUE) {
				reglering.P += 1;
			}
			else if (ASTERIX_VALUE == 2 && reglering.I < I_MAX_VALUE) {
				reglering.I += 5;
			}
			else if (ASTERIX_VALUE == 3 && reglering.D < D_MAX_VALUE) {
				reglering.D += 1;
			}
			//printf("----------------------UP----------------------\n");
			break;
			case DOWN:	/* Decrease the value for the specified regulation */
			if (ASTERIX_VALUE == 0 && reglering.setpoint > SETPOINT_MIN_VALUE) {
				reglering.setpoint -= 5;
			}
			else if (ASTERIX_VALUE == 1 && reglering.P > P_MIN_VALUE) {
				reglering.P -= 1;
			}
			else if (ASTERIX_VALUE == 2 && reglering.I > I_MIN_VALUE) {
				reglering.I -= 5;
			}
			else if (ASTERIX_VALUE == 3 && reglering.D > D_MIN_VALUE) {
				reglering.D -= 1;
			}
			//printf("----------------DOWN----------------------\n");
			break;
			case LEFT:	/* Move the asterix to the left, to choose the regulation */
			if (ASTERIX_VALUE == 3) {
				lcd_put_cursor(0, 13);
				lcd_write_str(" ");
				lcd_put_cursor(0, 9);
				lcd_write_str("*");
				ASTERIX_VALUE = 2;
			}
			else if (ASTERIX_VALUE == 2) {
				lcd_put_cursor(0, 9);
				lcd_write_str(" ");
				lcd_put_cursor(0, 4);
				lcd_write_str("*");
				ASTERIX_VALUE = 1;
			}
			else if (ASTERIX_VALUE == 1) {
				lcd_put_cursor(0, 4);
				lcd_write_str(" ");
				lcd_put_cursor(0, 0);
				lcd_write_str("*");
				ASTERIX_VALUE = 0;
			}
			//printf("----------------------LEFT----------------------\n");
			break;
			case SELECT: /* Confirm the new values */
			if(xSemaphoreTake(semafor_signal, portMAX_DELAY)) {
				 user_setpoint = reglering.setpoint;
				 user_P = reglering.P;
				 user_I = reglering.I;
				 user_D = reglering.D;
				 if(ASTERIX_VALUE == 0) {
					 lcd_put_cursor(0, 0);
					 lcd_write_str("+");
					 } else if(ASTERIX_VALUE == 1) {
					 lcd_put_cursor(0, 4);
					 lcd_write_str("+");
					 } else if(ASTERIX_VALUE == 2) {
					 lcd_put_cursor(0, 9);
					 lcd_write_str("+");
					 } else if(ASTERIX_VALUE == 3) {
					 lcd_put_cursor(0, 13);
					 lcd_write_str("+");
				 }
				 //printf("----------------------SELECT----------------------\n");
// 				 printf("Setpoint: %"PRId16"\n", reglering.setpoint);
// 				 printf("P-reglering: %"PRId16"\n", reglering.P);
// 				 printf("I-reglering: %"PRId16"\n", reglering.I);
// 				 printf("D-reglering: %"PRId16"\n", reglering.D);
 			 }
			break;
			default:
			break;
		}
	}
}

static void display_values(void)
{
	/**********************/
	/*     Borvarde      */
	/**********************/
	
	lcd_put_cursor(1, 0);
	lcd_write_int(reglering.setpoint);
	
	lcd_put_cursor(1, 5);
	lcd_write_int(reglering.P);
	
	lcd_put_cursor(1,9);
	lcd_write_int(reglering.I);
	
	lcd_put_cursor(1, 14);
	lcd_write_int(reglering.D);
}

/*
 * Reading A/D conversion value - determining which button was pressed.
 */
static button read_buttons(void)
{
	button btn_id = NO_BUTTON;
	/* read latest A/D value */
	uint32_t adc_value = adc_get_channel_value(ADC, ADC_CHANNEL_7);
	//printf("%u\n", adc_value);
	if (adc_value < 400) { /* RIGHT? */
		btn_id = RIGHT;
	} else if (adc_value < 1000) { /* UP? */
		btn_id = UP;
	} else if (adc_value < 1500) { /* DOWN? */
		btn_id = DOWN;
	} else if (adc_value < 2000) { /* LEFT? */
		btn_id = LEFT;
	} else if (adc_value < 2500) { /* SELECT? */
		btn_id = SELECT;
	}
	/* restart A/D conversion */
	//adc_start(ADC);
	/* return button ID */
	return btn_id;
}


/*
* This function generates an arbitrary delay. This is used as an alternative
* to a timer-based delay, which is potentially in conflict with FreeRTOS.
* A delay function provided by FreeRTOS could be an alternative, but that
* function is depending on the "tick rate frequency", which shouldn't be
* higher than 1000 Hz. Since we need delays measured in microseconds, then
* this function might do the job.
* A table of some delay times:
* lcd_delay(1) ~ 0.9 탎
* lcd_delay(17) ~ 10.1 탎
* lcd_delay(70) ~ 40.2 탎
* lcd_delay(90) ~ 50.2 탎
* lcd_delay(174) ~ 99.5 탎
* lcd_delay(3000) ~ 1.7 ms
*/
void lcd_delay(uint32_t x)
{
	volatile uint32_t i;
	volatile uint32_t j;
	for (i = 0; i < x; i++) {
		for (j = 0; j < 2; j++);
	}
}

void task2_LCD(void)
{
	uint32_t btn_id;
	/* read buttons on LCD shield */
	btn_id = read_buttons();
	//delay(150000); /* use this delay function temporary */
	lcd_delay(150000);
	handle_input(btn_id);
	/* lower flag: new values has been calculated */
	//new_value = 0;
	display_values();
}