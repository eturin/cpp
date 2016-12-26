#include "common_isapi.h"

#include <locale.h>
#include "server_isapi.h"

int main() {
	
	/*локализация*/
	setlocale(LC_ALL, "russian");

	/*инициализация среды*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR)
		show_err_wsa("(isapi)Ошибка инициализация среды");
	else {
		/*создаем сервер*/
		struct Server_isapi *psrv = init_server_isapi();
		if(psrv != NULL) {
			/*запускаем*/
			start_server_isapi(psrv);
			/*удаляем сервер*/
			psrv=release_server_isapi(psrv);
		}
		/*освобождение среды*/
		WSACleanup();
	}	
		
	return 0;
}