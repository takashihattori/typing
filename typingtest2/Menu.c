#include <stdio.h>
#ifdef __GNUG__
#include <std.h>
#endif

#include "general.h"
#include "Global.h"
#include "Menu.h"
#include "Test.h"

#define MENU_HEAD_X 15
#define MENU_HEAD_Y 6
#define MENU_CHAP_X 6
#define MENU_CHAP_Y 11
#define MENU_SEC_X 52
#define MENU_SEC_WIDTH 5

Intprt menu;

/**
 * !MENUコマンドが実行されたら処理される
 */
void Menu_run(Token* token_temp)
{
	Examination * examination;
	Test * test = Test_get_test();
	examination = &(test->examination);
	if(EXAM_MODE(examination->mode)){
	menu.token = token_temp;
	MenuCtrl_init();

	Intprt_goto_next(&menu);
	while(!Token_compare(token_temp, "!MENU_END")){
		if(Token_compare(token_temp, "!MESSAGE")){
			Menu_message();
		} else if (Token_compare(token_temp, "!TITLE")){
			Menu_title();
		} else if (Token_compare(token_temp, "!LOAD")){
			Menu_load();
		} else if (Token_compare(token_temp, "!ITEM")){
			Menu_item();
		} else if (Token_compare(token_temp, "!EXIT")){
			Menu_menu_exit();
		} else {
			Intprt_run(&menu);
		}
	}
	
	MenuCtrl_go_origin();
	
	while(Menu_mainloop()) ;
	}else{
		menu.token = token_temp;
	MenuCtrl_init();

	Intprt_goto_next(&menu);
	while(!Token_compare(token_temp, "!MENU_END")){
		if(Token_compare(token_temp, "!MESSAGE")){
			Menu_message();
		} else if (Token_compare(token_temp, "!TITLE")){
			Menu_title();
		} else if (Token_compare(token_temp, "!LOAD")){
			Menu_load();
		} else if (Token_compare(token_temp, "!ITEM")){
			Menu_item();
		} else if (Token_compare(token_temp, "!EXIT")){
			Menu_menu_exit();
		} else {
			Intprt_run(&menu);
		}
	}
	
	MenuCtrl_go_origin();
	
	while(Menu_mainloop()) ;
		
	}
}

/**
 * Menuを表示後にキー入力を待つ
 */
Bool Menu_mainloop()
{
	term_keymode(FALSE);
	term_keyclear();

	switch(term_getkey()){
		case CUR_UP:
			MenuCtrl_up();
			break;
		case CUR_DOWN:
			MenuCtrl_down();
			break;
		case CUR_LEFT:
			MenuCtrl_left();
			break;
		case CUR_RIGHT:
			MenuCtrl_right();
			break;
		case CR:
		case ' ':
			if (!MenuCtrl_run()){
				return FALSE;
			}
			MenuCtrl_redraw();
			break;
		case ESC:
			MenuCtrl_go_origin();
			break;
	}
		
	return TRUE;
}

void Menu_message()
{
    Token *token = menu.token;
    int len = 0;

    Token_get(token);

    while(Token_type(token) == STRING_token){
	if (MenuCtrl_set_message(Token_now(token))){
	    term_error("メニューのメッセージの文字数が多すぎます。");
	}
	Token_get(token);
    }
}

/**
 * !TITLEが読まれた時の処理
 */    
void Menu_title()
{
    Token *token = menu.token;
    Token_get(token);
    if (Token_type(token) == STRING_token){
	if (MenuCtrl_set_title(Token_now(token))){
	    term_error("レッスンの数が多すぎます。");
	}
    } else {
	term_error("!TITLE コマンドの使用法が誤っています。");
    }
	
    Intprt_goto_next(&menu);
}

/**
 * !LOADが読まれた時の処理
 */
void Menu_load()
{
    Token *token = menu.token;
    Token_get(token);
    if (Token_type(token) == STRING_token){
	MenuCtrl_set_fname(Token_now(token));
    } else {
	term_error("!LOAD コマンドの使用法が誤っています。");
    }

    Intprt_goto_next(&menu);
}

