/**
 * \file
 *
 * \brief Empty user application template
 *
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

/* signal characteristics */
struct {
	uint16_t setpoint;
	uint16_t P;
	uint16_t I;
	uint16_t D;
} reglering;

static void dacc_setup(void) {
	pmc_enable_periph_clk(ID_DACC);
	dacc_reset(DACC);
	dacc_set_transfer_mode(DACC, 0);
	dacc_set_timing(DACC,1,1,0); //1 is shortest refresh period, 1 max. speed, 0 startup time
	dacc_set_channel_selection(DACC,0); //Channel DAC0
	dacc_enable_channel(DACC, 0); //enable DAC0
}

/*
 * Making some necessary initializations for hardware and interrupts.
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
	//adc_start(ADC);
	//config_timer_irq(1, 10); /* timer 1: for reading ADC, 10 times/second */
	lcd_init();
	
}

void display_program_text(void)
{
	/* initial program text */
// 	lcd_put_cursor(0, 0);
// 	lcd_write_str("*");
	lcd_put_cursor(0, 1);
	lcd_write_str("B");
	lcd_put_cursor(0, 5);
	lcd_write_str("Kp");
	lcd_put_cursor(0, 10);
	lcd_write_str("Ti");
	lcd_put_cursor(0, 14);
	lcd_write_str("Td");
	/*lcd_put_cursor(1,8);
	lcd_write_str("0.0");
	*/
}

void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, & uart_serial_options); //& betyder att man hämtar minnesplatsen för det objektet!
	/* Stdout shall not be buffered */
	#if defined(__GNUC__)
	setbuf(stdout, NULL);
	#else
	/* Redan i fallet IAR's Normal DLIB default
	konfiguration:
	* sänder en tecken åtgången
	*/
	#endif
}

int main(void)
{
	init_hardware(); //LCD_Shield
	display_program_text(); //Setpoint, P, I, D- text.
	while(1) {
		reglering.setpoint = SETPOINT_START_VALUE;
		reglering.P = P_START_VALUE;
		reglering.I = I_START_VALUE;
		reglering.D = D_START_VALUE;
		
		configure_console();
	
		/* Print demo information */
		/*printf("-- Freertos Exempel - Semaforer --\n\r");
		printf("-- %s\n\r", BOARD_NAME);
		printf("-- Kompilerad: %s %s --\n\r", __DATE__, __TIME__);*/
	
		/* a semaphore cannot be used wihtout calling vSemaphoreCreateBinary() */
		vSemaphoreCreateBinary(semafor_signal);
		/* Create the task giving the semaphore */
		if (xTaskCreate(pid_reg, (const signed char * const) "Task1", 1024, NULL, 3, NULL) != pdPASS) {
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