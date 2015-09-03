/************************************************
 * �����ԥ󥰥ƥ���ex
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

//private�ؿ�����
void inputExaminationInfo(Examination *);
void	inputTime(Examination*);//���֤����Ϥ���
void	inputTimeAuto(Examination*);//��ư����Ver
void 	inputRoom(Examination*);//���������Ϥ���
void 	inputRoomAuto(Examination*);//���������Ϥ���
void 	inputManagerLogin(Examination*, char *);//���Ĵ��Υ�����̾�����Ϥ���
void 	setExaminationDate(Examination*);//���դ����ꤹ��
int confirmExaminationInfo(Examination * examination);//���Ϥ�����������������ǧ����
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
const char * YESNO[2] = {"�Ϥ�(Yes)","������(No)"};  
const char * MODES[3];

#define PASS_RECORD_SUCCEED 1
#define PASS_RECORD_ALREADY_PASS 2
#define PASS_RECORD_NOT_START 3
#define PASS_RECORD_DIFFERENT_QUESTION 4
#define PASS_RECORD_NOT_FOUND_QUESTION 5

/**********************************************************
 * �⡼�������Ϣ
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
 * �����������Ͽ
 ***********************************************************/

//���ּԤγƼ������������
void Examination_registration(Examination * examination, User * user){
	inputExaminationInfo(examination);//���������Ϥ���
	//	inputUserInfo(examination,user);//�����Ծ�������Ϥ���
	//	checkStartExamination(examination);
}

/*************************************************************
 * �������Ͽ��Ϣ
 * ************************************************************/

//���������Ϥ���
void inputExaminationInfo(Examination * examination){
	int result;
	char buf[256];
	int AUTO_SELECT=1;//auto
	
	while(1){
		//���������Ϥ������Ԥ�
		printf(ref("examination_info_title"));
		if(AUTO_SELECT){
			inputTimeAuto(examination);
			inputRoomAuto(examination);
		}else{
			inputTime(examination);//���֤����Ϥ���
			inputRoom(examination);
		}
		inputManagerLogin(examination, buf);//���Ĵ��Υ�����̾�����Ϥ���
		setExaminationDate(examination);//���դ����ꤹ��
		
		//���¸�ߤ��뤫��ǧ����
		result = checkExamination(examination) && getQuestionName(examination);
		if(!(result)){
			printf(ref("error_not_exist_examination"));
			if(!checkReInput()){
				typingtestExit();
			}
			AUTO_SELECT=selectMode(examination);
			continue;
		}
		
		//Ʊ�����֤��̤Υ桼�������������Ƥ��ʤ��ä�����ǧ����
		result= isFirstUserInHost(examination);
		if(!(result)){
			if(!checkReInput()){
				typingtestExit();
			}
			AUTO_SELECT=selectMode(examination);
			continue;
		}
		
		//���Ϸ�̤��ǧ����
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
		printf("\n��ư���Ϥ�Ԥ��ޤ�����\n(1)�Ϥ�\n(2)������\n");
		showPrompt();
		inputString(str,256);
		if(strlen(str)==1){
			if(atoi(str)==1){
				//��ư����
				return 0;
			}else if(atoi(str)==2){
				//��ư����
				return 1;
			}
		}
	}
}

//����֤����Ϥ���
void inputTime(Examination * examination){
	char * times[6];
	int i,number;
	
	//��������
	for(i = 0; i < 6; i++){
		times[i] = (char * )malloc(strlen("%d��(%d period)"));
		sprintf(times[i], "%d��(%d period)",i + 1,i+1);
	}
	
	//����֤����Ϥ���
	number = selectNumber(ref("select_examination_time"),times,6);
	examination->time = number;
	
	//�����
	for(i =0 ; i < 6; i++){
		free(times[i]);
	}
}
//��ư����Ver
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
	//printf("%d�¤ʤ�\n",number);
	
	examination->time=number;
	
}

//���������Ϥ���
void inputRoom(Examination * examination){
	int number;
	number = selectNumber(ref("select_examination_room"),ROOMS,5);
	strcpy(examination->room, ROOMS[number-1]);
}
//��ư����

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
	//printf("%s�ۤʤ�\n",ROOMS[number]);
}

//����Υ�����̾�����Ϥ���
void inputManagerLogin(Examination * examination, char * buf){
	printf(ref("input_manager_login"));
	showPrompt(); 
	inputString(examination->managerLogin,MAX_TEXT);
}

//���Ϥ������������������ǧ����
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

//������Ͽ������Ͽ����
void setExaminationDate(Examination * examination){
	time_t timer;
	timer = time(NULL);
	examination->date = localtime(&timer);
}

