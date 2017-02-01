#include "client_isapi.h"
#include "../SocketAsync/req.h"
#include "../SocketAsync/socket.h"

/*инициализация сведений о новом клиенте*/
struct Client_isapi * init_client_isapi(struct Server_isapi * psrv) {
	struct Client_isapi * pcln = malloc(sizeof(struct Client_isapi));
	memset(pcln, 0, sizeof(struct Client_isapi));
	pcln->psrv = psrv;
	
	return pcln;
}

/*исвобождение ресурсов занимаемых сведениями о клиенте*/
struct Client_isapi * release_client_isapi(struct Client_isapi * pcln) {
	if(pcln != NULL) {
		/*закрываем сокет связи с мастером*/
		close_socket(pcln->msfd);

		/*закрываем текущий сокет*/
		close_socket(pcln->sfd);
		
		/*освобождаем буфер обмена*/
		free(pcln->data);

		/*удаляем хеш-таблицу*/
		htab_delete_all(&pcln->pEnv);

		/*удаляем структуру разбора*/
		release_Req(pcln->preq);

		free(pcln);
	}
	return NULL;
}