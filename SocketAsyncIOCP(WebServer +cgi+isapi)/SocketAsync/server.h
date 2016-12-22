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

	/*Каталог событий (создается и закрывается в loop)*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*событие принудительной остановки хранится в hEvents (отправляется из поточных функций главному потоку по команде OFF)*/
	WSAEVENT *phEvent_STOP;
	
	/*порт очереди событий (создается и закрывается в loop)*/
	HANDLE iocp;

	/*сведения о процессе, обслуживающем загружаемые библиотеки*/
	struct Worker * pWIsapi;
	
	/*критическая секция, для инициализации одного процесса isapi*/
	CRITICAL_SECTION cs;
};


struct Server * init_Server();
int start_server(struct Server*);
BOOL start_async(void*, DWORD, LPVOID, struct Overlapped_inf*);

#endif