#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "Intprt.h"
#include "Terminal.h"
#include "Global.h"

/**
 * メインメソッド．
 * テキストファイルを読み込み，実行する．
 */
main(int argc, char *argv[])
{
	Intprt intprt;
	int i;
	char locale[MAXSTRLEN];//言語設定

#ifdef UNIX
	void            terminate();

	signal(SIGHUP, terminate);
	signal(SIGINT, terminate);
	signal(SIGTERM, terminate);
#endif

	//デフォルトは日本語
        strcpy(locale,"ja");

	//引数のチェックを行う
	for(i=1;i<argc;i++){		
          if (strcmp(argv[i], "--hide-err-ex") == 0) {//本番のエラーチェック結果の表示を消すモードを決める
            HIDE_ERR_MODE_EX = 1;
          }else if(strcmp(argv[i], "--hide-type-ex") == 0) {//本番のタイプエコーを隠すモードを決める
            HIDE_TYPE_MODE_EX = 1;
          }else if(strcmp(argv[i], "--hide-err-pra") == 0) {//練習のエラーチェック結果の表示を消すモードを決める
            HIDE_ERR_MODE_PRA = 1;
          }else if(strcmp(argv[i], "--hide-type-pra") == 0) {//練習のタイプエコーを隠すモードを決める
            HIDE_TYPE_MODE_PRA = 1;
          }else if(strcmp(argv[i], "ja") == 0) {//日本語起動オプション
	    strcpy(locale,"ja");
          }else if(strcmp(argv[i], "en") == 0) {//英語
	    strcpy(locale,"en");
          }
	}

	term_init();
	NIntprt_init(&intprt,strcat(locale,"/typing.txt"));
	NIntprt_run(&intprt, 0, 0);
	term_end();
}

#ifdef UNIX
/**
 * プログラムを終了する．
 */
void
terminate()
{

	term_end();
	fprintf(stderr, "\nTerminate! Signal received.\n");
	exit(1);
}
#endif
