#include "server.h"
#include "socket.h"
#include "client.h" 
#include "worker.h" 
#include "error.h" 


BOOL start_async(void *pp, DWORD len, LPVOID iocp, struct overlapped_inf *pOverlapped_inf) {
	BOOL isOk = TRUE; 

	//EnterCriticalSection(pOverlapped_inf->pcs);
	/*���� ����������� �������� ���������� �����������, �� ����� ��������� ���������*/
	while(TRUE) {
		if(pOverlapped_inf->type == READ_WORKER) {
			/*������� ������ STDOUT worker*/
			struct Worker * pwrk = pp;
			pwrk->fd[1].len = pwrk->fd[1].cur += len;			
			if(pwrk->fd[1].len + LEN >= pwrk->fd[1].size) {
				/*����������, ���� ��������� ���*/
				pwrk->fd[1].size *= 2;
				pwrk->fd[1].data = realloc(pwrk->fd[1].data, pwrk->fd[1].size);
			}			
			/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pwrk)*/
			if(!ReadFile(pwrk->fd[1].fd_r, pwrk->fd[1].data + pwrk->fd[1].len, LEN, &len, (OVERLAPPED*)pOverlapped_inf) && ERROR_IO_PENDING != GetLastError()) {			
				/*worker �������� ������*/
				release_Worker(pwrk);				
			} 
			/*����������� �������� ��������� � �������, ������*/
			break;			
		} else if(pOverlapped_inf->type == READ_WORKER_ERR) {
			/*������� ������ STDERR worker*/
			struct Worker * pwrk = pp;
			pwrk->fd[2].len = pwrk->fd[2].cur += len;
			if(pwrk->fd[2].len + LEN >= pwrk->fd[2].size) {
				/*����������, ���� ��������� ���*/
				pwrk->fd[2].size *= 2;
				pwrk->fd[2].data = realloc(pwrk->fd[2].data, pwrk->fd[2].size);
			}
			/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pwrk)*/
			if(!ReadFile(pwrk->fd[2].fd_r, pwrk->fd[2].data + pwrk->fd[2].len, LEN, &len, (OVERLAPPED*)pOverlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
				show_err("������ ReadFile [STDERR]", TRUE);				
			} 
			/*����������� �������� ��������� � �������, ������*/
			break;
		}else if(pOverlapped_inf->type==WRITE_WORKER) {
			/*������� ������ STDID worker*/
			struct Worker * pwrk = pp;
			pwrk->fd[0].cur += len;
			len = 0;
			if(pwrk->fd[0].cur < pwrk->fd[0].len) {
				/*��� �� ��� �������� worker (����������� �������� ������ STDOUT � STDERR �������� �����)*/
				DWORD len_ = LEN <= pwrk->fd[0].len - pwrk->fd[0].cur ? LEN : pwrk->fd[0].len - pwrk->fd[0].cur;
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pwrk)*/
				if(!WriteFile(pwrk->fd[0].fd_w, pwrk->fd[0].data + pwrk->fd[0].cur, len_, &len, (OVERLAPPED*)pOverlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
					/*worker �� ���� ���������� ��� ������*/
					show_err("������ WriteFile [STDIN]", TRUE);					
				} 				
			} 				
			/*����������� �������� ��������� � �������, ������*/
			break;
		} else if(pOverlapped_inf->type == READ) {
			/*������� ������ ������ �������*/
			struct Client * pcln = pp;
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;
			pcln->len = pcln->cur += len;			
			/*�������� ����� ���������*/
			if(pcln->len == 3 && !strncmp(pcln->data, "OFF", 3)) {
				printf("�������� ������� ��������������� ������\n");
				return FALSE;
			} 
			char * begin = strchr(pcln->data + pcln->cur - len - (pcln->cur - len >=3 ? 3 : 0), '\r');
			if(begin!=NULL && pcln->len >= 4 && !strncmp(begin, "\r\n\r\n", 4)) {
				/*��������� ��������, ������ worker � ����������� ��������*/
				work(pcln, iocp); //����������� �����								
			} else {				
				pcln->DataBuf.len = LEN;
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("������ WSARecv");
					release_Client(pcln);
				}				
			}
			/*����������� �������� ��������� � �������, ������*/
			break;
		} else if(pOverlapped_inf->type == WRITE) {
			/*������� ������ � ����� �������*/
			struct Client * pcln = pp;
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;
			pcln->cur += len;			
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;				
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
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
	//LeaveCriticalSection(pOverlapped_inf->pcs);

	return TRUE;
}

