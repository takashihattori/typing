	#include <stdio.h>
	#ifdef __GNUG__
	#include <std.h>
	#endif
	#ifdef MSDOS
	#include <stdlib.h>
	#include <malloc.h>
	#endif
	
	#include <time.h>
	#include <stdlib.h>
	
	#include "general.h"
	#include "Global.h"
	#include "Time.h"
	#include "Test.h"
	#include "Examination.h"

	#define SCROLL_LINE ref_i("scroll_line")
	#define TEST_X ref_i("test_x")
	#define TEST_Y ref_i("test_y")
	#define TEST_WIDTH ref_i("test_width")
	#define TEST_HEIGHT (SCROLL_LINE*2-1)
	#define TEST_HAND_X ref_i("test_hand_x")
	#define TEST_HAND_Y ref_i("test_hand_y")
	
	Test test;
	int typing_second;
	
	void Test_test()
	{
	  int i;
	  
	  for (i = 0; i < MAX_TEXT; i++){
	    test.text_buf[i] = NULL;
	  }
	  for (i = 0; i < MAX_TYPE_LINE; i++){
	    test.typed[i] = NULL;
	  }
	  for (i = 0; i < MAX_REPEAT; i++){
	    test.message_buf[i] = NULL;
	  }
	}
	
	void Test_end()
	{
	  int i;
	  
	  for (i = 0; i < MAX_TEXT; i++){
	    if (test.text_buf[i])   free(test.text_buf[i]);
	  }
	  for (i = 0; i < MAX_TYPE_LINE; i++){
	    if (test.typed[i])   free(test.typed[i]);
	  }
	  for (i = 0; i < MAX_REPEAT; i++){
	    if (test.message_buf[i])   free(test.message_buf[i]);
	  }
	}
	
	Bool Test_run(Token* token)
	{
	  int i;
	  
	  test.intprt.token = token;
	  test.intprt.status = TRUE;
	  
	  Intprt_goto_next(&test.intprt);
	  
	  while(!Token_compare(token, "!TEST_END")){
	    if(Token_compare(token, "!TEXT")){
	      Test_text();
	    } else if (Token_compare(token, "!MESSAGE")){
	      Test_message();
	    } else {
	      Intprt_run(&test.intprt);
	    }
	  }
	  
	  Test_mainloop();
	  if (!test.intprt.status){
	    return FALSE;
	  }
	  
	  return TRUE;
	}
	
	/**
	 * 試験テキスト内の!TEXT後を処理する
	 */
	void Test_text()
	{
	  int i;
	  Token *token = test.intprt.token;
	  
	  Token_get(token);
	  
	  for (i=0; Token_type(token) == STRING_token; i++){
	    if (i < MAX_TEXT){
	      test.text_buf[i] = Test_alloc(strlen(Token_now(token))+1);
	      strcpy(test.text_buf[i], Token_now(token));
	    } else {
	      term_error("練習テキストが多すぎます。");
	    }
	    
	    Token_get(token);
	  }
	  
	  test.text_no = i;
	}
	
	void Test_message()
	{
	  int i;
	  Token *token = test.intprt.token;
	  
	  Token_get(token);
	  
	  for (i=0; Token_type(token) == STRING_token; i++){
	    if (i >= MAX_REPEAT){
	      term_error("テストのメッセージが多すぎます。");
	    }
	    test.message_buf[i] = Test_alloc(strlen(Token_now(token))+1);
	    strcpy(test.message_buf[i], Token_now(token));
	    Token_get(token);
	  }
	}
	
	/**
	 * テストの間の処理
	 */
	void Test_mainloop()
	{
	  term_keymode(TRUE);
	  
	  Test_begin();
	  if (!test.intprt.status) return;
	  
	  Test_typing();
	  if (!test.intprt.status) return;
	  
	  Test_notify_end();
	  if (!test.intprt.status) return;
	  
	  check();
	  
	  Test_result();
	  
	  Test_error_check();
	  if (!test.intprt.status) return;  
	}
	
	/**
	 * 試験開始前の画面を表示する．
	 */
	void Test_begin()
	{
	  term_clear();
	  
	  term_gotoxy(ref_i("begin_x"),ref_i("begin_y"));
	  term_disp(ref("test_start_message"));
	  
	  Intprt_disp_guide(&test.intprt);
	  
	  Intprt_pause_core(&test.intprt, ref("test_pause_message"), FALSE);
	}
	
	/**
	 * タイピングを始めた時の処理
	 */
	void Test_typing()
	{
	  Bool cursor = ref_i("cursor");
	  Bool esc_begin = FALSE;//ESCシーケンスが始まったか
	  Bool fnc_begin = FALSE;//Fキーが始まったか
	  int test_time;
	  unsigned char code;
	  char dispcode[2] = {'\0','\0'};
	  Examination * examination;
	  
	  term_clear();
	  
	  test.x = 0;
	  test.y = 0;
	  
	  /*
	    if (ref_i("hand")){
	    Intprt_disp_hand_core(&test.intprt, TEST_HAND_X, TEST_HAND_Y);
	    }
	  */
	
	  //開始時間を記録する　typingtestex用拡張
	  examination = &(test.examination);
	  Examination_setTestStartDate(examination);
	  
	  Test_disp_info(TEST_HAND_X+12, 2);
	  
	  Intprt_disp_guide(&test.intprt);
	  
	  time_start();
	  
	  Test_disp_text();
	  
	  test_time = ref_i("time_limit");
	  test.typed[test.y] = Test_alloc(MAX_TYPE+1);
	  while(1){
	    code = term_getkey();
	    typing_second = time_now();
	    
	    //時間が来たら終了する
	    if (time_now() >= test_time){
	      *(test.typed[test.y]+test.x) = '\0';
	      term_keyclear();
	      return;
	    }
	    
	    switch(code){
	    case 0xff://escape sequenseが始まった場合
	      esc_begin = TRUE;
	      continue;
	    case DEL:
	    case BS:
	      continue;
	    case ESC:
	      test.intprt.status = FALSE;
	      return;
	    case CR:
				//if (test.y < MAX_TYPE_LINE-1){//タイプの最大行を指定する時は使う      
	      if(test.text_no-1 <= test.y){
			return;
	      }
	      
	      *(test.typed[test.y]+test.x) = '\0';
	      test.x = 0;
	      test.y++;
	      test.typed[test.y] = Test_alloc(MAX_TYPE+1);
	      Test_disp_text(test);
	      
	      //} else{
	      //return;
	      //}
	      break;
	    default:
	      dispcode[0] = code;
	
	      //ESCシーケンスの場合無視する
    	  if(esc_begin == TRUE && 0x30 <= dispcode[0] && dispcode[0] < 0x40){//ファンクションキーの場合
			fnc_begin = TRUE;
			continue;
	 	  }else if(esc_begin == TRUE){
			esc_begin = FALSE;
			continue;
    	  }else if(fnc_begin == TRUE){
			esc_begin = FALSE;
			fnc_begin = FALSE;
		  }
	      
		  if (test.x < MAX_TYPE){//行内でタイプしている場合
		
		
			*(test.typed[test.y]+test.x) = code;
			test.x++;
			if (cursor){
				Examination * examination;
				Test * test = Test_get_test();
				examination = &(test->examination);
				if(examination->mode  == EXAM_MODE){//本番モードの場合
	  	            if(HIDE_TYPE_MODE_EX==1){//本番モードでタイプエコーを隠す場合
	     		        term_disp(" ");
	     	        }else{//本番モードでタイプエコーを隠さない場合				
	          		    term_disp(dispcode);
	           		}
	       		}else{//練習モードの場合
	            	if(HIDE_TYPE_MODE_PRA){//練習モードでタイプエコーを隠す場合
	             		term_disp(" ");
	            	}else{//練習モードでタイプエコーを隠さない場合
	            		term_disp(dispcode);
	           		}
	        	}
			}
		  }else{//行末にカーソルが行った場合
			*(test.typed[test.y]+test.x) = '\0';
			test.x = 0;
			test.y++;
			test.typed[test.y] = Test_alloc(MAX_TYPE+1);
			Test_disp_text(test);
		  }
		}
	  }
	}
	
	char *Test_alloc(int len)
	{
	  char *ret;
	  int i;
	  
	  if (!(ret = (char*)malloc(sizeof(char) * len))){
	    fprintf(stderr, "Out of memory!\n");
	    exit(1);
	  }
	  for (i = 0; i < len; i++){
	    ret[i] = 0;
	  }
	  
	  return ret;
	}
	
	/**
	 * タイピングの問題文を表示する
	 */
	void Test_disp_text()
	{
	  int tmp;
	  if (test.y < SCROLL_LINE){
	    tmp = TEST_Y + test.y * 2;
	  } else {
	    tmp = TEST_Y + SCROLL_LINE * 2 - 2;
	    term_scroll_up(TEST_X, TEST_Y, TEST_WIDTH, TEST_HEIGHT);
	    term_scroll_up(TEST_X, TEST_Y, TEST_WIDTH, TEST_HEIGHT);
	  }
	  
	  term_gotoxy(TEST_X, tmp);
	  term_disp(test.text_buf[test.y % test.text_no]);
	  if (ref_i("cursor")){
	    term_gotoxy(TEST_X, tmp+1);
	  } else {
	    term_gotoxy(0,0);
	  }
	}
	
	#define ERR_MESS_X (DISP_X+13)
	#define ERR_MESS_Y DISP_Y
	#define ERR_DISP_X (DISP_X+8)
	#define ERR_DISP_Y (DISP_Y+4)
	
	/**
	 * エラーチェックする
	 */
	void check()
	{
	  int count = 0;
	  int i;
	  
	  err_start();
	  
	  while(count <= test.y){
	    
	    //エラー結果を表示する
    for (i=0; i < ref_i("err_correct_line") && count <= test.y; i++, count++){
	      test.text_buf[count % test.text_no];
	      test.typed[count];
	      err_check(test.text_buf[count % test.text_no],test.typed[count]);
	    }
	  }
	}
	
	/**
	 * エラーチェックをする(表示もする)
	 */
	void Test_error_check()
	{
	  int count = 0;
	  int i;
	  Examination * examination2;
	  Test * test2 = Test_get_test();
	  
	  err_start();
	  
	  while(count <= test.y){
	    term_clear();
	    
	    Test_disp_info(ERR_MESS_X+2, ERR_MESS_Y);
	    
	    term_gotoxy(ERR_MESS_X, ERR_MESS_Y+2);
	    term_rev_disp(ref("errchk_message"));
	    
	    Intprt_disp_guide(&test.intprt);
	    
	    //エラー結果を表示する
	    for (i=0; i < ref_i("err_correct_line") && count <= test.y; i++, count++){
	      term_gotoxy(ERR_DISP_X, ERR_DISP_Y+i*4);
	      term_disp(test.text_buf[count % test.text_no]);
	      term_gotoxy(ERR_DISP_X, ERR_DISP_Y+i*4+1);
	      term_disp(test.typed[count]);
	      term_gotoxy(ERR_DISP_X, ERR_DISP_Y+i*4+2);
	      term_disp(err_check(test.text_buf[count % test.text_no],
				  test.typed[count]));
	    }
	    
	    //HIDE_MODEでなければ誤字結果の詳細を表示する
	    examination2 = &(test2->examination);
		if(examination2->mode  == EXAM_MODE){//本番モードの場合
	      if(HIDE_ERR_MODE_EX==0){//エラーチェック画面を表示する
    	    Intprt_long_pause_core(&test.intprt, ref("long_pause_message_test"), FALSE);
	      }
	    }else{//練習モードの場合
	      if(HIDE_ERR_MODE_PRA==0){//エラーチェック画面を表示する
        	Intprt_long_pause_core(&test.intprt, ref("long_pause_message_test"), FALSE);
	      }
	    }
	    if (!test.intprt.status){
	      return;
	    }
	  }
	}
	
	/**
	 * タイピング試験の時間終了である事を知らせる
	 */
	void Test_notify_end()
	{
	  
	  term_clear();
	  
	  term_gotoxy(ref_i("begin_x"),ref_i("begin_y"));
	  term_disp(ref("test_end_message"));
	  
	  Intprt_disp_guide(&test.intprt);
	  
	  Intprt_pause_core(&test.intprt, ref("test_end_message"), FALSE);
	}
	
	#define RESULT_X ERR_MESS_X
	#define RESULT_Y ERR_MESS_Y
	
	/**
	 * タイピングの結果を表示する．
	 */
	void Test_result()
	{
	  char buf[MAXSTRLEN];
	  int writeResult = TRUE;
	  Examination * examination2;
	  Test * test2 = Test_get_test();
	  
	  //合格情報を記録する for typingtestex拡張
	  Examination_writePassInfoAndShowResult(&(test.examination), &(test.user));
	 
	  term_clear();
	  
	  Test_disp_info(RESULT_X+2, RESULT_Y);
	  
	  term_gotoxy(RESULT_X, RESULT_Y+1);
	  term_rev_disp(ref("result_message"));
	  
	  //合否を表示
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 3);
	  term_disp(ref("exam_result"));
	  if(exam_result(typing_second)==TRUE){    
	    term_disp(ref("pass_message"));
	  }else{
	    term_disp(ref("fail_message"));
	  }    
	  
	  //試験日を表示
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 4);
	  term_disp(ref("exam_date"));
	  get_date(buf);
	  term_disp(buf);
	  
	  //タイピング時間を表示
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 5);
	  term_disp(ref("typing_time"));
	  sprintf(buf, "%d ", typing_second);
	  term_disp(buf);
	  term_disp(ref("second_message"));
	  
	  //端末名を表示
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 6);
	  term_disp(ref("machine_name"));
	  get_hostname(buf);
	  term_disp(buf);
	  
	  //正打鍵数を表示    
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 7);
	  term_disp(ref("correct_type"));
	  sprintf(buf, "%d ", err_correct_type());
	  term_disp(buf);
	  term_disp(ref("word_message"));
	  
	  //ミスタッチ数を表示    
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 8);
	  term_disp(ref("miss_count"));
	  sprintf(buf, "%d ", err_error_type());
	  term_disp(buf);
	  term_disp(ref("word_message"));
	  
	  //1分あたりの正しいタッチ数を表示    
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 9);
	  term_disp(ref("correct_type_per_min"));
	  sprintf(buf, "%d ", correct_type_per_min(typing_second));
	  term_disp(buf);
	  term_disp(ref("per_min_message"));
	  
	  
	  //テキスト情報のタイトル表示
	  term_gotoxy(RESULT_X, RESULT_Y + 11);
	  term_rev_disp(ref("exam_info_message"));
	  
	  //問題名を表示
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 13);
	  term_disp(ref("exam_title"));
	  term_disp(ref("title"));
	  
	  //最大誤字数
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 14);
	  term_disp(ref("max_error_type_msg"));
	  term_disp(ref("max_error_type"));
	  term_disp(ref("word_message"));
	  
	  //制限時間
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 15);
	  term_disp(ref("time_limit_msg"));
	  term_disp(ref("time_limit"));
	  term_disp(ref("second_message"));
	  
	  //テキスト総文字数
	  term_gotoxy(RESULT_X + 5, RESULT_Y + 16);
	  term_disp(ref("text_length_msg"));
	  term_disp(ref("total_text_length"));
	  term_disp(ref("word_message"));
	  
	  Intprt_disp_guide(&test.intprt);
	  
	  //次の画面遷移のメッセージを表示する
	  examination2 = &(test2->examination);
	  if(examination2->mode  == EXAM_MODE){//本番モードの場合
	    if(HIDE_ERR_MODE_EX){  
	      Intprt_pause_core(&test.intprt, ref("result_end_message"), FALSE);
	    }else{
      	  Intprt_pause_core(&test.intprt, ref("go_error_check_message"), FALSE);
	    }
	  }else{//練習モードの場合
	    if(HIDE_ERR_MODE_PRA){
	      Intprt_pause_core(&test.intprt, ref("result_end_message"), FALSE);
	    }else{
     	  Intprt_pause_core(&test.intprt, ref("go_error_check_message"), FALSE);
	    }
	  }
	}
	
	
	/**
	 * 日付を取得する
	 * 2007/03/02 副作用があったので引数をとるように修正
	 */
	void get_date(char * buf){
	
	  struct tm *local;
	  time_t now;
	  now = time(NULL);
	  local = localtime(&now);
	
  sprintf(buf, "%d/%d/%d", 1900+local->tm_year , local->tm_mon+1 , local->tm_mday);
	
	  //return buf;
	}
	
	/**
	 * ホスト名を取得する
	 * 2007/03/02 副作用があったので引数をとるように修正
	 * 2007/03/04 cnsで利用ができなかったので変更
	 */
	void get_hostname(char * buf){
	
	  int result;
	
	  result = gethostname(buf,256);
	  if(result == -1){
		strcpy(buf,"cannot define host");
	  }
	}
	
	void Test_disp_info(int x, int y)
	{
	  term_gotoxy(x,y);
	  /*
	    sprintf(global_tmp, "《%s %d  %s %d》 %d %s",
	    ref("lesson_message"), lesson_no,
	    ref("section_message"), section_no,
	    test.times, ref("times_message") );
	    sprintf(global_tmp, "",
	    ref("lesson_message"), "",
	    ref("section_message"), "",
	    test.times, ref("times_message") );
	    term_disp(tozenkaku(global_tmp));
	  */
	  //  term_disp("ここに試験問題名が表示される");
	}
	
	Test* Test_get_test(){
		return &test;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
