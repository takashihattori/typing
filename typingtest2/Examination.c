/************************************************
 * タイピングテストex
 * @author rocky
 * @version 1.0(2007/03/19)
 *************************************************/

#include "Token.h"
#include "ErrChk.h"
#include "general.h"
#include "Intprt.h" 
#include "Examination.h"
#include <unistd.h>
#include "Terminal.h"
#include "Test.h"
#include <dirent.h>
#include <time.h>
#include <stdlib.h>
#include <ldap.h>
#include <pwd.h>
#include <sys/types.h>

#define l_SERVER     'ldaps://ldap.sfc.keio.ac.jp'
#define BASE_DN      "ou=people,dc=sfc,dc=keio,dc=ac,dc=jp"

//private関数たち
void inputExaminationInfo(Examination *);
void	inputTime(Examination*);//時間を入力する
void	inputTimeAuto(Examination*);//自動入力Ver
void 	inputRoom(Examination*);//教室を入力する
void 	inputRoomAuto(Examination*);//教室を入力する
void 	inputManagerLogin(Examination*, char *);//監督官のログイン名を入力する
void 	setExaminationDate(Examination*);//日付を設定する
int confirmExaminationInfo(Examination * examination);//入力した情報が正しいか確認する
void showExaminationInfo(Examination *);
int checkExamination(Examination *);
int isInitialized(Examination *);
void inputUserInfo(Examination *, User * );
void inputNumber(User *);
void inputNumberAuto(User*);
void inputName(User *);
int checkReInput();
int writeUserInfo(Examination *, User * );
void showUserInfo(User * );
void pauseByKey();
void showPrompt(void);
void getExaminationDir(Examination *,char *);
void typingtestExit();
int confirmUserInfo(User *,char *);
void checkStartExamination(Examination *);
void inputString(char *,int);
int isFirstUserInHost(Examination *);
int selectMode(Examination *);

const char * ROOMS[5] = {"k","e","i","o","l"};
const char * YESNO[2] = {"はい(Yes)","いいえ(No)"};  
const char * MODES[3];

#define PASS_RECORD_SUCCEED 1
#define PASS_RECORD_ALREADY_PASS 2
#define PASS_RECORD_NOT_START 3
#define PASS_RECORD_DIFFERENT_QUESTION 4
#define PASS_RECORD_NOT_FOUND_QUESTION 5

/**********************************************************
 * モード選択関連
 **********************************************************/
 
 void Examination_selectMode(Examination * examination){
 	int number;
	printf(ref("select_mode_title"));
	MODES[0] = ref("ordinary_mode");
	MODES[1] = ref("cover_mode");
	MODES[2] = ref("practice_mode");
	number = selectNumber(ref("select_mode_message"),MODES,3);
	examination->mode = number;
 }

/***********************************************************
 * 受験情報の登録
 ***********************************************************/

//受講者の各種情報を取得する
void Examination_registration(Examination * examination, User * user){
	inputExaminationInfo(examination);//試験情報を入力する
	//	inputUserInfo(examination,user);//受験者情報を入力する
	//	checkStartExamination(examination);
}

/*************************************************************
 * 試験情報登録関連
 * ************************************************************/

//試験情報を入力する
void inputExaminationInfo(Examination * examination){
	int result;
	char buf[256];
	int AUTO_SELECT=1;//auto
	
	while(1){
		//試験情報の入力と設定を行う
		printf(ref("examination_info_title"));
		if(AUTO_SELECT){
			inputTimeAuto(examination);
			inputRoomAuto(examination);
		}else{
			inputTime(examination);//時間を入力する
			inputRoom(examination);
		}
		inputManagerLogin(examination, buf);//監督官のログイン名を入力する
		setExaminationDate(examination);//日付を設定する
		
		//試験が存在するか確認する
		result = checkExamination(examination) && getQuestionName(examination);
		if(!(result)){
			printf(ref("error_not_exist_examination"));
			if(!checkReInput()){
				typingtestExit();
			}
			AUTO_SELECT=selectMode(examination);
			continue;
		}
		
		//同じ時間に別のユーザーが受験していなかったか確認する
		result= isFirstUserInHost(examination);
		if(!(result)){
			if(!checkReInput()){
				typingtestExit();
			}
			AUTO_SELECT=selectMode(examination);
			continue;
		}
		
		//入力結果を確認する
		result = confirmExaminationInfo(examination);
		if(result){
			break;
		}
		AUTO_SELECT=selectMode(examination);
	}
}

