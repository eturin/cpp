#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

extern int flag;

struct Req;
struct Worker;
struct Server;

/*структура определяющая тип асинхронной операции*/
struct Overlapped_inf {
	WSAOVERLAPPED overlapped;
	char type;
};

/*сокеты клиентов и их контекст*/
struct Client {
	/*Признак типа*/
	char type;

	/*дескрипторы сокетов*/
	int sfd, wsfd;
	/*признак обмена по зашифрованному каналу*/
	BOOL crypt;
	/*указатель на структуру дополнительных сведений*/
	void * ext;
	
	DWORD size; /*max объем данных    */
	DWORD len;  /*общий объем данных  */
	DWORD cur;  /*сколько уже передано*/
	char *data; /*передаваемые/получаемые данные*/
	
	/*буфер обмена*/
	WSABUF DataBuf;
	
	/*распарсенный запрос*/
	struct Req * preq;
	
	/*структура, для выполнения асинхронных вызовов*/
	struct Overlapped_inf overlapped_inf;	

	/*порт клиента*/
	HANDLE iocp;
	
	/*сетевой адрес*/
	struct sockaddr_in addr;		

	/*назначенный worker*/
	struct Worker * pwrk;

	/*сервер, связанный с клиентом*/
	struct Server * psrv;
};

/*инициализация сведений о новом клиенте*/
struct Client * init_Client(struct Server *);

/*сброс сведений о клиенте в исходное состояние*/
struct Client * clear_Client(struct Client *);

/*исвобождение ресурсов занимаемых сведениями о клиенте*/
struct Client * release_Client(struct Client *);

/*формируем ответ 200*/
BOOL make200(struct Client*);
/*формируем ответ 404*/
BOOL make404(struct Client*);
/*формируем ответ 500*/
BOOL make500(struct Client*);

#endif