#include "../SocketAsync/socket.h"
#include "../SocketAsync/error.h"
#include "../SocketAsync/req.h"
#include "server_isapi.h"
#include "client_isapi.h"
#include "isapi.h"


BOOL SendAndRecv(BOOL isSEND, SOCKET fd, struct Client_isapi * pcln, char * buf, DWORD bufLen, BOOL checkOff) {
	BOOL isOK = TRUE;                   /*признак успеха пересылки данных*/
		
	char *msg = NULL;                   /*лог ошибок*/
	WSAEVENT hEvent = WSACreateEvent(); /*создаем в ядре событие, для отслеживания асинхронных операций на сокете*/
	WSABUF wsaBuf;                     	/*буфер обмена*/
	DWORD ready = 0;                    /*размер обработанного фрагмента*/

	do {
		int rc = 0;		
		/*привязываем тип события на сокете с нашим событием*/
		if(isSEND)
			rc = WSAEventSelect(fd, hEvent, FD_WRITE | FD_CLOSE);
		else
			rc = WSAEventSelect(fd, hEvent, FD_READ | FD_CLOSE);

		WSANETWORKEVENTS hWSAevent;
		if(rc == SOCKET_ERROR) {
			msg = "(isapi)Ошибка select на socket мастер-процесса";
			isOK = FALSE;
		} else if(-1 == WSAWaitForMultipleEvents(1, &hEvent, FALSE, INFINITE, FALSE)) {                       /*ждём готовности socket*/
			msg = "Ошибка WSAWaitForMultipleEvents";			
			isOK = FALSE;
		} else if(SOCKET_ERROR == WSAEnumNetworkEvents(fd, hEvent, &hWSAevent)) {                             /*определяем тип наступившего события*/
			msg = "(isapi)Ошибка получения сведений о наступившем событие";
			isOK = FALSE;
		} else if(isSEND && hWSAevent.lNetworkEvents & FD_WRITE && hWSAevent.iErrorCode[FD_WRITE_BIT] == 0) {  /*готов к записи*/;
			wsaBuf.buf = buf + ready;
			wsaBuf.len = bufLen - ready;
			DWORD len = 0;
			if(SOCKET_ERROR == WSASend(fd, &wsaBuf, 1, &len, 0, NULL, NULL) && WSAGetLastError() != WSA_IO_PENDING) {
				msg = "(isapi)Ошибка WSASend (отправка)";
				isOK = FALSE;
			}else if((ready+=len) == bufLen) {
				/*всё отправлено*/
				break;
			}
			/*повторяем отправку в следующей итерации цикла*/
		} else if(!isSEND && hWSAevent.lNetworkEvents & FD_READ && hWSAevent.iErrorCode[FD_READ_BIT] == 0) {    /*готов к чтению*/
			wsaBuf.buf = buf    + ready;
			wsaBuf.len = bufLen - ready;
			DWORD len = 0;
			if(SOCKET_ERROR == WSARecv(fd, &wsaBuf, 1, &len, &flag, NULL, NULL) && WSAGetLastError() != WSA_IO_PENDING) {
				msg = "(isapi)Ошибка WSARecv (получение)";
				isOK = FALSE;
			} else if(3 == (ready += len) && checkOff && !strncmp(wsaBuf.buf, "OFF", 3)) {
				/*принудительное выключение (отмечаем событие остановки, как наступившее, чтоб остановить главный цикл ожидания подключений)*/
				WSASetEvent(*pcln->psrv->phEvent_STOP);
				isOK = FALSE;
			} else if(ready == bufLen) {
				/*всё получено*/
				break;
			}
			/*повторяем получение в следующей итерации цикла*/
		} else if(hWSAevent.lNetworkEvents & FD_CLOSE && hWSAevent.iErrorCode[FD_CLOSE_BIT] == 0) {
			msg = "(isapi)Потеряно соединение с мастер-процессом";
			isOK = FALSE;
		} else
			continue; /*случилось что-то непонятное*/		
	
	} while(isOK);

	if(msg != NULL) 
		show_err_wsa(msg);

	/*закрываем событие в ядре*/
	WSACloseEvent(hEvent);

	return isOK;
}

