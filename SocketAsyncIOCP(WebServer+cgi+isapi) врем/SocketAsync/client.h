#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

extern int flag;

struct Req;
struct Worker;
struct Server;

/*��������� ������������ ��� ����������� ��������*/
struct Overlapped_inf {
	WSAOVERLAPPED overlapped;
	char type;
};

/*������ �������� � �� ��������*/
struct Client {
	/*������� ����*/
	char type;

	/*����������� �������*/
	int sfd, wsfd;
	/*������� ������ �� �������������� ������*/
	BOOL crypt;
	/*��������� �� ��������� �������������� ��������*/
	void * ext;
	
	DWORD size; /*max ����� ������    */
	DWORD len;  /*����� ����� ������  */
	DWORD cur;  /*������� ��� ��������*/
	char *data; /*������������/���������� ������*/
	
	/*����� ������*/
	WSABUF DataBuf;
	
	/*������������ ������*/
	struct Req * preq;
	
	/*���������, ��� ���������� ����������� �������*/
	struct Overlapped_inf overlapped_inf;	

	/*���� �������*/
	HANDLE iocp;
	
	/*������� �����*/
	struct sockaddr_in addr;		

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