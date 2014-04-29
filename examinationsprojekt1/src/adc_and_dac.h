/**
 * my_adc.h
 *
 * Created by Mathias Beckius & Soded Alatia, 2013-11-27
 */

#include "asf.h"

#ifndef MY_ADC_H_
#define MY_ADC_H_

#define ADC_CLOCK 14000000
#define ADC_CHANNEL_LCDButtons ADC_CHANNEL_7
#define ADC_CHANNEL_Tank1 ADC_CHANNEL_10
#define ADC_CHANNEL_Tank2 ADC_CHANNEL_11


void adc_setup(void);
void dacc_setup(void);


#endif