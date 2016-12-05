#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

/*��������� ������*/
#define READ     0
#define WRITE    1
#define WAIT     2

/*������������ ������ ��������� ���������*/
#define MAX_HEAD_HTTP 4096

/*������������ ������ ������ ������ ����������� ��������*/
#define LEN 10

/**/
extern int flag;

struct Req;

struct Worker;
struct Server;


/*������ �������� � �� ��������*/
struct Client {
	/*����� ������ ��� ������ � �����*/
	char type;
	/*���������� ������*/
	int sfd;
	
	DWORD len;  /*����� ����� ������*/
	DWORD cur;  /*������� ��� ��������*/
	char * data;/*������������/���������� ������*/

	/*������������ ������*/
	struct Req * preq;
	
	/*���������, ��� ���������� ����������� �������*/
	WSAOVERLAPPED overlapped;
		
	/*����� ������*/
	WSABUF DataBuf;

	/*���� �������*/
	HANDLE iocp;
	
	/*������� �����*/
	struct sockaddr_in addr;

	/*����������� ������ ����� ������� (��� ���������� ��������)*/
	CRITICAL_SECTION cs;

	/*����������� worker*/
	struct Worker * pwrk;

	/*������, ��������� � ��������*/
	struct Server * psrv;
};

/*������������� �������� � ����� �������*/
struct Client * init_Client(struct Server *);

/*����� �������� � ������� � �������� ���������*/
struct Client * clear_Client(struct Client *);

/*������������ �������� ���������� ���������� � �������*/
struct Client * release_Client(struct Client *);

/*��������� ����� 200*/
BOOL make200(struct Client*);
/*��������� ����� 404*/
BOOL make404(struct Client*);
/*��������� ����� 500*/
BOOL make500(struct Client*);

#endif