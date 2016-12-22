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

	/*������� ������� (��������� � ����������� � loop)*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*������� �������������� ��������� �������� � hEvents (������������ �� �������� ������� �������� ������ �� ������� OFF)*/
	WSAEVENT *phEvent_STOP;
	
	/*���� ������� ������� (��������� � ����������� � loop)*/
	HANDLE iocp;

	/*�������� � ��������, ������������� ����������� ����������*/
	struct Worker * pWIsapi;
	
	/*����������� ������, ��� ������������� ������ �������� isapi*/
	CRITICAL_SECTION cs;
};


struct Server * init_Server();
int start_server(struct Server*);
BOOL start_async(void*, DWORD, LPVOID, struct Overlapped_inf*);

#endif