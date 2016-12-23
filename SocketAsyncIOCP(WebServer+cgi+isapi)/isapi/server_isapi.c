#include "../SocketAsync/socket.h"
#include "../SocketAsync/error.h"
#include "../SocketAsync/req.h"
#include "server_isapi.h"
#include "client_isapi.h"
#include "isapi.h"


DWORD WINAPI work_isapi(LPVOID lpParameter) {
	struct Client_isapi * pcln = (struct Client_isapi *)lpParameter; 	
	
	/*����� ��������� ���������������*/
	BOOL isSIZE   = TRUE, 
		 isDATA   = FALSE, 
		 isSOCKET = FALSE, 
		 isWORK   = FALSE, 
		 isFINAL  = FALSE;
	
	/*��� ������*/
	char *msg = NULL;

	/*����� ������*/
	WSABUF buf;
	DWORD len = 0;
	pcln->cur = 0;

	/*������� � ���� �������, ��� ������������ ����������� �������� �� ������*/
	WSAEVENT hEvent = WSACreateEvent();
	
	do{	
		int rc = 0;
		if(isSIZE || isDATA || isSOCKET) {
			/*���� ���������� socket*/
			rc = WSAEventSelect(pcln->msfd, hEvent, FD_READ | FD_CLOSE);			
		} else if(isFINAL) {
			/*���� ���������� socket*/
			rc = WSAEventSelect(pcln->msfd, hEvent, FD_WRITE | FD_CLOSE);			
		}
		if(rc == SOCKET_ERROR) {
			msg="������ select �� socket ������-��������";
			isFINAL = TRUE;
			continue;
		}
				
		if(!isWORK) { 
			/*���� �������*/
			rc = WSAWaitForMultipleEvents(1, &hEvent, FALSE, INFINITE, FALSE);
			if(rc == -1) {
				msg ="������ WSAWaitForMultipleEvents";
				isSIZE = isDATA = isSOCKET = isWORK = FALSE;
				isFINAL = TRUE;	
				continue;
			} else {
				WSANETWORKEVENTS hWSAevent;
				WSAEnumNetworkEvents(pcln->msfd, hEvent, &hWSAevent);
				if(rc == SOCKET_ERROR) {
					msg = "������ ��������� �������� � ����������� �������";
					isSIZE = isDATA = isSOCKET = isWORK = FALSE;
					isFINAL = TRUE;					
					continue;
				}else if(!isFINAL && hWSAevent.lNetworkEvents & FD_READ && hWSAevent.iErrorCode[FD_READ_BIT] == 0)
					/*����� � ������*/;
				else if(isFINAL && hWSAevent.lNetworkEvents & FD_WRITE && hWSAevent.iErrorCode[FD_WRITE_BIT] == 0)
					/*����� � ������*/;
				else if(hWSAevent.lNetworkEvents & FD_CLOSE && hWSAevent.iErrorCode[FD_CLOSE_BIT] == 0) {
					msg = "�������� ���������� � ������-���������";
					isSIZE = isDATA = isSOCKET = isWORK = FALSE;
					isFINAL = TRUE;					
					continue;
				} else 
					continue; /*��������� ���-�� ����������*/
			}
		}

		/*��������� ������*/
		if(isSIZE) {
			isSIZE = FALSE;
			/*�������� ������ ������������ ������*/
			buf.buf = (char*)&pcln->size + pcln->cur;
			buf.len = sizeof(DWORD) - pcln->cur;
			len = 0;
			if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL) || pcln->size == 0) {
				msg = "������ WSARecv (��������� ��������������� �������)";				
				isFINAL = TRUE;				
			} else if(len == 3 && !strncmp(buf.buf, "OFF", 3)) {
				/*�������������� ���������� (�������� ������� ���������, ��� �����������, ���� ���������� ������� ���� �������� �����������)*/
				WSASetEvent(*pcln->psrv->phEvent_STOP);
				isFINAL = TRUE;
			} else if((pcln->cur += len) != sizeof(DWORD)) {
				/*�������� �� ��*/
				isSIZE = TRUE;				
			} else {
				/*������� ����� ��� ������*/
				pcln->len  = pcln->size++;
				pcln->data = malloc(pcln->size);
				pcln->cur = 0;
				isDATA = TRUE;
			}
		} else if(isDATA) {			
			isDATA = FALSE;
			/*�������� ���� ������*/			
			buf.buf = pcln->data + pcln->cur;						
			buf.len = pcln->len - pcln->cur;
			len = 0;
			if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL)) {
				msg = "������ WSARecv (��������� �������� ������)";
				isFINAL = TRUE;				
			} else if((pcln->cur += len) != pcln->len) {
				/*�������� �� ��*/				
				isDATA = TRUE;				
			} else {				
				pcln->data[pcln->len] = '\0';				
				pcln->cur = 0;
				isSOCKET = TRUE;				
			}
		} else if(isSOCKET) {			
			isSOCKET = FALSE;
			/*�������� �������� ����������� ������ �������*/
			buf.buf = (char*)&pcln->inf + pcln->cur;
			buf.len = sizeof(WSAPROTOCOL_INFOW) - pcln->cur;
			len = 0;
			if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL)) {
				msg = "������ WSARecv (��������� ������ �� ������-�������)";
				isSOCKET = FALSE;
				isFINAL  = TRUE;				
			} else if((pcln->cur += len) != sizeof(WSAPROTOCOL_INFOW)) {
				/*�������� �� ��*/				
				isSOCKET = TRUE;				
			} else {				
				/*������������� ����� �������*/
				pcln->sfd = WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &pcln->inf, 0, WSA_FLAG_OVERLAPPED);
				if(pcln->sfd == -1) {
					msg = "�� ������� ���������� ������ �� ������ �������� master-�������";
					isFINAL = TRUE;
					continue;
				} else {
					pcln->cur = 0;
					isWORK = TRUE;
				}
			}
		} else if(isWORK) {
			isWORK  = FALSE;
			isFINAL = TRUE;
			/*������ �������� ������*/
			pcln->preq = pars_http(pcln->data, &pcln->len);
			if(pcln->preq == NULL) {
				msg = "�� ������� ��������� ������";				
				continue;
			}
			/*����������� ������������ ����������*/
			pcln->pIsapi = get_isapi(pcln);
			if(pcln->pIsapi == NULL) {
				msg = "��� �������� � ������������ ����������";								
			} else {
				/*�������� ������� �� ����������*/
				call_HttpExtensionProc(pcln);
				show_err("����������\n", FALSE);							
			}			
			pcln->cur = 0;
		} else if(isFINAL) {
			isFINAL = FALSE;
			/*������� ������-�������� � ���������� ��������� ����������*/
			if(msg != NULL) {
				buf.buf = msg;
				buf.len = strlen(msg);
				show_err_wsa(msg);
			} else {
				buf.buf = "OK";
				buf.len = 2;
			}
			len = 0;
			if(0 != WSASend(pcln->msfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
				msg = "�� ������� ��������� ������-�������� ��������� �� �������� ���������";
				show_err_wsa(msg);
			}
		}
	} while(isSIZE || isDATA || isSOCKET || isWORK || isFINAL);	
	
	/*��������� ������� � ����*/
	WSACloseEvent(hEvent);

	/*��������� ���������� � ������ ���������, ���� �� ������ ������ �����*/
	release_client_isapi(pcln);		

	return 0;
}


