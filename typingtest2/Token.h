/**
 *Token.h
 */

/*
    Token 解説
 typing 用のトークンは、以下のものに分類する。

(1)コマンドトークン
   COMMAND_PREFIX("!")で始まる英数字列("_"も含む)。
(2)文字列トークン
   " で始まり " で終るまでの間の文字列を一つのトークンとする。
   \" で " を表現することができる。

以下は、行の先頭から始まらないものである。

(3)数字トークン
   数字で始まる文字列。
(4)記号トークン
   英字で始まる文字列。
(5)未定義トークン
   上記以外の文字で始まる文字列。
(6)終了トークン
   ファイルの終りを表わす。

※ 空白、改行は無視する。
※ REMARK_PREFIX("#")以降は、注釈とみなして読み飛ばす。

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
