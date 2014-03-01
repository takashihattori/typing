#include "Token.h"
#include "Intprt.h"

#ifndef _MENU_H_
#define _MENU_H_

#define MAXCHAP 61
#define MAXSEC 10
#define MAXSECNAMELEN 10
#define MAXMESS 1000

typedef struct {
    int x;
    int y;
    int chap_num;
    struct {
	char title[MAXSTRLEN];
	char fname[MAXFNAMELEN];
	char sec[MAXSEC][MAXSECNAMELEN];
	int sec_num;
	Bool exit;
    } chap[MAXCHAP];
    char message[MAXMESS];
} MenuCtrl;

void MenuCtrl_moveto(int x0, int y0);
void MenuCtrl_chap_disp(int y);
void MenuCtrl_chap_rdisp(int y);
void MenuCtrl_sec_disp(int x, int y);
void MenuCtrl_sec_rdisp(int x, int y);

void MenuCtrl_init();

Bool MenuCtrl_set_message(char*);
Bool MenuCtrl_set_title(char*);
void MenuCtrl_set_fname(char*);
Bool MenuCtrl_add_sec(char*);
void MenuCtrl_set_exit();

void MenuCtrl_go_origin();
void MenuCtrl_left();
void MenuCtrl_right();
void MenuCtrl_up();
void MenuCtrl_down();
void MenuCtrl_redraw();
Bool MenuCtrl_run();

Bool Menu_mainloop();
void Menu_message();
void Menu_title();
void Menu_load();
void Menu_item();
void Menu_menu_exit();

void Menu_run(Token*);

#endif