void WorkingThread(LPVOID iocp) {
	/*��� ������� �������� � �������*/
	while(TRUE) {
		char * type = NULL;
		struct overlapped_inf *pOverlapped_inf = NULL;
		DWORD len = 0;
		if(!GetQueuedCompletionStatus(iocp,              /*���������� ����� ����������*/
			                          &len,              /*���������� ������������ ����*/
									  (ULONG_PTR*)&type, /*��� ���������� �����*/
									  (OVERLAPPED**)&pOverlapped_inf,      /*��������� ���� ����������� ��������*/
			                          INFINITE           /*������������ �������� ���������� ����������� ����������� ��������*/)) {
			if(type != NULL 
			   && *type==WORKER 
			   && pOverlapped_inf->type==READ_WORKER) {
				/*������ �� worker*/
				release_Worker((struct Worker*)type);
			} else if(type == NULL){
				show_err_wsa("������ � �������� ���������� ����������� ��������");
				break;
			}			
		} else if(type == NULL)
			break; //�������������� ��������� ����� PostQueuedCompletionStatus
		else if(len == 0 && pOverlapped_inf->type < WAIT) {
			/*������ ����������*/
			release_Client((struct Client*)type);
		} else if(!start_async(type, len, iocp, pOverlapped_inf)) {
			struct Server * psrv=NULL;
			if(*type == CLIENT)
				psrv = ((struct Client*)type)->psrv;
			else if(*type == WORKER)
				psrv = ((struct Worker*)type)->pcln->psrv;
			/*�������� ������� �������������� ��������� � ���������� ���������*/
			if(psrv!=NULL)
				WSASetEvent(*psrv->phEvent_STOP);			
		}
	}
	printf("����� ����������\n");
}

