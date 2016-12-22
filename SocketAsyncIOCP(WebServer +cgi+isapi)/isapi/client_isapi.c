#include "client_isapi.h"
#include "../SocketAsync/req.h"
#include "../SocketAsync/socket.h"

/*������������� �������� � ����� �������*/
struct Client_isapi * init_client_isapi(struct Server_isapi * psrv) {
	struct Client_isapi * pcln = malloc(sizeof(struct Client_isapi));
	memset(pcln, 0, sizeof(struct Client_isapi));
	pcln->psrv = psrv;
	
	return pcln;
}

/*������������ �������� ���������� ���������� � �������*/
struct Client_isapi * release_client_isapi(struct Client_isapi * pcln) {
	if(pcln != NULL) {
		/*��������� ����� ����� � ��������*/
		close_socket(pcln->msfd);

		/*��������� ������� �����*/
		close_socket(pcln->sfd);
		
		/*����������� ����� ������*/
		free(pcln->data);

		/*������� ���-�������*/
		htab_delete_all(&pcln->pEnv);

		/*������� ��������� �������*/
		release_Req(pcln->preq);

		free(pcln);
	}
	return NULL;
}