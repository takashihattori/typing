#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef __GNUG__
#include <std.h>
#endif

#ifdef MSDOS
#include <stdlib.h>
#include <malloc.h>
#endif

#include "Global.h"
#include "Intprt.h"
#include "Menu.h"
#include "Test.h"

int lesson_no = 0;
int section_no = 0;

/**
 * 初期化する．
 */
void NIntprt_init(Intprt *in, char *filename){

	//エラー処理をする
	if (!(in->token = (Token*)malloc(sizeof(Token)))){

 		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}
  
	Token_init(in->token, filename);
}

void NIntprt_end(Intprt *in){
  
	Token_end(in->token);
	free(in->token);
}

int NIntprt_run(Intprt *in, int lsn, int sec){
  
	in->status = TRUE;
  
	lesson_no = lsn;
	section_no = sec;
  
	if (sec > 0){
		if (!Intprt_goto_section(in, sec)){
			term_error("対応する !SECTION がありません。");
		}
	}
  
	Token_get(in->token);
	while(in->status){
		Intprt_run(in);
	}
	
	if (Token_type(in->token) == END_token){
		return -1;		
	} else {
		return section_no;
	}	
}

void Intprt_run(Intprt *in)
{
	Token *token = in->token;
  
	if (Token_type(token) != COMMAND_token){
		sprintf(global_tmp, "構文エラー:コマンドが来るべき所に、それ以外のものが書かれています。[%02d](%s)",
			Token_type(token), Token_now(token));
		term_error(global_tmp);
	}
  
	if (Token_compare(in->token, "!DISP")) Intprt_disp(in);
	else if (Token_compare(in->token, "!SET")) Intprt_set(in);
	else if (Token_compare(in->token, "!TEST")) Intprt_test(in);
	else if (Token_compare(in->token, "!DISP_HAND")) Intprt_disp_hand(in);
	else if (Token_compare(in->token, "!PAUSE")) Intprt_pause(in);
	else if (Token_compare(in->token, "!PAUSE_B")) Intprt_pause_canback(in);
	else if (Token_compare(in->token, "!LONG_PAUSE")) Intprt_long_pause(in);
	else if (Token_compare(in->token, "!LONG_PAUSE_B")) Intprt_long_pause_canback(in);
	else if (Token_compare(in->token, "!SECTION")) Intprt_section(in);
	else if (Token_compare(in->token, "!MENU")) Intprt_menu(in);
	else if (Token_compare(in->token, "!LOAD")) Intprt_load(in);
        else if(Token_compare(in->token,"!REGISTRATION")) Intprt_registration(in);
        else if(Token_compare(in->token,"!SELECTMODE")) Intprt_selectMode(in);
	else {
		sprintf(global_tmp, "\"%s\" コマンドは定義されていません。",
		Token_now(token));
		term_error(global_tmp);
	}  
	
	if (Token_type(token) == END_token){
		in->status = FALSE;
	}
}

Bool Intprt_goto_section(Intprt *in,int sec)
{
  Token *token = in->token;
  while(Token_type(token) != END_token){
    if  (Token_type(token) == COMMAND_token &&
	 Token_compare(token, "!SECTION")){
      Token_get(token);
      if (sec == atoi(Token_now(token))){
	section_no = sec;
	Token_push_point(token);
	return TRUE;
      }
    }
    Token_get(token);
  }
  return FALSE;
}

void Intprt_goto_next(Intprt *in)
{
  Token *token = in->token;
  Token_get(token);
  while(Token_type(token) != COMMAND_token &&
	Token_type(token) != END_token ){
    if (Token_type(token) == UNDEF_token){
      term_error("構文エラー");
    }
    Token_get(token);
  }
}

/* command methods */