//�����������Ͽ����
void Examination_setTestStartDate(Examination * examination){
	time_t timer;
	timer = time(NULL);
	examination->testStartDate = localtime(&timer);
}

//������Ϥ���Ƥ��뤫Ĵ�٤�
int isInitialized(Examination * examination){
	char buf[256];
	char path[256];
	FILE * fp;
	
	//��ʼԥǥ��쥯�ȥ�Υѥ����������
	get_hostname(buf);
	getExaminationDir(examination,path);
	strcat(path , "/");
	strcat(path, ref("pass_dir"));
	strcat(path , "/");
	
	//�ե������񤭹���뤫�Τ����
	strcat(path,buf);
	fp = fopen(path,"w");
	if(fp == NULL){
		return FALSE;
	}
	fclose(fp);
	remove(path);
	return TRUE;
}

//���¸�ߤ��뤫��ǧ����
int checkExamination(Examination * examination){
	char path[256];
	DIR * dir;
	
	//�����ѥ����������
	getExaminationDir(examination,path);
	
	//���ꤵ�줿�ѥ���¸�ߤ��뤫Ĵ�٤�
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

// ����Ʊ��ü���Ǽ��������桼�������ʤ����Τ����
int isFirstUserInHost(Examination * examination){
	char host[256];
	char * login;
	char path[256];
	char loginNameInExistFile[256];
	FILE * file;
	
	get_hostname(host);
	login = getlogin();
		
	//�桼���ե�����Υѥ������ꤹ��
	getExaminationDir(examination,path);
	strcat(path, "/");
	strcat(path,host);
	strcat(path,".txt");
	
	//�����Ծ���ե����뤬�ʤ����Ȥ�Τ����
	file = fopen(path,"r");
	if(file != NULL){
		//���˵�Ͽ����Ƥ�������Ծ��󤫤ɤ���Ĵ�٤�
		fgets(loginNameInExistFile,256,file);//����ܤΥ�����̾���������
		fclose(file);
		if(strncmp(loginNameInExistFile,login,strlen(login)) != 0){ // �㤦�桼�������ä����
			printf(ref("error_same_host"));
			return FALSE;
		}
	}
	return TRUE;
}


//�����Ͽ�ѥ����������
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

//��Ͽ�����ɽ������
void showExaminationInfo(Examination * examination){
	printf("\n%s",ref("confirm_bar"));
	printf("%s - %d\n",ref("time_message"),examination->time);
	printf("%s - %s\n",ref("room_message"),examination->room);
	printf("%s - %s\n",ref("manager_login_message"), examination->managerLogin);
	printf(ref("confirm_bar"));
}

//������Ϥ���Ƥ��뤫Ĵ�٤�
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
 * �����Ծ�����Ͽ��Ϣ
 * ************************************************************/

//�桼������������Ϥ���
void inputUserInfo(Examination * examination, User * user){	
	char buf[256];
	char name[32];
	int result;
	
	printf(ref("user_info_title"));
	int AUTO_SELECT=1;//auto
	while(1){
		//������̾���������
		user->loginName = getlogin(); // �Ķ��ˤ�äƤϻȤ��ʤ���ǽ��������Τ�getenv���ѹ�����ɬ�פ��ꡥ
		
		//̾�������Ϥ���
		inputName(user);
		if(AUTO_SELECT){
			inputNumberAuto(user);
		}else{
			//�����ֹ�����Ϥ���
			inputNumber(user);
		}
		
		//���Ͼ�������������ǧ����
		if(!confirmUserInfo(user,buf)){//���Ϥ��������������ʤ����
			AUTO_SELECT=selectMode(examination);
			continue;//������
		}

		//�����Ծ����Ͽ����
		result = writeUserInfo(examination, user);
		if(result){
			break;
		}else{
			if(!checkReInput()){ //�����Ϥ�Ԥ�ʤ����
				typingtestExit();
			}
		}
		AUTO_SELECT=selectMode(examination);
	}
	term_clear();
	printf(ref("registration_complete"));
}

//�����ֹ���������
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
	//�桼����̾���������̾�����
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

//̾�������Ϥ���
void inputName(User * user){
	char * p;
	printf(ref("input_family_name"));
	showPrompt();
	inputString(user->familyName,MAX_TEXT);
	printf(ref("input_first_name"));
	showPrompt();
	inputString(user->firstName,MAX_TEXT);
}

//�����Ϥ�Ԥ����ɤ���Ƚ�Ǥ���
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

//���Ϥ����������������ɤ�����ǧ����
int confirmUserInfo(User * user, char * buf){
	int number;
	showUserInfo(user);
	number = selectNumber(ref("confirm_input"),YESNO,2);
	if(number == 2){ //���󤬴ְ�äƤ�����
		if(checkReInput()){//�����Ϥ�Ԥ����
			return FALSE;
		}else{
			typingtestExit();//�ץ�����λ����
		}
	}
	return TRUE;
}

//�桼�������ե�����˽񤭹���
int writeUserInfo(Examination * examination, User * user){
	FILE * file;
	char path[256];
	char host[256];
	char dateString[256];
	char loginNameInExistFile[30];//���˵�Ͽ����Ƥ���ե�����˽񤫤�Ƥ��������̾���ݻ�����
	int i;
	
	//�ۥ���̾���������
	get_hostname(host);
	
	//�桼���ե�����Υѥ������ꤹ��
	getExaminationDir(examination,path);
	strcat(path, "/");
	strcat(path,host);
	strcat(path,".txt");
	
	//�����Ծ���ե����뤬�ʤ����Ȥ�Τ����
	file = fopen(path,"r");
	if(file != NULL){
		//���˵�Ͽ����Ƥ�������Ծ��󤫤ɤ���Ĵ�٤�
		fgets(loginNameInExistFile,256,file);//����ܤΥ�����̾���������
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
	
	//�����Ծ����񤭹���	
	fprintf(file, "%s\n",user->loginName);
	fprintf(file, "%d\n",user->number);
	fprintf(file, "%s-%s\n",user->familyName,user->firstName);
	strftime(dateString, 255, "%Y-%m-%d-%H:%M", examination->date);
	fprintf(file, "%s\n",dateString);
	
	//�ե�������Ĥ���
	fclose(file);
	
	return TRUE;
}

//�����Ծ����ɽ�����ޤ�
void showUserInfo(User * user){	    
	printf(ref("confirm_bar"));
	printf("%s - %s\n",ref("your_login_message"),user->loginName);
	printf("%s - %s %s\n",ref("your_name_message"),user->firstName,user->familyName);
	printf("%s - %d\n", ref("your_number_message"),user->number);
	printf(ref("confirm_bar"));
}

/******************************************************************
 * ��ʼԵ�Ͽ
 * *****************************************************************/

#define RESULT_X (DISP_X+13)
#define RESULT_Y DISP_Y
 
 //��ʼԤ�Ͽ����̤�ɽ������
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
 
 //��ʼԤ�Ͽ����
 int Examination_writePassInfo(Examination * examination,User * user){
 	char path[256];
 	char host[256];
 	FILE * file;
 	
 	//��ʼԵ�Ͽ�ѥե�����Υѥ�����������
 	getExaminationDir(examination,path);
 	strcat(path,"/");
 	strcat(path,ref("pass_dir"));
 	strcat(path,"/");
 	get_hostname(host);
 	strcat(path,host);
 	strcat(path,".");
 	strcat(path,ref("txt_extension"));
 	 	
 	//�ե����뤬¸�ߤ��뤫��ǧ����
 	file = fopen(path,"r");
 	if(file != NULL){//���˥ե����뤬¸�ߤ��Ƥ�����
 		fclose(file);
 		return PASS_RECORD_ALREADY_PASS;
 	}
 	
 	//��ʼԤ�Ͽ����
 	{
 		//�ե�������������
	 	file = fopen(path,"w");
	 	if(file == NULL){
	 		return PASS_RECORD_NOT_START;
	 	}
	 	
	 	//�ե�����˾����񤭹���
	 	{

	 		
	 		//����꥿���ȥ��Ͽ����
	 		fprintf(file,"%s\n",ref("title"));

	 		//���դ�Ͽ����
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
	 	
 		//�ե�������Ĥ���
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
  if(file == NULL){ // ����ե����뤬�ʤ����
    return FALSE;
  }
  fgets(examination->question, MAX_QUESTION_NAME, file);
  if (examination->mode == 2) {
    fgets(examination->question, MAX_QUESTION_NAME, file); //2���ܤ��������
  }
  fclose(file);
  return TRUE;
}

/******************************************************************
 * �����ƥ�����
 * ****************************************************************/

//�ץ�����λ���롡�������ˤ���ΤϤ褯�ʤ��ΤǸ�ǰ�ư����
void typingtestExit(){
	printf(ref("exit_program"));
	sleep(1);//����ä��Ԥ�
	term_end();//��λ����
	exit(1);
}

/******************************************************************
 * UI����
 * *****************************************************************/

void showPrompt(){
	printf(">");
}

//���������򤹤�
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

//ʸ��������Ϥ�������
void inputString(char * string,int size){
	char * p;
	fgets(string,size,stdin);
	p = strchr( string, '\n' );
	if ( p != NULL ){/* ����ʸ�������ä���� */
    	*p = '\0';/* ����ʸ����üʸ�����֤������� */
	}
}
