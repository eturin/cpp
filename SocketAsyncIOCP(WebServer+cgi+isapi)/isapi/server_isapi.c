#include "../SocketAsync/socket.h"
#include "../SocketAsync/error.h"
#include "../SocketAsync/req.h"
#include "server_isapi.h"
#include "client_isapi.h"
#include "isapi.h"


DWORD WINAPI work_isapi(LPVOID lpParameter) {
	struct Client_isapi * pcln = (struct Client_isapi *)lpParameter; 	
	
	/*этапы сменяются последовательно*/
	BOOL isSIZE   = TRUE, 
		 isDATA   = FALSE, 
		 isSOCKET = FALSE, 
		 isWORK   = FALSE, 
		 isFINAL  = FALSE;
	
	/*лог ошибок*/
	char *msg = NULL;

	/*буфер обмена*/
	WSABUF buf;
	DWORD len = 0;
	pcln->cur = 0;

	/*создаем в ядре событие, для отслеживания асинхронных операций на сокете*/
	WSAEVENT hEvent = WSACreateEvent();
	
	do{	
		int rc = 0;
		if(isSIZE || isDATA || isSOCKET) {
			/*ждем готовности socket*/
			rc = WSAEventSelect(pcln->msfd, hEvent, FD_READ | FD_CLOSE);			
		} else if(isFINAL) {
			/*ждем готовности socket*/
			rc = WSAEventSelect(pcln->msfd, hEvent, FD_WRITE | FD_CLOSE);			
		}
		if(rc == SOCKET_ERROR) {
			msg="Ошибка select на socket мастер-процесса";
			isFINAL = TRUE;
			continue;
		}
				
		if(!isWORK) { 
			/*ждем событие*/
			rc = WSAWaitForMultipleEvents(1, &hEvent, FALSE, INFINITE, FALSE);
			if(rc == -1) {
				msg ="Ошибка WSAWaitForMultipleEvents";
				isSIZE = isDATA = isSOCKET = isWORK = FALSE;
				isFINAL = TRUE;	
				continue;
			} else {
				WSANETWORKEVENTS hWSAevent;
				WSAEnumNetworkEvents(pcln->msfd, hEvent, &hWSAevent);
				if(rc == SOCKET_ERROR) {
					msg = "Ошибка получения сведений о наступившем событие";
					isSIZE = isDATA = isSOCKET = isWORK = FALSE;
					isFINAL = TRUE;					
					continue;
				}else if(!isFINAL && hWSAevent.lNetworkEvents & FD_READ && hWSAevent.iErrorCode[FD_READ_BIT] == 0)
					/*готов к чтению*/;
				else if(isFINAL && hWSAevent.lNetworkEvents & FD_WRITE && hWSAevent.iErrorCode[FD_WRITE_BIT] == 0)
					/*готов к записи*/;
				else if(hWSAevent.lNetworkEvents & FD_CLOSE && hWSAevent.iErrorCode[FD_CLOSE_BIT] == 0) {
					msg = "Потеряно соединение с мастер-процессом";
					isSIZE = isDATA = isSOCKET = isWORK = FALSE;
					isFINAL = TRUE;					
					continue;
				} else 
					continue; /*случилось что-то непонятное*/
			}
		}

		/*обработка этапов*/
		if(isSIZE) {
			isSIZE = FALSE;
			/*получаем размер передаваемых данных*/
			buf.buf = (char*)&pcln->size + pcln->cur;
			buf.len = sizeof(DWORD) - pcln->cur;
			len = 0;
			if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL) || pcln->size == 0) {
				msg = "Ошибка WSARecv (получение первоначального размера)";				
				isFINAL = TRUE;				
			} else if(len == 3 && !strncmp(buf.buf, "OFF", 3)) {
				/*принудительное выключение (отмечаем событие остановки, как наступившее, чтоб остановить главный цикл ожидания подключений)*/
				WSASetEvent(*pcln->psrv->phEvent_STOP);
				isFINAL = TRUE;
			} else if((pcln->cur += len) != sizeof(DWORD)) {
				/*получено не всё*/
				isSIZE = TRUE;				
			} else {
				/*выделим место для данных*/
				pcln->len  = pcln->size++;
				pcln->data = malloc(pcln->size);
				pcln->cur = 0;
				isDATA = TRUE;
			}
		} else if(isDATA) {			
			isDATA = FALSE;
			/*получаем сами данные*/			
			buf.buf = pcln->data + pcln->cur;						
			buf.len = pcln->len - pcln->cur;
			len = 0;
			if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL)) {
				msg = "Ошибка WSARecv (получение исходных данных)";
				isFINAL = TRUE;				
			} else if((pcln->cur += len) != pcln->len) {
				/*получено не всё*/				
				isDATA = TRUE;				
			} else {				
				pcln->data[pcln->len] = '\0';				
				pcln->cur = 0;
				isSOCKET = TRUE;				
			}
		} else if(isSOCKET) {			
			isSOCKET = FALSE;
			/*получаем дубликат дескриптора сокета клиента*/
			buf.buf = (char*)&pcln->inf + pcln->cur;
			buf.len = sizeof(WSAPROTOCOL_INFOW) - pcln->cur;
			len = 0;
			if(SOCKET_ERROR == WSARecv(pcln->msfd, &buf, 1, &len, &flag, NULL, NULL)) {
				msg = "Ошибка WSARecv (получение сокета от мастер-сервера)";
				isSOCKET = FALSE;
				isFINAL  = TRUE;				
			} else if((pcln->cur += len) != sizeof(WSAPROTOCOL_INFOW)) {
				/*получено не всё*/				
				isSOCKET = TRUE;				
			} else {				
				/*устанавливаем сокет клиента*/
				pcln->sfd = WSASocketW(FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, FROM_PROTOCOL_INFO, &pcln->inf, 0, WSA_FLAG_OVERLAPPED);
				if(pcln->sfd == -1) {
					msg = "Не получен дескриптор сокета на основе сведений master-сервера";
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
			/*разбор исходных данных*/
			pcln->preq = pars_http(pcln->data, &pcln->len);
			if(pcln->preq == NULL) {
				msg = "Не удалось разобрать данные";				
				continue;
			}
			/*определение динамической библиотеки*/
			pcln->pIsapi = get_isapi(pcln);
			if(pcln->pIsapi == NULL) {
				msg = "Нет сведений о динамической библиотеке";								
			} else {
				/*вызываем функцию из библиотеки*/
				call_HttpExtensionProc(pcln);
				show_err("обработано\n", FALSE);							
			}			
			pcln->cur = 0;
		} else if(isFINAL) {
			isFINAL = FALSE;
			/*сообщим мастер-процессу о результате обработки соединения*/
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
				msg = "Не удалось отправить мастер-процессу сообщение об успешной обработке";
				show_err_wsa(msg);
			}
		}
	} while(isSIZE || isDATA || isSOCKET || isWORK || isFINAL);	
	
	/*закрываем событие в ядре*/
	WSACloseEvent(hEvent);

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

	psrv->work_path = getenv("ISAPI_PATH");
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

