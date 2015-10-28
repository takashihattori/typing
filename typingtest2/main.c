#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "Intprt.h"
#include "Terminal.h"
#include "Global.h"

/**
 * �ᥤ��᥽�åɡ�
 * �ƥ����ȥե�������ɤ߹��ߡ��¹Ԥ��롥
 */
main(int argc, char *argv[])
{
	Intprt intprt;
	int i;
	char locale[MAXSTRLEN];//��������

#ifdef UNIX
	void            terminate();

	signal(SIGHUP, terminate);
	signal(SIGINT, terminate);
	signal(SIGTERM, terminate);
#endif

	//�ǥե���Ȥ����ܸ�
        strcpy(locale,"ja_JP.UTF-8");

	//�����Υ����å���Ԥ�
	for(i=1;i<argc;i++){		
          if (strcmp(argv[i], "--hide-err-ex") == 0) {//���֤Υ��顼�����å���̤�ɽ����ä��⡼�ɤ����
            HIDE_ERR_MODE_EX = 1;
          }else if(strcmp(argv[i], "--hide-err-pra") == 0) {//�����Υ��顼�����å���̤�ɽ����ä��⡼�ɤ����
            HIDE_ERR_MODE_PRA = 1;
          }else if(strcmp(argv[i], "--hide-type-pra") == 0) {//�����Υ����ץ������򱣤��⡼�ɤ����
            HIDE_TYPE_MODE_PRA = 1;
          }else if(strcmp(argv[i], "ja_JP.eucJP") == 0) {//���ܸ쵯ư���ץ����
	    strcpy(locale,"ja_JP.eucJP");
          }else if(strcmp(argv[i], "ja_JP.UTF-8") == 0) {//���ܸ쵯ư���ץ����
	    strcpy(locale,"ja_JP.UTF-8");
          }else if(strcmp(argv[i], "en") == 0) {//�Ѹ�
	    strcpy(locale,"en");
          } else {
	    fprintf(stderr, "Invalid argument\n");
	    exit(1);
	  }
	}

	term_init();
	NIntprt_init(&intprt,strcat(locale,"/typing.txt"));
	NIntprt_run(&intprt, 0, 0);
	term_end();
}

#ifdef UNIX
/**
 * �ץ�����λ���롥
 */
void
terminate()
{

	term_end();
	fprintf(stderr, "\nTerminate! Signal received.\n");
	exit(1);
}
#endif
