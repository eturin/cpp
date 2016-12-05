#include "server.h"
#include "socket.h"
#include "client.h" 
#include "worker.h" 
#include "error.h" 

/*рабочий каталог, в котором будет работать сервер*/
char work_path[256];

BOOL start_async(struct Client * pcln, DWORD len, LPVOID iocp) {
	BOOL isOk = TRUE;

	EnterCriticalSection(&pcln->cs);
	/*если асинхронная операция немедленно выполнилась, то нужно назначить следующую*/
	while(TRUE) {
		/*сдвигаем буфер по cln->data на количество переданных байт*/
		pcln->DataBuf.buf += len;
		pcln->cur += len;		
		len = 0;

		if(pcln->type == WAIT && pcln->pwrk->type == READ) {			
			pcln->len = pcln->cur;
			pcln->DataBuf.len = LEN;
			if(!ReadFile(pcln->pwrk->fd[1].fd_r, pcln->DataBuf.buf, pcln->DataBuf.len, &len, &pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {
				show_err("Ошибка ReadFile",TRUE);
				release_Worker(pcln->pwrk);
				len = 0;    //для последующей операции записи
			}else
				/*асинхронную операцию поставили в очередь, выходи*/
				break;
		}if(pcln->type == WAIT && pcln->pwrk->type == WRITE) {
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;
				if(!WriteFile(pcln->pwrk->fd[0].fd_w, pcln->DataBuf.buf, pcln->DataBuf.len, &len, &pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {
					show_err("Ошибка ReadFile", TRUE);
					release_Worker(pcln->pwrk);
					len = 0;    //для последующей операции записи
				} else
					/*асинхронную операцию поставили в очередь, выходи*/
					break;
			} else {
				/*все передано worker, ждем ответа от него*/
				clear_Client(pcln);
				pcln->type = WAIT;  
				pcln->pwrk->type = READ;
				len = 0;    //для последующей операции чтения
			}
		}

		if(pcln->type == READ) {
			pcln->len = pcln->cur;
			/*проверим конец сообщения*/
			if(pcln->len == 3 && !strncmp(pcln->data, "OFF", 3)) {
				printf("Получена команда принудительного выхода\n");
				return FALSE;
			} else if(pcln->len >= 4 && !strncmp(pcln->data + pcln->len - 4, "\r\n\r\n", 4)) {
				/*сообщение получено*/
				work(pcln, iocp); //подготовить ответ
				len = 0;          //для последующей операции записи
			} else {
				len = 0;
				pcln->DataBuf.len = LEN;
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, &pcln->overlapped, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("Ошибка WSARecv");
					release_Client(pcln);
				}
				/*асинхронную операцию поставили в очередь, выходи*/
				break;
			}
		}

		if(pcln->type == WRITE) {			
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN : pcln->len - pcln->cur;				
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, &pcln->overlapped, NULL);
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
	LeaveCriticalSection(&pcln->cs);

	return TRUE;
}

void WorkingThread(LPVOID iocp) {
	/*эта функция работает в потоках*/
	while(TRUE) {
		struct Client * pcln = NULL;
		OVERLAPPED * pOverlapped = NULL;
		DWORD len = 0;
		if(!GetQueuedCompletionStatus(iocp,              /*дескриптор порта завершения*/
			                          &len,              /*количество перемещенных байт*/
			                          (ULONG_PTR*)&pcln, /*код завершения файла*/
			                          &pOverlapped,      /*структура этой асинхронной операции*/
			                          INFINITE           /*длительность ожидания завершения запрошенной асинхронной операции*/)) {
			if(pcln != NULL) {
				/*разрыв на worker*/
				release_Worker(pcln->pwrk);
				start_async(pcln, len, iocp);
			} else {
				show_err_wsa("Ошибка в ожидания завершения асинхронной операции");
				break;
			}			
		} else if(pcln == NULL)
			break; //принудительная остановка через PostQueuedCompletionStatus
		else if(len == 0 && pcln->type!=WAIT) {
			/*разрыв соединения*/
			release_Client(pcln);
		} else if(!start_async(pcln, len, iocp)) {//запускаем асинхронные операции
			/*узнаем количество ядер процессора*/
			SYSTEM_INFO sys_inf;
			GetSystemInfo(&sys_inf);
			for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors - 1; ++i)
				PostQueuedCompletionStatus(iocp, 0, 0, 0);//это вызовет немедленное пробуждение одного потоков 
			TerminateProcess(INVALID_HANDLE_VALUE, NO_ERROR); //останется пристрелить главный поток
			break;
		}
	}
	printf("Поток завершился\n");
}

int loop(int msfd) {
	BOOL is_repeat = TRUE;

	/*узнаем количество ядер процессора*/
	SYSTEM_INFO sys_inf;
	GetSystemInfo(&sys_inf);

	/*создаем порт (очередь) в ядре, для отслеживания асинхронных операций*/
	HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*дескриптор файла*/
										 NULL,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
										 (ULONG_PTR)0,                      /*ключ завершения (параметр будет доступен при наступление события)*/
										 0                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
	if(iocp == NULL) {
		show_err("Не удалось создать порт завершения в ядре OS",TRUE);
		is_repeat = FALSE;
	}

	/*создаем потоки, для обработки массива событий по количеству ядер*/
	HANDLE *phWorking = malloc(sizeof(HANDLE)*sys_inf.dwNumberOfProcessors);
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i)
		if(is_repeat)
			phWorking[i] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&WorkingThread, iocp, 0, 0);
		else
			phWorking[i] = INVALID_HANDLE_VALUE;

	while(is_repeat) {
		/*подготовимся к хранению сведений о новом подключение*/
		struct Client * pcln = init_Client();
		
		/*принимаем новое соединение (только такие события на мастере)*/
		size_t len = sizeof(struct sockaddr_in);
		int sfd_slave = WSAAccept(msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
		if(sfd_slave != -1) {
			pcln->sfd = sfd_slave; //!!!соект нельзя делать не блокирующим!!!!						
			
			/*связываем socket с портом (любая асинхронная операция с этим сокетом будет использовать указанный порт)*/
			pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*дескриптор сокета*/
												iocp,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
												(ULONG_PTR)pcln,                   /*ключ завершения (параметр будет доступен при наступление события)*/
												1                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
			if(pcln->iocp == NULL) {
				show_err("Не удалось привязать slave socket к порту завершения в ядре OS", TRUE);
				release_Client(pcln);
			} else {
				/*запускаем асинхронные операции*/
				DWORD len = 0;
				start_async(pcln, len, iocp);
			}
		} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
			show_err_wsa("Не удалось принять соединение");
			release_Client(pcln);
		}
	}

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
	CloseHandle(iocp);	

	/*ресурсы клиентов отвалятся с закрытием программы(!!!УВЫ!!!)*/

	return 0;
}

