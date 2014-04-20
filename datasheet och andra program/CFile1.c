/*
 * Lab1D
 *
 * This is a PWM generator program, which is programmed to work with
 * a LCD shield from DFRobot.
 * Buttons UP & DOWN increases/decreases frequency,
 * LEFT & RIGHT decreases/increases duty cycle.
 *
 * Programmed by Mathias Beckius & Soded Al Atia, 2013-12-09
 */

#include <math.h>
#include "asf.h"
#include "timer_delay.h"
#include "my_adc.h"
#include "lcd_shield.h"

/* signal output */
#define SPEAKER_PIN PIO_PB26_IDX
/* minimum period (microseconds), frequency: 20kHz */
#define PERIOD_MIN_VALUE 50
/* maximum period (microseconds), frequency: 20 Hz */
#define PERIOD_MAX_VALUE 50000
/* initial period for generating signal */
#define PERIOD_START_VALUE 10000
/* minimum duty cycle (%) */
#define MIN_DUTY_CYCLE 0
/* maximum duty cycle (%) */
#define MAX_DUTY_CYCLE 100

/* data type for button identification */
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
	uint32_t period;
	uint32_t frequency;
	uint8_t dutycycle;
	uint32_t time_high;
	uint32_t time_low;
} signal;

/* flag: indicate there's new values to calculate & display */
uint32_t new_value;

static void init_hardware(void);
static void display_program_text(void);
static void display_values(void);
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
	ioport_set_pin_dir(SPEAKER_PIN, IOPORT_DIR_OUTPUT);
}

/*
 * Display some "standard text" on the LCD:
 *		1) start screen
 *		2) labels/captions for frequency and duty cycle
 */
static void display_program_text(void)
{
	/* initial program text */
	lcd_write_str("PWM Generator");
	lcd_put_cursor(1, 0);
	lcd_write_str("version 1.0");
	delay(2000000);
	/* write text that won't be changed */
	lcd_clear();
	lcd_put_cursor(0, 0);
	lcd_write_str("freq:");
	lcd_put_cursor(0, 14);
	lcd_write_str("Hz");
	lcd_put_cursor(1, 0);
	lcd_write_str("duty cycle:");
	lcd_put_cursor(1, 15);
	lcd_write_str("%");	
}

/*
 * Display frequency and duty cycle on the LCD.
 */
static void display_values(void)
{
	/**********************/
	/*     FREQUENCY      */
	/**********************/
	lcd_put_cursor(0, 8);
	lcd_write_int(signal.frequency);
	/* "clean" old digits */
	if (signal.dutycycle < 100) {
		lcd_write(' ', HIGH);
	}
	if (signal.dutycycle < 1000) {
		lcd_write(' ', HIGH);
	}
	if (signal.dutycycle < 10000) {
		lcd_write(' ', HIGH);
	}	
	/**********************/
	/*     DUTY CYCLE     */
	/**********************/
	lcd_put_cursor(1, 12);
	/* "clean" old digits */
	if (signal.dutycycle < 100) {
		lcd_write(' ', HIGH);
	}
	if (signal.dutycycle < 10) {
		lcd_write(' ', HIGH);
	}
	/* display duty cycle */
	lcd_write_int(signal.dutycycle);
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

/*
 * Timer ISR for reading the A/D conversion value,
 * to determine which button was pressed.
 */
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
		case RIGHT:	/* Increase duty cycle... */
			if (signal.dutycycle < MAX_DUTY_CYCLE) {
				signal.dutycycle += 1;
			}
			break;
		case UP:	/* Decrease period (frequency goes up) */
			if (signal.period >= (PERIOD_MIN_VALUE + 100)) {
				signal.period -= 100;
			} else {
				signal.period = PERIOD_MIN_VALUE;
			}
			break;
		case DOWN:	/* Increase period (frequency goes down) */		o	
			if (signal.period <= (PERIOD_MAX_VALUE - 100)) {
				signal.period += 100;
			} else {
				signal.period = PERIOD_MAX_VALUE;
			}
			break;
		case LEFT:	/* Decrease duty cycle... */
			if (signal.dutycycle > MIN_DUTY_CYCLE) {
				signal.dutycycle -= 1;
			}
			break;
		default:
			break;
		}
		new_value = 1; /* set flag high: change frequency or duty cycle */
	}
}

/*
 * Main program starts here....
 */
int main(void)
{
	/* setting initial values */
	signal.period = PERIOD_START_VALUE;
	signal.dutycycle = 50;
	new_value = 1;
	/* disabling interrupts, don't want to start interrupts yet */
	Disable_global_interrupt();
	init_hardware();
	display_program_text();
	/* now it's time to enable interrupts */
	Enable_global_interrupt();
	
	/* display status (frequency & duty cycle) in a infinite loop... */
	while (1) {
		/* perform some calculations */
		signal.frequency = (uint32_t) (1000000.0 / signal.period);
		signal.time_high = (signal.period * signal.dutycycle) / 100;
		signal.time_low = signal.period - signal.time_high;
		
		/* lower flag: new values has been calculated */
		new_value = 0;
		
		/* display frequency & duty cycle */
		display_values();
		
		/* generate PWM signal as long as there's no new value */
		while (new_value == 0) {
			ioport_set_pin_level(SPEAKER_PIN, HIGH);
			delay(signal.time_high);
			ioport_set_pin_level(SPEAKER_PIN, LOW);
			delay(signal.time_low);
		}
	}
}