int selectMode(Examination * examination){
	char str[256];
	while(1){
		printf("\n手動入力を行いますか？\n(1)はい\n(2)いいえ\n");
		showPrompt();
		inputString(str,256);
		if(strlen(str)==1){
			if(atoi(str)==1){
				//手動入力
				return 0;
			}else if(atoi(str)==2){
				//自動入力
				return 1;
			}
		}
	}
}

//試験時間を入力する
void inputTime(Examination * examination){
	char * times[6];
	int i,number;
	
	//選択肢を作る
	for(i = 0; i < 6; i++){
		times[i] = (char * )malloc(strlen("%d限(%d period)"));
		sprintf(times[i], "%d限(%d period)",i + 1,i+1);
	}
	
	//試験時間を入力する
	number = selectNumber(ref("select_examination_time"),times,6);
	examination->time = number;
	
	//後処理
	for(i =0 ; i < 6; i++){
		free(times[i]);
	}
}
//自動入力Ver
void inputTimeAuto(Examination * examination){
	
	int i;
	char buf[256];
	int number;
	
	time_t timer;
	struct tm *t_st;
	int t;
	
	time(&timer);
	t_st=localtime(&timer);
	//printf("%d:%d\n",t_st->tm_hour,t_st->tm_min);
	t=t_st->tm_hour*100+t_st->tm_min;
	
	if(t<925)number=1;
	else if(t<1110)number=1;
	else if(t<1300)number=2;
	else if(t<1445)number=3;
	else if(t<1615)number=4;
	else if(t<1800)number=5;
	else if(t<1930)number=6;
	else number=6;
	//printf("%d限なう\n",number);
	
	examination->time=number;
	
}

//教室を入力する
void inputRoom(Examination * examination){
	int number;
	number = selectNumber(ref("select_examination_room"),ROOMS,5);
	strcpy(examination->room, ROOMS[number-1]);
}
//自動入力

void inputRoomAuto(Examination * examination){
	char host[256];
	char *str;
	get_hostname(host);
	int number;
	
	str=strtok(host,".");
	str=strtok(str,"zmac");
	number=atoi(str);
	if(number<40)number=0;
	else if(number<80)number=1;
	else if(number<120)number=2;
	else if(number<160)number=3;
	else number=4;
	
	strcpy(examination->room, ROOMS[number]);
	//printf("%s館なう\n",ROOMS[number]);
}

//試験官のログイン名を入力する
void inputManagerLogin(Examination * examination, char * buf){
	printf(ref("input_manager_login"));
	showPrompt(); 
	inputString(examination->managerLogin,MAX_TEXT);
}

//入力した試験情報が正しいか確認する
int confirmExaminationInfo(Examination * examination){
	char buf[256];
	int number;
	showExaminationInfo(examination);
	number = selectNumber(ref("confirm_input"),YESNO,2);
	if(number == 1){
		return TRUE;
	}else{
		return FALSE;
	}
}

//受験登録日時を記録する
void setExaminationDate(Examination * examination){
	time_t timer;
	timer = time(NULL);
	examination->date = localtime(&timer);
}

//試験開始日時を記録する
void Examination_setTestStartDate(Examination * examination){
	time_t timer;
	timer = time(NULL);
	examination->testStartDate = localtime(&timer);
}

