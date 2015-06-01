#include <time.h>
#include "Test.h"

#if !defined(EXAMINATION_H)
#define EXAMINATION_H

#define MAX_REPEAT 10
#define MAX_TEXT 30
#define MAX_TYPE_LINE 50
#define MAX_TYPE 70

#define TRUE 1
#define FALSE 0

//�����Ծ���򤢤�魯��¤��
typedef struct {
    char firstName[MAX_TEXT];
    char familyName[MAX_TEXT];
    int number;
    char* loginName;
} User;

//��������򤢤�魯��¤��
typedef struct {
	struct tm * date; //��������
	struct tm * testStartDate; //���������
	char managerLogin[MAX_TEXT];//����ĥ�����
	int time; //����
	char room[10]; //����
	int mode;
}Examination;

void Examination_selectMode(Examination*);
void Examination_registration(Examination*,User*);
void Examination_writePassInfoAndShowResult(Examination *, User *);
int Examination_writePassInfo(Examination*,User*);
void Examination_setTestStartDate(Examination *);
#endif
