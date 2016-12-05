#ifndef REQ_H
#define REQ_H

#include "common.h"

/*���� ��� ������ ����������������� ������*/
struct Req {
	/*����������� URL*/
	char  *url;
	/*���������� ����������*/
	int    head_cnt;
	/*������ ���� � �������� ���������*/
	struct Head {
		char * field;
		char * value;
	}     *head;
	/*������ ������*/
	char  *status;
	/*���� ������*/
	char  *body;
	/*������� ���������*/
	size_t head_length;
	size_t body_length;
};
struct Req * init_Req();
struct Req * release_Req(struct Req *);

#endif