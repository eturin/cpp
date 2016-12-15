#ifndef SERVER_H
#define SERVER_H

#include "common.h"

/*���������� ������� ��������� � ������� ������*/
#define MAX_EVENTS 2

struct Server {
	/*������� ����*/
	char type;
	/*��� ���������*/
	char *name;
	/*������� �������, � ������� ����� �������� ������*/
	char work_path[256];
	/*������-����� �������*/
	int msfd;
	/*�������������� ����*/
	int port;
	/*�������������� ������� �����*/
	struct sockaddr_in addr;
	/*������� �������*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*������� �������������� ��������� (������������ �� �������� ������� �������� ������ �� ������� OFF)*/
	WSAEVENT *phEvent_STOP;
	/*���� ������� �������*/
	HANDLE iocp;
};


struct Server * init_Server();
int start_server(struct Server*);
BOOL start_async(void*, DWORD, LPVOID, struct overlapped_inf*);

#endif