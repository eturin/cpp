#include "client.h"
#include "worker.h"
#include "socket.h"

int cnt = 0;
char * format200 =	"HTTP / 1.1 200 OK\r\nVersion: HTTP / 1.1\r\n"
                    "Content-Type: text/html; charset=utf-8\r\n"
                    "Content-Length: %d\r\n\r\n%s";

char * format404 =  "HTTP/1.1 404 ERROR\r\n"
					"Version: HTTP/1.1\r\n"
					"Content-Type: text/html; charset=utf-8\r\n"
					"Content-Length: 0 \r\n\r\n";

char * format500 =  "HTTP/1.1 500 ERROR\r\n"
				    "Version: HTTP/1.1\r\n"
					"Content-Type: text/html; charset=utf-8\r\n"
					"Connection: close\r\n"
					"Content-Length: 0 \r\n\r\n";

struct Client * init_Client(struct Server * psrv) {
	++cnt;
	struct Client * new_Client = (struct Client*)malloc(sizeof(struct Client));
	memset(new_Client, 0, sizeof(struct Client));

	new_Client->psrv = psrv;
	new_Client->sfd = 0;
	new_Client->type = READ;

	new_Client->len = new_Client->cur = 0;
	new_Client->data = malloc(MAX_HEAD_HTTP);
	new_Client->DataBuf.len = LEN;
	new_Client->DataBuf.buf = new_Client->data;

	/*формируем структуру события*/
	new_Client->overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
												TRUE, /*тип сброса TRUE - ручной*/
												TRUE, /*начальное состояние TRUE - сигнальное*/
												NULL  /*имя обьекта*/);
	
	/*инициализация критической секции*/
	InitializeCriticalSection(&new_Client->cs);

	return new_Client;
}
struct Client * release_Client(struct Client * pcln) {
	/*эту операцию следует выполнять не одновременно*/
	if(pcln != NULL && TryEnterCriticalSection(&pcln->cs)) {
		/*закрываем хендл события в ядре*/
		WSACloseEvent(pcln->overlapped.hEvent);
		/*закрываем порт клиента*/
		//if(pcln->iocp) CloseHandle(pcln->iocp); //валится ошибка

		/*освобождаем данные*/
		free(pcln->data);
		/*закрываем сокет*/
		close_socket(pcln->sfd);
		LeaveCriticalSection(&pcln->cs);
		DeleteCriticalSection(&pcln->cs);
		if(pcln->pwrk != NULL)
			pcln->pwrk=release_Worker(pcln->pwrk);

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

BOOL make200(struct Client * pcln) {
	BOOL isOk = TRUE;
	pcln->type = WAIT;
	
	size_t len = pcln->len + strlen(format200)*sizeof(char) + 15; //15 байт на вставку %d
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
	pcln->pwrk = NULL;
	
	return isOk;
}

BOOL make404(struct Client * pcln) {
	BOOL isOk = TRUE;
	pcln->type = WAIT;

	free(pcln->data); //запрос больше не нужен (на его месте будет ответ)
	pcln->len = (strlen(format404) + 1)*sizeof(char);
	pcln->data = malloc(pcln->len);
	memcpy(pcln->data, format404, pcln->len);	

	/*еще не отправлено ничего*/
	pcln->type = WRITE;
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;
	pcln->pwrk = NULL;

	return isOk;
}

BOOL make500(struct Client * pcln) {
	BOOL isOk = TRUE;
	pcln->type = WAIT;

	free(pcln->data); //запрос больше не нужен (на его месте будет ответ)
	pcln->len = (strlen(format500) + 1)*sizeof(char);
	pcln->data = malloc(pcln->len);
	memcpy(pcln->data, format500, pcln->len);

	/*еще не отправлено ничего*/
	pcln->type = WRITE;
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;
	pcln->pwrk = NULL;
	
	return isOk;
}