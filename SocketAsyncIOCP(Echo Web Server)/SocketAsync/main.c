#define _CRT_SECURE_NO_WARNINGS
#define WINVER 0x0602 /*API Windows 7*/

#define _GNU_SOURCE
#pragma comment(lib, "ws2_32.lib") 
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

/*��������� ������*/
#define READ     0
#define WRITE    1
#define WAIT     2
/*������������ ������ ��������� ���������*/
#define MAX_HEAD_HTTP 4096

#define LEN 10

/*������ �������� � �� ��������*/
int flag = 0;
int cnt = 0;
struct Client {
	/*����������� ������ ����� �������*/
	CRITICAL_SECTION cs;
	/*����� ������ ��� ������ � �����*/
	char type;
	/*���������� ������*/
	int sfd;
	
	/*����� ����� ������*/
	DWORD len;
	/*������� ��� ��������*/
	DWORD cur;	
	/*������*/
	char * data;

	/*���������, ��� ���������� ����������� �������*/
	struct my_overlapped {
		WSAOVERLAPPED overlapped;
		struct Client * cln;
	} my_overlapped;
	/*����� ������*/
	WSABUF DataBuf;	

	/*���� �������*/
	HANDLE iocp;
	/*������� �����*/
	struct sockaddr_in addr;
};
struct Client * init_Client();
struct Client * release_Client(struct Client *);
struct Client * clear_Client(struct Client *);

int show_err_wsa(const char * msg) {
	int      no = WSAGetLastError();
	char     str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s: %d\n%s\n", msg, no, str_err);
	else
		printf("%s:\n����� ������ %d\n", msg, no);

	return no;
}
int show_err(const char * msg) {
	int      no = GetLastError();
	char     str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s: %d\n%s\n", msg, no, str_err);
	else
		printf("%s:\n����� ������ %d\n", msg, no);	

	return no;
}
int set_repitable(int sfd) {
	int optval = 1;
	return setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
}
void close_socket(int fd) {
	shutdown(fd, SD_BOTH);
	closesocket(fd);
}

struct Client * init_Client() {
	++cnt;
	struct Client * new_Client = (struct Client*)malloc(sizeof(struct Client));
	memset(new_Client, 0, sizeof(struct Client));

	new_Client->sfd = 0;
	new_Client->type = READ;	
	
	new_Client->len = new_Client->cur = 0;
	new_Client->data = malloc(MAX_HEAD_HTTP);
	new_Client->DataBuf.len = LEN;
	new_Client->DataBuf.buf = new_Client->data;
	
	/*��������� ��������� �������*/
	new_Client->my_overlapped.overlapped.hEvent = CreateEvent(NULL, /*������� ������*/
				                                              TRUE, /*��� ������ TRUE - ������*/
				                                              TRUE, /*��������� ��������� TRUE - ����������*/
				                                              NULL  /*��� �������*/);
	new_Client->my_overlapped.cln = new_Client;

	/*������������� ����������� ������*/
	InitializeCriticalSection(&new_Client->cs);	

	return new_Client;
}
struct Client * release_Client(struct Client * pcln) {
	/*��� �������� ������� ��������� �� ������������*/	
	if(pcln != NULL && TryEnterCriticalSection(&pcln->cs)) {		
		/*��������� ����� ������� � ����*/
		WSACloseEvent(pcln->my_overlapped.overlapped.hEvent);
		/*��������� ���� �������*/
		//if(pcln->iocp) CloseHandle(pcln->iocp);

		/*����������� ������*/
		free(pcln->data);
		/*��������� �����*/
		close_socket(pcln->sfd);		
		LeaveCriticalSection(&pcln->cs);
		DeleteCriticalSection(&pcln->cs);
		free(pcln);	
	}	

	return NULL;
}
struct Client * clear_Client(struct Client * pcln) {		
	/*��� �������� ������� ��������� �� ������������*/
	if(TryEnterCriticalSection(&pcln->cs)) {
		if(pcln != NULL) {
			pcln->type = READ;
			/*���������������� ������ ��������*/
			free(pcln->data);
			pcln->len = pcln->cur = 0;
			pcln->data = malloc(MAX_HEAD_HTTP);
			memset(pcln->data, 0, MAX_HEAD_HTTP);
			pcln->DataBuf.len = LEN;
			pcln->DataBuf.buf = pcln->data;
		}
		LeaveCriticalSection(&pcln->cs);
	}

