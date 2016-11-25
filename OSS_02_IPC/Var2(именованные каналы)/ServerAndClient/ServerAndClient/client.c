#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define LEN 256

int show_err(const char * msg);

int StartClient() {
	

	printf("Укажите имя сервера: ");
	char buf_1[LEN];
	scanf("%s", buf_1);
	printf("Укажите имя именованного канала \\\\%s\\pipe\\<имя?>: ", buf_1);
	char buf_2[LEN];
	scanf("%s", buf_2);
	char path[LEN];
	sprintf(path, "\\\\%s\\pipe\\%s", buf_1, buf_2);

	printf("Соединение с %s...", path);
	/*Ждем свободный канал бесконечно*/
	WaitNamedPipe(path, NMPWAIT_WAIT_FOREVER);

	/*открываем существующий канал*/
	HANDLE fd = CreateFile(path,                              /*имя канала*/
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
		while(getchar() != '\n');
		
		BOOL bRes = TRUE;
		DWORD len_in = 0, len_out = 0, cnt = 1, flags = 0;
		/*получение свойств канала*/
		bRes = GetNamedPipeInfo(fd,       /*дескриптр канала*/
								&flags,   /*флаги типа канала*/
								&len_out, /*размера выходного буфера*/
								&len_in,  /*размера входного буфера*/
								&cnt      /*количество реализаций*/);
		if(!bRes)
			show_err("Ошибка получения свойств канала");
		else
			printf("Свойства именованного канала:\n\tразмера выходного буфера: %d байт\n\tразмера входного буфера: %d байт\n\tколичество реализаций: %d\n", len_out, len_in, cnt);


		do {
			printf("Введите команду: ");
			gets(buf_1);

			/*запись в канал*/
			DWORD len = 0;
			bRes = WriteFile(fd,                /*дескриптор канала*/
							 buf_1,             /*записываемые данные*/
							 strlen(buf_1) + 1, /*размер записываемых данных (если не весь объем задать, то отправит указанное количество байт)*/
							 &len,              /*фактически записано байт*/
							 NULL);

			if(!bRes || (strlen(buf_1) + 1 != len)) {
				show_err("Ошибка записи в канал");				
				continue;
			} 
									
			/*чтение из канала*/
			bRes = ReadFile(fd,       /*дескриптор канала*/
							buf_2,    /*принимаемые данные*/
							LEN,      /*размер для приема (если не весь объем задать, то прочитает указанное количество байт)*/
							&len,     /*фактически прочитано байт*/
							NULL);

			if(!bRes || 0 == len) {
				show_err("Ошибка чтение из канала");			
				continue;
			} else {
				buf_2[len] = '\0';
				printf("Получено: %s\n", buf_2);
			}
		} while(strcmp(buf_1, "quit"));

		/*закрываем дескриптор канала*/
		CloseHandle(fd);
	}
	return 0;
}