#include "../SocketAsync/socket.h"
#include "../SocketAsync/req.h"
#include "../SocketAsync/error.h"

#include "server_isapi.h"
#include "client_isapi.h"
#include "isapi.h"

DWORD WINAPI work_isapi(LPVOID lpParameter) {
	struct Client_isapi * pcln = (struct Client_isapi *)lpParameter; 
	
	do{		
		/*получаем размер передаваемых данных*/
		WSABUF buf;
		buf.buf = (char*)&pcln->size;
		buf.len = sizeof(DWORD);
		if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &buf.len, &flag, NULL, NULL)) {
			show_err_wsa("Ошибка WSARecv (получение первоначального размера)");
			break;
		}
		if(pcln->size<0) {
			/*принудительное выключение (отмечаем событие остановки, как наступившее, чтоб остановить главный цикл ожидания подключений)*/
			WSASetEvent(*pcln->psrv->phEvent_STOP);
			break;
		}

		/*получаем сами данные*/
		pcln->data = malloc(pcln->size+1);
		buf.buf = pcln->data;
		buf.len = pcln->size;
		if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &pcln->len, &flag, NULL, NULL) || pcln->size != pcln->len) {
			show_err_wsa("Ошибка WSARecv (получение исходных данных)");				
			break;
		}
		pcln->data[pcln->size]='\0';
		++pcln->size;
				
		/*получаем дупликат сокета клиента*/
		buf.buf = (char*)&pcln->inf;
		buf.len = sizeof(WSAPROTOCOL_INFOW);
		DWORD len = 0;
		if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL) || len != buf.len) {
			show_err_wsa("Ошибка WSARecv (получение сокета от мастер-сервера)");				
			break;
		}
		/*устанавливаем сокет клиента*/
		pcln->sfd = WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &pcln->inf, 0, WSA_FLAG_OVERLAPPED);		
		if(pcln->sfd == -1) {
			show_err_wsa("Не получен дескриптор сокета на основе сведений master-сервера");				
			break;
		}			
		/*разбор исходных данных*/
		pcln->preq = pars_http(pcln->data, &pcln->len);
		if(pcln->preq == NULL) {
			/*не удалось разобрать данные*/		
			break;
		}
		//Sleep(15000);
		/*определение динамической библиотеки*/
		pcln->pIsapi = get_isapi(pcln);
		if(pcln->pIsapi == NULL) {
			show_err_wsa("Нет сведений о динамической библиотеке");
			break;
		}

		/*вызываем функцию из библиотеки*/
		call_HttpExtensionProc(pcln);			
	} while(FALSE);
	
	/*закрываем соединение с мастер процессом, чтоб он дальше слушал сокет*/
	release_client_isapi(pcln);		

	return 0;
}


