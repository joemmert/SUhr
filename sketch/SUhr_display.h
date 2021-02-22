

#include "SUhr_HAL.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>
#include "Arduino.h"

#include <avr/pgmspace.h>


#define VCOM_bit 0x02
#define sharp_write 0x01


extern volatile bool disp_update;

extern int SUhr_disp_curx;
extern int SUhr_disp_cury;


void SUhr_disp_clrbfr();
void SUhr_disp_clr();
void SUhr_disp_init();
void SUhr_disp_refresh();

void SUhr_disp_line(int x1,int y1,int x2,int y2, uint8_t col);
void SUhr_disp_pixel(int x, int y, uint8_t col);
void SUhr_disp_print_char(char text);
void SUhr_disp_setcursor(int x, int y);
void SUhr_disp_settextcolor(int te, int bg);
void SUhr_disp_settextsize(int siz);
void SUhr_disp_setleftalign(int x);

void SUhr_disp_print_string(char * text);
void SUhr_disp_print_int(uint32_t i);
void SUhr_disp_print_double(double i, int afterpoint);

void SUhr_disp_println_string(char * text);
void SUhr_disp_println_int(int i);
void SUhr_disp_println_double(double i, int afterpoint) ;
void SUhr_disp_fillrect(int x, int y, int width, int height, int col);

void SUhr_disp_buttonlabels(char SW1s, char SW2s, char SW3s, char SW4s);

void SUhr_disp_print_char_noninterp(char text) ;
void SUhr_disp_set_interpolation(bool intp);
