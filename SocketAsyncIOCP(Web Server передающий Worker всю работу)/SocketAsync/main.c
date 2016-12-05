#include "server.h"
#include "error.h"
#include <locale.h>

int main(int argc, char **argv) {
	/*локализация*/
	setlocale(LC_ALL, "russian");

	/*заполнение из параметров командной строки*/
	if(argc>1) {
		int i = 1;
		while(i<argc)
			if(!strcmp(argv[i], "-d")) {
				++i;
				//mbstowcs(work_path, argv[i], strlen(argv[i]) + 1);
				strcpy(work_path, argv[i]);
				++i;
			} else if(!strcmp(argv[i], "-h")
					  || !strcmp(argv[i], "/?")
					  || !strcmp(argv[i], "/h")
					  || !strcmp(argv[i], "/help")) {
				printf("Используйте программу со следующими параметрами:\n"
					   "\t-h\n\t\tдля получения этой справки\n"
					   "\t-d \"<путь к каталогу с обработчиками>\"\n\t\tдля указания каталога с программами обработчиками\n");
				system("pause");
				return 0;
			} else
				++i;
	}
	/*проверка заполнения из командной строки*/
	if(strlen(work_path) == 0)
		/*каталог вызова программы*/
		//mbstowcs(work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
		memcpy(work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
	else if(work_path[strlen(work_path) - 1] != '\\')
		//work_path[wcslen(work_path) - 1] = L'\\';
		work_path[strlen(work_path) - 1] = '\\';

	if(!SetCurrentDirectory(work_path)) {
		show_err("Не удалось зайти в каталог с обработчиками", TRUE);
		return 1;
	}

	/*запуск сервера*/
	start_server();

	system("pause");
	return 0;
}