//試験が開始されているか調べる
int isInitialized(Examination * examination){
	char buf[256];
	char path[256];
	FILE * fp;
	
	//合格者ディレクトリのパスを取得する
	get_hostname(buf);
	getExaminationDir(examination,path);
	strcat(path , "/");
	strcat(path, ref("pass_dir"));
	strcat(path , "/");
	
	//ファイルを書き込めるか確かめる
	strcat(path,buf);
	fp = fopen(path,"w");
	if(fp == NULL){
		return FALSE;
	}
	fclose(fp);
	remove(path);
	return TRUE;
}

//試験が存在するか確認する
int checkExamination(Examination * examination){
	char path[256];
	DIR * dir;
	
	//試験情報パスを取得する
	getExaminationDir(examination,path);
	
	//指定されたパスが存在するか調べる
	dir = opendir(path);
	if(dir != NULL){
		closedir(dir);
		return TRUE;
	}else{
		
#ifdef DEBUG
	printf("[DEBUG]path = %s",path);
#endif
		return FALSE;
	}
}

// 既に同じ端末で受験したユーザがいないか確かめる
int isFirstUserInHost(Examination * examination){
	char host[256];
	char * login;
	char path[256];
	char loginNameInExistFile[256];
	FILE * file;
	
	get_hostname(host);
	login = getlogin();
		
	//ユーザファイルのパスを設定する
	getExaminationDir(examination,path);
	strcat(path, "/");
	strcat(path,host);
	strcat(path,".txt");
	
	//受験者情報ファイルがないことを確かめる
	file = fopen(path,"r");
	if(file != NULL){
		//既に記録されている受験者情報かどうか調べる
		fgets(loginNameInExistFile,256,file);//一行目のログイン名を取得する
		fclose(file);
		if(strncmp(loginNameInExistFile,login,strlen(login)) != 0){ // 違うユーザーだった場合
			printf(ref("error_same_host"));
			return FALSE;
		}
	}
	return TRUE;
}


//試験情報記録パスを取得する
void getExaminationDir(Examination * examination, char * path){
	char dateString[11];
	char examinationDir[256];
	
	sprintf(path, "%s",ref("home_dir"));  
	strcat(path, examination->managerLogin);
	strcat(path, "/");
	strcat(path, ref("admin_dir"));
	strcat(path, "/");
	strftime(dateString, 255, "%Y-%m-%d", examination->date);
	sprintf(examinationDir,"%s-%s-%d",dateString,examination->room,examination->time);
	strcat(path,examinationDir);
}

//登録情報を表示する
void showExaminationInfo(Examination * examination){
	printf("\n%s",ref("confirm_bar"));
	printf("%s - %d\n",ref("time_message"),examination->time);
	printf("%s - %s\n",ref("room_message"),examination->room);
	printf("%s - %s\n",ref("manager_login_message"), examination->managerLogin);
	printf(ref("confirm_bar"));
}

//試験が開始されているか調べる
void checkStartExamination(Examination * examination){
	char buf[256];
	char * candidates[2];
	int number;
	candidates[0] = ref("start_examination_message");
	candidates[1] = ref("exit_program_message");
	while(!isInitialized(examination)){
		number = selectNumber(ref("error_start_examination"),candidates,2);
		if(number == 1){
			continue;
		}else if(number == 2){
			typingtestExit();
		}
	}
}

/**************************************************************
 * 受験者情報登録関連
 * ************************************************************/

//ユーザー情報を入力する
void inputUserInfo(Examination * examination, User * user){	
	char buf[256];
	char name[32];
	int result;
	
	printf(ref("user_info_title"));
	int AUTO_SELECT=1;//auto
	while(1){
		//ログイン名を取得する
		user->loginName = getlogin(); // 環境によっては使えない可能性があるのでgetenvに変更する必要あり．
		
		//名前を入力する
		inputName(user);
		if(AUTO_SELECT){
			inputNumberAuto(user);
		}else{
			//学籍番号を入力する
			inputNumber(user);
		}
		
		//入力情報が正しいか確認する
		if(!confirmUserInfo(user,buf)){//入力した情報が正しくなければ
			AUTO_SELECT=selectMode(examination);
			continue;//再入力
		}

		//受験者情報を記録する
		result = writeUserInfo(examination, user);
		if(result){
			break;
		}else{
			if(!checkReInput()){ //再入力を行わない場合
				typingtestExit();
			}
		}
		AUTO_SELECT=selectMode(examination);
	}
	term_clear();
	printf(ref("registration_complete"));
}

