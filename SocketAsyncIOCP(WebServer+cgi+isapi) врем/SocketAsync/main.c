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
		while(i < argc) {
			if(!strcmp(argv[i], "-d")) 				
				strcpy(psrv->work_path, argv[++i]);				
			else if(!strcmp(argv[i], "-p") && !sscanf(argv[++i], "%d", &psrv->port))
				printf("Не удалось выделить открытый порт из аргументов коммандной строки\n");
			else if(!strcmp(argv[i], "-sp") && !sscanf(argv[++i], "%d", &psrv->sport))
				printf("Не удалось выделить порт кодированных сообщений из аргументов коммандной строки\n");
			else if(!strcmp(argv[i], "-ip") && !sscanf(argv[++i], "%d", &psrv->iport))
				printf("Не удалось выделить открытый порт isapi из аргументов коммандной строки\n");
			else if(!strcmp(argv[i], "-sf"))
				strcpy(psrv->cert_path, argv[++i]);
			else if(!strcmp(argv[i], "-sn"))
				strcpy(psrv->cert_name, argv[++i]);
			else if(!strcmp(argv[i], "-ss"))
				strcpy(psrv->cert_path, argv[++i]);
			else if(!strcmp(argv[i], "-h")
					  || !strcmp(argv[i], "/?")
					  || !strcmp(argv[i], "/h")
					  || !strcmp(argv[i], "/help")) {
				printf("Используйте программу со следующими параметрами:\n"
					   "  -h\n"
					   "\tДля получения этой справки\n"
					   "  -p <номер порта>\n"
					   "\tПрослушиваемый сервером открытый порт, для не зашифрованных сообщений (поумолчанию 12001)\n"
					   "  -sp <номер порта>\n"
					   "\tПрослушиваемый сервером закрытый порт, для зашифрованных сообщений (поумолчанию 12443)\n"
					   "  -sf \"<путь к файлу импорта сертификата>\"\n"
					   "\tДля утсановки зашифрованного соединения требуется сертификат:\n"
					   "\t\t1) созаем сертификат в личном хранилище(сразу пару ключей)\n"
					   "\t\t\tmakecert -pe -ss my\n"
					   "\t\t2) экспортируем сертификат вместе с закрытым ключем в файл без цепочки родителей\n"
					   "\t\t3) удаляем сертификат из хранилища(не обязательно)\n"
					   "  -sn \"<на кого выписан сертификат>\"\n"
					   "  -ss \"<имя хранилища>\"\n"
					   "\tСертификат может быть установлен в личное хранилище\n"
					   "\t\tmakecert -n \"CN=<на кого выписан сертификат>\" -pe -ss <имя хранилища>\n"
					   "  -ip <номер порта>\n"
					   "\tПрот прослушиваемый сервисом isapi (поумолчанию на единицу больше открытого порта)\n"
					   "  -d \"<путь к каталогу с обработчиками>\"\n"
					   "\tДля указания каталога с программами-обработчиками\n");
				//system("pause");
				return 0;
			} 
			
			++i;
		}
	}
	/*проверка заполнения из командной строки*/
	if(strlen(psrv->work_path) == 0)
		/*каталог вызова программы*/		
		memcpy(psrv->work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
	else if(psrv->work_path[strlen(psrv->work_path) - 1] != '\\')
		psrv->work_path[strlen(psrv->work_path) - 1] = '\\';

	if(!SetCurrentDirectory(psrv->work_path)) {
		show_err("(master)Не удалось зайти в каталог с обработчиками", TRUE);
		return 1;
	}

	if(psrv->port == 0)
		psrv->port = 12001;
	if(psrv->sport == 0)
		psrv->sport = 12443;
	if(psrv->iport == 0)
		psrv->iport = psrv->port+1;
	if(psrv->port == psrv->sport
	   || psrv->port == psrv->iport
	   || psrv->sport == psrv->iport
	   || psrv->port<=0
	   || psrv->sport<=0
	   || psrv->iport<=0) {
		printf("Некоторые прослушиваемые порты заданы не верно:\n\tоткрытый порт: %d\n\tзакрытый порт: %d\n\tпорт isapi: %d\n", psrv->port, psrv->sport, psrv->iport);
		return 1;
	}
	
	/*инициализация среды*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) 
		show_err_wsa("(master)Ошибка инициализация среды");		
	else {
		/*запуск сервера*/
		start_server(psrv);

		/*освобождение среды*/
		WSACleanup();
	}
	
	/*освобождаем ресурсы сервера*/
	free(psrv);

	//system("pause");
	return 0;
}