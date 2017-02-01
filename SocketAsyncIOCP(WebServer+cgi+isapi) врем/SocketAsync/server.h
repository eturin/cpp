#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "crypt.h"

/*���������� ������� ��������� � ������� ������*/
#define MAX_EVENTS 3

struct CredHandle;

struct Server {
	/*������� ����*/
	char type;
	/*��� ���������*/
	char *name;
	/*������� �������, � ������� ����� �������� ������*/
	char work_path[256];
	/*�� ���� ������� ����������*/
	char cert_name[256];
	/*���� � ����������� (��� ����� ��� ��� ���������)*/
	char cert_path[256];
	/*������-����� �������*/
	int msfd, smsfd;
	/*�������������� ����*/
	int port, sport;
	/*�������������� ������� �����*/
	struct sockaddr_in addr, saddr;
	/*������ �������*/
	CredHandle hServerCreds;

	/*������� ������� (��������� � ����������� � loop)*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*������� �������������� ��������� �������� � hEvents (������������ �� �������� ������� �������� ������ �� ������� OFF)*/
	WSAEVENT *phEvent_STOP;
	
	/*���� ������� ������� (��������� � ����������� � loop)*/
	HANDLE iocp;

	/*�������� � ��������, ������������� ����������� ����������*/
	struct Worker * pWIsapi;
	int iport;

	/*����������� ������, ��� ������������� ������ �������� isapi*/
	CRITICAL_SECTION cs;
};


struct Server * init_Server();
int start_server(struct Server*);
BOOL start_async(void*, DWORD, LPVOID, struct Overlapped_inf*);

#endif