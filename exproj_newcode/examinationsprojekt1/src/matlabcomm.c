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

portTickType matlab_comm_startTime = 0;

void communication(void *p) {
	for(;;) {
		matlab_comm_startTime = xTaskGetTickCount();
		if(xSemaphoreTake(semafor_signal, portMAX_DELAY)) {
			printf("%d\n", tank1_value);
			printf("%d\n", tank2_value);
			printf("%d\n", error_value);
			printf("%d\n", output_signal);
		}
		vTaskDelayUntil(&matlab_comm_startTime, 500);
	}
}