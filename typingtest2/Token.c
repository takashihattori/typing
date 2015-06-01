#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifdef __GNUG__
#include <std.h>
#endif

#include "Global.h"
#include "Token.h"

/**
 * 初期化する．
 */
void Token_init(Token *token, char* filename){
  
  //ファイルが開けなかったらエラーを表示する
  if (! (token->file=fopen(filename, "r"))){
    
    sprintf(global_tmp, "ファイル %s がみつかりません。", filename);
    term_error(global_tmp);
  }

  token->topline = TRUE;
  Token_next(token);
  token->tokentype = UNDEF_token;
  token->stack_top = 0;
}

void Token_end(Token *token){

  //ファイルが閉じれなかったらエラーを表示する
  if (fclose(token->file)){

    term_error ("ファイルのクローズに失敗しました。");
  }
}

char* Token_get(Token *token){
  char* p = token->token;
  Bool cont = TRUE;
  
  Token_blank_skip(token);
  
  while(cont){
    cont = FALSE;
    switch (token->nextchar){
    case (char)EOF:
      token->tokentype = END_token;
      *p = '\0';
      break;
      
    case STRING_PREFIX:
      token->tokentype = STRING_token;
      Token_next(token);
      while(token->nextchar != STRING_PREFIX){
	*p = token->nextchar;
	Token_next(token);
	p++;
      }
      *p = '\0';
      Token_next(token);
      break;
      
    case COMMAND_PREFIX:
      token->tokentype = COMMAND_token;
      Token_get_sub(token, token->nextchar);
      break;
      
    default:
      if (token->nextchar >= '0' && token->nextchar <= '9'){
	token->tokentype = NUMBER_token;
      } else if ((token->nextchar >= 'A' && token->nextchar <= 'Z') ||
		 (token->nextchar >= 'a' && token->nextchar <= 'z')){
	token->tokentype = SYMBOL_token;
      } else {
	token->tokentype = UNDEF_token;
      }
      Token_get_sub(token, token->nextchar);
    }
  }
  
  return token->token;
}

char *Token_now(Token *token)
{
  return token->token;
}

void Token_blank_skip(Token *token)
{
  while(1){
    if (token->nextchar == REMARK_PREFIX){
      while(token->nextchar != '\n' && token->nextchar != (char)EOF){
	Token_next(token);
      }
    } else if (is_separater(token->nextchar)){
      Token_next(token);
    } else {
      return;
    }
  }
}

void Token_get_sub(Token *token, char first)
{
  char *p = token->token;
  
  if (first){
    *p = first;
    p++;
  }
  
  Token_next(token);
  
  while (!(is_separater(token->nextchar))){
    *p = token->nextchar;
    Token_next(token);
    p++;
  }
  *p = '\0';
}

/**
 * 次の文字を読んでTokenに付加する．
 */
void Token_next(Token *token){

  token->nextchar = getc(token->file);
}

tokenType Token_type(Token *token)
{
  return token->tokentype;
}

Bool Token_compare(Token *token, char *str)
{
  return !strcmp(token->token, str);
}

void Token_push_point(Token *token)
{
  int i;
  if (token->stack_top >= PSTACK_SIZE){
    for (i = 0; i < PSTACK_SIZE-1; i++){
      token->point_stack[i] = token->point_stack[i+1];
    }
    token->stack_top--;
  }
  token->point_stack[token->stack_top] = ftell(token->file);
  token->stack_top++;
}

Bool Token_pop_point(Token *token)
{
  if (token->stack_top > 0){
    if (fseek(token->file, token->point_stack[token->stack_top-1], 0) == 0){
      token->stack_top--;
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }
}
