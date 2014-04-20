/**
 * my_adc.c
 *
 * This file contains some functions for A/D conversion.
 *
 * Created by Mathias Beckius & Soded Alatia, 2013-11-27
 */

#include "my_adc.h"

/*
 * Initializing A/D conversion.
 */
void adc_setup(void)
{
	/* Enable the specified peripheral clock (ADC clock).
	   If function returns 0, then we can proceed... */
		pmc_enable_periph_clk(ID_ADC);
		/* init A/D conversion */
		adc_init(ADC, sysclk_get_main_hz(), ADC_CLOCK, 8);
		/* configure timing for A/D conversion */
		adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
		/* set 12 bit resolution */
		adc_set_resolution(ADC, ADC_MR_LOWRES_BITS_12);
		/* enable ADC channel - specified in 'adc.h' */
		/*adc_enable_channel(ADC, ADC_CHANNEL_LCDButtons);
		adc_enable_channel(ADC, ADC_CHANNEL_Tank1);
 		adc_enable_channel(ADC, ADC_CHANNEL_Tank2);*/
		ADC->ADC_CHER = 3200;
		
		/* configure conversion to be triggered by software */
		adc_configure_trigger(ADC, ADC_TRIG_SW, 0);
		/* indicate everything's OK! */
}
