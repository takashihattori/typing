/****************************************************************/
/*								*/
/*						ESC_SEQ.H	*/
/*								*/
/*	Typing III : Blind touch typing training system		*/
/*		Originated by Mr. K. Noguchi (Typing II)	*/
/*								*/
/*		Edited by Dr. K. Juli Kawai			*/
/*			started at 20th Feb. 1987		*/
/*			completed at __th Feb. 1987		*/
/*              Modified by Yoshihiro Ohmi                      */
/*								*/
/****************************************************************/

#ifndef ESC
#define ESC 0x1b
#endif

//#define         kanji_in()      printf("\033$@")
#define         kanji_in()      printf("\033$B")
//#define         kanji_out()     printf("\033(J")
#define         kanji_out()     printf("\033(B")

#define		cur_add(y,x) 	printf("\033[%d;%dH",y,x)
#define		cur_up(y)	printf("\033[%dA",y)
#define		cur_down(y)	printf("\033[%dB",y)
#define		cur_fwd(x)	printf("\033[%dC",x)
#define		cur_bak(x)	printf("\033[%dD",x)
#define		clr_eos()	printf("\033[0J")
#define		clr_scr()	printf("\033[2J")
#define		clr_eol()	printf("\033[0K")
#define		clr_bol()	printf("\033[1K")
#define		clr_ln()	printf("\033[2K")
#define		del_ln(y)	printf("\033[%dM",y)
#define		ins_ln(y)	printf("\033[%dL",y)
#define		t_index() 	printf("\033D")
#define		next_ln()	printf("\033E")
#define		t_rindex()	printf("\033M")
#define		char_atr(n)	printf("\033[%dm",n)
/*
#define		encur_dis()	printf("\033[>5l")
#define		disa_dis()	printf("\033[>5h")
*/
#define		encur_dis()	printf("\033[?25h")
#define		disa_dis()	printf("\033[?25l")
#define		ena_bl()		
#define		dis_bl()		
/*
#define		ena_bl()	printf("\033[>1h")
#define		dis_bl()	printf("\033[>1l")
*/
#define		bell()		printf("\007")