int loop(struct Server_isapi * psrv) {
	BOOL is_repeat = TRUE;
	
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
			struct Client_isapi * pcln = init_client_isapi(psrv);
			size_t len = sizeof(struct sockaddr_in);
			/*��������� ����� ���������� (������ ����� ������� �� �������)*/
			int sfd_slave = WSAAccept(psrv->msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
			if(sfd_slave != -1) {
				pcln->msfd = sfd_slave; //!!!����� ������ ������ �� �����������!!!!						

				/*��������� ��������� ����� �� ��������� ����������*/
				HANDLE hThread = CreateThread(NULL,                    /*���������� ������ (NULL - �� ����� ���� �����������)*/
											  0,                       /*��������� ������ ����� (0-����� �������� �����������)*/
											  work_isapi,              /*������� ������*/
											  pcln,                    /*�������� ������*/
											  DETACHED_PROCESS,        /*����� �������� (� ������ ������ ��������� �����, ������� �� ����� ����������� � ����������)*/
											  NULL                     /*������������� ������ (NULL - ������������� ������������ �� �����)*/);
				if(hThread == NULL) {
					show_err("�� ������� ��������� ��������� ����� ��������� isapi", TRUE);
					release_client_isapi(pcln);
				}
			} else
				release_client_isapi(pcln);
		} else if(hEvent.lNetworkEvents & FD_CLOSE &&	hEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
			/*������ ������ �����*/
			is_repeat = FALSE;
		}
	}
	
	/*��������� � ���� ������� ������������� ����������� �������� �� ������-������*/
	for(int i = 0; i < MAX_EVENTS; ++i)
		WSACloseEvent(psrv->hEvents[i]);

	/*������� �������� ��������� � ��������� ���������(!!!���!!!)*/

	return 0;
}

BOOL start_server_isapi(struct Server_isapi * psrv) {
	BOOL isOk = TRUE;
	
	/*�������� ���������� ������ ������*/
	psrv->msfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //���� WSA_FLAG_OVERLAPPED ����� ��� �������� ������� ����������� �������� (���� ����� ���������� "��������" � ������ - ����� ������ SOCKET_ERROR � WSAGetLastError() ������� � ������� �������������� �������� - WSA_IO_PENDING)
	if(psrv->msfd == -1) {
		show_err_wsa("�� ������� ���������� ������");
		return 1;
	}

	/*��������� ����� � ������� ������� (������� ����� ������������)*/
	psrv->addr.sin_port = htons(psrv->port);
	psrv->addr.sin_family = AF_INET;
	char * ip_str = "127.0.0.1";
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


	return isOk;
}

struct Server_isapi * init_server_isapi() {	
	struct Server_isapi * psrv = malloc(sizeof(struct Server_isapi));
	memset(psrv, 0, sizeof(struct Server_isapi));
	psrv->name = "worker-isapi";

	psrv->work_path = getenv("ISAPI_PATH");
	if(!SetCurrentDirectory(psrv->work_path)) {
		show_err("�� ������� ����� � ������� worker isapi", TRUE);
		free(psrv);
		psrv = NULL;
	} else {
		char * pPort = getenv("ISAPI_PORT");
		if(pPort != NULL)
			psrv->port = atoi(pPort);
		else
			psrv->port = 12002;

		/*������������ ��������� ������ isapi � �� ������������� url (����� �����: /.../isapi/edo[/? ]...)*/
		//htab_add(&psrv->hISAPI, "edo", 0, "C:\\Program Files (x86)\\1cv82\\8.2.17.143\\bin\\wsisapi.dll", 0);
	}
	return psrv;
}

struct Server_isapi * release_server_isapi(struct Server_isapi * psrv) {
	if(psrv != NULL) {
		for(struct hTab * s = psrv->hISAPI; s != NULL; s = (struct hTab *)s->hh.next) {
			s->pIsapi = release_isapi((struct Isapi*)s->pIsapi);
		}
		htab_delete_all(&psrv->hISAPI);
		free(psrv);
	}
	return NULL;
}