	return pcln;
}

char * format200 = "HTTP / 1.1 200 OK\r\nVersion: HTTP / 1.1\r\n"
		           "Content-Type: text/html; charset=utf-8\r\n"
				   "Content-Length: %d\r\n\r\n%s";

BOOL work(struct Client * pcln) {
	BOOL isOk = TRUE;
	pcln->type = WAIT;

	size_t len = pcln->len + strlen(format200)*sizeof(char)+15; //15 ���� �� ������� %d
	char * response = malloc(len);	
	pcln->data[pcln->len] = '\0';
	sprintf(response, format200, pcln->len, pcln->data);
	free(pcln->data);
	pcln->data = response;
	pcln->len = strlen(response)*sizeof(char);

	/*��� �� ���������� ������*/
	pcln->type = WRITE;
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;		

	return isOk;
}

BOOL start_async(struct Client * pcln, DWORD len) {
	BOOL isOk = TRUE;

	EnterCriticalSection(&pcln->cs);	
	/*���� ����������� �������� ���������� �����������, �� ����� ��������� ���������*/
	while(TRUE) {
		if(pcln->type == READ) {
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/			
			pcln->DataBuf.buf += len;
			pcln->cur = pcln->len += len;

			/*�������� ����� ���������*/
			if(pcln->len == 3 && !strncmp(pcln->data, "OFF", 3)) {
				printf("�������� ������� ��������������� ������\n");
				return FALSE;
			} else if(pcln->len >= 4 && !strncmp(pcln->data + pcln->len - 4, "\r\n\r\n", 4)) {
				/*��������� ��������*/
				work(pcln); //����������� �����
				len = 0;    //��� ����������� �������� ������
			} else {
				len = 0;
				pcln->DataBuf.len = LEN;
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (OVERLAPPED*)&pcln->my_overlapped, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("������ WSARecv");
					release_Client(pcln);
				} 
				/*����������� �������� ��������� � �������, ������*/
				break;
			}
		}

		if(pcln->type == WRITE) {
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;
			pcln->cur += len;
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN: pcln->len - pcln->cur;
				len = 0;
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, (OVERLAPPED*)&pcln->my_overlapped, NULL);
				if(SOCKET_ERROR == rc  &&  WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("������ WSASend");
					release_Client(pcln);					
				} 
				/*����������� �������� ��������� � �������, ������*/
				break;
			} else {
				//��� ����������
				clear_Client(pcln);
				len = 0; //��� ����������� �������� ������
			}
		}
	}	
	LeaveCriticalSection(&pcln->cs);

	return TRUE;
}

void WorkingThread(LPVOID iocp) {
	/*��� ������� �������� � �������*/
	while(TRUE) {
		ULONG_PTR key = 0;
		struct my_overlapped * pmy_overlapped;		
		DWORD len=0;		
		if(!GetQueuedCompletionStatus(iocp,              /*���������� ����� ����������*/
			                          &len,              /*���������� ������������ ����*/
									  (ULONG_PTR*)&key,  /*��� ���������� �����*/
									  (OVERLAPPED**)&pmy_overlapped,  /*��������� ���� ����������� ��������*/
			                          INFINITE           /*������������ �������� ���������� ����������� ����������� ��������*/)) {
			show_err_wsa("������ � �������� ���������� ����������� ��������");
			break;
		} else if(pmy_overlapped == NULL)
			break; //�������������� ��������� ����� PostQueuedCompletionStatus
		else if(len == 0) {
			/*������ ����������*/
			release_Client(pmy_overlapped->cln);
		} else if(!start_async(pmy_overlapped->cln, len)) {//��������� ����������� ��������
			/*������ ���������� ���� ����������*/
			SYSTEM_INFO sys_inf;
			GetSystemInfo(&sys_inf);
			for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors - 1;++i)
				PostQueuedCompletionStatus(iocp,0,0,0);//��� ������� ����������� ����������� ������ ������� 
			TerminateProcess(INVALID_HANDLE_VALUE, NO_ERROR); //��������� ����������� ������� �����
			break;
		}
	}
	printf("����� ����������\n");
}

