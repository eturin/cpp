#include "server.h"
#include "socket.h"
#include "client.h" 
#include "worker.h" 
#include "error.h" 

/*������� �������, � ������� ����� �������� ������*/
char work_path[256];

BOOL start_async(struct Client * pcln, DWORD len, LPVOID iocp) {
	BOOL isOk = TRUE;

	EnterCriticalSection(&pcln->cs);
	/*���� ����������� �������� ���������� �����������, �� ����� ��������� ���������*/
	while(TRUE) {
		/*�������� ����� �� cln->data �� ���������� ���������� ����*/
		pcln->DataBuf.buf += len;
		pcln->cur += len;		
		len = 0;

		if(pcln->type == WAIT && pcln->pwrk->type == READ) {			
			pcln->len = pcln->cur;
			pcln->DataBuf.len = LEN;
			if(!ReadFile(pcln->pwrk->fd[1].fd_r, pcln->DataBuf.buf, pcln->DataBuf.len, &len, &pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {
				show_err("������ ReadFile",TRUE);
				release_Worker(pcln->pwrk);
				len = 0;    //��� ����������� �������� ������
			}else
				/*����������� �������� ��������� � �������, ������*/
				break;
		}if(pcln->type == WAIT && pcln->pwrk->type == WRITE) {
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;
				if(!WriteFile(pcln->pwrk->fd[0].fd_w, pcln->DataBuf.buf, pcln->DataBuf.len, &len, &pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {
					show_err("������ ReadFile", TRUE);
					release_Worker(pcln->pwrk);
					len = 0;    //��� ����������� �������� ������
				} else
					/*����������� �������� ��������� � �������, ������*/
					break;
			} else {
				/*��� �������� worker, ���� ������ �� ����*/
				clear_Client(pcln);
				pcln->type = WAIT;  
				pcln->pwrk->type = READ;
				len = 0;    //��� ����������� �������� ������
			}
		}

		if(pcln->type == READ) {
			pcln->len = pcln->cur;
			/*�������� ����� ���������*/
			if(pcln->len == 3 && !strncmp(pcln->data, "OFF", 3)) {
				printf("�������� ������� ��������������� ������\n");
				return FALSE;
			} else if(pcln->len >= 4 && !strncmp(pcln->data + pcln->len - 4, "\r\n\r\n", 4)) {
				/*��������� ��������*/
				work(pcln, iocp); //����������� �����
				len = 0;          //��� ����������� �������� ������
			} else {
				len = 0;
				pcln->DataBuf.len = LEN;
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, &pcln->overlapped, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("������ WSARecv");
					release_Client(pcln);
				}
				/*����������� �������� ��������� � �������, ������*/
				break;
			}
		}

		if(pcln->type == WRITE) {			
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;				
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, &pcln->overlapped, NULL);
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
		struct Client * pcln = NULL;
		OVERLAPPED * pOverlapped = NULL;
		DWORD len = 0;
		if(!GetQueuedCompletionStatus(iocp,              /*���������� ����� ����������*/
			                          &len,              /*���������� ������������ ����*/
			                          (ULONG_PTR*)&pcln, /*��� ���������� �����*/
			                          &pOverlapped,      /*��������� ���� ����������� ��������*/
			                          INFINITE           /*������������ �������� ���������� ����������� ����������� ��������*/)) {
			if(pcln != NULL) {
				/*������ �� worker*/
				release_Worker(pcln->pwrk);
				start_async(pcln, len, iocp);
			} else {
				show_err_wsa("������ � �������� ���������� ����������� ��������");
				break;
			}			
		} else if(pcln == NULL)
			break; //�������������� ��������� ����� PostQueuedCompletionStatus
		else if(len == 0 && pcln->type!=WAIT) {
			/*������ ����������*/
			release_Client(pcln);
		} else if(!start_async(pcln, len, iocp)) {//��������� ����������� ��������
			/*������ ���������� ���� ����������*/
			SYSTEM_INFO sys_inf;
			GetSystemInfo(&sys_inf);
			for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors - 1; ++i)
				PostQueuedCompletionStatus(iocp, 0, 0, 0);//��� ������� ����������� ����������� ������ ������� 
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
	HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*���������� �����*/
										 NULL,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
										 (ULONG_PTR)0,                      /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
										 0                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
	if(iocp == NULL) {
		show_err("�� ������� ������� ���� ���������� � ���� OS",TRUE);
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
		/*������������ � �������� �������� � ����� �����������*/
		struct Client * pcln = init_Client();
		
		/*��������� ����� ���������� (������ ����� ������� �� �������)*/
		size_t len = sizeof(struct sockaddr_in);
		int sfd_slave = WSAAccept(msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
		if(sfd_slave != -1) {
			pcln->sfd = sfd_slave; //!!!����� ������ ������ �� �����������!!!!						
			
			/*��������� socket � ������ (����� ����������� �������� � ���� ������� ����� ������������ ��������� ����)*/
			pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*���������� ������*/
												iocp,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
												(ULONG_PTR)pcln,                   /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
												1                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
			if(pcln->iocp == NULL) {
				show_err("�� ������� ��������� slave socket � ����� ���������� � ���� OS", TRUE);
				release_Client(pcln);
			} else {
				/*��������� ����������� ��������*/
				DWORD len = 0;
				start_async(pcln, len, iocp);
			}
		} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
			show_err_wsa("�� ������� ������� ����������");
			release_Client(pcln);
		}
	}

	/*��������� ���������� � ����*/
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(phWorking[i] != INVALID_HANDLE_VALUE) {
			printf("�������� ����� %d...", phWorking[i]);
			if(TerminateThread(phWorking[i], NO_ERROR))
				printf("ok\n");
			else
				show_err("������ ��������� ������", TRUE);
			CloseHandle(phWorking[i]);
		}
	free(phWorking);
	CloseHandle(iocp);	

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

	
	return 0;
}