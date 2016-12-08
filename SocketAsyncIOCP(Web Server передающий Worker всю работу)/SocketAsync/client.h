#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

/*��������� ������*/
#define READ            0
#define WRITE           1
#define WAIT            2
#define READ_WORKER     3
#define WRITE_WORKER    4
#define READ_WORKER_ERR 5

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
	/*������� ����*/
	char type;

	/*���������� ������*/
	int sfd;
	
	DWORD size; /*max ����� ������    */
	DWORD len;  /*����� ����� ������  */
	DWORD cur;  /*������� ��� ��������*/
	char *data; /*������������/���������� ������*/
	
	/*����� ������*/
	WSABUF DataBuf;
	
	/*������������ ������*/
	struct Req * preq;
	
	/*���������, ��� ���������� ����������� �������*/
	struct overlapped_inf {
		WSAOVERLAPPED overlapped;
		char type;		
		CRITICAL_SECTION * pcs; /*����������� ������ ����� ������� (��� ���������� ��������)*/
	} overlapped_inf;
		
	

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