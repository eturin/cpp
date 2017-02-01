#include "server.h"
#include "socket.h"
#include "client.h" 
#include "worker.h"
#include "req.h"
#include "error.h" 
#include "crypt.h"


BOOL start_async(void *pp, DWORD len, LPVOID iocp, struct Overlapped_inf *pOverlapped_inf) {
	BOOL isOk = TRUE; 
		
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
				show_err("(master)Ошибка ReadFile [STDERR]", TRUE);				
			} 
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		} else if(pOverlapped_inf->type == WRITE_WORKER) {
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
					show_err("(master)Ошибка WriteFile [STDIN]", TRUE);
				}
			} else if(pwrk->pcln->preq != NULL && pwrk->pcln->preq->body_length > pwrk->pcln->preq->cur) {
				/*приостановим отправку worker*/
				pOverlapped_inf->type = WAIT;
				free(pwrk->fd[0].data);
				pwrk->fd[0].cur = pwrk->fd[0].len = pwrk->fd[0].size = 0;
				pwrk->fd[0].data = NULL;

				/*дочитаем от клиента тело сообщения*/
				pwrk->pcln->overlapped_inf.type = READ_TO_SEND_WORKER;
				pwrk->pcln->size = MAX_BODY_HTTP;
				pwrk->pcln->data = malloc(MAX_BODY_HTTP);				
				memset(pwrk->pcln->data, 0, pwrk->pcln->size);
				pwrk->pcln->DataBuf.buf = pwrk->pcln->data;
				pwrk->pcln->cur = pwrk->pcln->len = 0;
				start_async(pwrk->pcln, 0, pwrk->pcln->psrv->iocp, &pwrk->pcln->overlapped_inf);
			} else {
				/*нужно остановить worker (закроем STDIN)*/
				CloseHandle(pwrk->fd[0].fd_w);
				pwrk->fd[0].fd_w = INVALID_HANDLE_VALUE;
			}
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		} else if(pOverlapped_inf->type == READ_TO_SEND_WORKER) {
			/*событие чтения сокета клиента*/
			struct Client * pcln = pp;
			pcln->len = pcln->cur += len;

			/*увеличиваем размер уже прочитанной части тела сообщения*/
			if(!pcln->crypt)
				pcln->preq->cur += len;			
			
			if(!pcln->crypt && (pcln->len >= pcln->size || pcln->preq->cur >= pcln->preq->body_length)
			   || pcln->crypt && crypt_check(pcln) /*расшифруем фрагмент сообщения*/) {

				if(pcln->preq->cur >= pcln->preq->body_length) {
					/*признак конца данных, для worker*/
					pcln->data[pcln->len] = '\0';
					++pcln->len;
				}			
				
				/*прочитан очередной фрагмент тела сообщения*/
				pOverlapped_inf->type = WAIT;
				pcln->pwrk->fd[0].data = pcln->data;
				pcln->data = NULL;
				pcln->DataBuf.buf = NULL;
				pcln->pwrk->fd[0].size = pcln->size;
				pcln->pwrk->fd[0].len  = pcln->len;
				pcln->pwrk->fd[0].cur  = 0;
				pcln->cur = pcln->len = pcln->size = 0;
				
				/*запишем его в worker*/				
				pcln->pwrk->fd[0].overlapped_inf.type = WRITE_WORKER;
				start_async(pcln->pwrk, 0, pcln->psrv->iocp, &pcln->pwrk->fd[0].overlapped_inf);
			} else {
				if(pcln->len + LEN >= pcln->size) {
					/*вытягиваем, если заполнено уже*/
					pcln->size *= 2;
					pcln->data = realloc(pcln->data, pcln->size);
					/*сдвигаем буфер по cln->data на количество уже переданных байт*/
					pcln->DataBuf.buf = pcln->data + pcln->len;
				} else
					/*сдвигаем буфер по cln->data на количество переданных байт*/
					pcln->DataBuf.buf += len;

				pcln->DataBuf.len = pcln->size - pcln->len;
				pcln->DataBuf.len = LEN < pcln->DataBuf.len ? LEN : pcln->DataBuf.len;
				pcln->DataBuf.len = pcln->DataBuf.len <= pcln->preq->body_length - pcln->preq->cur ? pcln->DataBuf.len : pcln->preq->body_length - pcln->preq->cur;
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться к памяти по адресу указателя pcln)*/
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("Ошибка WSARecv");
					release_Client(pcln);
				}
			}
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		} else if(pOverlapped_inf->type == READ) {
			/*событие чтения сокета клиента*/
			struct Client * pcln = pp;			
			pcln->len = pcln->cur += len;
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;

			if(pcln->len == 3 && !pcln->crypt && !strncmp(pcln->data, "OFF", 3)) {
				show_err("(master)Получена команда принудительного выхода\n",FALSE);
				return FALSE;
			} 

			if(!pcln->crypt) {/*не зашифрованный канал*/
				/*проверим конец сообщения*/
				char * begin = strstr(pcln->data + pcln->cur - len - (pcln->cur - len > 3 ? 3 : pcln->cur - len), "\r\n\r\n");
				if(pcln->len >= pcln->size || begin != NULL) {
					/*сообщение получено, запуск worker и асинхронной операции*/
					work(pcln, iocp); //подготовить ответ								
					break;
				}
			} else if(crypt_check(pcln)) {/*зашифрованный канал*/				
				break;
			}

			if(pcln->len + LEN >= pcln->size) {
				/*вытягиваем, если заполнено уже*/
				pcln->size *= 2;
				pcln->data = realloc(pcln->data, pcln->size);
				/*сдвигаем буфер по cln->data на количество уже переданных байт*/
				pcln->DataBuf.buf = pcln->data + pcln->len;
			}
				

			pcln->DataBuf.len = pcln->size - pcln->len;
			pcln->DataBuf.len = LEN < pcln->DataBuf.len ? LEN : pcln->DataBuf.len;
			/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pcln)*/
			int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
			if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
				/*сервер сам не закрывает соединений и не анализирует команды удержания соединения*/					
				release_Client(pcln);
			}				
			
			/*асинхронную операцию поставили в очередь, выходи*/
			break;
		} else if(pOverlapped_inf->type == WRITE) {
			/*событие записи в сокет клиента*/
			struct Client * pcln = pp;
			pcln->cur += len;
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;

			/*для зашифрованного канала требуется зашифровать сообщение*/
			if(pcln->crypt && crypt_check(pcln))
				break;			
			
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;				
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pcln)*/
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc  &&  WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("(master)Ошибка WSASend");
					release_Client(pcln);
				}
				/*асинхронную операцию поставили в очередь, выходи*/
				break;
			} else {
				//все отправлено
				clear_Client(pcln);
				len = 0; //для последующей операции чтения
			}
		} else if(pOverlapped_inf->type == WRITE_ISAPI) {
			/*событие записи в сокет клиента*/
			struct Client * pcln = pp;
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;
			pcln->cur += len;
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pcln)*/
				int rc = WSASend(pcln->wsfd, &pcln->DataBuf, 1, &len, 0, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc  &&  WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("(master)Ошибка WSASend (отправка данных isapi)");
					release_Client(pcln);
				}
				/*асинхронную операцию поставили в очередь, выходи*/
				break;
			} else {
				//все отправлено				
				clear_Client(pcln);
				pOverlapped_inf->type = READ_ISAPI;
				len = 0; //для последующей операции чтения
			}
		} else if(pOverlapped_inf->type == READ_ISAPI) {
			/*событие чтения сокета клиента*/
			struct Client * pcln = pp;
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;
			pcln->len = pcln->cur += len;
			/*проверим результат обработки isapi*/
			if(pcln->len == 2 && !strncmp(pcln->data, "OK", 2)) {
				/*успешно обработано*/
				pOverlapped_inf->type = READ;
				pcln->wsfd = close_socket(pcln->wsfd);
			} else if(pcln->len >= 2) {
				/*неудача*/
				make500(pcln);
				pOverlapped_inf->type = WRITE;
				pcln->wsfd = close_socket(pcln->wsfd);
			} else {
				pcln->DataBuf.len = pcln->size - pcln->len;
				pcln->DataBuf.len = LEN < pcln->DataBuf.len ? LEN : pcln->DataBuf.len;
				/*запуск асинхронной операции (в случае успеха, недьзя обращаться п памяти по адресу указателя pcln)*/
				int rc = WSARecv(pcln->wsfd, &pcln->DataBuf, 1, &len, &flag, (WSAOVERLAPPED*)pOverlapped_inf, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					/*worker isapi свалился*/
					make500(pcln);
					pOverlapped_inf->type = WRITE;
					pcln->wsfd = close_socket(pcln->wsfd);
				} else {
					/*асинхронную операцию поставили в очередь, выходи*/
					break;
				}
			}			
		}
	}	

	return TRUE;
}

