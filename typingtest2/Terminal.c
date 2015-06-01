#include <stdio.h>
#ifdef __GNUG__
#include <std.h>
#endif
#ifdef CURSES
#include <curses.h>
#endif
#ifdef MSDOS
#include <conio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "Terminal.h"

static Bool kanji;
static Bool key_mode;  /* direct_mode:TRUE cursor_mode:FALSE */
static int term_attr = COL_WHITE;

void term_init()
{
#ifdef UNIX
#ifdef CURSES
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
#else
    system("stty -echo");
    system("stty raw");
#endif
#endif
    term_reset();
}

void term_end()
{
#ifdef UNIX
#ifdef CURSES
    endwin();
#else
    system("stty -raw");
    system("stty echo");
#endif
#endif
    term_disp("\n\n");
}

void term_reset()
{
    kanji_out();
    key_mode = TRUE;
}

#ifdef UNIX
void term_disp(char* str)
{

#ifdef CYGWIN
  printf("%s", str);
#else
#ifdef CURSES
    addstr(str);
#else
    /* Convert from EUC code to JIS code */
    //unsigned char *p;
    char *p;
    for(p = str; *p; p++){
	if (*p > 0x7f){
	    if (!kanji){
		kanji_in();
		kanji = TRUE;
	    }
	    putchar(*p % 0x80);
	} else {
	    if (kanji){
		kanji_out();
		kanji = FALSE;
	    }
	    putchar(*p);
	}
    }
#endif
#endif
}
#endif
#ifdef MSDOS
void term_disp(unsigned char* str)
{
    printf("%s", str);
}
#endif

void term_center_disp(char *str, int y)
{
    term_gotoxy((TERM_W-strlen(str))/2, y);
    term_disp(str);
}

void term_color(int code)
{
    term_attr = (code%8) | (term_attr & (256-8));
    term_attr_set();
}
void term_reverse(Bool onoff)
{
/* Another attributes are ignored. */
#ifdef CURSES
    if (onoff){
	attron(A_REVERSE);
    } else {
	attroff(A_REVERSE);
    }
#else
    term_attr = (term_attr & 7) | (onoff << 3);
    term_attr_set();
#endif
}

void term_attr_set()
{ 
#ifndef CURSES
    static char colortable[8] = {0,4,1,5,2,6,3,7};
    int tmp;
    if (term_attr & 8){
	tmp = 7;
    } else {
	tmp = 0;
    }
#ifdef MSDOS
    printf("\033[%d;%dm", tmp, colortable[term_attr%8]+30);
#endif
#ifdef UNIX
    printf("\033[%dm", tmp);
#endif
#endif
}

void term_scroll_up(int x, int y, int width, int height)
/* x and width are ignored. */
/* scroll_up between line y to line y+height-1 */
{
#ifdef CURSES
    term_gotoxy(0, y);
    deleteln();
    term_gotoxy(0, y+height-1);
    insertln();
#else
    term_gotoxy(0, y);
#ifdef FMR
    printf("\033R");
#else
    del_ln(1);
#endif FMR
    term_gotoxy(0, y+height-1);
#ifdef FMR
    printf("\033E");
#else
    ins_ln(1);
#endif
#endif
}

/**
 * エラー表示し 終了する
 */
void term_error( char* message){

    term_gotoxy(0, 24);
    term_end();

    term_disp(message);
    kanji_out();

    exit(1);
}

void term_keymode(Bool flag)
{
    key_mode = flag;
}

void term_keyclear()
{
#ifdef MSDOS
    while(kbhit()) getch();
#endif
}

