#include "server.h"
#include "socket.h"
#include "client.h" 
#include "worker.h"
#include "req.h"
#include "error.h" 
#include "crypt.h"


BOOL start_async(void *pp, DWORD len, LPVOID iocp, struct Overlapped_inf *pOverlapped_inf) {
	BOOL isOk = TRUE; 
		
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
				show_err("(master)������ ReadFile [STDERR]", TRUE);				
			} 
			/*����������� �������� ��������� � �������, ������*/
			break;
		} else if(pOverlapped_inf->type == WRITE_WORKER) {
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
					show_err("(master)������ WriteFile [STDIN]", TRUE);
				}
			} else if(pwrk->pcln->preq != NULL && pwrk->pcln->preq->body_length > pwrk->pcln->preq->cur) {
				/*������������ �������� worker*/
				pOverlapped_inf->type = WAIT;
				free(pwrk->fd[0].data);
				pwrk->fd[0].cur = pwrk->fd[0].len = pwrk->fd[0].size = 0;
				pwrk->fd[0].data = NULL;

				/*�������� �� ������� ���� ���������*/
				pwrk->pcln->overlapped_inf.type = READ_TO_SEND_WORKER;
				pwrk->pcln->size = MAX_BODY_HTTP;
				pwrk->pcln->data = malloc(MAX_BODY_HTTP);				
				memset(pwrk->pcln->data, 0, pwrk->pcln->size);
				pwrk->pcln->DataBuf.buf = pwrk->pcln->data;
				pwrk->pcln->cur = pwrk->pcln->len = 0;
				start_async(pwrk->pcln, 0, pwrk->pcln->psrv->iocp, &pwrk->pcln->overlapped_inf);
			} else {
				/*����� ���������� worker (������� STDIN)*/
				CloseHandle(pwrk->fd[0].fd_w);
				pwrk->fd[0].fd_w = INVALID_HANDLE_VALUE;
			}
			/*����������� �������� ��������� � �������, ������*/
			break;
		} else if(pOverlapped_inf->type == READ_TO_SEND_WORKER) {
			/*������� ������ ������ �������*/
			struct Client * pcln = pp;
			pcln->len = pcln->cur += len;

			/*����������� ������ ��� ����������� ����� ���� ���������*/
			if(!pcln->crypt)
				pcln->preq->cur += len;			
			
			if(!pcln->crypt && (pcln->len >= pcln->size || pcln->preq->cur >= pcln->preq->body_length)
			   || pcln->crypt && crypt_check(pcln) /*���������� �������� ���������*/) {

				if(pcln->preq->cur >= pcln->preq->body_length) {
					/*������� ����� ������, ��� worker*/
					pcln->data[pcln->len] = '\0';
					++pcln->len;
				}			
				
				/*�������� ��������� �������� ���� ���������*/
				pOverlapped_inf->type = WAIT;
				pcln->pwrk->fd[0].data = pcln->data;
				pcln->data = NULL;
				pcln->DataBuf.buf = NULL;
				pcln->pwrk->fd[0].size = pcln->size;
				pcln->pwrk->fd[0].len  = pcln->len;
				pcln->pwrk->fd[0].cur  = 0;
				pcln->cur = pcln->len = pcln->size = 0;
				
				/*������� ��� � worker*/				
				pcln->pwrk->fd[0].overlapped_inf.type = WRITE_WORKER;
				start_async(pcln->pwrk, 0, pcln->psrv->iocp, &pcln->pwrk->fd[0].overlapped_inf);
			} else {
				if(pcln->len + LEN >= pcln->size) {
					/*����������, ���� ��������� ���*/
					pcln->size *= 2;
					pcln->data = realloc(pcln->data, pcln->size);
					/*�������� ����� �� cln->data �� ���������� ��� ���������� ����*/
					pcln->DataBuf.buf = pcln->data + pcln->len;
				} else
					/*�������� ����� �� cln->data �� ���������� ���������� ����*/
					pcln->DataBuf.buf += len;

				pcln->DataBuf.len = pcln->size - pcln->len;
				pcln->DataBuf.len = LEN < pcln->DataBuf.len ? LEN : pcln->DataBuf.len;
				pcln->DataBuf.len = pcln->DataBuf.len <= pcln->preq->body_length - pcln->preq->cur ? pcln->DataBuf.len : pcln->preq->body_length - pcln->preq->cur;
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("������ WSARecv");
					release_Client(pcln);
				}
			}
			/*����������� �������� ��������� � �������, ������*/
			break;
		} else if(pOverlapped_inf->type == READ) {
			/*������� ������ ������ �������*/
			struct Client * pcln = pp;			
			pcln->len = pcln->cur += len;
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;

			if(pcln->len == 3 && !pcln->crypt && !strncmp(pcln->data, "OFF", 3)) {
				show_err("(master)�������� ������� ��������������� ������\n",FALSE);
				return FALSE;
			} 

			if(!pcln->crypt) {/*�� ������������� �����*/
				/*�������� ����� ���������*/
				char * begin = strstr(pcln->data + pcln->cur - len - (pcln->cur - len > 3 ? 3 : pcln->cur - len), "\r\n\r\n");
				if(pcln->len >= pcln->size || begin != NULL) {
					/*��������� ��������, ������ worker � ����������� ��������*/
					work(pcln, iocp); //����������� �����								
					break;
				}
			} else if(crypt_check(pcln)) {/*������������� �����*/				
				break;
			}

			if(pcln->len + LEN >= pcln->size) {
				/*����������, ���� ��������� ���*/
				pcln->size *= 2;
				pcln->data = realloc(pcln->data, pcln->size);
				/*�������� ����� �� cln->data �� ���������� ��� ���������� ����*/
				pcln->DataBuf.buf = pcln->data + pcln->len;
			}
				

			pcln->DataBuf.len = pcln->size - pcln->len;
			pcln->DataBuf.len = LEN < pcln->DataBuf.len ? LEN : pcln->DataBuf.len;
			/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
			int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
			if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
				/*������ ��� �� ��������� ���������� � �� ����������� ������� ��������� ����������*/					
				release_Client(pcln);
			}				
			
			/*����������� �������� ��������� � �������, ������*/
			break;
		} else if(pOverlapped_inf->type == WRITE) {
			/*������� ������ � ����� �������*/
			struct Client * pcln = pp;
			pcln->cur += len;
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;

			/*��� �������������� ������ ��������� ����������� ���������*/
			if(pcln->crypt && crypt_check(pcln))
				break;			
			
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;				
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc  &&  WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("(master)������ WSASend");
					release_Client(pcln);
				}
				/*����������� �������� ��������� � �������, ������*/
				break;
			} else {
				//��� ����������
				clear_Client(pcln);
				len = 0; //��� ����������� �������� ������
			}
		} else if(pOverlapped_inf->type == WRITE_ISAPI) {
			/*������� ������ � ����� �������*/
			struct Client * pcln = pp;
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;
			pcln->cur += len;
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
				int rc = WSASend(pcln->wsfd, &pcln->DataBuf, 1, &len, 0, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc  &&  WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("(master)������ WSASend (�������� ������ isapi)");
					release_Client(pcln);
				}
				/*����������� �������� ��������� � �������, ������*/
				break;
			} else {
				//��� ����������				
				clear_Client(pcln);
				pOverlapped_inf->type = READ_ISAPI;
				len = 0; //��� ����������� �������� ������
			}
		} else if(pOverlapped_inf->type == READ_ISAPI) {
			/*������� ������ ������ �������*/
			struct Client * pcln = pp;
			/*�������� ����� �� cln->data �� ���������� ���������� ����*/
			pcln->DataBuf.buf += len;
			pcln->len = pcln->cur += len;
			/*�������� ��������� ��������� isapi*/
			if(pcln->len == 2 && !strncmp(pcln->data, "OK", 2)) {
				/*������� ����������*/
				pOverlapped_inf->type = READ;
				pcln->wsfd = close_socket(pcln->wsfd);
			} else if(pcln->len >= 2) {
				/*�������*/
				make500(pcln);
				pOverlapped_inf->type = WRITE;
				pcln->wsfd = close_socket(pcln->wsfd);
			} else {
				pcln->DataBuf.len = pcln->size - pcln->len;
				pcln->DataBuf.len = LEN < pcln->DataBuf.len ? LEN : pcln->DataBuf.len;
				/*������ ����������� �������� (� ������ ������, ������ ���������� � ������ �� ������ ��������� pcln)*/
				int rc = WSARecv(pcln->wsfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					/*worker isapi ��������*/
					make500(pcln);
					pOverlapped_inf->type = WRITE;
					pcln->wsfd = close_socket(pcln->wsfd);
				} else {
					/*����������� �������� ��������� � �������, ������*/
					break;
				}
			}			
		}
	}	

	return TRUE;
}

