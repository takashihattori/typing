/**
 *Token.h
 */

/*
    Token ����
 typing �ѤΥȡ�����ϡ��ʲ��Τ�Τ�ʬ�ह�롣

(1)���ޥ�ɥȡ�����
   COMMAND_PREFIX("!")�ǻϤޤ�ѿ�����("_"��ޤ�)��
(2)ʸ����ȡ�����
   " �ǻϤޤ� " �ǽ���ޤǤδ֤�ʸ������ĤΥȡ�����Ȥ��롣
   \" �� " ��ɽ�����뤳�Ȥ��Ǥ��롣

�ʲ��ϡ��Ԥ���Ƭ����Ϥޤ�ʤ���ΤǤ��롣

(3)�����ȡ�����
   �����ǻϤޤ�ʸ����
(4)����ȡ�����
   �ѻ��ǻϤޤ�ʸ����
(5)̤����ȡ�����
   �嵭�ʳ���ʸ���ǻϤޤ�ʸ����
(6)��λ�ȡ�����
   �ե�����ν����ɽ�魯��

�� ���򡢲��Ԥ�̵�뤹�롣
�� REMARK_PREFIX("#")�ʹߤϡ����Ȥߤʤ����ɤ����Ф���

*/

#include <stdio.h>
#include <string.h>

#ifdef __GNUG__
#include <std.h>
#include <stddef.h>
#endif

#include "general.h"

#ifndef _TOKEN_H_
#define _TOKEN_H_

#define REMARK_PREFIX '#'
#define COMMAND_PREFIX '!'
#define STRING_PREFIX '\"'

#define PSTACK_SIZE 30

typedef char tokenType;
#define COMMAND_token 0
#define NUMBER_token 1
#define SYMBOL_token 2
#define STRING_token 3
#define END_token 4
#define UNDEF_token 5

typedef struct {
    FILE *file;
    char token[256];
    char nextchar;
    tokenType tokentype;
    Bool topline;
    long int point_stack[PSTACK_SIZE];
    int stack_top;
} Token;

#define is_blank(c) (c == ' ' || c == '\t')
#define is_separater(c) (is_blank(c) || c == '\n')

void Token_get_sub(Token*, char);
void Token_blank_skip(Token*);
void Token_next(Token*);

void Token_end(Token*);

void Token_init(Token*, char*);
char *Token_get(Token*);

char *Token_now(Token*);
tokenType Token_type(Token*);
Bool Token_compare(Token*, char*);

void Token_push_point(Token*);
Bool Token_pop_point(Token*);

#endif
