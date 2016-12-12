#include "server.h"
#include "socket.h"
#include "client.h" 
#include "worker.h" 
#include "error.h" 


BOOL start_async(void *pp, DWORD len, LPVOID iocp, struct overlapped_inf *pOverlapped_inf) {
	BOOL isOk = TRUE; 

	//EnterCriticalSection(pOverlapped_inf->pcs);
	/*если асинхронная операция немедленно выполнилась, то нужно назначить следующую*/
	while(TRUE) {
		if(pOverlapped_inf->type == READ_WORKER) {
			/*событие чтения STDOUT worker*/
			struct Worker * pwrk = pp;
			pwrk->fd[1].len = pwrk->fd[1].cur += len;			
			if(pwrk->fd[1].len + LEN >= pwrk->fd[1].size) {
				/*вытягиваем, если заполнено уже*/
				pwrk->fd[1].size *= 2;
				pwrk->fd[1].data = realloc(pwrk->fd[1].data, pwrk->fd[1].size);
			}			
			/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pwrk)*/
			if(!ReadFile(pwrk->fd[1].fd_r, pwrk->fd[1].data + pwrk->fd[1].len, LEN, &len, (OVERLAPPED*)pOverlapped_inf) && ERROR_IO_PENDING != GetLastError()) {			
				/*worker завершил работу*/
				release_Worker(pwrk);				
			} 
			/*асинхронную операцию поставили в очередь, выходи*/
			break;			
		} else if(pOverlapped_inf->type == READ_WORKER_ERR) {
			/*событие чтения STDERR worker*/
			struct Worker * pwrk = pp;
			pwrk->fd[2].len = pwrk->fd[2].cur += len;
			if(pwrk->fd[2].len + LEN >= pwrk->fd[2].size) {
				/*вытягиваем, если заполнено уже*/
				pwrk->fd[2].size *= 2;
				pwrk->fd[2].data = realloc(pwrk->fd[2].data, pwrk->fd[2].size);
			}
			/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pwrk)*/
			if(!ReadFile(pwrk->fd[2].fd_r, pwrk->fd[2].data + pwrk->fd[2].len, LEN, &len, (OVERLAPPED*)pOverlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
				show_err("Ошибка ReadFile [STDERR]", TRUE);				
			} 
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		}else if(pOverlapped_inf->type==WRITE_WORKER) {
			/*событие записи STDID worker*/
			struct Worker * pwrk = pp;
			pwrk->fd[0].cur += len;
			len = 0;
			if(pwrk->fd[0].cur < pwrk->fd[0].len) {
				/*еще не все передано worker (асинхронные операции чтения STDOUT и STDERR запущены сразу)*/
				DWORD len_ = LEN <= pwrk->fd[0].len - pwrk->fd[0].cur ? LEN : pwrk->fd[0].len - pwrk->fd[0].cur;
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pwrk)*/
				if(!WriteFile(pwrk->fd[0].fd_w, pwrk->fd[0].data + pwrk->fd[0].cur, len_, &len, (OVERLAPPED*)pOverlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
					/*worker не стал вычитывать все данные*/
					show_err("Ошибка WriteFile [STDIN]", TRUE);					
				} 				
			} 				
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		} else if(pOverlapped_inf->type == READ) {
			/*событие чтения сокета клиента*/
			struct Client * pcln = pp;
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;
			pcln->len = pcln->cur += len;			
			/*проверим конец сообщения*/
			if(pcln->len == 3 && !strncmp(pcln->data, "OFF", 3)) {
				printf("Получена команда принудительного выхода\n");
				return FALSE;
			} 
			char * begin = strchr(pcln->data + pcln->cur - len - (pcln->cur - len >=3 ? 3 : 0), '\r');
			if(begin!=NULL && pcln->len >= 4 && !strncmp(begin, "\r\n\r\n", 4)) {
				/*сообщение получено, запуск worker и асинхронной операции*/
				work(pcln, iocp); //подготовить ответ								
			} else {				
				pcln->DataBuf.len = LEN;
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pcln)*/
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("Ошибка WSARecv");
					release_Client(pcln);
				}				
			}
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		} else if(pOverlapped_inf->type == WRITE) {
			/*событие записи в сокет клиента*/
			struct Client * pcln = pp;
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;
			pcln->cur += len;			
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;				
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pcln)*/
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc  &&  WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("Ошибка WSASend");
					release_Client(pcln);
				}
				/*асинхронную операцию поставили в очередь, выходи*/
				break;
			} else {
				//все отправлено
				clear_Client(pcln);
				len = 0; //для последующей операции чтения
			}
		}
	}
	//LeaveCriticalSection(pOverlapped_inf->pcs);

	return TRUE;
}

