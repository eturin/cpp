#ifndef CLIENT_ISAPI_H
#define CLIENT_ISAPI_H

#include "common_isapi.h"

extern int flag;

struct hTab; /*���������� � req.h*/
struct Req;
struct Server_isapi;
struct Worker_isapi;


/*������ �������� � �� ��������*/
struct Client_isapi {	
	/*����������� ������*/
	int msfd, sfd;
	/*�������� � ������*/
	WSAPROTOCOL_INFOW inf;

	size_t size; /*max ����� ������    */
	size_t len;  /*����� ����� ������  */
	size_t cur;  /*������� ����� ��� ���������� ��������*/
	char *data;  /*������������/���������� ������*/

	/*������������ ������*/
	struct Req * preq;
	
	/*������� �����*/
	struct sockaddr_in addr;
	
	/*���������� �����*/
	struct hTab *pEnv;

	/*������, ��������� � ��������*/
	struct Server_isapi * psrv;

	/*���������� ����������, �������������� ���� ������*/
	struct Isapi *pIsapi;
};

/*������������� �������� � ����� �������*/
struct Client_isapi * init_client_isapi(struct Server_isapi *);

/*������������ �������� ���������� ���������� � �������*/
struct Client_isapi * release_client_isapi(struct Client_isapi *);

/*��������� ����� 200*/
BOOL make200(struct Client_isapi*);
/*��������� ����� 404*/
BOOL make404(struct Client_isapi*);
/*��������� ����� 500*/
BOOL make500(struct Client_isapi*);

#endif