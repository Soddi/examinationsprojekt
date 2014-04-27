/**
 * main.c
 *
 *  Author: Soded alatia & Erik Gustafsson
 *	This project controls the watertank and regulates it with a classic PID-regulation.
 *	There is also a HMI-interface to manually change the parameters of the regulation
 *	Finally it communicates with matlab to plot it in real time.
 */

#include <asf.h>
#include "PID.h"
#include "HMI.h"
#include "matlabcomm.h"

#include <math.h>
#include "timer_delay.h"
#include "my_adc.h"
#include "lcd_shield.h"
#include "global_variables.h"

/* Define a variable of the type xSemaphoreHandle */
xSemaphoreHandle semafor_signal = 0;

/* data type for button identification */

/* regulation characteristics */
struct {
	uint16_t setpoint;
	double P;
	uint16_t I;
	uint16_t D;
} regulation;

/*Setup Digital to Analog conversion*/
static void dacc_setup(void) {
	pmc_enable_periph_clk(ID_DACC);
	dacc_reset(DACC);
	dacc_set_transfer_mode(DACC, 0);
	dacc_set_timing(DACC,1,1,0); //1 is shortest refresh period, 1 max. speed, 0 startup time
	dacc_set_channel_selection(DACC,0); //Channel DAC0
	dacc_enable_channel(DACC, 0); //enable DAC0
}

/*
 * Making some necessary initializations for hardware.
 */
static void init_hardware(void)
{
	/* Initiera SAM systemet */
	sysclk_init();
	board_init();
	delay_init(0); /* timer 0: for delay() */
	ioport_init();
	adc_setup();
	dacc_setup();
	lcd_init();
	
}

/*	initial program text on the LCD-shield */
void display_program_text(void)
{
	lcd_put_cursor(0, 1);
	lcd_write_str("B");
	lcd_put_cursor(0, 5);
	lcd_write_str("Kp");
	lcd_put_cursor(0, 10);
	lcd_write_str("Ti");
	lcd_put_cursor(0, 14);
	lcd_write_str("Td");
}

/*	Initialise serial communication */
void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, & uart_serial_options); //retrieves the memory location of the object
	/* Stdout shall not be buffered */
	#if defined(__GNUC__)
	setbuf(stdout, NULL);
	#else
	#endif
}

int main(void)
{
	init_hardware();
	display_program_text(); //Setpoint, P, I, D- text.
	regulation.setpoint = SETPOINT_START_VALUE;
	regulation.P = P_START_VALUE;
	regulation.I = I_START_VALUE;
	regulation.D = D_START_VALUE;
	while(1) {
		configure_console();

		/* a semaphore cannot be used wihtout calling vSemaphoreCreateBinary() */
		vSemaphoreCreateBinary(semafor_signal);
		/* Create the task giving the semaphore */
		/* Ranges between prio 1-3 where 3 is highest prio and 1 is lowest */
		if (xTaskCreate(PID_regulation, (const signed char * const) "Task1", 1024, NULL, 3, NULL) != pdPASS) {
		}
		/* Create a task taking the semaphore and doing it’s stuff */
		if (xTaskCreate(disp_buttons, (const signed char * const) "Task2", 1024, NULL, 2, NULL) != pdPASS) {
		}
		/* Create a task taking the semaphore and doing it’s stuff */
 		if (xTaskCreate(communication, (const signed char * const) "Task3", 1024, NULL, 1, NULL) != pdPASS) {
 		}
		vTaskStartScheduler();
	}
}