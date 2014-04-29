/*
 * global_variables.h
 *
 *	Created: 2014-03-25 11:50:03
 *  Author: Soded Alatia & Erik Gustafsson
 *	Start values are defined here so it's a bit more functional.
 */ 


#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_
#include <asf.h>

#define SETPOINT_MIN_VALUE 1000		/* minimum set point allowed */
#define SETPOINT_MAX_VALUE 2048		/* maximum set point allowed */
#define SETPOINT_START_VALUE 1000	/* start value */

#define P_MIN_VALUE 10				/* minimum value for P-reglering */
#define P_MAX_VALUE 50				/* maximum value for P-reglering */
#define P_START_VALUE 25			/* start value for P-reglering */

#define I_MIN_VALUE 10				/* minimum value for I-reglering */
#define I_MAX_VALUE 300				/* maximum value for I-reglering */
#define I_START_VALUE 25			/* start value for I-reglering */

#define D_MIN_VALUE 1				/* minimum value for D-reglering */
#define D_MAX_VALUE 90				/* maximum value for D-reglering */
#define D_START_VALUE 40			/* start value for D-reglering */

extern uint16_t user_setpoint;
extern double user_P;
extern double user_I;
extern double user_D;

extern double tank1_value;
extern double tank2_value;
extern double error_value;
extern uint32_t output_value;

#endif /* GLOBAL_VARIABLES_H_ */