DWORD WINAPI work_isapi(LPVOID lpParameter) {
	struct Client_isapi * pcln = (struct Client_isapi *)lpParameter;

	/*лог ошибок*/
	char *msg = NULL;

	do {
		/*получаем размер передаваемых данных*/
		if(!SendAndRecv(FALSE, pcln->msfd, pcln, (char*)&pcln->size, sizeof(DWORD), TRUE)) {
			msg = "(isapi)Ошибка получения первоначального размера";
			break;
		}

		/*выделим место для данных*/
		pcln->len = pcln->size++;
		pcln->data = malloc(pcln->size);

		/*получаем сами данные*/
		if(!SendAndRecv(FALSE, pcln->msfd, pcln, pcln->data, pcln->len, FALSE)) {
			msg = "(isapi)Ошибка получения исходных данных";
			break;
		}
		pcln->data[pcln->len] = '\0';

		/*получаем дубликат дескриптора сокета клиента*/
		if(!SendAndRecv(FALSE, pcln->msfd, pcln, (char*)&pcln->inf, sizeof(WSAPROTOCOL_INFOW), FALSE)) {
			msg = "(isapi)Ошибка получения сокета от мастер-процесса";
			break;
		}

		/*устанавливаем сокет клиента*/
		pcln->sfd = WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &pcln->inf, 0, WSA_FLAG_OVERLAPPED);
		if(pcln->sfd == -1) {
			msg = "(isapi)Не получен дескриптор сокета на основе сведений master-сервера";
			break;
		}

		/*разбор исходных данных*/
		pcln->preq = pars_http(pcln->data, &pcln->len);
		if(pcln->preq == NULL) {
			msg = "(isapi)Не удалось разобрать данные, полученные от мастер-процесса";
			break;
		}

		/*определение динамической библиотеки*/
		pcln->pIsapi = get_isapi(pcln);
		if(pcln->pIsapi == NULL) {
			msg = "(isapi)Нет сведений о динамической библиотеке затребованной в URL";
			break;
		} else {
			/*вызываем функцию из библиотеки*/
			call_HttpExtensionProc(pcln);			
		}
	} while(FALSE);

	/*сообщим мастер-процессу о результате обработки соединения*/
	if(msg != NULL) {		
		show_err(msg,FALSE);
		SendAndRecv(TRUE, pcln->msfd, pcln, msg, strlen(msg), FALSE);
	} else 
		SendAndRecv(TRUE, pcln->msfd, pcln, "OK", 2, FALSE);
	
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
			show_err_wsa("(isapi)Не удалось зарегистрировать событие select на мастер-сокете");
			break;
		}

		/*ждём наступления события (!!!не более 64 разных WSAEVENT событий!!!)*/
		rc = WSAWaitForMultipleEvents(MAX_EVENTS, psrv->hEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("(isapi)Ошибка ожидания асинхронных событий WSAWaitForMultipleEvents");
			break;
		} else if(rc == 1) {
			/*наступило событие принудительной остановки*/
			show_err("(isapi)Наступило событие принудительной остановки", FALSE);
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
					show_err("(isapi)Не удалось запустить отдельный поток обработки isapi", TRUE);
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
		show_err_wsa("(isapi)Не получен дескриптор сокета");
		return 1;
	}

	/*связываем сокет с сетевым адресом (коротый хотим использовать)*/
	psrv->addr.sin_port = htons(psrv->port);
	psrv->addr.sin_family = AF_INET;
	char * ip_str = "127.0.0.1";
	inet_ntop(psrv->addr.sin_family, &psrv->addr, ip_str, strlen(ip_str) + 1);
	int res = bind(psrv->msfd, (struct sockaddr*)&psrv->addr, sizeof(psrv->addr));
	if(res == -1) {
		show_err_wsa("(isapi)Ошибка связывания мастер сокета с сетевым адресом");
		closesocket(psrv->msfd);
		return 2;
	}

	/*включаем повторное использование*/
	res = set_repitable(psrv->msfd);
	if(res != 0) {
		show_err_wsa("(isapi)Не удалось установить опцию повторного использования мастер сокета");
		closesocket(psrv->msfd);
		return 2;
	}

	/*начинаем слушать сетевой адрес*/
	res = listen(psrv->msfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("(isapi)Не удалось начать прослущивание сетевого адреса");
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

	psrv->work_path = getenv("ISAPI_PATH");
	if(!SetCurrentDirectory(psrv->work_path)) {
		show_err("(isapi)Не удалось зайти в каталог worker isapi", TRUE);
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

