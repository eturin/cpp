#include "../SocketAsync/socket.h"
#include "../SocketAsync/error.h"
#include "../SocketAsync/req.h"
#include "server_isapi.h"
#include "client_isapi.h"
#include "isapi.h"


BOOL SendAndRecv(BOOL isSEND, SOCKET fd, struct Client_isapi * pcln, char * buf, DWORD bufLen, BOOL checkOff) {
	BOOL isOK = TRUE;                   /*������� ������ ��������� ������*/
		
	char *msg = NULL;                   /*��� ������*/
	WSAEVENT hEvent = WSACreateEvent(); /*������� � ���� �������, ��� ������������ ����������� �������� �� ������*/
	WSABUF wsaBuf;                     	/*����� ������*/
	DWORD ready = 0;                    /*������ ������������� ���������*/

	do {
		int rc = 0;		
		/*����������� ��� ������� �� ������ � ����� ��������*/
		if(isSEND)
			rc = WSAEventSelect(fd, hEvent, FD_WRITE | FD_CLOSE);
		else
			rc = WSAEventSelect(fd, hEvent, FD_READ | FD_CLOSE);

		WSANETWORKEVENTS hWSAevent;
		if(rc == SOCKET_ERROR) {
			msg = "(isapi)������ select �� socket ������-��������";
			isOK = FALSE;
		} else if(-1 == WSAWaitForMultipleEvents(1, &hEvent, FALSE, INFINITE, FALSE)) {                       /*��� ���������� socket*/
			msg = "������ WSAWaitForMultipleEvents";			
			isOK = FALSE;
		} else if(SOCKET_ERROR == WSAEnumNetworkEvents(fd, hEvent, &hWSAevent)) {                             /*���������� ��� ������������ �������*/
			msg = "(isapi)������ ��������� �������� � ����������� �������";
			isOK = FALSE;
		} else if(isSEND && hWSAevent.lNetworkEvents & FD_WRITE && hWSAevent.iErrorCode[FD_WRITE_BIT] == 0) {  /*����� � ������*/;
			wsaBuf.buf = buf + ready;
			wsaBuf.len = bufLen - ready;
			DWORD len = 0;
			if(SOCKET_ERROR == WSASend(fd, &wsaBuf, 1, &len, 0, NULL, NULL) && WSAGetLastError() != WSA_IO_PENDING) {
				msg = "(isapi)������ WSASend (��������)";
				isOK = FALSE;
			}else if((ready+=len) == bufLen) {
				/*�� ����������*/
				break;
			}
			/*��������� �������� � ��������� �������� �����*/
		} else if(!isSEND && hWSAevent.lNetworkEvents & FD_READ && hWSAevent.iErrorCode[FD_READ_BIT] == 0) {    /*����� � ������*/
			wsaBuf.buf = buf    + ready;
			wsaBuf.len = bufLen - ready;
			DWORD len = 0;
			if(SOCKET_ERROR == WSARecv(fd, &wsaBuf, 1, &len, &flag, NULL, NULL) && WSAGetLastError() != WSA_IO_PENDING) {
				msg = "(isapi)������ WSARecv (���������)";
				isOK = FALSE;
			} else if(3 == (ready += len) && checkOff && !strncmp(wsaBuf.buf, "OFF", 3)) {
				/*�������������� ���������� (�������� ������� ���������, ��� �����������, ���� ���������� ������� ���� �������� �����������)*/
				WSASetEvent(*pcln->psrv->phEvent_STOP);
				isOK = FALSE;
			} else if(ready == bufLen) {
				/*�� ��������*/
				break;
			}
			/*��������� ��������� � ��������� �������� �����*/
		} else if(hWSAevent.lNetworkEvents & FD_CLOSE && hWSAevent.iErrorCode[FD_CLOSE_BIT] == 0) {
			msg = "(isapi)�������� ���������� � ������-���������";
			isOK = FALSE;
		} else
			continue; /*��������� ���-�� ����������*/		
	
	} while(isOK);

	if(msg != NULL) 
		show_err_wsa(msg);

	/*��������� ������� � ����*/
	WSACloseEvent(hEvent);

	return isOK;
}

