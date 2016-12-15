#ifndef WORKER_H
#define WORKER_H

#include "common.h"

/*������� �������*/
extern char work_path[];

/*������������ ������ ����� worker*/
#define LEN_WORKER 50

struct Client;

/*�������� ������������*/
struct Worker {
	/*������� ����*/
	char type;

	/*��� �������*/
	char name[LEN_WORKER];
	/*������������� ���� �� ���������*/
	char abs_path[256];
	/*������������� ���� �� ���������*/
	char path[256];

	/*����������� ������� STDIN, STDOUT � STDERR*/
	struct FD {
		HANDLE fd_r, fd_w;  /*����������� �������*/
		HANDLE iocp;        /*���� ������ worker*/
		DWORD size;         /*max ����� ������    */
		DWORD len;          /*����� ����� ������  */
		DWORD cur;          /*������� ��� ��������*/
		char * data;        /*������������/���������� ������*/
		
		/*���������, ��� ���������� ����������� �������*/
		struct overlapped_inf_w {
			WSAOVERLAPPED overlapped;
			char type;
			CRITICAL_SECTION * pcs; /*����������� ������ ����� ������� (��� ���������� ��������)*/
		} overlapped_inf;
	} fd[3];

	/*�������� � ��������*/
	PROCESS_INFORMATION procInf;

	/*��������� ��������*/
	STARTUPINFO sti;

	/*������� ������*/
	struct Client *pcln;		
};

/*������������� worker*/
struct Worker * init_Worker(const char *, size_t, struct Client *, LPVOID);

/*������������ �������� worker*/
struct Worker * release_Worker(struct Worker *);

BOOL work(struct Client *, LPVOID);

#endif