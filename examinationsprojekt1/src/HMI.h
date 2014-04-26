/*
 * task2.h
 *
 * Created: 2014-03-17 17:18:16
 *  Author: Erik Gustafsson & Soded Alatia
 */ 


#ifndef TASK2_H_
#define TASK2_H_
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
//void display_values(void);
//button read_buttons(void);
void display_program_text(void);
void lcd_delay(uint32_t x);
void task2_LCD(void);

#endif /* TASK2_H_ */