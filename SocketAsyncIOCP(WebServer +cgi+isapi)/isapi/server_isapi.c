#include "../SocketAsync/socket.h"
#include "../SocketAsync/req.h"
#include "../SocketAsync/error.h"

#include "server_isapi.h"
#include "client_isapi.h"
#include "isapi.h"

DWORD WINAPI work_isapi(LPVOID lpParameter) {
	struct Client_isapi * pcln = (struct Client_isapi *)lpParameter; 
	
	do{		
		/*�������� ������ ������������ ������*/
		WSABUF buf;
		buf.buf = (char*)&pcln->size;
		buf.len = sizeof(DWORD);
		if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &buf.len, &flag, NULL, NULL)) {
			show_err_wsa("������ WSARecv (��������� ��������������� �������)");
			break;
		}
		if(pcln->size<0) {
			/*�������������� ���������� (�������� ������� ���������, ��� �����������, ���� ���������� ������� ���� �������� �����������)*/
			WSASetEvent(*pcln->psrv->phEvent_STOP);
			break;
		}

		/*�������� ���� ������*/
		pcln->data = malloc(pcln->size+1);
		buf.buf = pcln->data;
		buf.len = pcln->size;
		if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &pcln->len, &flag, NULL, NULL) || pcln->size != pcln->len) {
			show_err_wsa("������ WSARecv (��������� �������� ������)");				
			break;
		}
		pcln->data[pcln->size]='\0';
		++pcln->size;
				
		/*�������� �������� ������ �������*/
		buf.buf = (char*)&pcln->inf;
		buf.len = sizeof(WSAPROTOCOL_INFOW);
		DWORD len = 0;
		if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL) || len != buf.len) {
			show_err_wsa("������ WSARecv (��������� ������ �� ������-�������)");				
			break;
		}
		/*������������� ����� �������*/
		pcln->sfd = WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &pcln->inf, 0, WSA_FLAG_OVERLAPPED);		
		if(pcln->sfd == -1) {
			show_err_wsa("�� ������� ���������� ������ �� ������ �������� master-�������");				
			break;
		}			
		/*������ �������� ������*/
		pcln->preq = pars_http(pcln->data, &pcln->len);
		if(pcln->preq == NULL) {
			/*�� ������� ��������� ������*/		
			break;
		}
		//Sleep(15000);
		/*����������� ������������ ����������*/
		pcln->pIsapi = get_isapi(pcln);
		if(pcln->pIsapi == NULL) {
			show_err_wsa("��� �������� � ������������ ����������");
			break;
		}

		/*�������� ������� �� ����������*/
		call_HttpExtensionProc(pcln);			
	} while(FALSE);
	
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

	psrv->work_path = getenv("ISAPI_PARH");
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
		htab_add(&psrv->hISAPI, "edo", 0, "C:\\Program Files (x86)\\1cv82\\8.2.17.143\\bin\\wsisapi.dll", 0);
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

