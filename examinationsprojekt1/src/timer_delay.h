/**
 * timer_delay.h
 *
 * Created by Mathias Beckius & Soded Alatia, 2013-11-27
 */

#ifndef TIMER_DELAY_H_
#define TIMER_DELAY_H_

#include "asf.h"

#define ISR_TIMER_0() TC0_Handler(void)
#define ISR_TIMER_1() TC3_Handler(void)
#define ISR_TIMER_2() TC6_Handler(void)

#define GET_TC0_STATUS() tc_get_status(TC0, 0)
#define GET_TC1_STATUS() tc_get_status(TC1, 0)
#define GET_TC2_STATUS() tc_get_status(TC2, 0)

int config_timer_irq(uint8_t, uint32_t);
void delay_init(uint8_t);
void delay(uint32_t);

#endif