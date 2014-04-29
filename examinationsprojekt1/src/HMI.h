/*
 * HMI.h
 *
 * Created: 2014-03-17 17:18:16
 *  Author: Erik Gustafsson & Soded Alatia
 */ 


#ifndef HMI_H_
#define HMI_H_
#include <asf.h>

 typedef enum {
 	NO_BUTTON = 0,
 	UP = 1,
 	DOWN = 2,
 	LEFT = 3,
 	RIGHT = 4,
 	SELECT = 5
 } button;

void disp_buttons(void*);
void handle_input(button btn_id);
void display_program_text(void);
void task2_LCD(void);

#endif /* HMI_H_ */