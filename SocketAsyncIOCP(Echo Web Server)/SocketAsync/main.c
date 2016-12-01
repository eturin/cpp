#define _CRT_SECURE_NO_WARNINGS
#define WINVER 0x0602 /*API Windows 7*/

#define _GNU_SOURCE
#pragma comment(lib, "ws2_32.lib") 
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

/*состояние сокета*/
#define READ     0
#define WRITE    1
#define WAIT     2
/*максимальный размер входящего сообщения*/
#define MAX_HEAD_HTTP 4096

#define LEN 10

/*сокеты клиентов и их контекст*/
int flag = 0;
int cnt = 0;
struct Client {
	/*критическая секция этого клиента*/
	CRITICAL_SECTION cs;
	/*режим чтения или записи в сокет*/
	char type;
	/*дескриптор сокета*/
	int sfd;
	
	/*общий объем данных*/
	DWORD len;
	/*сколько уже передано*/
	DWORD cur;	
	/*данные*/
	char * data;

	/*структура, для выполнения асинхронных вызовов*/
	struct my_overlapped {
		WSAOVERLAPPED overlapped;
		struct Client * cln;
	} my_overlapped;
	/*буфер обмена*/
	WSABUF DataBuf;	

	/*порт клиента*/
	HANDLE iocp;
	/*сетевой адрес*/
	struct sockaddr_in addr;
};
struct Client * init_Client();
struct Client * release_Client(struct Client *);
struct Client * clear_Client(struct Client *);

int show_err_wsa(const char * msg) {
	int      no = WSAGetLastError();
	char     str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s: %d\n%s\n", msg, no, str_err);
	else
		printf("%s:\nномер ошибки %d\n", msg, no);

	return no;
}
int show_err(const char * msg) {
	int      no = GetLastError();
	char     str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s: %d\n%s\n", msg, no, str_err);
	else
		printf("%s:\nномер ошибки %d\n", msg, no);	

	return no;
}
int set_repitable(int sfd) {
	int optval = 1;
	return setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
}
void close_socket(int fd) {
	shutdown(fd, SD_BOTH);
	closesocket(fd);
}

struct Client * init_Client() {
	++cnt;
	struct Client * new_Client = (struct Client*)malloc(sizeof(struct Client));
	memset(new_Client, 0, sizeof(struct Client));

	new_Client->sfd = 0;
	new_Client->type = READ;	
	
	new_Client->len = new_Client->cur = 0;
	new_Client->data = malloc(MAX_HEAD_HTTP);
	new_Client->DataBuf.len = LEN;
	new_Client->DataBuf.buf = new_Client->data;
	
	/*формируем структуру события*/
	new_Client->my_overlapped.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
				                                              TRUE, /*тип сброса TRUE - ручной*/
				                                              TRUE, /*начальное состояние TRUE - сигнальное*/
				                                              NULL  /*имя обьекта*/);
	new_Client->my_overlapped.cln = new_Client;

	/*инициализация критической секции*/
	InitializeCriticalSection(&new_Client->cs);	

	return new_Client;
}
struct Client * release_Client(struct Client * pcln) {
	/*эту операцию следует выполнять не одновременно*/	
	if(pcln != NULL && TryEnterCriticalSection(&pcln->cs)) {		
		/*закрываем хендл события в ядре*/
		WSACloseEvent(pcln->my_overlapped.overlapped.hEvent);
		/*закрываем порт клиента*/
		//if(pcln->iocp) CloseHandle(pcln->iocp);

		/*освобождаем данные*/
		free(pcln->data);
		/*закрываем сокет*/
		close_socket(pcln->sfd);		
		LeaveCriticalSection(&pcln->cs);
		DeleteCriticalSection(&pcln->cs);
		free(pcln);	
	}	

	return NULL;
}
struct Client * clear_Client(struct Client * pcln) {		
	/*эту операцию следует выполнять не одновременно*/
	if(TryEnterCriticalSection(&pcln->cs)) {
		if(pcln != NULL) {
			pcln->type = READ;
			/*реинициализируем данные передачи*/
			free(pcln->data);
			pcln->len = pcln->cur = 0;
			pcln->data = malloc(MAX_HEAD_HTTP);
			memset(pcln->data, 0, MAX_HEAD_HTTP);
			pcln->DataBuf.len = LEN;
			pcln->DataBuf.buf = pcln->data;
		}
		LeaveCriticalSection(&pcln->cs);
	}

	return pcln;
}

char * format200 = "HTTP / 1.1 200 OK\r\nVersion: HTTP / 1.1\r\n"
		           "Content-Type: text/html; charset=utf-8\r\n"
				   "Content-Length: %d\r\n\r\n%s";

BOOL work(struct Client * pcln) {
	BOOL isOk = TRUE;
	pcln->type = WAIT;

	size_t len = pcln->len + strlen(format200)*sizeof(char)+15; //15 байт на вставку %d
	char * response = malloc(len);	
	pcln->data[pcln->len] = '\0';
	sprintf(response, format200, pcln->len, pcln->data);
	free(pcln->data);
	pcln->data = response;
	pcln->len = strlen(response)*sizeof(char);

	/*еще не отправлено ничего*/
	pcln->type = WRITE;
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;		

	return isOk;
}

