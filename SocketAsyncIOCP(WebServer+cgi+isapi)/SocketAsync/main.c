#include "server.h"
#include "error.h"
#include <locale.h>

int main(int argc, char **argv) {
	/*локализация*/
	setlocale(LC_ALL, "russian");
	/*инициализация*/
	struct Server *psrv=init_Server();
	/*заполнение из параметров командной строки*/
	if(argc>1) {
		int i = 1;
		while(i<argc)
			if(!strcmp(argv[i], "-d")) {
				++i;				
				strcpy(psrv->work_path, argv[i]);
				++i;
			}else if(!strcmp(argv[i], "-p")) {
				++i;				
				strcpy(psrv->work_path, argv[i]);
				++i;
			}else if(!strcmp(argv[i], "-h")
					  || !strcmp(argv[i], "/?")
					  || !strcmp(argv[i], "/h")
					  || !strcmp(argv[i], "/help")) {
				printf("Используйте программу со следующими параметрами:\n"
					   "\t-h\n\t\tдля получения этой справки\n"
					   "\t-p\n\t\tпрослушиваемый порт мастер-сокета\n"
					   "\t-d \"<путь к каталогу с обработчиками>\"\n\t\tдля указания каталога с программами обработчиками\n");
				system("pause");
				return 0;
			} else
				++i;
	}
	/*проверка заполнения из командной строки*/
	if(strlen(psrv->work_path) == 0)
		/*каталог вызова программы*/		
		memcpy(psrv->work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
	else if(psrv->work_path[strlen(psrv->work_path) - 1] != '\\')
		psrv->work_path[strlen(psrv->work_path) - 1] = '\\';

	if(!SetCurrentDirectory(psrv->work_path)) {
		show_err("Не удалось зайти в каталог с обработчиками", TRUE);
		return 1;
	}

	if(psrv->port == 0)
		psrv->port = 12001;

	/*инициализация среды*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) 
		show_err_wsa("Ошибка инициализация среды");		
	else {
		/*запуск сервера*/
		start_server(psrv);

		/*освобождение среды*/
		WSACleanup();
	}
	
	/*освобождаем ресурсы сервера*/
	free(psrv);

	system("pause");
	return 0;
}