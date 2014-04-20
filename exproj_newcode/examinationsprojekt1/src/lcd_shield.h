/*
 * lcd_shield.h
 *
 * Created by Ulrik on 2013-11-06.
 * Based on code from Tommy Andersson.
 *
 * Modified by Mathias Beckius & Soded Alatia 2013-12-03
 */

#ifndef LCDShield_h
#define LCDShield_h

#include <stdio.h>
#include <string.h>
#include "asf.h"
#include "timer_delay.h"

#define LCD_RS PIO_PC22_IDX
#define LCD_Enable PIO_PC21_IDX

#define LCD_D4 PIO_PC26_IDX
#define LCD_D5 PIO_PC25_IDX
#define LCD_D6 PIO_PC24_IDX
#define LCD_D7 PIO_PC23_IDX

#define LCD_BACKLIGHT PIO_PC29_IDX

#define LCD_mask_D4_D7 0b00000111100000000000000000000000

#define LCD_NO_CURSOR 0
#define LCD_CURSOR_NO_BLINK 2
#define LCD_CURSOR_BLINK 3

#define LCD_ROWS	2
#define LCD_COLS	40	/* 40 positions per row, only 16 are visible */

/* get ASCII value from character */
#define ASCII(chr) ((uint8_t)chr)

/* get character from ASCII value */
#define CHR(ascii) ((char)ascii)

/* get value of bit at position n */
#define BIT(number, n) (((number) >> (n)) & 1)

/* flip 4 bits, ex: 1010 -> 0101 */
#define FLIP_NIBBLE(nibble) ((BIT((nibble),0) << 3) | (BIT((nibble),1) << 2) | \
							 (BIT((nibble),2) << 1) | (BIT((nibble),3) << 0))

/* flip both nibbles, ex: 1010 0001 -> 0101 1000 */
#define FLIP_NIBBLES(byte) ((FLIP_NIBBLE(byte >> 4) << 4) | \
							 FLIP_NIBBLE(byte & 0x0F))

int lcd_init(void);
int lcd_clear(void);
int lcd_set_cursor(uint8_t);
int lcd_put_cursor(uint8_t, uint8_t);
uint8_t lcd_write(uint8_t, bool);
int lcd_write_int(int32_t);
int lcd_write_str(const char*);

#endif