/*
 * d_a_c.c
 * This file contains some functions for D/A conversion.
 * Created by: Mathias Beckius & Soded Alatia, 2013-01-04
 */ 

#include "d_a_c.h"

/*
 * Initializing D/A conversion.
 * Returns 1 if no errors occurred during setup, otherwise 0 is returned.
 */
int dac_setup(void) {
	/* Enable the specified peripheral clock (ADC clock).
	   If function returns 0, then we can proceed... */
	if (pmc_enable_periph_clk(ID_DACC) == 0) {
		dacc_reset(DACC);
		dacc_set_transfer_mode(DACC, 0);
		dacc_set_timing(DACC, 1, 1, 0);
		/* select and enable ADC channel - specified in 'adc.h' */
		dacc_set_channel_selection(DACC, DAC_CHANNEL);
		dacc_enable_channel(DACC, DAC_CHANNEL);
		/* indicate everything's OK! */
		return 1;
	}
	/* indicate that something went wrong! */
	return 0;
}

