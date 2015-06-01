#include <stdio.h>
#include <string.h>
#ifdef __GNUG__
#include <std.h>
#endif

#include "ErrChk.h"
#include "general.h"

static char err_buf[MAXSTRLEN];
static int total_count;
static int error_count;
static char *text;
static char *type;
static char *err;

char* err_check(char* text_tmp, char* type_tmp)
{
    text = text_tmp;
    type = type_tmp;
    strcpy(err_buf, "");
    err = err_buf;

    while(*type){
	if(*text != *type){
	    if (*text == *(type+1) && *(text+1) == *type) {
		/* exchange */
		err_check_sub("XX", 2);
	    } else if (*(text+1) == *(type+1) && *(text+1) != *type){
		/* incollect */
		err_check_sub("E ", 2);
	    } else if (*text == *(type+1) && *(text+1) != *type){
		/* insert */
		err_check_sub("I ", 1);
	    } else if (*(text+1) == *type){
		/* delete */
		err_check_sub2("D", 2, 0);
	    } else if (*(text+2) == *(type+2)){
		err_check_sub("EE ", 3);
	    } else if (*(text+1) == *(type+2)){
		err_check_sub("EE ", 2);
	    } else if (*(text+2) == *(type+1)){
		err_check_sub("E ", 3);
	    } else if (*text == *(type+2)){
		err_check_sub("EE ", 1);
	    } else if (*(text+2) == *type){
		err_check_sub2("D ", 3, 0);
	    } else if (*(text+3) == *(type+3)){
		err_check_sub("EEE ", 4);
	    } else {
		err_check_sub("E", 1);
	    }
	} else {
	    err_check_sub(" ", 1);
	}
    }
    return err_buf;
}

void err_check_sub(char* mess, int t_cnt)
{
    int e_cnt = 0;
    char *p;
    for (p = mess; *p; p++){
	if (*p != ' '){
	    e_cnt++;
	}
    }
    err_check_sub2(mess, t_cnt, e_cnt);
}

void err_check_sub2(char* mess, int t_cnt, int e_cnt)
{
    strcpy(err, mess);
    err += strlen(mess);
    type += strlen(mess);
    text += t_cnt;
    total_count += t_cnt;
    error_count += e_cnt;
}

int err_correct_rate()
{
    if (err_total_type() > 0){
	return (long int)err_correct_type() * 100 / err_total_type();
    } else {
	return 0;
    }
}

void err_start()
{
    error_count = total_count = 0;
}

int err_total_type()
{
    return total_count;
};

int err_error_type()
{
    return error_count;
}

int err_correct_type()
{
    return total_count - error_count;
}

int err_score()
{
    return total_count - error_count * ref_i("penalty");
}

/**
 * 試験の合否を計算する．
 */
Bool exam_result(int typing_second){
  //テキストの文字数を取得する
  int text_length = ref_i("total_text_length");

  //誤字制限を取得する
  int max_error_type = ref_i("max_error_type");
  
  //目標タイプ速度を取得する
  int type_min_speed = ref_i("type_min_speed");

  //if(text_length-ref_i("max_error_type")<=err_correct_type()){
  if(text_length-max_error_type <= err_correct_type() &&
     error_count <= max_error_type &&
     type_min_speed <= correct_type_per_min(typing_second) ){
    return TRUE;
  }else{
    return FALSE;
  }
}

/**
 * 1分間あたりの正しいタッチ数を計得する．
 */
int correct_type_per_min(int typing_second){

  if(typing_second==0){
    return 0;
  }else{
    return (err_correct_type()*60)/typing_second;
  }
}
