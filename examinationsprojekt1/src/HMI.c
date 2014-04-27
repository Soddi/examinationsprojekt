/*
 * HMI.c
 *
 *	Created: 2014-03-17 17:18:26
 *  Author: Soded alatia & Erik Gustafsson
 *	This is the Human-Machine interface code. Users can change values in the LCD-display
 *	and then confirm the changes which will be saved in global variables.
 */ 

#include <asf.h>
#include "HMI.h"
#include "semafor.h"

#include <math.h>
#include "my_adc.h"
#include "lcd_shield.h"
#include "global_variables.h"

int32_t ASTERISK_VALUE = -1;			/* Asterisk_value range between 0 - 3*/ 


portTickType HMI_startTime = 0;


/* regulation characteristics */
struct {
	uint16_t setpoint;
	double P;
	uint16_t I;
	uint16_t D;
} regulation;

/**
* this function starts a tick-count, and then calls the function task2_LCD().
* this function has a periodicity of 250 milliseconds.
*/
void disp_buttons(void *p)
{
	HMI_startTime = xTaskGetTickCount();
	for(;;)
	{
		task2_LCD();
		vTaskDelayUntil(&HMI_startTime, 250);
	}
}

/*	Most of the logic is inside this function. Checks whether any button is pressed,
*	finds out which one and does the commands it's suppose to do.
*	To be frank this function should be divided into sub-functions for better readability.
*/
void handle_input(button btn_id)
{
	/* is one of the buttons pressed? */
	
	if(btn_id == NO_BUTTON) {
	}
	else if (btn_id != NO_BUTTON) {
		switch (btn_id) {
			case RIGHT:	/* Move the asterisk to the right, to choose the regulation */
			if(ASTERISK_VALUE == -1) {
				lcd_put_cursor(0, 0);
				lcd_write_str("*");
				ASTERISK_VALUE = 0;
			}
			else if (ASTERISK_VALUE == 0) {
				lcd_put_cursor(0, 0);
				lcd_write_str(" ");
				lcd_put_cursor(0, 4);
				lcd_write_str("*");
				ASTERISK_VALUE = 1;
			}
			else if (ASTERISK_VALUE == 1) {
				lcd_put_cursor(0, 4);
				lcd_write_str(" ");
				lcd_put_cursor(0, 9);
				lcd_write_str("*");
				ASTERISK_VALUE = 2;
			}
			else if (ASTERISK_VALUE == 2) {
				lcd_put_cursor(0, 9);
				lcd_write_str(" ");
				lcd_put_cursor(0, 13);
				lcd_write_str("*");
				ASTERISK_VALUE = 3;
			}
			//printf("----------------------RIGHT----------------------\n");
			break;
			case UP:	/* Increase the value for the specified regulation */
			if (ASTERISK_VALUE == 0 && regulation.setpoint < SETPOINT_MAX_VALUE) {
				regulation.setpoint += 16;
			}
			else if (ASTERISK_VALUE == 1 && regulation.P < P_MAX_VALUE) {
				regulation.P += 1;
			}
			else if (ASTERISK_VALUE == 2 && regulation.I < I_MAX_VALUE) {
				regulation.I += 50;
			}
			else if (ASTERISK_VALUE == 3 && regulation.D < D_MAX_VALUE) {
				regulation.D += 1;
			}
			//printf("----------------------UP----------------------\n");
			break;
			case DOWN:	/* Decrease the value for the specified regulation */
			if (ASTERISK_VALUE == 0 && regulation.setpoint > SETPOINT_MIN_VALUE) {
				regulation.setpoint -= 16;
			}
			else if (ASTERISK_VALUE == 1 && regulation.P > P_MIN_VALUE) {
				regulation.P -= 1;
			}
			else if (ASTERISK_VALUE == 2 && regulation.I > I_MIN_VALUE) {
				regulation.I -= 50;
			}
			else if (ASTERISK_VALUE == 3 && regulation.D > D_MIN_VALUE) {
				regulation.D -= 1;
			}
			//printf("----------------DOWN----------------------\n");
			break;
			case LEFT:	/* Move the asterisk to the left, to choose the regulation */
			if (ASTERISK_VALUE == 3) {
				lcd_put_cursor(0, 13);
				lcd_write_str(" ");
				lcd_put_cursor(0, 9);
				lcd_write_str("*");
				ASTERISK_VALUE = 2;
			}
			else if (ASTERISK_VALUE == 2) {
				lcd_put_cursor(0, 9);
				lcd_write_str(" ");
				lcd_put_cursor(0, 4);
				lcd_write_str("*");
				ASTERISK_VALUE = 1;
			}
			else if (ASTERISK_VALUE == 1) {
				lcd_put_cursor(0, 4);
				lcd_write_str(" ");
				lcd_put_cursor(0, 0);
				lcd_write_str("*");
				ASTERISK_VALUE = 0;
			}
			//printf("----------------------LEFT----------------------\n");
			break;
			case SELECT: /* Confirm the new values */
			if(xSemaphoreTake(semafor_signal, portMAX_DELAY)) {
				 user_setpoint = regulation.setpoint;
				 user_P = regulation.P;
				 user_I = regulation.I;
				 user_D = regulation.D;
				 if(ASTERISK_VALUE == 0) {
					 lcd_put_cursor(0, 0);
					 lcd_write_str("+");
					 } else if(ASTERISK_VALUE == 1) {
					 lcd_put_cursor(0, 4);
					 lcd_write_str("+");
					 } else if(ASTERISK_VALUE == 2) {
					 lcd_put_cursor(0, 9);
					 lcd_write_str("+");
					 } else if(ASTERISK_VALUE == 3) {
					 lcd_put_cursor(0, 13);
					 lcd_write_str("+");
				 }
				 //printf("----------------------SELECT----------------------\n");
 			 }
			break;
			default:
			break;
		}
	}
}


/**********************/
/* display PID-values */
/**********************/
static void display_values(void)
{
	lcd_put_cursor(1, 0);
	lcd_write_int(regulation.setpoint);
	
	lcd_put_cursor(1, 5);
	lcd_write_int(regulation.P);
	
	lcd_put_cursor(1,9);
	lcd_write_int(regulation.I);
	
	lcd_put_cursor(1, 14);
	lcd_write_int(regulation.D);
}

/*
 * Reading A/D conversion value - determining which button was pressed.
 */
static button read_buttons(void)
{
	button btn_id = NO_BUTTON;
	/* read latest A/D value */
	uint32_t adc_value = adc_get_channel_value(ADC, ADC_CHANNEL_7);
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
	/* return button ID */
	return btn_id;
}


/**************************************************************************
* This function was provided by Mathias Beckius, All cred goes to him! :) *
**************************************************************************/
/*
* This function generates an arbitrary delay. This is used as an alternative
* to a timer-based delay, which is potentially in conflict with FreeRTOS.
* A delay function provided by FreeRTOS could be an alternative, but that
* function is depending on the "tick rate frequency", which shouldn't be
* higher than 1000 Hz. Since we need delays measured in microseconds, then
* this function might do the job.
* A table of some delay times:
* lcd_delay(1) ~ 0.9 �s
* lcd_delay(17) ~ 10.1 �s
* lcd_delay(70) ~ 40.2 �s
* lcd_delay(90) ~ 50.2 �s
* lcd_delay(174) ~ 99.5 �s
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

/*Checks which button was pressed, does the following commands 
* and finally displays the new values on the LCD-display*/
void task2_LCD(void)
{
	uint32_t btn_id;
	/* read buttons on LCD shield */
	btn_id = read_buttons();
	lcd_delay(150000);
	handle_input(btn_id);
	display_values();
}