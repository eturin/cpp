#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

/*состояние сокета*/
#define READ     0
#define WRITE    1
#define WAIT     2

/*максимальный размер входящего сообщения*/
#define MAX_HEAD_HTTP 4096

/*максимальный размер буфера обмена асинхронной операции*/
#define LEN 10

/**/
extern int flag;

#ifndef REQ_H
struct Req;
#endif

#ifndef WORKER_H
struct Worker;
#endif

/*сокеты клиентов и их контекст*/
struct Client {
	/*режим чтения или записи в сокет*/
	char type;
	/*дескриптор сокета*/
	int sfd;
	
	DWORD len;  /*общий объем данных*/
	DWORD cur;  /*сколько уже передано*/
	char * data;/*передаваемые/получаемые данные*/

	/*распарсенный запрос*/
	struct Req * preq;
	
	/*структура, для выполнения асинхронных вызовов*/
	WSAOVERLAPPED overlapped;
		
	/*буфер обмена*/
	WSABUF DataBuf;

	/*порт клиента*/
	HANDLE iocp;
	
	/*сетевой адрес*/
	struct sockaddr_in addr;

	/*критическая секция этого клиента (для управления потоками)*/
	CRITICAL_SECTION cs;

	/*назначенный worker*/
	struct Worker * pwrk;
};

/*инициализация сведений о новом клиенте*/
struct Client * init_Client();

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