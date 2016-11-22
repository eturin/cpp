#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#define LEN 256
int show_err(const char*);

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	/*спрашиваем имя сервера*/
	printf("Имя сервера: ");
	char server[LEN];
	scanf("%s",server);

	/*спрашиваем имя канала*/
	printf("Укажите имя именованного канала: \\\\%s\\pipe\\<имя>\n\tимя: ",server);
	char name[LEN];
	scanf("%s", name);
	while(getchar() != '\n');

	/*формируем конечное имя*/
	char path[LEN];
	sprintf(path, "\\\\%s\\pipe\\%s", server, name);

	printf("Соединение с %s...",path);
	/*Ждем свободный канал бесконечно*/
	WaitNamedPipe(path, NMPWAIT_WAIT_FOREVER);

	/*открываем существующий канал*/
	HANDLE fd=CreateFile(path,                              /*имя канала*/
						 GENERIC_READ | GENERIC_WRITE,      /*режим доступа*/
						 FILE_SHARE_READ | FILE_SHARE_WRITE,/*режим совместного использования*/ 
		                 NULL,                              /*особые атрибуты*/ 
		                 // защиты 
		                 OPEN_EXISTING,                     /*открывать существующий или ошибка*/
		                 0,                                 /*атрибуты файла*/
						 NULL                               /*особые атрибуты*/);  
	if(fd == INVALID_HANDLE_VALUE) {
		printf(" неудача!\n");
		show_err("Не удалось открыть существующий канал");
	} else {
		printf(" ok!\n");
		BOOL bRes = TRUE;
		DWORD len_in = 0, len_out = 0, cnt=1, flags=0;
		/*получение свойств канала*/
		bRes=GetNamedPipeInfo(fd,       /*дескриптр канала*/
			&flags,   /*флаги типа канала*/
			&len_out, /*размера выходного буфера*/
			&len_in,  /*размера входного буфера*/
			&cnt      /*количество реализаций*/);
		if(!bRes)
			show_err("Ошибка получения свойств канала");
		else
			printf("Свойства именованного канала:\n\tразмера выходного буфера: %d байт\n\tразмера входного буфера: %d байт\n\tколичество реализаций: %d\n",len_out,len_in,cnt);


		while(bRes){
			char *buf=malloc(len_out*sizeof(char));
			printf("Введите сообщение: ");
			gets(buf);

			/*запись в канал*/
			DWORD len = 0;
			bRes = WriteFile(fd,              /*дескриптор канала*/
							 buf,             /*записываемые данные*/
							 strlen(buf) + 1, /*размер записываемых данных (если не весь объем задать, то отправит указанное количество байт)*/
							 &len,            /*фактически записано байт*/
							 NULL);

			if(!bRes || (strlen(buf) + 1 != len)) {
				show_err("Ошибка записи в канал");
				free(buf);
				break;
			} else 
				printf("Отправлено.\n");
			
			free(buf);
			buf = malloc(len_in*sizeof(char));
			/*чтение из канала*/
			bRes = ReadFile(fd,         /*дескриптор канала*/
				            buf,        /*принимаемые данные*/
							len_in,     /*размер для приема (если не весь объем задать, то прочитает указанное количество байт)*/
				            &len,       /*фактически прочитано байт*/
				            NULL);  

			if(!bRes || 0 == len) {
				show_err("Ошибка чтение из канала");
				free(buf);
				break;
			} else {
				buf[len] = '\0';
				printf("Получено: %s\n", buf);
			}
			
			free(buf);			
		}

		/*закрываем дескриптор канала*/
		CloseHandle(fd);
	}

	return 0;
}

int show_err(const char * msg) {
	int      no = GetLastError();
	char  str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s:\n%s\n", msg, str_err);
	else
		printf("%s:\nномер ошибки %d\n", msg, no);

	return no;
}