unsigned char term_getkey()
{
    unsigned char c = term_getkey_sub();

    	if (key_mode){	  
    	    return c;
    	} else {
		switch(c){
			case '8':    /* 10 */
			#ifdef UNIX
			case 'K':    /* vi */
			case 'k':    /* vi */
			case 0x10:   /* emacs(^p) */
			#endif
			#ifdef PC98
			case 0x0b:
			#endif
				return CUR_UP;
			case '2':    /* 10 */
			#ifdef UNIX
			case 'J':    /* vi */
			case 'j':    /* vi */
			case 0x0e:    /* emacs(^n) */
			#endif
			#ifdef PC98
			case 0x0a:
			#endif
				return CUR_DOWN;
			case '6':    /* 10 */
			#ifdef UNIX
			case 'L':    /* vi */
			case 'l':    /* vi */
			case 0x06:   /* emacs(^f) */
			#endif
			#ifdef PC98
			case 0x0c:
			#endif				
				return CUR_RIGHT;
			case '4':    /* 10 */
			#ifdef UNIX
			case 'H':    /* vi */
			case 'h':    /* vi */
			case 0x02:   /* emacs(^b) */
			#endif
			#ifdef PC98
			case 0x08:
			#endif
				return CUR_LEFT;
			case ESC:
			#if defined(PC98) || defined(J3100)
				return ESC;
			#else
				return term_esc_key();
			#endif
			#ifdef J3100
			case 0:			  
				switch(term_getkey_sub()){
					case 0x48:
						return CUR_UP;
					case 0x50:
						return CUR_DOWN;
					case 0x4d:
						return CUR_RIGHT;
					case 0x4b:
						return CUR_LEFT;
					default:
						return ESC;
				}
			#endif
			default:
				return c;
		}
    }
}

Bool esc_mode = FALSE;

unsigned char term_getkey_sub()
{
    unsigned char c;

	#ifdef UNIX
	#ifdef CURSES
    c = getch();
	#else
    c = getchar();
	#endif
	#endif
	#ifdef MSDOS
    c = getch();
	#endif
    
	if (c == CTRLC){
		term_end();
		exit(1);
	}
	
	if(c == ESC && esc_mode == FALSE){
		return term_esc_key();
	}

/*
	//上下左右キーの場合，このフィルターをする事で，ESCキーと解釈するのを防ぐ
	switch(c){
		case 0x41:	
			return CUR_UP;
		case 0x42:
			return CUR_DOWN;
		case 0x43:
			return CUR_RIGHT;
		case 0x44:				
			return CUR_LEFT;
		default:
			return c;
	}
*/
	return c;
}

/**
 * ESCシーケンスが来た時のキーコードを取得する
 */
unsigned char term_esc_key(){
	esc_mode = TRUE;

	switch(term_getkey_sub()){	  
		/*
       	case 0x5b:
           switch(term_getkey_sub()){
		       case 0x31:
		       case 0x32:
		       case 0x33:
		       case 0x35:
		       case 0x36:
					printf("hoge");
			       esc_mode=FALSE;
			       return OTHER_ESC_SEQUENCE;
			   default:
			   		return ESC;
           }
           */
		case 0x4f:
		case 0x5b:
			switch(term_getkey_sub()){		
				case 0x31:
				case 0x32:
				case 0x33://[Page Up]
				case 0x34:	//[Page Down]
				case 0x35://[End]
				case 0x36://[Home]
					esc_mode = FALSE;
					return OTHER_ESC_SEQUENCE;					
				case 0x41:
					esc_mode = FALSE;
					return CUR_UP;
				case 0x42:
					esc_mode = FALSE;					
					return CUR_DOWN;
				case 0x43:
					esc_mode = FALSE;					
					return CUR_RIGHT;
				case 0x44:
					esc_mode = FALSE;					
					return CUR_LEFT;
				default:
					esc_mode = FALSE;					
					return ESC;
			}
		default:
			esc_mode = FALSE;					
			return ESC;
	}
}

void term_rev_disp( char* str)
{
    term_reverse(TRUE);
    term_disp(str);
    term_reverse(FALSE);
}

void term_left(int times)
{
    cur_bak(times);
}
void term_right(int times)
{
    cur_fwd(times);
}

void term_up(int times)
{
    cur_up(times);
}
void term_down(int times)
{
    cur_down(times);
}

void term_gotoxy(int x, int y)
{
#ifdef CURSES
    move(y,x);
#else
    cur_add(y, x);
#endif
}

void term_clear()
{
#ifdef CURSES
    clear();
#else
    clr_scr();
#endif
    term_color(COL_WHITE);
}

char* tozenkaku(char* src)
{
    static char tmp[MAXSTRLEN];
    char *p;

    for (p = tmp; *src && p-tmp < MAXSTRLEN ; src++, p++){
	if (*src <= 0x7f && *src != ' '){
#ifdef UNIX
	    *p++ = 0xa3;
	    *p = *src | 0x80;
#else
	    *p = *src;
#endif
	} else {
	    *p = *src;
	}
    }
    *p = '\0';
    return tmp;
}

#ifndef UNIX
#include <time.h>
sleep(int sec)
{
    time_t prev, now;

    time(&prev);
    while( time(&now) - prev < sec);
}
#endif


