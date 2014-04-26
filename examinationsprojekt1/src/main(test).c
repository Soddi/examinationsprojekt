/**
 * \file
 *
 * \brief Empty user application template
 *
 */

#include <asf.h>
#include "task1.h"
#include "task2.h"

#include <math.h>
#include "timer_delay.h"
#include "my_adc.h"
#include "lcd_shield.h"

/* Define a variable of the type xSemaphoreHandle */
xSemaphoreHandle semafor_signal = 0;

/* data type for button identification */


#define SETPOINT_MIN_VALUE 100			/* minimum börvärde allowed */
#define SETPOINT_MAX_VALUE 500			/* maximum börvärde allowed */
#define SETPOINT_START_VALUE 360			/* initial value */


#define P_MIN_VALUE 0				/* minimum value for P-reglering */
#define P_MAX_VALUE 9				/* minimum value for P-reglering */
#define P_START_VALUE 5				/* minimum value for P-reglering */

#define I_MIN_VALUE 0				/* minimum value for I-reglering */
#define I_MAX_VALUE 99				/* minimum value for I-reglering */
#define I_START_VALUE 2				/* minimum value for I-reglering, Will be 0.XX, in this case 0.02*/

#define D_MIN_VALUE 10				/* minimum value for D-reglering */
#define D_MAX_VALUE 99				/* minimum value for D-reglering */
#define D_START_VALUE 13			/* minimum value for D-reglering */

uint32_t ASTERIX_VALUE = 0;			/* Asterix_value range between 0 - 3*/



typedef enum {
	NO_BUTTON = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4,
	SELECT = 5
} button;

/* signal characteristics */
struct {
	uint32_t setpoint;
	uint32_t P;
	uint8_t I;
	uint32_t D;
} regulation;

/* flag: indicate there's new values to calculate & display */
uint32_t new_value;

static void init_hardware(void);
static void display_program_text(void);
//static void display_values(void);
static button read_buttons(void);

/*
 * Making some necessary initializations for hardware and interrupts.
 */
static void init_hardware(void)
{
	sysclk_init();
	board_init();
	delay_init(0); /* timer 0: for delay() */
	ioport_init();
	adc_setup();
	adc_start(ADC);
	config_timer_irq(1, 10); /* timer 1: for reading ADC, 10 times/second */
	lcd_init();
}

static void display_program_text(void)
{
	/* initial program text */
	lcd_put_cursor(0, 0);
	lcd_write_str("*");
	lcd_put_cursor(0, 1);
	lcd_write_str("B");
	lcd_put_cursor(0, 5);
	lcd_write_str("P");
	lcd_put_cursor(0, 10);
	lcd_write_str("I");
	lcd_put_cursor(0, 14);
	lcd_write_str("D");
	lcd_put_cursor(1, 0);
	lcd_write_str(500);
	lcd_put_cursor(1, 5);
	lcd_write_str("9");
	lcd_put_cursor(1, 8);
	lcd_write_str("0.02");
	lcd_put_cursor(1, 14);
	lcd_write_str("13");
}

/*
 * Reading A/D conversion value - determining which button was pressed.
 */
static button read_buttons(void)
{
	button btn_id = NO_BUTTON;
	/* read latest A/D value */
	uint32_t adc_value = adc_get_latest_value(ADC);
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
	adc_start(ADC);
	/* return button ID */
	return btn_id;
}
static void display_values(void)
{
	/**********************/
	/*     Borvarde      */
	/**********************/
	lcd_put_cursor(1, 0);
	lcd_write_int(regulation.setpoint);
	
	lcd_put_cursor(1, 5);
	lcd_write_int(regulation.P);

	if (regulation.I < 10) {
		lcd_put_cursor(1, 11);
		lcd_write_int(regulation.I);
	}
	else {
		lcd_put_cursor(1, 10);
		lcd_write_int(regulation.I);
	}
	
	lcd_put_cursor(1, 13);
	lcd_write_int(regulation.D);
}