//学籍番号を取得する
void inputNumber(User * user){
	char buf[256];
	while(1){
		printf(ref("input_number"));
		showPrompt();
		inputString(buf,256);
		if(strlen(buf) == 8 && atoi(buf) != 0){
			user->number = atoi(buf);
			break;
		}else{
			printf(ref("error_input_number"),buf);
		}
	}
}

void inputNumberAuto(User * user){
	
	struct passwd *p;
	LDAP        *ld;
    LDAPMessage *result=NULL;
	int          entries;
	char ll[64]="ldap.sfc.keio.ac.jp";
	const char *hostname=ll;
	
	char *base=BASE_DN;
	char str[80];
	char *attrs[2]={"keioIDNumber",0};
	//ユーザー名からログイン名を取得
	uid_t uid = getuid();
	p=getpwuid(uid);
	//printf("login name:%s\n",p->pw_name);
	// Initialize
	ld = ldap_init(hostname, 389);
	if (ld == NULL) {
        perror("ldap_init");
        exit(1);
	}
	if (ldap_simple_bind_s(ld, NULL, NULL) != LDAP_SUCCESS) {
        ldap_perror(ld, "ldap_simple_bind_s");
        ldap_unbind(ld);
        exit(1);
	}
	strcpy(str,p->pw_name);
	char attr[100]="(uid=";
	strcat(attr,str);
	strcat(attr,")");
	//printf("%s\n",attr);
	char *a=attr;
	if (ldap_search_s(ld, base, LDAP_SCOPE_SUBTREE,
        a, 0, 0, &result) != LDAP_SUCCESS)
    {
        ldap_perror(ld, "ldap_search_s");
        ldap_unbind(ld);
        exit(1);
	}
	entries = ldap_count_entries(ld, result);
    //printf("Found %d entries\n", entries);
	
	result=ldap_first_entry(ld,result);
	int i,j,lim;
	char **vals;
	char *num;
	if(result==NULL){
		ldap_perror(ld,"ldap_first_entry");
		ldap_unbind(ld);
		exit(1);
	}else{
		while(result!=NULL){
			vals=ldap_get_values(ld,result,"keioIDNumber");
			lim=ldap_count_values(vals);
			for(i=0;i<lim;i++){
				//printf("ID:%s\n",vals[i]);
				if(strlen(vals[i]) == 8 && atoi(vals[i]) != 0)j=atoi(vals[i]);
			}
			result=ldap_next_entry(ld,result);
		}
		ldap_value_free(vals);
	}
    ldap_msgfree(result);
	ldap_unbind(ld);
	
	user->number=j;
}

//名前を入力する
void inputName(User * user){
	char * p;
	printf(ref("input_family_name"));
	showPrompt();
	inputString(user->familyName,MAX_TEXT);
	printf(ref("input_first_name"));
	showPrompt();
	inputString(user->firstName,MAX_TEXT);
}

//再入力を行うかどうか判断する
int checkReInput(){
	char buf[256];
	int number;
	number = selectNumber(ref("confirm_re_input"),YESNO,2);
	if(number == 1){
		return TRUE;
	}else{
		return FALSE;
	}
} 

//入力した情報が正しいかどうか確認する
int confirmUserInfo(User * user, char * buf){
	int number;
	showUserInfo(user);
	number = selectNumber(ref("confirm_input"),YESNO,2);
	if(number == 2){ //情報が間違っている場合
		if(checkReInput()){//再入力を行う場合
			return FALSE;
		}else{
			typingtestExit();//プログラムを終了する
		}
	}
	return TRUE;
}

