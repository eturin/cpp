#include "client.h"
#include "worker.h"
#include "socket.h"
#include "req.h"

int cnt = 0;
char * format200 =	"HTTP/1.1 200 OK\r\n"
                    "Version: HTTP / 1.1\r\n"
					"Cache-Control: no-cache\r\n"
                    "Content-Type: text/html; charset=windows-1251\r\n"					
                    "Content-Length: %d\r\n\r\n%s";

char * format404 =  "HTTP/1.1 404 ERROR\r\n"
					"Version: HTTP/1.1\r\n"
					"Cache-Control: no-cache\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 0 \r\n\r\n";

char * format500 =  "HTTP/1.1 500 ERROR\r\n"
				    "Version: HTTP/1.1\r\n"
					"Cache-Control: no-cache\r\n"
					"Content-Type: text/html\r\n"
					"Connection: close\r\n"
					"Content-Length: 0 \r\n\r\n";

struct Client * init_Client(struct Server * psrv) {	
	struct Client * pcln = (struct Client*)malloc(sizeof(struct Client));
	memset(pcln, 0, sizeof(struct Client));

	pcln->type = CLIENT;              /*первым полем указываем, что это клиент*/
	pcln->psrv = psrv;                /*св€зываем его с сервером*/

	/*готовим и форматируем буфер клиента*/
	pcln->size = MAX_HEAD_HTTP;
	pcln->data = malloc(MAX_HEAD_HTTP);
	memset(pcln->data, 0, MAX_HEAD_HTTP);
	pcln->DataBuf.len = LEN;
	pcln->DataBuf.buf = pcln->data;

	/*формируем структуру событи€, дл€ обслуживани€ асинхронных операций*/
	pcln->overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
														 TRUE, /*тип сброса TRUE - ручной*/
														 TRUE, /*начальное состо€ние TRUE - сигнальное*/
														 NULL  /*им€ обьекта*/);
	pcln->overlapped_inf.type = READ; /*перва€ асинхронна€ операци€ на клиенте - чтение из сокета*/
	
	return pcln;
}
struct Client * release_Client(struct Client * pcln) {
	if(pcln != NULL) {
		/*закрываем хендл событи€ в €дре*/
		WSACloseEvent(pcln->overlapped_inf.overlapped.hEvent);
		
		/*закрываем сокет*/
		close_socket(pcln->sfd);
		close_socket(pcln->wsfd);
		
		/*закрываем порт клиента (!!!!Ё“ќ ƒ≈Ћј“№ Ќ≈Ћ№«я!!!! т.к. отваливаетс€ порт завершени€ сервера)*/
		/////////////CloseHandle(pcln->iocp);

		/*освобождаем данные*/
		free(pcln->data);

		if(pcln->pwrk != NULL && pcln->pwrk->type==WORKER)
			release_Worker(pcln->pwrk);
		release_Req(pcln->preq);

		free(pcln);
	}

	return NULL;
}
struct Client * clear_Client(struct Client * pcln) {
	/*эту операцию следует выполн€ть не одновременно*/
	if(pcln != NULL) {
		pcln->overlapped_inf.type = READ;
		/*реинициализируем данные передачи*/
		free(pcln->data);
		pcln->len = pcln->cur = 0;
		pcln->size = MAX_HEAD_HTTP;
		pcln->data = malloc(pcln->size);			
		memset(pcln->data, 0, pcln->size);
		pcln->DataBuf.len = LEN;
		pcln->DataBuf.buf = pcln->data;
	}	

	return pcln;
}

BOOL make200(struct Client * pcln) {
	BOOL isOk = TRUE;	
	
	size_t len = pcln->len + strlen(format200)*sizeof(char) + 15; //15 байт на вставку %d
	char * response = malloc(len);
	pcln->data[pcln->len] = '\0';
	sprintf(response, format200, pcln->len, pcln->data);
	free(pcln->data);
	pcln->data = response;
	pcln->size = (strlen(response) + 1)*sizeof(char);
	pcln->len = pcln->size;
	/*еще не отправлено ничего*/	
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;
		
	return isOk;
}

BOOL make404(struct Client * pcln) {
	BOOL isOk = TRUE;	

	free(pcln->data); //запрос больше не нужен (на его месте будет ответ)
	pcln->size = (strlen(format404) + 1)*sizeof(char);
	pcln->len = pcln->size;
	pcln->data = malloc(pcln->size);
	memcpy(pcln->data, format404, pcln->size);

	/*еще не отправлено ничего*/	
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;
	pcln->pwrk = NULL;

	return isOk;
}

BOOL make500(struct Client * pcln) {
	BOOL isOk = TRUE;
	
	free(pcln->data); //запрос больше не нужен (на его месте будет ответ)
	pcln->size = (strlen(format500) + 1)*sizeof(char);
	pcln->len = pcln->size;
	pcln->data = malloc(pcln->size);
	memcpy(pcln->data, format500, pcln->size);

	/*еще не отправлено ничего*/	
	pcln->DataBuf.buf = pcln->data;
	pcln->cur = 0;
	pcln->pwrk = NULL;
	
	return isOk;
}