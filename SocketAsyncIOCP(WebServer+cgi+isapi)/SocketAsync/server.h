#ifndef SERVER_H
#define SERVER_H

#include "common.h"

/*количество событий ожидаемых в главной потоке*/
#define MAX_EVENTS 2

struct Server {
	/*Признак типа*/
	char type;
	/*имя программы*/
	char *name;
	/*рабочий каталог, в котором будет работать сервер*/
	char work_path[256];
	/*мастер-сокет сервера*/
	int msfd;
	/*прослушиваемый порт*/
	int port;
	/*прослушиваемый сетевой адрес*/
	struct sockaddr_in addr;
	/*Каталог событий*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*событие принудительной остановки (отправляется из поточных функций главному потоку по команде OFF)*/
	WSAEVENT *phEvent_STOP;
	/*порт очереди событий*/
	HANDLE iocp;
};


struct Server * init_Server();
int start_server(struct Server*);
BOOL start_async(void*, DWORD, LPVOID, struct overlapped_inf*);

#endif