void ISR_TIMER_1()
{
	/* clear status bit to acknowledge interrupt,
	by reading status on the timer channel */
	GET_TC1_STATUS();
	/* read buttons on LCD shield */
	button btn_id = read_buttons();
	/* is one of the buttons pressed? */
	if (btn_id != NO_BUTTON) {
		switch (btn_id) {
		case RIGHT:	/* Move the asterix to the right, to choose the regulation */
			if (ASTERIX_VALUE == 0) {
				lcd_write_str(" ");
				lcd_put_cursor(0, 4);
				lcd_write_str("*");
				ASTERIX_VALUE = 1;
			}
			else if (ASTERIX_VALUE == 1) {
				lcd_write_str(" ");
				lcd_put_cursor(0, 9);
				lcd_write_str("*");
				ASTERIX_VALUE = 2;
			}
			else if (ASTERIX_VALUE == 2) {
				lcd_write_str(" ");
				lcd_put_cursor(0, 13);
				lcd_write_str("*");
				ASTERIX_VALUE = 3;
			}
			break;
		case UP:	/* Increase the value for the specified regulation */
			if (ASTERIX_VALUE == 0 && regulation.setpoint < SETPOINT_MAX_VALUE) {
				regulation.setpoint += 5;
			}
			else if (ASTERIX_VALUE == 1 && regulation.P < P_MAX_VALUE) {
				regulation.P += 1;
			}
			else if (ASTERIX_VALUE == 2 && regulation.I < I_MAX_VALUE) {
				regulation.I += 1;
			}
			else if (ASTERIX_VALUE == 3 && regulation.D < D_MAX_VALUE) {
				regulation.D += 1;
			}
			break;
		case DOWN:	/* Decrease the value for the specified regulation */
			if (ASTERIX_VALUE == 0 && regulation.setpoint > SETPOINT_MIN_VALUE) {
				regulation.setpoint -= 5;
			}
			else if (ASTERIX_VALUE == 1 && regulation.P > P_MIN_VALUE) {
				regulation.P -= 1;
			}
			else if (ASTERIX_VALUE == 2 && regulation.I > I_MIN_VALUE) {
				regulation.I -= 1;
			}
			else if (ASTERIX_VALUE == 3 && regulation.D > D_MIN_VALUE) {
				regulation.D -= 1;
			}
			break;
		case LEFT:	/* Move the asterix to the left, to choose the regulation */
			if (ASTERIX_VALUE == 3) {
				lcd_write_str(" ");
				lcd_put_cursor(0, 8);
				lcd_write_str("*");
				ASTERIX_VALUE = 2;
			}
			else if (ASTERIX_VALUE == 2) {
				lcd_write_str(" ");
				lcd_put_cursor(0, 4);
				lcd_write_str("*");
				ASTERIX_VALUE = 1;
			}
			else if (ASTERIX_VALUE == 1) {
				lcd_write_str(" ");
				lcd_put_cursor(0, 0);
				lcd_write_str("*");
				ASTERIX_VALUE = 0;
			}
			break;
		default:
			break;
		}
		new_value = 1; /* set flag high: change regulation */
	}
}

static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, & uart_serial_options);
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
	display_program_text(); //Just testing to write sth...

	/* Initiera SAM systemet */
	sysclk_init();
	board_init();
	configure_console();

	regulation.setpoint = SETPOINT_START_VALUE;
	regulation.P = P_START_VALUE;
	regulation.I = I_START_VALUE;
	regulation.D = D_START_VALUE;
	ASTERIX_VALUE = 0;
	/*Testar att skapa den innan schemat går igång för att se att allting funkar som det ska*/
	while (1)
	{
		/* lower flag: new values has been calculated */
		new_value = 0;
		display_values();
	}
	
	/* Print demo information */
	printf("-- Freertos Exempel - Semaforer --\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Kompilerad: %s %s --\n\r", __DATE__, __TIME__);
	
	/* a semaphore cannot be used wihtout calling vSemaphoreCreateBinary() */
	vSemaphoreCreateBinary(semafor_signal);
	/* Create the task giving the semaphore */
	if (xTaskCreate(task1, (const signed char * const) "Task1", 1024, NULL, 1, NULL) != pdPASS) {
		printf("Misslyckades med att skapa Boss tasken\r\n");
	}
	/* Create a task taking the semaphore and doing it’s stuff */
	if (xTaskCreate(task2, (const signed char * const) "Task2", 1024, NULL, 2, NULL) != pdPASS) {
		printf("Misslyckades med att skapa Employee tasken\r\n");
	}
	/* Start the scheduler */
	vTaskStartScheduler();
}