//ユーザ情報をファイルに書き込む
int writeUserInfo(Examination * examination, User * user){
	FILE * file;
	char path[256];
	char host[256];
	char dateString[256];
	char loginNameInExistFile[30];//既に記録されているファイルに書かれているログイン名を保持する
	int i;
	
	//ホスト名を取得する
	get_hostname(host);
	
	//ユーザファイルのパスを設定する
	getExaminationDir(examination,path);
	strcat(path, "/");
	strcat(path,host);
	strcat(path,".txt");
	
	//受験者情報ファイルがないことを確かめる
	file = fopen(path,"r");
	if(file != NULL){
		//既に記録されている受験者情報かどうか調べる
		fgets(loginNameInExistFile,256,file);//一行目のログイン名を取得する
		fclose(file);
		if(strncmp(loginNameInExistFile,user->loginName,strlen(user->loginName)) != 0){
			printf(ref("error_same_host"));
			return FALSE;
		}
	}
	
	file = fopen(path,"w");
	if(file == NULL){
		printf(ref("error_faild_making_user_file"));
		typingtestExit();
	}
	
	//受験者情報を書き込む	
	fprintf(file, "%s\n",user->loginName);
	fprintf(file, "%d\n",user->number);
	fprintf(file, "%s-%s\n",user->familyName,user->firstName);
	strftime(dateString, 255, "%Y-%m-%d-%H:%M", examination->date);
	fprintf(file, "%s\n",dateString);
	
	//ファイルを閉じる
	fclose(file);
	
	return TRUE;
}

//受験者情報を表示します
void showUserInfo(User * user){	    
	printf(ref("confirm_bar"));
	printf("%s - %s\n",ref("your_login_message"),user->loginName);
	printf("%s - %s %s\n",ref("your_name_message"),user->firstName,user->familyName);
	printf("%s - %d\n", ref("your_number_message"),user->number);
	printf(ref("confirm_bar"));
}

/******************************************************************
 * 合格者記録
 * *****************************************************************/

