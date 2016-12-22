#ifndef SERVER_ISAPI_H
#define SERVER_ISAPI_H

#include "common_isapi.h"

/*количество событий ожидаемых в главной потоке*/
#define MAX_EVENTS 2

struct hTab;

struct Server_isapi {
	/*имя сервера*/
	char *name;

	/*мастер-сокет сервера*/
	int msfd;
	/*прослушиваемый порт*/
	int port;
	/*прослушиваемый сетевой адрес*/
	struct sockaddr_in addr;	

	/*известные загружаемые библиотеки*/
	struct hTab * hISAPI;

	/*рабочий каталог, в котором будет работать сервер (из переменных среды)*/
	const char *work_path;

	/*Каталог событий (создается и закрывается в loop)*/
	WSAEVENT hEvents[MAX_EVENTS];
	/*событие принудительной остановки хранится в hEvents (отправляется из поточных функций главному потоку по команде OFF)*/
	WSAEVENT *phEvent_STOP;
};

struct Server_isapi * init_server_isapi();
struct Server_isapi * release_server_isapi(struct Server_isapi *);
BOOL start_server_isapi(struct Server_isapi * psrv);

#endif