void WorkingThread(LPVOID iocp) {
	/*эта функция работает в потоках*/
	while(TRUE) {
		char * type = NULL;
		struct overlapped_inf *pOverlapped_inf = NULL;
		DWORD len = 0;
		if(!GetQueuedCompletionStatus(iocp,              /*дескриптор порта завершения*/
			                          &len,              /*количество перемещенных байт*/
									  (ULONG_PTR*)&type, /*код завершения файла*/
									  (OVERLAPPED**)&pOverlapped_inf,      /*структура этой асинхронной операции*/
			                          INFINITE           /*длительность ожидания завершения запрошенной асинхронной операции*/)) {
			if(type != NULL 
			   && *type==WORKER 
			   && pOverlapped_inf->type==READ_WORKER) {
				/*разрыв на worker*/
				release_Worker((struct Worker*)type);
			} else if(type == NULL){
				show_err_wsa("Ошибка в ожидания завершения асинхронной операции");
				break;
			}			
		} else if(type == NULL)
			break; //принудительная остановка через PostQueuedCompletionStatus
		else if(len == 0 && pOverlapped_inf->type < WAIT) {
			/*разрыв соединения*/
			release_Client((struct Client*)type);
		} else if(!start_async(type, len, iocp, pOverlapped_inf)) {
			struct Server * psrv=NULL;
			if(*type == CLIENT)
				psrv = ((struct Client*)type)->psrv;
			else if(*type == WORKER)
				psrv = ((struct Worker*)type)->pcln->psrv;
			/*приводим событие принудительной остановки в сигнальное состояние*/
			if(psrv!=NULL)
				WSASetEvent(*psrv->phEvent_STOP);			
		}
	}
	printf("Поток завершился\n");
}

int loop(struct Server * psrv) {
	BOOL is_repeat = TRUE;	

	/*узнаем количество ядер процессора*/
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);

	/*создаем порт (очередь) в ядре, для отслеживания асинхронных операций*/
	psrv->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*дескриптор файла*/
										 NULL,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
										 (ULONG_PTR)0,                      /*ключ завершения (параметр будет доступен при наступление события)*/
										 0                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
	if(psrv->iocp == NULL) {
		show_err("Не удалось создать порт завершения в ядре OS",TRUE);
		is_repeat = FALSE;
	}

	/*создаем потоки, для обработки массива событий по количеству ядер*/
	HANDLE *phWorking = malloc(sizeof(HANDLE)*sys_inf.dwNumberOfProcessors);
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(is_repeat)
			phWorking[i] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&WorkingThread, psrv->iocp, 0, 0);
		else
			phWorking[i] = INVALID_HANDLE_VALUE;

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
			struct Client * pcln = init_Client(psrv);			
			size_t len = sizeof(struct sockaddr_in);
			/*принимаем новое соединение (только такие события на мастере)*/			
			int sfd_slave = WSAAccept(psrv->msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);			
			if(sfd_slave != -1) {
				pcln->sfd = sfd_slave; //!!!соект нельзя делать не блокирующим!!!!						

				/*связываем socket с портом (любая асинхронная операция с этим сокетом будет использовать указанный порт)*/
				pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*дескриптор сокета*/
													psrv->iocp,                        /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
													(ULONG_PTR)pcln,                   /*ключ завершения (параметр будет доступен при наступление события)*/
													1                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
				if(pcln->iocp == NULL) {
					show_err("Не удалось привязать slave socket к порту завершения в ядре OS", TRUE);
					release_Client(pcln);
				} else {
					/*запускаем асинхронные операции*/
					DWORD len = 0;
					start_async(pcln, len, psrv->iocp, &pcln->overlapped_inf);
				}
			} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
				show_err_wsa("Не удалось принять соединение");
				release_Client(pcln);
			}
		} else if(hEvent.lNetworkEvents & FD_CLOSE &&	hEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
			/*закрыт мастер сокет*/
			is_repeat=FALSE;
		}		
	}

	/*закрываем в ядре событие отслеживающее асинхронные операции на мастер-сокете*/
	for(int i = 0; i < MAX_EVENTS;++i)
		CloseHandle(psrv->hEvents[i]);

	/*закрываем дескриптор в ядре*/
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(phWorking[i] != INVALID_HANDLE_VALUE) {
			printf("Тормозим поток %d...", phWorking[i]);
			if(TerminateThread(phWorking[i], NO_ERROR))
				printf("ok\n");
			else
				show_err("Ошибка остановки потока", TRUE);
			CloseHandle(phWorking[i]);
		}
	free(phWorking);
	CloseHandle(psrv->iocp);	

	/*ресурсы клиентов отвалятся с закрытием программы(!!!УВЫ!!!)*/

	return 0;
}

int start_server(struct Server* psrv) {
	/*получаем дескриптор мастер сокета*/
	psrv->msfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //флаг WSA_FLAG_OVERLAPPED нужен для быстрого запуска асинхронных операций (чтоб сразу возвращали "сведения" о старте - будут кидать SOCKET_ERROR и WSAGetLastError() сообщит о статусе продолжающейся операции - WSA_IO_PENDING)
	if(psrv->msfd == -1) {
		show_err_wsa("Не получен дескриптор сокета");
		return 1;
	}

	/*связываем сокет с сетевым адресом (коротый хотим использовать)*/	
	psrv->addr.sin_port = htons(psrv->port);
	psrv->addr.sin_family = AF_INET;
	char * ip_str = "0.0.0.0";
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
	
	return 0;
}

struct Server * init_Server() {
	struct Server *psrv = malloc(sizeof(struct Server));
	memset(psrv, 0, sizeof(struct Server));		
	psrv->type = SERVER;

	return psrv;
}