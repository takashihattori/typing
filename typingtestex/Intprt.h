#ifdef __GNUG__
#include <stddef.h>
#endif

#include "general.h"
#include "Token.h"

#ifndef _INTPRT_H_
#define _INTPRT_H_

#define DISP_X ref_i("disp_x")
#define DISP_Y ref_i("disp_y")

#define PAUSE_X ref_i("pause_x")
#define PAUSE_Y ref_i("pause_y")
#define HAND_X ref_i("hand_x")
#define HAND_Y ref_i("hand_y")

#define LONG_PAUSE_X ref_i("long_pause_x")
#define LONG_PAUSE_Y ref_i("long_pause_y")
#define LONG_PAUSE_WIDTH ref_i("long_pause_width")

//typingtestモード定義　（typingtestex用拡張）
#define EXAM_MODE 1
#define PRACTICE_MODE 2

typedef struct {
    Bool status;
    Token *token;
    char buf[MAXSTRLEN];
    char *buf_p;
} Intprt;

extern int lesson_no;
extern int section_no;

void Intprt_backward(Intprt*);
void Intprt_disp(Intprt*);
void Intprt_disp_guide(Intprt*);
void Intprt_pause(Intprt*);
void Intprt_pause_canback(Intprt*);
void Intprt_pause_core(Intprt*, char*, Bool);
void Intprt_long_pause(Intprt*);
void Intprt_long_pause_canback(Intprt*);
void Intprt_long_pause_core(Intprt*, char*, Bool);
void Intprt_disp_hand(Intprt*);
void Intprt_disp_hand_core(Intprt*, int, int);
void Intprt_set(Intprt*);
void Intprt_section(Intprt*);
void Intprt_menu(Intprt*);
void Intprt_test(Intprt*);
void Intprt_load(Intprt*);
void Intprt_run(Intprt*);
void Intprt_goto_next(Intprt*);
Bool Intprt_goto_section(Intprt*, int);
void NIntprt_init(Intprt*, char*);
void NIntprt_end(Intprt*);
int NIntprt_run(Intprt*, int,int);

//typingtestex拡張
void Intprt_registration(Intprt*);
void Intprt_selectMode(Intprt*);
#endif