int start_server() {
	/*Инициализация*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) {
		show_err_wsa("Ошибка инициализация среды");
		return -1;
	}

	/*получаем дескриптор мастер сокета*/
	int msfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED); //флаг WSA_FLAG_OVERLAPPED нужен для быстрого запуска асинхронных операций (чтоб сразу возвращали "сведения" о старте - будут кидать SOCKET_ERROR и WSAGetLastError() сообщит о статусе продолжающейся операции - WSA_IO_PENDING)
	if(msfd == -1) {
		show_err_wsa("Не получен дескриптор сокета");
		return 1;
	}

	/*связываем сокет с сетевым адресом (коротый хотим использовать)*/
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12001);
	char * ip_str = "0.0.0.0";
	inet_ntop(addr.sin_family, &addr, ip_str, strlen(ip_str) + 1);
	int res = bind(msfd, (struct sockaddr*)&addr, sizeof(addr));
	if(res == -1) {
		show_err_wsa("Ошибка связывания мастер сокета с сетевым адресом");
		closesocket(msfd);
		return 2;
	}

	/*включаем повторное использование*/
	res = set_repitable(msfd);
	if(res != 0) {
		show_err_wsa("Не удалось установить опцию повторного использования мастер сокета");
		closesocket(msfd);
		return 2;
	}

	/*начинаем слушать сетевой адрес*/
	res = listen(msfd, SOMAXCONN);
	if(res == -1) {
		show_err_wsa("Не удалось начать прослущивание сетевого адреса");
		closesocket(msfd);
		return 2;
	}

	/*цикл мультиплексирования*/
	loop(msfd);

	/*закрываем дескриптор мастер сокета*/
	shutdown(msfd, SD_BOTH);
	closesocket(msfd);
	WSACleanup();

	
	return 0;
}