#ifndef SERVER_ISAPI_H
#define SERVER_ISAPI_H

#include "common_isapi.h"

/*���������� ������� ��������� � ������� ������*/
#define MAX_EVENTS 2

struct hTab;

struct Server_isapi {
	/*��� �������*/
	char *name;

	/*������-����� �������*/
	int msfd;
	/*�������������� ����*/
	int port;
	/*�������������� ������� �����*/
	struct sockaddr_in addr;	

	/*��������� ����������� ����������*/
	struct hTab * hISAPI;

	/*������� �������, � ������� ����� �������� ������ (�� ���������� �����)*/
	const char *work_path;

	/*������� ������� (��������� � ����������� � loop)*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*������� �������������� ��������� �������� � hEvents (������������ �� �������� ������� �������� ������ �� ������� OFF)*/
	WSAEVENT *phEvent_STOP;
};

struct Server_isapi * init_server_isapi();
struct Server_isapi * release_server_isapi(struct Server_isapi *);
BOOL start_server_isapi(struct Server_isapi * psrv);

#endif