int loop(struct Server * psrv) {
	BOOL is_repeat = TRUE;	

	/*������ ���������� ���� ����������*/
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);

	/*������� ���� (�������) � ����, ��� ������������ ����������� ��������*/
	psrv->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*���������� �����*/
										 NULL,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
										 (ULONG_PTR)0,                      /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
										 0                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
	if(psrv->iocp == NULL) {
		show_err("�� ������� ������� ���� ���������� � ���� OS",TRUE);
		is_repeat = FALSE;
	}

	/*������� ������, ��� ��������� ������� ������� �� ���������� ����*/
	HANDLE *phWorking = malloc(sizeof(HANDLE)*sys_inf.dwNumberOfProcessors);
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(is_repeat)
			phWorking[i] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&WorkingThread, psrv->iocp, 0, 0);
		else
			phWorking[i] = INVALID_HANDLE_VALUE;

	/*������� � ���� �������, ��� ������������ ����������� �������� �� ������-������*/
	for(int i = 0; i < MAX_EVENTS; ++i)
		psrv->hEvents[i] = WSACreateEvent();
	/*������� �������������� ���������*/
	psrv->phEvent_STOP = &psrv->hEvents[1];

	while(is_repeat) {
		/*������� ������� ��� master socket*/
		int rc = WSAEventSelect(psrv->msfd, psrv->hEvents[0], FD_ACCEPT | FD_CLOSE);
		if(rc == SOCKET_ERROR) {
			show_err_wsa("�� ������� ���������������� ������� select �� ������-������");
			break;
		}	

		/*��� ����������� ������� (!!!�� ����� 64 ������ WSAEVENT �������!!!)*/
		rc = WSAWaitForMultipleEvents(MAX_EVENTS, psrv->hEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("������ �������� ����������� ������� WSAWaitForMultipleEvents");
			break;
		} else if(rc == 1) {
			/*��������� ������� �������������� ���������*/
			break;
		}
		
		WSANETWORKEVENTS hEvent;
		/*�������� ��������� �� ������� �� ������-������*/		
		rc = WSAEnumNetworkEvents(psrv->msfd, psrv->hEvents[0], &hEvent);
		if(hEvent.lNetworkEvents & FD_ACCEPT &&	hEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
			/*������������ � �������� �������� � ����� �����������*/
			struct Client * pcln = init_Client(psrv);			
			size_t len = sizeof(struct sockaddr_in);
			/*��������� ����� ���������� (������ ����� ������� �� �������)*/			
			int sfd_slave = WSAAccept(psrv->msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);			
			if(sfd_slave != -1) {
				pcln->sfd = sfd_slave; //!!!����� ������ ������ �� �����������!!!!						

				/*��������� socket � ������ (����� ����������� �������� � ���� ������� ����� ������������ ��������� ����)*/
				pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*���������� ������*/
													psrv->iocp,                        /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
													(ULONG_PTR)pcln,                   /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
													1                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
				if(pcln->iocp == NULL) {
					show_err("�� ������� ��������� slave socket � ����� ���������� � ���� OS", TRUE);
					release_Client(pcln);
				} else {
					/*��������� ����������� ��������*/
					DWORD len = 0;
					start_async(pcln, len, psrv->iocp, &pcln->overlapped_inf);
				}
			} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
				show_err_wsa("�� ������� ������� ����������");
				release_Client(pcln);
			}
		} else if(hEvent.lNetworkEvents & FD_CLOSE &&	hEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
			/*������ ������ �����*/
			is_repeat=FALSE;
		}		
	}

	/*��������� � ���� ������� ������������� ����������� �������� �� ������-������*/
	for(int i = 0; i < MAX_EVENTS;++i)
		CloseHandle(psrv->hEvents[i]);

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
	CloseHandle(psrv->iocp);	

	/*������� �������� ��������� � ��������� ���������(!!!���!!!)*/

	return 0;
}

int start_server(struct Server* psrv) {
	/*�������� ���������� ������ ������*/
	psrv->msfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //���� WSA_FLAG_OVERLAPPED ����� ��� �������� ������� ����������� �������� (���� ����� ���������� "��������" � ������ - ����� ������ SOCKET_ERROR � WSAGetLastError() ������� � ������� �������������� �������� - WSA_IO_PENDING)
	if(psrv->msfd == -1) {
		show_err_wsa("�� ������� ���������� ������");
		return 1;
	}

	/*��������� ����� � ������� ������� (������� ����� ������������)*/	
	psrv->addr.sin_port = htons(psrv->port);
	psrv->addr.sin_family = AF_INET;
	char * ip_str = "0.0.0.0";
	inet_ntop(psrv->addr.sin_family, &psrv->addr, ip_str, strlen(ip_str) + 1);
	int res = bind(psrv->msfd, (struct sockaddr*)&psrv->addr, sizeof(psrv->addr));
	if(res == -1) {
		show_err_wsa("������ ���������� ������ ������ � ������� �������");
		closesocket(psrv->msfd);
		return 2;
	}

	/*�������� ��������� �������������*/
	res = set_repitable(psrv->msfd);
	if(res != 0) {
		show_err_wsa("�� ������� ���������� ����� ���������� ������������� ������ ������");
		closesocket(psrv->msfd);
		return 2;
	}

	/*�������� ������� ������� �����*/
	res = listen(psrv->msfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("�� ������� ������ ������������� �������� ������");
		closesocket(psrv->msfd);
		return 2;
	}

	/*���� �������������������*/
	loop(psrv);

	/*��������� ���������� ������ ������*/
	shutdown(psrv->msfd, SD_BOTH);
	closesocket(psrv->msfd);
	
	return 0;
}

struct Server * init_Server() {
	struct Server *psrv = malloc(sizeof(struct Server));
	memset(psrv, 0, sizeof(struct Server));		
	psrv->type = SERVER;

	return psrv;
}