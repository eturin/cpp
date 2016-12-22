#include "common_isapi.h"

#include <locale.h>
#include "server_isapi.h"

int main() {
	
	/*�����������*/
	setlocale(LC_ALL, "russian");

	/*������������� �����*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR)
		show_err_wsa("������ ������������� �����");
	else {
		/*������� ������*/
		struct Server_isapi *psrv = init_server_isapi();
		if(psrv != NULL) {
			/*���������*/
			start_server_isapi(psrv);
			/*������� ������*/
			psrv=release_server_isapi(psrv);
		}
		/*������������ �����*/
		WSACleanup();
	}	

	system("pause");
	return 0;
}