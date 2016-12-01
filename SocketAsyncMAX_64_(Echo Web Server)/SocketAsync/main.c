#define _CRT_SECURE_NO_WARNINGS
#define WINVER 0x0601 /*API Windows 7*/

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
/*максимальный размер входящего сообщения*/
#define MAX_HEAD_HTTP 4096

#define LEN 10

/*сокеты клиентов и их контекст*/
int cnt_clients = 0;
struct Client {
	/*режим чтения или записи в сокет*/
	char type;
	/*дескриптор сокета*/
	int sfd;
	/*объем данных*/
	int len;
	/*сколько уже отправлено*/
	int cur;
	/*данные*/
	char * data;
	/*дескриптор события в ядре OS*/
	WSAEVENT hEvent;
		
	struct Client *next;
};
struct Client *root_client, *tail_client;
struct Client * init_Client(int);
struct Client * release_Client(struct Client *);
struct Client * clear_Client(struct Client *);

int show_err_wsa(const char * msg) {
	int      no = WSAGetLastError();
	char     str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s:\n%s\n", msg, str_err);
	else
		printf("%s:\nномер ошибки %d\n", msg, no);

	return no;
}

int set_nonblock(int fd) {
	int flags;
#if defined(O_NONBLOCK) 
	if(-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else 
	flags = 1;
	return ioctlsocket(fd, FIONBIO, &flags);
#endif 
}
int set_repitable(int sfd) {
	int optval = 1;
	return setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
}
void close_socket(int fd) {
	shutdown(fd, SD_BOTH);
	closesocket(fd);
}

struct Client * init_Client(int sfd) {
	struct Client * new_Client = (struct Client*)malloc(sizeof(struct Client));
	memset(new_Client, 0, sizeof(struct Client));

	new_Client->sfd = sfd;
	new_Client->type = READ;
	new_Client->hEvent = WSACreateEvent();

	new_Client->data = (char*)malloc(MAX_HEAD_HTTP);
	memset(new_Client->data, 0, MAX_HEAD_HTTP);
	++cnt_clients;
	return new_Client;
}
struct Client * release_Client(struct Client * cln) {
	if(cln != NULL) {
		free(cln->data);		
		close_socket(cln->sfd);
		CloseHandle(cln->hEvent);
		free(cln);
		--cnt_clients;
	}

	return NULL;
}
struct Client * clear_Client(struct Client * cln) {
	if(cln != NULL) {		
		free(cln->data);
		cln->len = cln->cur = 0;

		cln->type = READ;
		cln->data = (char*)malloc(MAX_HEAD_HTTP);
		memset(cln->data, 0, MAX_HEAD_HTTP);
				
	}

	return cln;
}

char * format200 = "HTTP / 1.1 200 OK\r\nVersion: HTTP / 1.1\r\n"
		         "Content-Type: text/html; charset=utf-8\r\n"
				 "Content-Length: %d\r\n\r\n%s";

BOOL work(struct Client * cln) {
	BOOL isOk = TRUE;

	char * response = malloc(sizeof(char)*(cln->len + strlen(format200)));
	sprintf(response, format200, cln->len, cln->data);
	free(cln->data);
	cln->data = response;
	cln->len = strlen(response);

	/*еще не отправлено ничего*/
	cln->cur = 0;
	cln->type = WRITE;

	return isOk;
}

int loop(int msfd) {
	BOOL is_repeat = TRUE;

	/*создаем в ядре событие, для отслеживания асинхронных операций на мастер сокете*/
	WSAEVENT hEvent_master = WSACreateEvent();

	/*мультиплексирование на событиях*/	
	root_client = tail_client = NULL;
	while(is_repeat) {
		int cnt = 0;		
		/*взводим события для master socket*/
		int rc = WSAEventSelect(msfd, hEvent_master, FD_ACCEPT | FD_CLOSE);
		++cnt;
		if(rc == SOCKET_ERROR) {
			show_err_wsa("Ошибка select на master socket");
			return 1;
		}
		WSAEVENT * pEvents = malloc((cnt_clients+1)*sizeof(WSAEVENT));
		pEvents[0] = hEvent_master;	

		/*взводим события для socket клиентов*/		
		struct Client * cur_client = root_client;
		while(cur_client != NULL) {
			if(cur_client->type == READ) {
				rc = WSAEventSelect(cur_client->sfd, cur_client->hEvent, FD_READ | FD_CLOSE);
			} else if(cur_client->type == WRITE) {
				rc = WSAEventSelect(cur_client->sfd, cur_client->hEvent, FD_WRITE | FD_CLOSE);
			}
			if(rc == SOCKET_ERROR) {
				show_err_wsa("Ошибка select на slave socket");
				return 1;
			}
			pEvents[cnt++] = cur_client->hEvent;			
			cur_client = cur_client->next;
		}

		rc = WSAWaitForMultipleEvents(cnt, pEvents, FALSE, INFINITE, FALSE);
		if(rc == -1) {
			show_err_wsa("Ошибка WSAWaitForMultipleEvents");
			break;
		}

		cur_client = root_client;
		WSANETWORKEVENTS hEvent;

		struct Client *last_client = NULL;
		while(cur_client != NULL) {
			WSAEnumNetworkEvents(cur_client->sfd, cur_client->hEvent, &hEvent);
			if(cur_client->type == READ && hEvent.lNetworkEvents & FD_READ &&	hEvent.iErrorCode[FD_READ_BIT] == 0) {
				//получение очередного фрагмента сообщения
				char data[LEN];
				int len = recv(cur_client->sfd, data, LEN, 0);
				if(len == 0) {
					//все ошибки считаем разрывом соединения 
					if(WSAGetLastError())
						show_err_wsa("Ошибка получения фрагмента от клиента");

					if(last_client == NULL)
						root_client = cur_client->next;
					else
						last_client->next = cur_client->next;

					if(cur_client == tail_client)
						tail_client = last_client;

					cur_client = release_Client(cur_client);
					cur_client = last_client;
				} else if(len > 0) {
					/*добавляем форагмент сообщения к контексту клиента*/
					strncpy(cur_client->data + cur_client->len / sizeof(char), data, len);
					cur_client->len += len;
					cur_client->data[cur_client->len] = '\0';

					if(!strncmp(cur_client->data + cur_client->len - 4, "\r\n\r\n", 4)) {
						//все сообщение получено												
						work(cur_client);
					} else if(!strncmp(cur_client->data, "OFF", 5)) //!!!!условие выхода!!!!!
						is_repeat = FALSE;
				}
			} else if(cur_client->type == WRITE && hEvent.lNetworkEvents & FD_WRITE &&	hEvent.iErrorCode[FD_WRITE_BIT] == 0) {
				/*отправка данных*/
				int len_ = cur_client->len - cur_client->cur;// LEN > (cur_client->len - cur_client->cur) ? cur_client->len - cur_client->cur : LEN;
				int len = send(cur_client->sfd, cur_client->data + cur_client->cur, len_, 0);
				if(len == SOCKET_ERROR) {
					//разрыв соединения
					show_err_wsa("Ошибка отправки фрагмента клиенту");

					if(last_client == NULL)
						root_client = cur_client->next;
					else
						last_client->next = cur_client->next;
					if(cur_client == tail_client)
						tail_client = last_client;

					cur_client = release_Client(cur_client);
					cur_client = last_client;
				} else if(len > 0) {
					cur_client->cur += len;
					if(cur_client->cur == cur_client->len)
						clear_Client(cur_client); //все отправлено															
				}
			}

			last_client = cur_client;
			cur_client = cur_client->next;
		}

		/*проверяем master socket*/
		WSAEnumNetworkEvents(msfd, hEvent_master, &hEvent);
		if(hEvent.lNetworkEvents & FD_ACCEPT &&	hEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
			/*принимаем новое соединение (только такие события на мастере)*/
			int sfd_slave = accept(msfd, NULL, 0);
			if(sfd_slave != -1) {
				set_nonblock(sfd_slave);
				struct Client * cur_client = init_Client(sfd_slave);
				if(root_client == NULL)
					root_client = tail_client = cur_client;
				else
					tail_client = tail_client->next = cur_client;
			} else if(WSAEWOULDBLOCK != WSAGetLastError())
				show_err_wsa("Не удалось принять соединение");
		}

		free(pEvents);
	}
	

	/*освобождаем ресурсы*/
	struct Client *cur_client = root_client;
	while(cur_client != NULL) {
		struct Client *tmp = cur_client;
		cur_client = cur_client->next;
		release_Client(tmp);
	}
	root_client = tail_client = NULL;

	

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
	int msfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	/*делаем сокет не блокирующим*/
	res = set_nonblock(msfd);
	if(res == -1) {
		show_err_wsa("Не удалось установить опцию повторного использования мастер сокета");
		closesocket(msfd);
		return 3;
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