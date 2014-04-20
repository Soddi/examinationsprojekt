/**
 * timer_delay.c
 *
 * This file contains some functions to generate delays.
 *
 * Created by Mathias Beckius & Soded Alatia, 2013-11-27
 */
 
#include "timer_delay.h"

static uint32_t TC_CLK[] = {ID_TC0, ID_TC3, ID_TC6};
static Tc *TC_INS[] = {TC0, TC1, TC2};
static Tc *tc_ins_delay;
 
int config_timer_irq(uint8_t timer_nr, uint32_t desired_freq)
{
	uint32_t tc_clk = TC_CLK[timer_nr];
	Tc *tc_ins = TC_INS[timer_nr];
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();
	/* Configure power management of timer clocks */
	pmc_enable_periph_clk(tc_clk);
	/* Configure TC for a 44,1kHz frequency and trigger on RC compare. */
	tc_find_mck_divisor(desired_freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(tc_ins, 0, (ul_tcclks | TC_CMR_CPCTRG));
	tc_write_rc(tc_ins, 0, (ul_sysclk / ul_div) / desired_freq);
	/* Configure and enable interrupt on RC compare */
	NVIC_EnableIRQ((IRQn_Type) tc_clk);
	tc_enable_interrupt(tc_ins, 0, TC_IER_CPCS);
	tc_start(tc_ins, 0);
	return 1;
}
 
/*
 * Initializing timer for delay function.
 */
void delay_init(uint8_t timer_nr)
{
	pmc_enable_periph_clk(TC_CLK[timer_nr]);
	tc_ins_delay = TC_INS[timer_nr];
	tc_init(tc_ins_delay, 0, 0);
	tc_set_block_mode(tc_ins_delay, 0);
	tc_stop(tc_ins_delay, 0);	//to be sure..
}

/*
 * Generating a delay of 'us' microseconds.
 */
void delay(uint32_t us)
{
	tc_start(tc_ins_delay, 0);
	while (tc_read_cv(tc_ins_delay, 0) < us*42);
	tc_stop(tc_ins_delay, 0);
}