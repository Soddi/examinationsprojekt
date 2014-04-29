/*
 * matlabcomm.c
 *
 * Created: 2014-04-18 10:49:36
 *  Author: Soded alatia, Erik gustavsson
 */ 

#include <asf.h>
#include "semafor.h"
#include "global_variables.h"
#include "matlabcomm.h"

/* Sends data through serial communication, should be obtained my matlab */
/* This function is executed with a periodicity of 500ms */
void communication(void *p) {
	portTickType matlab_comm_startTime = xTaskGetTickCount(); //Starts the ticks,
	for(;;) {
		if(xSemaphoreTake(semafor_signal, portMAX_DELAY)) {
			printf("%d\n", (uint32_t)tank1_value);
			printf("%d\n", (uint32_t)tank2_value);
			printf("%d\n", (int32_t)error_value);
			printf("%d\n", output_value);
		}
		vTaskDelayUntil(&matlab_comm_startTime, 500);
	}
}