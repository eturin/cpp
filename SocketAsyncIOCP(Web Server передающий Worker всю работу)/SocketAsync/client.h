#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

/*состо€ние сокета*/
#define READ            0
#define WRITE           1
#define WAIT            2
#define READ_WORKER     3
#define WRITE_WORKER    4
#define READ_WORKER_ERR 5

/*максимальный размер вход€щего сообщени€*/
#define MAX_HEAD_HTTP 4096

/*максимальный размер буфера обмена асинхронной операции*/
#define LEN 10

/**/
extern int flag;

struct Req;

struct Worker;
struct Server;


/*сокеты клиентов и их контекст*/
struct Client {
	/*ѕризнак типа*/
	char type;

	/*дескриптор сокета*/
	int sfd;
	
	DWORD size; /*max объем данных    */
	DWORD len;  /*общий объем данных  */
	DWORD cur;  /*сколько уже передано*/
	char *data; /*передаваемые/получаемые данные*/
	
	/*буфер обмена*/
	WSABUF DataBuf;
	
	/*распарсенный запрос*/
	struct Req * preq;
	
	/*структура, дл€ выполнени€ асинхронных вызовов*/
	struct overlapped_inf {
		WSAOVERLAPPED overlapped;
		char type;		
		CRITICAL_SECTION * pcs; /*критическа€ секци€ этого клиента (дл€ управлени€ потоками)*/
	} overlapped_inf;
		
	

	/*порт клиента*/
	HANDLE iocp;
	
	/*сетевой адрес*/
	struct sockaddr_in addr;
		

	/*назначенный worker*/
	struct Worker * pwrk;

	/*сервер, св€занный с клиентом*/
	struct Server * psrv;
};

/*инициализаци€ сведений о новом клиенте*/
struct Client * init_Client(struct Server *);

/*сброс сведений о клиенте в исходное состо€ние*/
struct Client * clear_Client(struct Client *);

/*исвобождение ресурсов занимаемых сведени€ми о клиенте*/
struct Client * release_Client(struct Client *);

/*формируем ответ 200*/
BOOL make200(struct Client*);
/*формируем ответ 404*/
BOOL make404(struct Client*);
/*формируем ответ 500*/
BOOL make500(struct Client*);

#endif