BOOL start_async(struct Client * pcln, DWORD len) {
	BOOL isOk = TRUE;

	EnterCriticalSection(&pcln->cs);	
	/*если асинхронная операция немедленно выполнилась, то нужно назначить следующую*/
	while(TRUE) {
		if(pcln->type == READ) {
			/*сдвигаем буфер по cln->data на количество переданных байт*/			
			pcln->DataBuf.buf += len;
			pcln->cur = pcln->len += len;

			/*проверим конец сообщения*/
			if(pcln->len == 3 && !strncmp(pcln->data, "OFF", 3)) {
				printf("Получена команда принудительного выхода\n");
				return FALSE;
			} else if(pcln->len >= 4 && !strncmp(pcln->data + pcln->len - 4, "\r\n\r\n", 4)) {
				/*сообщение получено*/
				work(pcln); //подготовить ответ
				len = 0;    //для последующей операции записи
			} else {
				len = 0;
				pcln->DataBuf.len = LEN;
				int rc = WSARecv(pcln->sfd, &pcln->DataBuf, 1, &len, &flag, (OVERLAPPED*)&pcln->my_overlapped, NULL);
				if(SOCKET_ERROR == rc && WSA_IO_PENDING != WSAGetLastError()) {
					show_err_wsa("Ошибка WSARecv");
					release_Client(pcln);
				} 
				/*асинхронную операцию поставили в очередь, выходи*/
				break;
			}
		}

		if(pcln->type == WRITE) {
			/*сдвигаем буфер по cln->data на количество переданных байт*/
			pcln->DataBuf.buf += len;
			pcln->cur += len;
			if(pcln->cur < pcln->len) {
				pcln->DataBuf.len = LEN <= pcln->len - pcln->cur ? LEN: pcln->len - pcln->cur;
				len = 0;
				int rc = WSASend(pcln->sfd, &pcln->DataBuf, 1, &len, 0, (OVERLAPPED*)&pcln->my_overlapped, NULL);
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
		ULONG_PTR key = 0;
		struct my_overlapped * pmy_overlapped;		
		DWORD len=0;		
		if(!GetQueuedCompletionStatus(iocp,              /*дескриптор порта завершения*/
			                          &len,              /*количество перемещенных байт*/
									  (ULONG_PTR*)&key,  /*код завершения файла*/
									  (OVERLAPPED**)&pmy_overlapped,  /*структура этой асинхронной операции*/
			                          INFINITE           /*длительность ожидания завершения запрошенной асинхронной операции*/)) {
			show_err_wsa("Ошибка в ожидания завершения асинхронной операции");
			break;
		} else if(pmy_overlapped == NULL)
			break; //принудительная остановка через PostQueuedCompletionStatus
		else if(len == 0) {
			/*разрыв соединения*/
			release_Client(pmy_overlapped->cln);
		} else if(!start_async(pmy_overlapped->cln, len)) {//запускаем асинхронные операции
			/*узнаем количество ядер процессора*/
			SYSTEM_INFO sys_inf;
			GetSystemInfo(&sys_inf);
			for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors - 1;++i)
				PostQueuedCompletionStatus(iocp,0,0,0);//это вызовет немедленное пробуждение одного потоков 
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
	HANDLE iocp=CreateIoCompletionPort(INVALID_HANDLE_VALUE,              /*дескриптор файла*/
		                               NULL,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/ 
		                               (ULONG_PTR)0,                      /*ключ завершения (параметр будет доступен при наступление события)*/
		                               0                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
	if(iocp == NULL) {
		show_err("Не удалось создать порт завершения в ядре OS");
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
		/*принимаем новое соединение (только такие события на мастере)*/
		struct Client * pcln = init_Client();		
		size_t len=sizeof(struct sockaddr_in);
		int sfd_slave = WSAAccept(msfd, (struct sockaddr *)&pcln->addr, &len, NULL, 0);
		if(sfd_slave != -1) {
			pcln->sfd = sfd_slave; //!!!соект нельзя делать не блокирующим!!!!						
			/*связываем socket с портом (любая асинхронная операция с этим сокетом будет использовать указанный порт)*/
			pcln->iocp = CreateIoCompletionPort((HANDLE)sfd_slave,                 /*дескриптор файла*/
												iocp,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
												(ULONG_PTR)0,                      /*ключ завершения (параметр будет доступен при наступление события)*/
												1                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/); 
			if(pcln->iocp == NULL) {
				show_err("Не удалось привязать slave socket к порту завершения в ядре OS");
				release_Client(pcln);
			} else {
				/*запускаем асинхронные операции*/
				DWORD len = 0;
				start_async(pcln, len);
			}
		} else if(WSAEWOULDBLOCK != WSAGetLastError()) {
			show_err_wsa("Не удалось принять соединение");
			release_Client(pcln);
		}

	}
	
	/*закрываем дескриптор в ядре*/
	for(DWORD i = 0; i < sys_inf.dwNumberOfProcessors; ++i) 
		if(phWorking[i]!=INVALID_HANDLE_VALUE){
			printf("Тормозим поток %d...", phWorking[i]);
			if(TerminateThread(phWorking[i], NO_ERROR))
				printf("ok\n");
			else
				show_err("Ошибка остановки потока");
			CloseHandle(phWorking[i]);
		}
	free(phWorking);
	//CloseHandle(iocp);	
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

	system("pause");
	return 0;
}

int main() {
	setlocale(LC_ALL, "russian");

	start_server();

	return 0;
}