#define RESULT_X (DISP_X+13)
#define RESULT_Y DISP_Y
 
 //合格者を記録し結果を表示する
 void Examination_writePassInfoAndShowResult(Examination * examination, User * user){
	int writeResult;
	Test * test;
	
#if !defined DEBUG  
	if(exam_result(typing_second)){
#endif
	
	  if(EXAM_MODE(examination->mode)){
	    term_clear();
	  	writeResult = Examination_writePassInfo(examination,user);
	  	term_gotoxy(RESULT_X, RESULT_Y+2);
	  	term_rev_disp(ref("pass_message"));
	  	term_gotoxy(RESULT_X+2, RESULT_Y+4);
	  	term_disp(ref("result_passing"));
	  	term_gotoxy(RESULT_X+2, RESULT_Y+5);
	  	term_disp(ref("check_passing"));
	  	term_gotoxy(RESULT_X, RESULT_Y+7);
	  	term_rev_disp(ref("pass_record_label"));
	  	if(writeResult == PASS_RECORD_SUCCEED){
	  		term_gotoxy(RESULT_X+2, RESULT_Y+9);
	  		term_disp(ref("succeed_pass_record"));
	  	}else{
	  		term_gotoxy(RESULT_X+2, RESULT_Y+9);
	  		term_disp(ref("faild_pass_record"));
	  		term_gotoxy(RESULT_X+2, RESULT_Y+10);
	  		if(writeResult == PASS_RECORD_ALREADY_PASS){
	  			term_disp(ref("error_already_pass"));
	  			term_gotoxy(RESULT_X+2, RESULT_Y+11);
	  		}else if(writeResult == PASS_RECORD_NOT_START){
	  			term_disp(ref("error_not_start"));
	  			term_gotoxy(RESULT_X+2, RESULT_Y+11);
	  		}else if(writeResult == PASS_RECORD_DIFFERENT_QUESTION){
	  			term_disp(ref("error_different_question"));
	  			term_gotoxy(RESULT_X+2, RESULT_Y+11);
	  		}else if(writeResult == PASS_RECORD_NOT_FOUND_QUESTION){
	  			term_disp(ref("error_not_found_question"));
	  			term_gotoxy(RESULT_X+2, RESULT_Y+11);
	  			term_disp(ref("select_right_question"));
	  			term_gotoxy(RESULT_X+2, RESULT_Y+12);
	  		}
	  		term_disp(ref("call_manager_help"));
	  	}
	  	test = Test_get_test();
	  	Intprt_pause_core(&(test->intprt),ref("test_end_message"), FALSE);
	  }
 #if !defined DEBUG
  }
  #endif
 	
 }
 
 //合格者を記録する
 int Examination_writePassInfo(Examination * examination,User * user){
 	char path[256];
 	char host[256];
 	FILE * file;
 	
 	//合格者記録用ファイルのパスを生成する
 	getExaminationDir(examination,path);
 	strcat(path,"/");
 	strcat(path,ref("pass_dir"));
 	strcat(path,"/");
 	get_hostname(host);
 	strcat(path,host);
 	strcat(path,".");
 	strcat(path,ref("txt_extension"));
 	 	
 	//ファイルが存在するか確認する
 	file = fopen(path,"r");
 	if(file != NULL){//既にファイルが存在していたら
 		fclose(file);
 		return PASS_RECORD_ALREADY_PASS;
 	}
 	
 	//合格者を記録する
 	{
 		//ファイルを作成する
	 	file = fopen(path,"w");
	 	if(file == NULL){
	 		return PASS_RECORD_NOT_START;
	 	}
	 	
	 	//ファイルに情報を書き込む
	 	{

	 		
	 		//試験問題タイトルを記録する
	 		fprintf(file,"%s\n",ref("title"));

	 		//日付を記録する
	 		{
		 		time_t timer;
		 		struct tm *nowTime;
		 		char date[20];
				strftime(date, 255, "%Y-%m-%d-%H:%M", examination->testStartDate);
				fprintf(file, "%s\n",date);
				timer = time(NULL);
				nowTime = localtime(&timer);
				strftime(date, 255, "%Y-%m-%d-%H:%M", nowTime);
				fprintf(file, "%s",date);
	 		}
	 	}
	 	
 		//ファイルを閉じる
 		fclose(file);
 	}
 	
	return PASS_RECORD_SUCCEED;
 }
 
int getQuestionName(Examination *examination) {
  char path[256];
  FILE * file;

  getExaminationDir(examination,path);
  strcat(path,"/");
  strcat(path,ref("question_file"));
  file = fopen(path,"r");
  if(file == NULL){ // 問題ファイルがない場合
    return FALSE;
  }
  fgets(examination->question, MAX_QUESTION_NAME, file);
  if (examination->mode == 2) {
    fgets(examination->question, MAX_QUESTION_NAME, file); //2行目を取得する
  }
  fclose(file);
  return TRUE;
}

/******************************************************************
 * システム制御
 * ****************************************************************/

//プログラムを終了する　※ここにあるのはよくないので後で移動する
void typingtestExit(){
	printf(ref("exit_program"));
	sleep(1);//ちょっと待つ
	term_end();//終了する
	exit(1);
}

/******************************************************************
 * UI部品
 * *****************************************************************/

void showPrompt(){
	printf(">");
}

//数字を選択する
int selectNumber(char * message, const char * candidates[], int max){
	int i;
	char buf[256];
	while(1){
		printf(message);
		printf(ref("input_candidates"),1,max);
		for(i = 0; i< max; i++){
			printf("%d) %s\n", i+1, candidates[i]);
		}
		showPrompt();
		inputString(buf,256);
		if(atoi(buf) > 0 && atoi(buf) < max + 1){
			return atoi(buf);
		}else{
			printf(ref("error_invalid_input"),max,buf);
		}
	}
}

//文字列の入力を受け取る
void inputString(char * string,int size){
	char * p;
	fgets(string,size,stdin);
	p = strchr( string, '\n' );
	if ( p != NULL ){/* 改行文字があった場合 */
    	*p = '\0';/* 改行文字を終端文字に置き換える */
	}
}