DWORD WINAPI work_isapi(LPVOID lpParameter) {
	struct Client_isapi * pcln = (struct Client_isapi *)lpParameter;

	/*��� ������*/
	char *msg = NULL;

	do {
		/*�������� ������ ������������ ������*/
		if(!SendAndRecv(FALSE, pcln->msfd, pcln, (char*)&pcln->size, sizeof(DWORD), TRUE)) {
			msg = "(isapi)������ ��������� ��������������� �������";
			break;
		}

		/*������� ����� ��� ������*/
		pcln->len = pcln->size++;
		pcln->data = malloc(pcln->size);

		/*�������� ���� ������*/
		if(!SendAndRecv(FALSE, pcln->msfd, pcln, pcln->data, pcln->len, FALSE)) {
			msg = "(isapi)������ ��������� �������� ������";
			break;
		}
		pcln->data[pcln->len] = '\0';

		/*�������� �������� ����������� ������ �������*/
		if(!SendAndRecv(FALSE, pcln->msfd, pcln, (char*)&pcln->inf, sizeof(WSAPROTOCOL_INFOW), FALSE)) {
			msg = "(isapi)������ ��������� ������ �� ������-��������";
			break;
		}

		/*������������� ����� �������*/
		pcln->sfd = WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &pcln->inf, 0, WSA_FLAG_OVERLAPPED);
		if(pcln->sfd == -1) {
			msg = "(isapi)�� ������� ���������� ������ �� ������ �������� master-�������";
			break;
		}

		/*������ �������� ������*/
		pcln->preq = pars_http(pcln->data, &pcln->len);
		if(pcln->preq == NULL) {
			msg = "(isapi)�� ������� ��������� ������, ���������� �� ������-��������";
			break;
		}

		/*����������� ������������ ����������*/
		pcln->pIsapi = get_isapi(pcln);
		if(pcln->pIsapi == NULL) {
			msg = "(isapi)��� �������� � ������������ ���������� ������������� � URL";
			break;
		} else {
			/*�������� ������� �� ����������*/
			call_HttpExtensionProc(pcln);			
		}
	} while(FALSE);

	/*������� ������-�������� � ���������� ��������� ����������*/
	if(msg != NULL) {		
		show_err(msg,FALSE);
		SendAndRecv(TRUE, pcln->msfd, pcln, msg, strlen(msg), FALSE);
	} else 
		SendAndRecv(TRUE, pcln->msfd, pcln, "OK", 2, FALSE);
	
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
			show_err_wsa("(isapi)�� ������� ���������������� ������� select �� ������-������");
			break;
		}

		/*��� ����������� ������� (!!!�� ����� 64 ������ WSAEVENT �������!!!)*/
		rc = WSAWaitForMultipleEvents(MAX_EVENTS, psrv->hEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("(isapi)������ �������� ����������� ������� WSAWaitForMultipleEvents");
			break;
		} else if(rc == 1) {
			/*��������� ������� �������������� ���������*/
			show_err("(isapi)��������� ������� �������������� ���������", FALSE);
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
					show_err("(isapi)�� ������� ��������� ��������� ����� ��������� isapi", TRUE);
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
		show_err_wsa("(isapi)�� ������� ���������� ������");
		return 1;
	}

	/*��������� ����� � ������� ������� (������� ����� ������������)*/
	psrv->addr.sin_port = htons(psrv->port);
	psrv->addr.sin_family = AF_INET;
	char * ip_str = "127.0.0.1";
	inet_ntop(psrv->addr.sin_family, &psrv->addr, ip_str, strlen(ip_str) + 1);
	int res = bind(psrv->msfd, (struct sockaddr*)&psrv->addr, sizeof(psrv->addr));
	if(res == -1) {
		show_err_wsa("(isapi)������ ���������� ������ ������ � ������� �������");
		closesocket(psrv->msfd);
		return 2;
	}

	/*�������� ��������� �������������*/
	res = set_repitable(psrv->msfd);
	if(res != 0) {
		show_err_wsa("(isapi)�� ������� ���������� ����� ���������� ������������� ������ ������");
		closesocket(psrv->msfd);
		return 2;
	}

	/*�������� ������� ������� �����*/
	res = listen(psrv->msfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("(isapi)�� ������� ������ ������������� �������� ������");
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
		show_err("(isapi)�� ������� ����� � ������� worker isapi", TRUE);
		free(psrv);
		psrv = NULL;
	} else {
		char * pPort = getenv("ISAPI_PORT");
		if(pPort != NULL)
			psrv->port = atoi(pPort);
		else
			psrv->port = 12002;		
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