int loop(int msfd) {
	BOOL is_repeat = TRUE;

	/*������ ���������� ���� ����������*/
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);

	/*������� ���� (�������) � ����, ��� ������������ ����������� ��������*/
	HANDLE iocp=CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*���������� �����*/
		                               NULL,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/ 
		                               (ULONG_PTR)0,                      /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
		                               0                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
	if(iocp == NULL) {
		show_err("�� ������� ������� ���� ���������� � ���� OS");
		is_repeat = FALSE;
	}
		
	/*������� ������, ��� ��������� ������� ������� �� ���������� ����*/
	HANDLE *phWorking = malloc(sizeof(HANDLE)*sys_inf.dwNumberOfProcessors);
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(is_repeat)
			phWorking[i] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&WorkingThread, iocp, 0, 0);
		else
			phWorking[i] = INVALID_HANDLE_VALUE;
	
	while(is_repeat) {
		/*��������� ����� ���������� (������ ����� ������� �� �������)*/
		struct Client * pcln = init_Client();		
		size_t len=sizeof(struct sockaddr_in);
		int sfd_slave = WSAAccept(msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
		if(sfd_slave != -1) {
			pcln->sfd = sfd_slave; //!!!����� ������ ������ �� �����������!!!!						
			/*��������� socket � ������ (����� ����������� �������� � ���� ������� ����� ������������ ��������� ����)*/
			pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*���������� �����*/
												iocp,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
												(ULONG_PTR)0,                      /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
												1                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/); 
			if(pcln->iocp == NULL) {
				show_err("�� ������� ��������� slave socket � ����� ���������� � ���� OS");
				release_Client(pcln);
			} else {
				/*��������� ����������� ��������*/
				DWORD len = 0;
				start_async(pcln, len);
			}
		} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
			show_err_wsa("�� ������� ������� ����������");
			release_Client(pcln);
		}

	}
	
	/*��������� ���������� � ����*/
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i) 
		if(phWorking[i]!=INVALID_HANDLE_VALUE){
			printf("�������� ����� %d...", phWorking[i]);
			if(TerminateThread(phWorking[i], NO_ERROR))
				printf("ok\n");
			else
				show_err("������ ��������� ������");
			CloseHandle(phWorking[i]);
		}
	free(phWorking);
	//CloseHandle(iocp);	
	/*������� �������� ��������� � ��������� ���������(!!!���!!!)*/

	return 0;
}


int start_server() {
	/*�������������*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) {
		show_err_wsa("������ ������������� �����");
		return -1;
	}

	/*�������� ���������� ������ ������*/
	int msfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //���� WSA_FLAG_OVERLAPPED ����� ��� �������� ������� ����������� �������� (���� ����� ���������� "��������" � ������ - ����� ������ SOCKET_ERROR � WSAGetLastError() ������� � ������� �������������� �������� - WSA_IO_PENDING)
	if(msfd == -1) {
		show_err_wsa("�� ������� ���������� ������");
		return 1;
	}

	/*��������� ����� � ������� ������� (������� ����� ������������)*/
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12001);
	char * ip_str = "0.0.0.0";
	inet_ntop(addr.sin_family, &addr, ip_str, strlen(ip_str) + 1);
	int res = bind(msfd, (struct sockaddr*)&addr, sizeof(addr));
	if(res == -1) {
		show_err_wsa("������ ���������� ������ ������ � ������� �������");
		closesocket(msfd);
		return 2;
	}

	/*�������� ��������� �������������*/
	res = set_repitable(msfd);
	if(res != 0) {
		show_err_wsa("�� ������� ���������� ����� ���������� ������������� ������ ������");
		closesocket(msfd);
		return 2;
	}

	/*�������� ������� ������� �����*/
	res = listen(msfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("�� ������� ������ ������������� �������� ������");
		closesocket(msfd);
		return 2;
	}

	/*���� �������������������*/
	loop(msfd);

	/*��������� ���������� ������ ������*/
	shutdown(msfd, SD_BOTH);
	closesocket(msfd);
	WSACleanup();

	system("pause");
	return 0;
}

int main() {
	setlocale(LC_ALL, "russian");

	start_server();

	return 0;
}