void Intprt_disp(Intprt *in)
{
  int line = 0;
  int local_x = 0;
  int local_y = 0;
  Token *token = in->token;
  
  term_clear();
  
  Token_get(token);
  if (Token_type(token) == NUMBER_token){
    local_x = atoi(Token_now(token));
    Token_get(token);
    if (Token_type(token) == NUMBER_token){
      local_y = atoi(Token_now(token));
      Token_get(token);
    } else {
      term_error("!DISP コマンドの y座標の指定がありません。");
    }
  }
  while(Token_type(token) == STRING_token){
    term_gotoxy(DISP_X + local_x, DISP_Y + local_y + line);
    term_disp(Token_now(token));
    Token_get(token);
    line++;
  }
  
  Intprt_disp_guide(in);
  
  if (Token_type(token) != COMMAND_token){
    term_error("!DISP コマンドの表記が誤っています。");
  }
}

/**
 * 右上にガイド(disp_message)を表示する．デフォルトでは"Escでメニュー"を表示する．
 */
void Intprt_disp_guide(Intprt* in)
{
  term_gotoxy(60, 0);
  term_disp(ref("disp_message"));
}

void Intprt_pause(Intprt *in)
{
  int time;
  Token_get(in->token);
  if (Token_type(in->token) == NUMBER_token){
    time = atoi(Token_now(in->token));
    if (time >= 0 && time < 60){
      term_gotoxy(0, PAUSE_Y-1);
      term_disp("\n");
      sleep(time);
    } else {
      term_error("!PAUSE の時間が妥当ではありません。");
    }
    Token_get(in->token);
  } else {
    Intprt_pause_core(in, ref("pause_message"),FALSE);
  }
}

void Intprt_pause_canback(Intprt *in)
{
  Intprt_pause_core(in, ref("pause_message_b"),TRUE);
  
  Token_get(in->token);
}

/**
 * ポーズ状態の処理
 */
void Intprt_pause_core(Intprt *in, char *message, Bool canback)
{
	unsigned char code;

	//下のメッセージを表示する
	term_reverse(ON);
	term_center_disp(message, PAUSE_Y);
	term_reverse(OFF);
  
	term_keymode(TRUE);
	
	while((code = term_getkey()) != CR){

		switch(code){
			case ESC:
				in->status = FALSE;
				return;
			case BS:
			case DEL:
				if (canback){
					Intprt_backward(in);
					return;
				}
				break;
		}
	}
	Token_push_point(in->token);
}  

void Intprt_long_pause(Intprt *in)
{
  Intprt_long_pause_core(in, ref("long_pause_message"),FALSE);
  
  Token_get(in->token);
}

void Intprt_long_pause_canback(Intprt *in)
{
  Intprt_long_pause_core(in, ref("long_pause_message_b"),TRUE);
  
  Token_get(in->token);
}

/**
 * タイピングエラーチェック画面
 */
void Intprt_long_pause_core(Intprt *in, char *message, Bool canback)
{
  int i;
  char key_tmp[2];
  int count;
  char code;
  char *str;
  
  term_keymode(TRUE);
  
  while(1){
    //入力枠を表示する
    if (ref_top("long_pause_window")){
      for (i = 0;str = ref_now("long_pause_window");i++){
	term_gotoxy(LONG_PAUSE_X, LONG_PAUSE_Y+i);
       	term_disp(str);
      }
      term_reverse(ON);
      term_center_disp(message, LONG_PAUSE_Y+3);
      term_reverse(OFF);
    }
    
    term_gotoxy(LONG_PAUSE_X + 5, LONG_PAUSE_Y + 1);
    
    count = 0;
    
	while(count < LONG_PAUSE_WIDTH){
		code=term_getkey();
      
		switch(code) {
			case CR:
				Token_push_point(in->token);
				return;
			case ESC:
				in->status = FALSE;
				return;
			case BS:
			case DEL:
				if (canback){
					Intprt_backward(in);
					return;
				}
				break;
			default:
				if (code >= ' ' && code <= 0x7f){
					sprintf(key_tmp, "%c", code);
					term_disp(key_tmp);
					count ++;
				}
			}
		}
	}
}

