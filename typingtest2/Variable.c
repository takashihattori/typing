#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __GNUG__
#include <std.h>
#endif
#ifdef MSDOS
#include <stdlib.h>
#include <malloc.h>
#endif

#include "Global.h"
#include "Variable.h"

typedef struct _StrElement{
    char *value;
    struct _StrElement *next;
} StrElement;

void var_delete_vlist(StrElement*);
StrElement* var_new_element(char*);

struct {
    char* name;
    StrElement* vlist;
    StrElement* point;
} var[MAX_VAR];

void var_init()
{
    var[0].name = NULL;
    var[0].vlist = NULL;
}

void var_end()
{
    int i;
    for(i = 0; var[i].name; i++){
	free(var[i].name);
	var_delete_vlist(var[i].vlist);
    }
}

void var_delete_vlist(StrElement* p)
{
    for (;p; p = p->next){
	free(p->value);
    }
}

void var_clear(char* name)
{
    int i;
    for (i = 0; var[i].name; i++){
	if (!strcmp(var[i].name, name)){
	    var_delete_vlist(var[i].vlist);
	    var[i].vlist = var[i].point = NULL;
	    return;
	}
    }
    
    if (i >= MAX_VAR-1){
	term_error("変数の数が多すぎます。");
    }
    if (!(var[i].name = (char*)malloc(sizeof(char) * strlen(name) + 1))){
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    strcpy(var[i].name, name);
    var[i].vlist = var[i].point = NULL;
    var[i+1].name = NULL;
    var[i+1].vlist = NULL;
}

void var_set(char* name, char* value)
{
    int i;
    StrElement* p;
    for (i = 0; var[i].name; i++){
	if (!strcmp(var[i].name, name)){
	    break;
	}
    }
    if (!var[i].name){
	term_error("内部エラー:変数定義がおかしい。");
	return;
    }

    if (! var[i].vlist){
	var[i].vlist = var[i].point = var_new_element(value);
    } else {
	for (p = var[i].vlist; p->next; p = p->next);
	p->next = var_new_element(value);
    }
}

StrElement* var_new_element(char* value)
{
    StrElement* p;
    if (!(p = (StrElement*)malloc(sizeof(StrElement)))){
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    if (!(p->value = (char*)malloc(sizeof(char)*strlen(value)+1))){
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    strcpy(p->value, value);
    p->next = NULL;
    return p;
}

char* ref(char* name)
{
    ref_top(name);
    return ref_now(name);
}

int ref_i(char* name)
{
    return (atoi(ref(name)));
}

Bool ref_top(char* name)
{
    int i;
    for (i = 0; var[i].name; i++){
	if (!strcmp(var[i].name, name)){
	    var[i].point = var[i].vlist;
	    return TRUE;
	}
    }
    sprintf(global_tmp, "変数 %s は定義されていません。", name);
    term_error(global_tmp);
    return FALSE;
}    

char* ref_now(char* name)
{
    int i;
    char *p;
    for (i = 0; var[i].name; i++){
	if (!strcmp(var[i].name, name)){
	    if (var[i].point){
		p = var[i].point->value;
		var[i].point = var[i].point->next;
		return p;
	    } else {
		return NULL;
	    }
	}
    }
    sprintf(global_tmp, "変数 %s は定義されていません。", name);
    term_error(global_tmp);
    return FALSE;
}

void var_disp_table()
{
    int i;
    for(i = 0; var[i].name; i++){
	printf("%10s:%50s\n", var[i].name, var[i].vlist->value);
    }
}
