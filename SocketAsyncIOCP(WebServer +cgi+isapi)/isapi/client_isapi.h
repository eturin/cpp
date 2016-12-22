#ifndef CLIENT_ISAPI_H
#define CLIENT_ISAPI_H

#include "common_isapi.h"

extern int flag;

struct hTab; /*определена в req.h*/
struct Req;
struct Server_isapi;
struct Worker_isapi;


/*сокеты клиентов и их контекст*/
struct Client_isapi {	
	/*дескрипторы сокета*/
	int msfd, sfd;
	/*сведения о сокете*/
	WSAPROTOCOL_INFOW inf;

	size_t size; /*max объем данных    */
	size_t len;  /*общий объем данных  */
	size_t cur;  /*текущий сдвиг для синхронных операций*/
	char *data;  /*передаваемые/получаемые данные*/

	/*распарсенный запрос*/
	struct Req * preq;
	
	/*сетевой адрес*/
	struct sockaddr_in addr;
	
	/*переменные среды*/
	struct hTab *pEnv;

	/*сервер, связанный с клиентом*/
	struct Server_isapi * psrv;

	/*конкретная библиотека, обрабатывающая этот запрос*/
	struct Isapi *pIsapi;
};

/*инициализация сведений о новом клиенте*/
struct Client_isapi * init_client_isapi(struct Server_isapi *);

/*исвобождение ресурсов занимаемых сведениями о клиенте*/
struct Client_isapi * release_client_isapi(struct Client_isapi *);

/*формируем ответ 200*/
BOOL make200(struct Client_isapi*);
/*формируем ответ 404*/
BOOL make404(struct Client_isapi*);
/*формируем ответ 500*/
BOOL make500(struct Client_isapi*);

#endif