#include <stdio.h>
#ifdef __GNUG__
#include <std.h>
#endif

#include "Terminal.h"
#include "Variable.h"

char global_tmp[MAXSTRLEN];

//オプションのデフォルトを設定
int HIDE_ERR_MODE_EX = 0;
int HIDE_TYPE_MODE_EX = 1;
int HIDE_ERR_MODE_PRA = 0;
int HIDE_TYPE_MODE_PRA = 0;
