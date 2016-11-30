#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#define _GNU_SOURCE
#include <stdio.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") 
#include <Windows.h>
#include <locale.h>
#include <string.h>

/*количество читаемых байт из сокета за один раз*/
#define LEN 100 
/*максимальный размер входящего сообщения*/
#define MAX_HTTP_REQUEST 4096

int show_err(const wchar_t*);
int set_nonblock(int);
int set_repitable(int);
extern size_t make_response(char**,size_t*);

/*сокеты клиентов и их контекст*/
struct Client {	
	BOOL is_read;
	int sfd;
	int len;
	int cur;
	char * data;
	struct Client *next;	
};

int main() {
	setlocale(LC_ALL, "Russian");
	/*Инициализация*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) {
		show_err(L"Ошибка инициализация среды");
		return -1;
	}
	
	/*получаем дескриптор мастер сокета*/
	int msfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(msfd == -1) {
		show_err(L"Не получен дескриптор сокета");		
		return 1;
	}

	/*связываем сокет с сетевым адресом (коротый хотим использовать)*/
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(12001);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");	
	int res = bind(msfd, (struct sockaddr*)&addr, sizeof(addr));
	if(res == -1) {
		show_err(L"Ошибка связывания мастер сокета с сетевым адресом"); 
		closesocket(msfd);
		return 2;
	}

	/*включаем повторное использование*/
	res=set_repitable(msfd);
	if(res != 0) {
		show_err(L"Не удалось установить опцию повторного использования мастер сокета");
		closesocket(msfd);
		return 2;
	}
	/*делаем сокет не блокирующим*/
	res = set_nonblock(msfd);
	if(res == -1) {
		show_err(L"Не удалось установить опцию повторного использования мастер сокета");
		closesocket(msfd);
		return 3;
	}

	/*начинаем слушать сетевой адрес*/
	res = listen(msfd, SOMAXCONN);
	if(res == -1) {
		show_err(L"Не удалось начать прослущивание сетевого адреса");
		closesocket(msfd);
		return 2;
	}

	/*сокеты клиентов и их контекст*/
	struct Client *root = NULL, *tail = root;
	
	/*мультиплексирование*/	
	fd_set set_r, set_w;
	BOOL is_repeat = TRUE;
	while(is_repeat) {
		/*инициализация*/
		FD_ZERO(&set_r);
		FD_ZERO(&set_w);
		
		/*регистрируем мастер сокет*/
		FD_SET(msfd, &set_r);
		
		/*регистрируем слейв сокеты*/
		int max = msfd;
		struct Client * cur = root;
		while(cur != NULL) {
			if(cur->is_read)
				FD_SET(cur->sfd, &set_r);
			else
				FD_SET(cur->sfd, &set_w);
			if(max<cur->sfd)
				max = cur->sfd;
			cur = cur->next;
		}

		/*засыпаем (эта команда сбрасывает не готовые биты поля, поэтому нужно регистрировать поновой)*/
		select(max + 1, &set_r, &set_w, NULL, NULL);

		/*проверяем слейв сокеты*/
		struct Client * last = NULL;
		cur = root;
		while(cur != NULL) {
			if(FD_ISSET(cur->sfd, &set_r)) {
				//получение очередного форагмента сообщения
				char data[LEN];
				int len = recv(cur->sfd, data, LEN, 0);
				if(len == 0) {
					//все ошибки считаем разрывом соединения 
					show_err(L"Ошибка получения фрагмента от клиента");
					shutdown(cur->sfd, SD_BOTH);
					closesocket(cur->sfd);
					free(cur->data);
					if(last == NULL)
						root = cur->next;
					else
						last->next = cur->next;
					if(cur == tail)
						tail = last;
					free(cur);
					cur = last;
				} else if(len>0) {
					/*добавляем форагмент сообщения к контексту клиента*/
					strncpy(cur->data + cur->len / sizeof(char), data, len);
					cur->len += len;
					cur->data[cur->len] = '\0';
					
					//!!!!условие выхода!!!!!
					if(!strncmp(cur->data, "OFF",5))
						is_repeat = FALSE;
					else if(!strncmp(cur->data + cur->len-4, "\r\n\r\n", 4)) {
						cur->is_read = FALSE;//все сообщение получено
						//подготовить ответ
						make_response(&cur->data, &cur->len);
					}
				}
			} else if(FD_ISSET(cur->sfd, &set_w)) {
				//отправка очередного фрагмента
				int len = send(cur->sfd, cur->data + cur->cur, LEN>(cur->len - cur->cur) ? cur->len - cur->cur : LEN, 0);
				if(len == SOCKET_ERROR) {
					//разрыв соединения
					show_err(L"Ошибка отправки фрагмента клиенту");
					shutdown(cur->sfd, SD_BOTH);
					closesocket(cur->sfd);
					free(cur->data);
					if(last == NULL)
						root = cur->next;
					else
						last->next = cur->next;
					if(cur == tail)
						tail = last;
					free(cur);
					cur = last;
				} else if(len>0) {
					cur->cur += len;
					 
					if(cur->cur == cur->len) {//все отправлено						
						cur->len = 0;
						cur->cur = 0;
						free(cur->data);
						cur->data = (char*)malloc(MAX_HTTP_REQUEST * sizeof(char));						
						cur->is_read = TRUE;
					}
				}
			}
			last = cur;
			if(cur != NULL)
				cur = cur->next;
		}

		/*проверяем мастер сокет*/
		if(FD_ISSET(msfd, &set_r)) {
			/*принимаем новое соединение (только такие события на мастере)*/
			int sfd_slave = accept(msfd, NULL, 0);
			if(sfd_slave != -1) {
				set_nonblock(sfd_slave);
				struct Client * cur = (struct Client*)malloc(sizeof(struct Client));
				cur->sfd = sfd_slave;
				cur->next = NULL;
				cur->is_read = TRUE;
				cur->len = 0;
				cur->cur = 0;
				cur->data = (char*)malloc(MAX_HTTP_REQUEST * sizeof(char));
				cur->data[0] = '\0';
				if(root == NULL)
					root = tail = cur;
				else
					tail = tail->next = cur;
			}else
				show_err(L"Не удалось принять соединение");
		}
	}

	/*освобождаем ресурсы*/
	struct Client * cur = root;
	while(cur != NULL) {
		free(cur->data);
		shutdown(cur->sfd, SD_BOTH);
		closesocket(cur->sfd);
		struct Client *tmp = cur;
		cur = cur->next;
		free(tmp);
	}
	root = tail = NULL;

	/*закрываем дескриптор мастер сокета*/
	shutdown(msfd, SD_BOTH);
	closesocket(msfd);
	WSACleanup();
	
	system("pause");
	return 0;
}

int show_err(const wchar_t * msg) {
	int      no = WSAGetLastError();
	wchar_t  str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		wprintf(L"%s:\n%s\n", msg, str_err);
	else
		wprintf(L"%s:\nномер ошибки %d\n", msg, no);	
	
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
	return setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

