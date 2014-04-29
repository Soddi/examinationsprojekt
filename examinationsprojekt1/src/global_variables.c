/*
 * global_variables.c
 *
 *	Created: 2014-04-20 12:35:36
 *  Author: Soded Alatia, Erik Gustafsson
 *	Global variables initiated here.
 */ 

#include "global_variables.h"
#include <asf.h>

uint16_t user_setpoint = 0;
double user_P = 0;
double user_I = 0;
double user_D = 0;

double tank1_value = 0;
double tank2_value = 0;
double error_value = 0;
uint32_t output_value = 0;