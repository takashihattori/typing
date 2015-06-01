#include "general.h"

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "esc_seq.h"

void term_reset();
void term_end();
void term_attr_set();

void term_init();

void term_disp(char*);
void term_rev_disp( char*);
void term_center_disp(char*, int);
void term_left(int);
void term_right(int);
void term_up(int);
void term_down(int);
void term_gotoxy(int, int);
void term_clear();
void term_color(int);
void term_reverse(Bool);
void term_scroll_up(int, int, int, int);
void term_error( char* message);

void term_keymode(Bool);
void term_keyclear();
unsigned char term_getkey();
unsigned char term_getkey_sub();
unsigned char term_esc_key();

char *tozenkaku(char*);

#define TERM_W 80
#define TERM_H 25

#ifndef ESC
#define ESC 0x1b
#endif

#define CTRLC 0x03
#define CR 0x0d
#define BS 0x08
#define DEL 0x7f

#define CUR_RIGHT 0x1c
#define CUR_LEFT 0x1d
#define CUR_UP 0x1e
#define CUR_DOWN 0x1f
#define OTHER_ESC_SEQUENCE 0xff

#define COL_BLACK 0
#define COL_BLUE 1
#define COL_RED 2
#define COL_MAGENTA 3
#define COL_GREEN 4
#define COL_CYAN 5
#define COL_YELLOW 6
#define COL_WHITE 7

#endif