void Intprt_disp_hand(Intprt *in)
{
  int x,y;
  
  Token_get(in->token);
  if (Token_type(in->token) == NUMBER_token){
    x = atoi(Token_now(in->token));
    Token_get(in->token);
    if (Token_type(in->token) == NUMBER_token){
      y = atoi(Token_now(in->token));
      Token_get(in->token);
    } else {
      y = x;
      x = HAND_X;
    }
  } else {
    x = HAND_X;
    y = HAND_Y;
  }
  
  Intprt_disp_hand_core(in, x, y);
}

void Intprt_disp_hand_core(Intprt *in, int x, int y)
{
  char *p;
  char out[MAXSTRLEN];
  char *out_p;
  char c;
  
  static char* key_char = "qwertyuiopasdfghjkl;zxcvbnm,./1234567890";
  
  int line;
  
  ref_top("hand_pattern");
  for (line = y; p = ref_now("hand_pattern"); line++){
    term_gotoxy(x, line);
    for(out_p = out;c = *p; p++){
      if (c > 0x7f){
	*(out_p++) = c;
	p++;
	*(out_p++) = *p;
      } else if (strchr(key_char, c)){
	if (strchr(ref("new"), c)){
	  *out_p = '\0';
	  term_disp(out);
	  out[0] = c;
	  out[1] = '\0';
	  term_rev_disp(out);
	  out_p = out;
	} else if (strchr(ref("learned"), c)){
	  *(out_p++) = c;
	} else {
	  *(out_p++) = ' ';
	}
      } else {
	*(out_p++) = c;
      }
    }
    *out_p = '\0';
    term_disp(out);
  }
  term_gotoxy(x + 38, line-2);
  //  term_disp("space");//タイピング試験のためにコメントアウト
}

void Intprt_set(Intprt *in)
{
  char var_name[MAXSTRLEN];
  Token *token = in->token;
  Token_get(token);
  if (Token_type(token) == SYMBOL_token){
    strcpy(var_name, Token_now(token));
    var_clear(var_name);
    Token_get(token);
    while(Token_type(token) == STRING_token ||
	  Token_type(token) == NUMBER_token ||
	  Token_type(token) == SYMBOL_token){
      var_set(var_name, Token_now(token));
      Token_get(token);
    }
  } else {
    term_error("!SET コマンドの変数名の型が誤っています。");
  }
}
void Intprt_section(Intprt *in)
{
  Token_get(in->token);
  if (Token_type(in->token) != NUMBER_token){
    term_error("!SECTION コマンドの使用法が誤っています。");
  }
  section_no = atoi(Token_now(in->token));
  
  Token_get(in->token);
}

void Intprt_menu(Intprt *in)
{
  Menu_run(in->token);
  
  Token_get(in->token);
}

void Intprt_test(Intprt *in)
{
  Test_test();
  if (!Test_run(in->token)){
    in->status = FALSE;
  }
  Test_end();
  
  Token_get(in->token);
}

void Intprt_load(Intprt *in)
{
  char filename[MAXSTRLEN];
  Intprt intprt;
  
  Token_get(in->token);
  if (Token_type(in->token) == STRING_token){
    
    NIntprt_init(&intprt, Token_now(in->token));
    NIntprt_run(&intprt, 0, 0);
    NIntprt_end(&intprt);
    
  } else {
    term_error("load コマンドの使用法が間違っています。");
  }
  
  Token_get(in->token);
}

void Intprt_backward(Intprt *in){

  Token_pop_point(in->token);

  if (!Token_pop_point(in->token)){
    in->status = FALSE;
  } else {
    Token_push_point(in->token);
  }
}


/**
 *  モード選択を行う
 */
void Intprt_selectMode(Intprt *in){
	Test * test;
	
	Test_test();
	test = Test_get_test();
	
	term_end();
	Examination_selectMode(&(test->examination));
	term_init();
	
	Token_get(in->token);
}

/**
 * 受験情報を登録する
 */
void Intprt_registration(Intprt *in){
	Examination * examination;
	Test * test = Test_get_test();
	examination = &(test->examination);
	if(EXAM_MODE(examination->mode)){
		term_end();//入力モードを変更する
		Examination_registration(&(test->examination),&(test->user));
		term_init();//入力モードを戻す
	}
  	Token_get(in->token);
}
