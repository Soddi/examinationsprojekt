/*
 * global_variables.h
 *
 * Created: 2014-03-25 11:50:03
 *  Author: ac0542
 */ 


#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_
#include <asf.h>

#define SETPOINT_MIN_VALUE 1024			/* minimum börvärde allowed */
#define SETPOINT_MAX_VALUE 4096			/* maximum börvärde allowed */
#define SETPOINT_START_VALUE 1024		/* start value */

#define P_MIN_VALUE 1				/* minimum value for P-reglering */
#define P_MAX_VALUE 9				/* maximum value for P-reglering */
#define P_START_VALUE 1				/* start value for P-reglering */

#define I_MIN_VALUE 100				/* minimum value for I-reglering */
#define I_MAX_VALUE 999				/* maximum value for I-reglering */
#define I_START_VALUE 999			/* start value for I-reglering */

#define D_MIN_VALUE 1				/* minimum value for D-reglering */
#define D_MAX_VALUE 10				/* maximum value for D-reglering */
#define D_START_VALUE 10				/* start value for D-reglering */

extern uint16_t user_setpoint;
extern uint16_t user_P;
extern uint16_t user_I;
extern uint16_t user_D;

extern uint16_t tank1_value;
extern uint16_t tank2_value;
extern int32_t error_value;
extern uint16_t output_signal;

/*int16_t ticks_in_milliseconds = 100;*/

#endif /* GLOBAL_VARIABLES_H_ */