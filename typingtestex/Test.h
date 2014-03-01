#ifndef _TEST_H_
#define _TEST_H_

#include "Token.h"
#include "ErrChk.h"
#include "Intprt.h"
#include "Examination.h"

#define MAX_REPEAT 10
#define MAX_TEXT 30
#define MAX_TYPE_LINE 50
#define MAX_TYPE 70

typedef struct {
    char *text_buf[MAX_TEXT];
    int text_no;
    char *typed[MAX_TYPE_LINE];
    char *message_buf[MAX_REPEAT];
    int x,y;
    int times;
    Intprt intprt;

    //typingtestex用拡張
    Examination examination;//受験情報
    User user;//ユーザ情報
  
} Test;

void Test_text();
void Test_message();
void Test_mainloop();
void Test_begin();
void Test_typing();
char *Test_alloc();
void check();
void Test_error_check();
void Test_notify_end();
void Test_result();
void Test_disp_text();
void get_date(char *);
void get_hostname(char *);
void Test_disp_info(int,int);


void Test_test();
void Test_end();
Bool Test_run(Token*);

//typingtestex拡張
Test* Test_get_test();
extern Test test;
extern int typing_second;
#endif