/**
 * !ITEMが読まれた時の処理
 */
void Menu_item()
{
    Token *token = menu.token;
    Token_get(token);

    while(Token_type(token) == STRING_token){
	if (MenuCtrl_add_sec(Token_now(token))){
	    term_error("セクションの数が多すぎます。");
	}
	Token_get(token);
    }

    if (Token_type(token) != COMMAND_token){
	Intprt_goto_next(&menu);
    }
}

/**
 * !EXITが読まれた時の処理
 */
void Menu_menu_exit()
{
    Menu_item();
    MenuCtrl_set_exit();
}


/* methods definition of class MenuControl */

MenuCtrl ctrl;

void MenuCtrl_init()
{
    strcpy(ctrl.message, "");
    ctrl.chap_num = -1;
}

Bool MenuCtrl_set_message(char* mes)
{
    if (strlen(mes)+strlen(ctrl.message) < MAXMESS){
	strcat(ctrl.message, mes);
	strcat(ctrl.message, "\n");
	return FALSE;
    } else {
	return TRUE;
    }
}

Bool MenuCtrl_set_title(char* str)
{
    if (ctrl.chap_num < MAXCHAP-1){
	ctrl.chap_num++;
	ctrl.chap[ctrl.chap_num].sec_num = -1;
	ctrl.chap[ctrl.chap_num].exit = FALSE;

	strcpy(ctrl.chap[ctrl.chap_num].title, str);

	ctrl.chap[ctrl.chap_num].exit = FALSE;
	return FALSE;
    } else {
	return TRUE;
    }
}

void MenuCtrl_set_fname(char* str)
{
    strcpy(ctrl.chap[ctrl.chap_num].fname, str);
}

Bool MenuCtrl_add_sec(char* str)
{
    int tmp;
    if (ctrl.chap[ctrl.chap_num].sec_num < MAXSEC-1){
	ctrl.chap[ctrl.chap_num].sec_num ++;
	tmp = ctrl.chap[ctrl.chap_num].sec_num;
	strcpy(ctrl.chap[ctrl.chap_num].sec[tmp], str);

	return FALSE;
    } else {
	return TRUE;
    }
}

void MenuCtrl_set_exit()
{
    ctrl.chap[ctrl.chap_num].exit = TRUE;
}

void MenuCtrl_go_origin()
{
    ctrl.x = 0;
    ctrl.y = ref_i("menu_start_line") - 1;

    MenuCtrl_redraw();
}

/**
 * メニュー画面で左キーが押された時の処理
 */
void MenuCtrl_left()
{
	/*
	if (ctrl.x > 0){
		MenuCtrl_moveto(ctrl.x - 1, ctrl.y);
	} else {
		MenuCtrl_moveto(-1, ctrl.y);
		MenuCtrl_up();
	}*/
}

/**
 * メニュー画面で右キーが押された時の処理
 */
void MenuCtrl_right()
{
	/*
	if (ctrl.x < ctrl.chap[ctrl.y].sec_num){
		MenuCtrl_moveto(ctrl.x + 1, ctrl.y);
	} else {
		MenuCtrl_moveto(0, ctrl.y);
		MenuCtrl_down();
	}*/
}

void MenuCtrl_up()
{
    int y0, x0;
    if (ctrl.y > 0){
	y0 = ctrl.y - 1;
    } else {
	y0 = ctrl.chap_num;
    }
    
    if (ctrl.x < 0 || ctrl.x > ctrl.chap[y0].sec_num){
	x0 = ctrl.chap[y0].sec_num;
    } else {
	x0 = ctrl.x;
    }
    MenuCtrl_moveto(x0, y0);
}

void MenuCtrl_down()
{
    int y0, x0;
    if (ctrl.y < ctrl.chap_num){
	y0 = ctrl.y + 1;
    } else {
	y0 = 0;
    }
    
    if (ctrl.x < 0 || ctrl.x > ctrl.chap[y0].sec_num){
	x0 = ctrl.chap[y0].sec_num;
    } else {
	x0 = ctrl.x;
    }
    MenuCtrl_moveto(x0, y0);
}

