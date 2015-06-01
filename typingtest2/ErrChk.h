#include "general.h"
#include "Global.h"

#ifndef _ERRCHK_H_
#define _ERRCHK_H_

void err_check_sub(char*, int);
void err_check_sub2(char*, int, int);

void err_start();
char* err_check(char*, char*);
int err_total_type();
int err_error_type();
int err_correct_type();
int err_correct_rate();
int err_score();
Bool exam_result(int);
int correct_type_per_min();

#endif