void WorkingThread(LPVOID iocp) {
	/*��� ������� �������� � �������*/
	while(TRUE) {
		char * type = NULL;
		struct Overlapped_inf *pOverlapped_inf = NULL;
		DWORD len = 0;
		if(!GetQueuedCompletionStatus(iocp,              /*���������� ����� ����������*/
			                          &len,              /*���������� ������������ ����*/
									  (ULONG_PTR*)&type, /*��� ���������� �����*/
									  (OVERLAPPED**)&pOverlapped_inf,      /*��������� ���� ����������� ��������*/
			                          INFINITE           /*������������ �������� ���������� ����������� ����������� ��������*/)) {
			if(type != NULL 
			   && (*type == WORKER)
			   && pOverlapped_inf->type==READ_WORKER) {
				/*������ �� worker*/
				release_Worker((struct Worker*)type);
			} else if(type == NULL){
				show_err_wsa("(master)������ �������� ���������� ����������� ��������");
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

	/*�������������� ����������� ������ (��������� ������ ���� ������� isapi)*/
	InitializeCriticalSection(&psrv->cs);

	/*������ ���������� ���� ����������*/
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);

	/*������� ���� (�������) � ����, ��� ������������ ����������� ��������*/
	psrv->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*���������� �����*/
										NULL,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
										(ULONG_PTR)0,                      /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
										0                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
	if(psrv->iocp == NULL) {
		show_err("(master)�� ������� ������� ���� ���������� � ���� OS",TRUE);
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
	psrv->phEvent_STOP = &psrv->hEvents[2];

	while(is_repeat) {
		/*������� ������� ��� ��������� ������*/
		int rc = WSAEventSelect(psrv->msfd, psrv->hEvents[0], FD_ACCEPT | FD_CLOSE);
		if(rc == SOCKET_ERROR) {
			show_err_wsa("(master)�� ������� ���������������� ������� ACCEPT � CLOSE ��� ��������� ������");
			break;
		}	
		/*������� ������� ��� ��������� ������, ���� �� ������������*/
		if(psrv->smsfd) {
			rc = WSAEventSelect(psrv->smsfd, psrv->hEvents[1], FD_ACCEPT | FD_CLOSE);
			if(rc == SOCKET_ERROR) {
				show_err_wsa("(master)�� ������� ���������������� ������� ACCEPT � CLOSE ��� ��������� ������");
				break;
			}
		}

		/*��� ����������� ������� (!!!�� ����� 64 ������ WSAEVENT �������!!!)*/
		rc = WSAWaitForMultipleEvents(MAX_EVENTS, psrv->hEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("(master)������ �������� ����������� ������� WSAWaitForMultipleEvents");
			break;
		} else if(rc == 2) {
			/*��������� ������� �������������� ���������*/
			break;
		}
		
		WSANETWORKEVENTS hEvent;
		/*�������� ��������� �� ������� �� ������-������� (0 - �������� � 1 - ��������)*/
		for(int i = 0; i < 2; ++i) {
			if(i) {
				if(psrv->smsfd)
					rc = WSAEnumNetworkEvents(psrv->smsfd, psrv->hEvents[1], &hEvent);
				else
					continue; /*�������� ����� �� ������������*/
			} else
				rc = WSAEnumNetworkEvents(psrv->msfd, psrv->hEvents[0], &hEvent);

			if(hEvent.lNetworkEvents & FD_ACCEPT &&	hEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
				/*������������ � �������� �������� � ����� �����������*/
				struct Client * pcln = init_Client(psrv);
				pcln->crypt = i;
				size_t len = sizeof(struct sockaddr_in);
				/*��������� ����� ���������� (������ ����� ������� �� �������)*/
				int sfd_slave = WSAAccept(i ? psrv->smsfd : psrv->msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
				if(sfd_slave != -1) {
					pcln->sfd = sfd_slave; //!!!����� ������ ������ �� �����������!!!!						

					/*��������� socket � ������ (����� ����������� �������� � ���� ������� ����� ������������ ��������� ����)*/
					pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*���������� ������*/
														psrv->iocp,                        /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
														(ULONG_PTR)pcln,                   /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
														1                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
					if(pcln->iocp == NULL) {
						show_err("(master)�� ������� ��������� slave socket � ����� ���������� � ���� OS", TRUE);
						release_Client(pcln);
					} else {
						/*��������� ����������� ��������*/
						DWORD len = 0;
						printf("[%x] ������� ����� ����������\n", pcln);
						start_async(pcln, len, psrv->iocp, &pcln->overlapped_inf);
					}
				} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
					show_err_wsa("(master)�� ������� ������� ����������");
					release_Client(pcln);
				}
			} else if(hEvent.lNetworkEvents & FD_CLOSE &&	hEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
				/*������ ������ �����*/
				is_repeat = FALSE;
			}
		}
	}

	/*��������� � ���� ������� ������������� ����������� �������� �� ������-������*/
	for(int i = 0; i < MAX_EVENTS;++i)
		WSACloseEvent(psrv->hEvents[i]);

	/*��������� ���������� � ����*/
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(phWorking[i] != INVALID_HANDLE_VALUE) {
			printf("�������� ����� %d...", phWorking[i]);
			if(TerminateThread(phWorking[i], NO_ERROR))
				printf("ok\n");
			else
				show_err("(master)������ ��������� ������", TRUE);
			CloseHandle(phWorking[i]);
		}
	free(phWorking);
	/*��������� ���� ����������*/
	CloseHandle(psrv->iocp);	

	/*������� ����������� ������*/
	DeleteCriticalSection(&psrv->cs);

	/*������� ������ isapi*/
	release_Worker(psrv->pWIsapi);

	/*������� �������� ��������� � ��������� ���������(!!!���!!!)*/

	return 0;
}

int prepare_port(struct Server* psrv, int * psfd, int port, struct sockaddr_in * paddr) {
	/*�������� ���������� ������ ������*/
	*psfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //���� WSA_FLAG_OVERLAPPED ����� ��� �������� ������� ����������� �������� (���� ����� ���������� "��������" � ������ - ����� ������ SOCKET_ERROR � WSAGetLastError() ������� � ������� �������������� �������� - WSA_IO_PENDING)
	if(*psfd == -1) {
		show_err_wsa("(master)�� ������� ���������� ������");
		return 1;
	}

	/*��������� ����� � ������� ������� (������� ����� ������������)*/
	paddr->sin_port = htons(port);
	paddr->sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &paddr->sin_addr);	
	int res = bind(*psfd, (struct sockaddr*)paddr, sizeof(struct sockaddr_in));
	if(res == -1) {
		show_err_wsa("(master)������ ���������� ������ ������ � ������� �������");
		closesocket(*psfd);
		return 2;
	}

	/*�������� ��������� �������������*/
	res = set_repitable(*psfd);
	if(res != 0) {
		show_err_wsa("(master)�� ������� ���������� ����� ���������� ������������� ������ ������");
		closesocket(*psfd);
		return 2;
	}

	/*�������� ������� ������� �����*/
	res = listen(*psfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("(master)�� ������� ������ ������������� �������� ������");
		closesocket(*psfd);
		return 2;
	}

	return 0;
}

int start_server(struct Server* psrv) {
	/*������������� ��������� �����*/
	int res = prepare_port(psrv, &psrv->msfd, psrv->port, &psrv->addr);

	/*������������� ��������� �����*/
	if(!CreateCredentials(psrv->cert_name, psrv->cert_path, &psrv->hServerCreds))
		res = prepare_port(psrv, &psrv->smsfd, psrv->sport, &psrv->saddr);
	

	/*���� �������������������*/
	if(!res)
		loop(psrv);

	/*������������ ����������� SSPI ��������*/
	if(tab!=NULL)
		tab->FreeCredentialsHandle(&psrv->hServerCreds);

	/*��������� ����������� ������ �������*/
	shutdown(psrv->msfd, SD_BOTH);
	closesocket(psrv->msfd);	
	if(psrv->smsfd) {
		shutdown(psrv->smsfd, SD_BOTH);
		closesocket(psrv->smsfd);
	}

	return 0;
}

struct Server * init_Server() {
	struct Server *psrv = malloc(sizeof(struct Server));
	memset(psrv, 0, sizeof(struct Server));		
	psrv->type = SERVER;
	psrv->name = "SocketAsync";

	return psrv;
}

