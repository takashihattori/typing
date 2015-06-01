#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#define MAX_VAR 200

void var_init();
void var_end();
void var_clear(char*);
void var_set(char*, char*);

char* ref(char*);
int ref_i(char*);
Bool ref_top(char*);
char* ref_now(char*);

void var_disp_table();

#endif