int loop(struct Server_isapi * psrv) {
	BOOL is_repeat = TRUE;
	
	/*создаем в ядре событие, для отслеживания асинхронных операций на мастер-сокете*/
	for(int i = 0; i < MAX_EVENTS; ++i)
		psrv->hEvents[i] = WSACreateEvent();
	/*событие принудительной остановки*/
	psrv->phEvent_STOP = &psrv->hEvents[1];

	while(is_repeat) {
		/*взводим события для master socket*/
		int rc = WSAEventSelect(psrv->msfd, psrv->hEvents[0], FD_ACCEPT | FD_CLOSE);
		if(rc == SOCKET_ERROR) {
			show_err_wsa("Не удалось зарегистрировать событие select на мастер-сокете");
			break;
		}

		/*ждём наступления события (!!!не более 64 разных WSAEVENT событий!!!)*/
		rc = WSAWaitForMultipleEvents(MAX_EVENTS, psrv->hEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("Ошибка ожидания асинхронных событий WSAWaitForMultipleEvents");
			break;
		} else if(rc == 1) {
			/*наступило событие принудительной остановки*/
			break;
		}

		WSANETWORKEVENTS hEvent;
		/*выясняем случилось ли событие на мастер-сокете*/
		rc = WSAEnumNetworkEvents(psrv->msfd, psrv->hEvents[0], &hEvent);
		if(hEvent.lNetworkEvents & FD_ACCEPT &&	hEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
			/*подготовимся к хранению сведений о новом подключение*/
			struct Client_isapi * pcln = init_client_isapi(psrv);
			size_t len = sizeof(struct sockaddr_in);
			/*принимаем новое соединение (только такие события на мастере)*/
			int sfd_slave = WSAAccept(psrv->msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
			if(sfd_slave != -1) {
				pcln->msfd = sfd_slave; //!!!соект нельзя делать не блокирующим!!!!						

				/*запускаем отдельный поток на обработку соединения*/
				HANDLE hThread = CreateThread(NULL,                    /*дескриптор защиты (NULL - не может быть унаследован)*/
											  0,                       /*начальный размер стека (0-взять значение поумолчанию)*/
											  work_isapi,              /*функция потока*/
											  pcln,                    /*параметр потока*/
											  DETACHED_PROCESS,        /*опции создания (в данном случае создается поток, который не будет присоединен в дальнейшем)*/
											  NULL                     /*идентификатор потока (NULL - идентификатор возвращаться не будет)*/);
				if(hThread == NULL) {
					show_err("Не удалось запустить отдельный поток обработки isapi", TRUE);
					release_client_isapi(pcln);
				}
			} else
				release_client_isapi(pcln);
		} else if(hEvent.lNetworkEvents & FD_CLOSE &&	hEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
			/*закрыт мастер сокет*/
			is_repeat = FALSE;
		}
	}
	
	/*закрываем в ядре событие отслеживающее асинхронные операции на мастер-сокете*/
	for(int i = 0; i < MAX_EVENTS; ++i)
		WSACloseEvent(psrv->hEvents[i]);

	/*ресурсы клиентов отвалятся с закрытием программы(!!!УВЫ!!!)*/

	return 0;
}

BOOL start_server_isapi(struct Server_isapi * psrv) {
	BOOL isOk = TRUE;
	
	/*получаем дескриптор мастер сокета*/
	psrv->msfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //флаг WSA_FLAG_OVERLAPPED нужен для быстрого запуска асинхронных операций (чтоб сразу возвращали "сведения" о старте - будут кидать SOCKET_ERROR и WSAGetLastError() сообщит о статусе продолжающейся операции - WSA_IO_PENDING)
	if(psrv->msfd == -1) {
		show_err_wsa("Не получен дескриптор сокета");
		return 1;
	}

	/*связываем сокет с сетевым адресом (коротый хотим использовать)*/
	psrv->addr.sin_port = htons(psrv->port);
	psrv->addr.sin_family = AF_INET;
	char * ip_str = "127.0.0.1";
	inet_ntop(psrv->addr.sin_family, &psrv->addr, ip_str, strlen(ip_str) + 1);
	int res = bind(psrv->msfd, (struct sockaddr*)&psrv->addr, sizeof(psrv->addr));
	if(res == -1) {
		show_err_wsa("Ошибка связывания мастер сокета с сетевым адресом");
		closesocket(psrv->msfd);
		return 2;
	}

	/*включаем повторное использование*/
	res = set_repitable(psrv->msfd);
	if(res != 0) {
		show_err_wsa("Не удалось установить опцию повторного использования мастер сокета");
		closesocket(psrv->msfd);
		return 2;
	}

	/*начинаем слушать сетевой адрес*/
	res = listen(psrv->msfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("Не удалось начать прослущивание сетевого адреса");
		closesocket(psrv->msfd);
		return 2;
	}

	/*цикл мультиплексирования*/
	loop(psrv);

	/*закрываем дескриптор мастер сокета*/
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
		show_err("Не удалось зайти в каталог worker isapi", TRUE);
		free(psrv);
		psrv = NULL;
	} else {
		char * pPort = getenv("ISAPI_PORT");
		if(pPort != NULL)
			psrv->port = atoi(pPort);
		else
			psrv->port = 12002;

		/*регистрируем известные модули isapi и их относительный url (здесь такой: /.../isapi/edo[/? ]...)*/
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