/*
 * メニューを選択してSpaceキーを押した時の処理
 */
Bool MenuCtrl_run()
{
    Intprt intprt;
    int ret;
    if (ctrl.chap[ctrl.y].exit){
	return FALSE;
    }

    NIntprt_init(&intprt, ctrl.chap[ctrl.y].fname);

    ret = NIntprt_run(&intprt, ctrl.y - 1, ctrl.x + 1);

    NIntprt_end(&intprt);

    if (ret > 0){
	ctrl.x = ret-1;
    } else {
	if (ctrl.y >= ctrl.chap_num){
	    ctrl.y = 0;
	} else {
	    ctrl.y++;
	}
	ctrl.x = 0;
    }
    return TRUE;
}

void MenuCtrl_moveto(int x0, int y0)
{
    if ((y0 / ref_i("menu_height")) !=
	(ctrl.y / ref_i("menu_height"))){
	ctrl.x = x0;
	ctrl.y = y0;
	MenuCtrl_redraw();
    } else {
	if (y0 != ctrl.y){
	    MenuCtrl_chap_disp(ctrl.y);
	    MenuCtrl_chap_rdisp(y0);
	}
	MenuCtrl_sec_disp(ctrl.x, ctrl.y);
	MenuCtrl_sec_rdisp(x0, y0);
	ctrl.x = x0;
	ctrl.y = y0;
    }
}

void MenuCtrl_redraw()
{
	int i,j;
	char *mes;
	char *tmp;
	int y0;
	int menu_height;

	term_clear();
	term_color(COL_WHITE);
	
	for (mes=ctrl.message,i = 0; *mes; i++){

		tmp = global_tmp;

		for (j=0; *(mes+j) != '\n'; j++){
			*(tmp+j) = *(mes+j);
		}

		*(tmp+j) = '\0';
		mes += j;
		mes++;
		term_gotoxy(MENU_HEAD_X, MENU_HEAD_Y+i);
		term_disp(tmp);
    }

	menu_height = ref_i("menu_height");
	y0 = ctrl.y / menu_height * menu_height;
	term_color(COL_CYAN);
	for (i = y0; i <= ctrl.chap_num && i < y0+menu_height; i++){
		if (i == ctrl.y){
	    	MenuCtrl_chap_rdisp(i);
		} else {
	    	MenuCtrl_chap_disp(i);
		}

		for (j = 0; j <= ctrl.chap[i].sec_num; j++){
			if (i == ctrl.y && j == ctrl.x){
				MenuCtrl_sec_rdisp(j, i);
			} else {
				MenuCtrl_sec_disp(j, i);
			}
		}
    }
}

/**
 * メニュー画面の未選択の章の名前を表示する
 */
void MenuCtrl_chap_disp(int y)
{
    term_gotoxy(MENU_CHAP_X, MENU_CHAP_Y + y % ref_i("menu_height"));
	term_disp(ctrl.chap[y].title);
}

/**
 * メニュー画面の選択中の章の名前を表示する
 */
void MenuCtrl_chap_rdisp(int y)
{
    term_gotoxy(MENU_CHAP_X, MENU_CHAP_Y + y % ref_i("menu_height"));
    term_rev_disp(ctrl.chap[y].title);
}

/**
 * メニュー画面の未選択の節の名前を表示する
 */
void MenuCtrl_sec_disp(int x, int y)
{
    if (x >= 0){
	term_gotoxy(MENU_SEC_X + MENU_SEC_WIDTH * x,
		    MENU_CHAP_Y + y % ref_i("menu_height"));
	term_disp(ctrl.chap[y].sec[x]);
    }
}

/**
 * メニュー画面の選択中の節の名前を表示する
 */
void MenuCtrl_sec_rdisp(int x, int y)
{
    if (x >= 0){
	term_gotoxy(MENU_SEC_X + MENU_SEC_WIDTH * x,
		    MENU_CHAP_Y + y % ref_i("menu_height"));
	term_rev_disp(ctrl.chap[y].sec[x]);
    }
}