void WorkingThread(LPVOID iocp) {
	/*эта функция работает в потоках*/
	while(TRUE) {
		char * type = NULL;
		struct Overlapped_inf *pOverlapped_inf = NULL;
		DWORD len = 0;
		if(!GetQueuedCompletionStatus(iocp,              /*дескриптор порта завершения*/
			                          &len,              /*количество перемещенных байт*/
									  (ULONG_PTR*)&type, /*код завершения файла*/
									  (OVERLAPPED**)&pOverlapped_inf,      /*структура этой асинхронной операции*/
			                          INFINITE           /*длительность ожидания завершения запрошенной асинхронной операции*/)) {
			if(type != NULL 
			   && (*type == WORKER)
			   && pOverlapped_inf->type==READ_WORKER) {
				/*разрыв на worker*/
				release_Worker((struct Worker*)type);
			} else if(type == NULL){
				show_err_wsa("(master)Ошибка ожидания завершения асинхронной операции");
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

	/*инициализируем критическую секцию (требуется только один процесс isapi)*/
	InitializeCriticalSection(&psrv->cs);

	/*узнаем количество ядер процессора*/
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);

	/*создаем порт (очередь) в ядре, для отслеживания асинхронных операций*/
	psrv->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*дескриптор файла*/
										NULL,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
										(ULONG_PTR)0,                      /*ключ завершения (параметр будет доступен при наступление события)*/
										0                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
	if(psrv->iocp == NULL) {
		show_err("(master)Не удалось создать порт завершения в ядре OS",TRUE);
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
	psrv->phEvent_STOP = &psrv->hEvents[2];

	while(is_repeat) {
		/*взводим события для открытого сокета*/
		int rc = WSAEventSelect(psrv->msfd, psrv->hEvents[0], FD_ACCEPT | FD_CLOSE);
		if(rc == SOCKET_ERROR) {
			show_err_wsa("(master)Не удалось зарегистрировать событие ACCEPT и CLOSE для открытого сокета");
			break;
		}	
		/*взводим события для закрытого сокета, если он используется*/
		if(psrv->smsfd) {
			rc = WSAEventSelect(psrv->smsfd, psrv->hEvents[1], FD_ACCEPT | FD_CLOSE);
			if(rc == SOCKET_ERROR) {
				show_err_wsa("(master)Не удалось зарегистрировать событие ACCEPT и CLOSE для закрытого сокета");
				break;
			}
		}

		/*ждём наступления события (!!!не более 64 разных WSAEVENT событий!!!)*/
		rc = WSAWaitForMultipleEvents(MAX_EVENTS, psrv->hEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("(master)Ошибка ожидания асинхронных событий WSAWaitForMultipleEvents");
			break;
		} else if(rc == 2) {
			/*наступило событие принудительной остановки*/
			break;
		}
		
		WSANETWORKEVENTS hEvent;
		/*выясняем случилось ли событие на мастер-сокетах (0 - открытый и 1 - закрытый)*/
		for(int i = 0; i < 2; ++i) {
			if(i) {
				if(psrv->smsfd)
					rc = WSAEnumNetworkEvents(psrv->smsfd, psrv->hEvents[1], &hEvent);
				else
					continue; /*закрытый сокет не используется*/
			} else
				rc = WSAEnumNetworkEvents(psrv->msfd, psrv->hEvents[0], &hEvent);

			if(hEvent.lNetworkEvents & FD_ACCEPT &&	hEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
				/*подготовимся к хранению сведений о новом подключение*/
				struct Client * pcln = init_Client(psrv);
				pcln->crypt = i;
				size_t len = sizeof(struct sockaddr_in);
				/*принимаем новое соединение (только такие события на мастере)*/
				int sfd_slave = WSAAccept(i ? psrv->smsfd : psrv->msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
				if(sfd_slave != -1) {
					pcln->sfd = sfd_slave; //!!!соект нельзя делать не блокирующим!!!!						

					/*связываем socket с портом (любая асинхронная операция с этим сокетом будет использовать указанный порт)*/
					pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*дескриптор сокета*/
														psrv->iocp,                        /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
														(ULONG_PTR)pcln,                   /*ключ завершения (параметр будет доступен при наступление события)*/
														1                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
					if(pcln->iocp == NULL) {
						show_err("(master)Не удалось привязать slave socket к порту завершения в ядре OS", TRUE);
						release_Client(pcln);
					} else {
						/*запускаем асинхронные операции*/
						DWORD len = 0;
						printf("[%x] Принято новое соединение\n", pcln);
						start_async(pcln, len, psrv->iocp, &pcln->overlapped_inf);
					}
				} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
					show_err_wsa("(master)Не удалось принять соединение");
					release_Client(pcln);
				}
			} else if(hEvent.lNetworkEvents & FD_CLOSE &&	hEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
				/*закрыт мастер сокет*/
				is_repeat = FALSE;
			}
		}
	}

	/*закрываем в ядре событие отслеживающее асинхронные операции на мастер-сокете*/
	for(int i = 0; i < MAX_EVENTS;++i)
		WSACloseEvent(psrv->hEvents[i]);

	/*закрываем дескриптор в ядре*/
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(phWorking[i] != INVALID_HANDLE_VALUE) {
			printf("Тормозим поток %d...", phWorking[i]);
			if(TerminateThread(phWorking[i], NO_ERROR))
				printf("ok\n");
			else
				show_err("(master)Ошибка остановки потока", TRUE);
			CloseHandle(phWorking[i]);
		}
	free(phWorking);
	/*закрываем порт завершения*/
	CloseHandle(psrv->iocp);	

	/*удаляем критическую секцию*/
	DeleteCriticalSection(&psrv->cs);

	/*погасим воркер isapi*/
	release_Worker(psrv->pWIsapi);

	/*ресурсы клиентов отвалятся с закрытием программы(!!!УВЫ!!!)*/

	return 0;
}

int prepare_port(struct Server* psrv, int * psfd, int port, struct sockaddr_in * paddr) {
	/*получаем дескриптор мастер сокета*/
	*psfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //флаг WSA_FLAG_OVERLAPPED нужен для быстрого запуска асинхронных операций (чтоб сразу возвращали "сведения" о старте - будут кидать SOCKET_ERROR и WSAGetLastError() сообщит о статусе продолжающейся операции - WSA_IO_PENDING)
	if(*psfd == -1) {
		show_err_wsa("(master)Не получен дескриптор сокета");
		return 1;
	}

	/*связываем сокет с сетевым адресом (коротый хотим использовать)*/
	paddr->sin_port = htons(port);
	paddr->sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &paddr->sin_addr);	
	int res = bind(*psfd, (struct sockaddr*)paddr, sizeof(struct sockaddr_in));
	if(res == -1) {
		show_err_wsa("(master)Ошибка связывания мастер сокета с сетевым адресом");
		closesocket(*psfd);
		return 2;
	}

	/*включаем повторное использование*/
	res = set_repitable(*psfd);
	if(res != 0) {
		show_err_wsa("(master)Не удалось установить опцию повторного использования мастер сокета");
		closesocket(*psfd);
		return 2;
	}

	/*начинаем слушать сетевой адрес*/
	res = listen(*psfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("(master)Не удалось начать прослущивание сетевого адреса");
		closesocket(*psfd);
		return 2;
	}

	return 0;
}

int start_server(struct Server* psrv) {
	/*инициализация открытого порта*/
	int res = prepare_port(psrv, &psrv->msfd, psrv->port, &psrv->addr);

	/*инициализация закрытого порта*/
	if(!CreateCredentials(psrv->cert_name, psrv->cert_path, &psrv->hServerCreds))
		res = prepare_port(psrv, &psrv->smsfd, psrv->sport, &psrv->saddr);
	

	/*цикл мультиплексирования*/
	if(!res)
		loop(psrv);

	/*освобождение дескриптора SSPI мандатов*/
	if(tab!=NULL)
		tab->FreeCredentialsHandle(&psrv->hServerCreds);

	/*закрываем дескрипторы мастер сокетов*/
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

