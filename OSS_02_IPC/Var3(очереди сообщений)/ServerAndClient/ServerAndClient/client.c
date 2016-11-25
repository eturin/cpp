#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>

#define LEN 256

int show_err(const char * msg);

/*клиент только пишет в очередь, но не может читать из неё (зато передавать может одновременно всем серверам)
  широковещательный путь такой \\*\mailslot\[Путь]ИмяКанала
*/
int start_client() {
	int res = 0;

	/*запрашиваем имя очереди сообщений*/
	printf("Укажите имя сервера: ");
	char server[LEN];
	scanf("%s", server);
	printf("Укажите имя очереди сообщений \\\\%s\\mailslot\\<имя?>: ",server);
	char name[LEN];
	scanf("%s", name);
	char path[LEN];
	sprintf(path, "\\\\%s\\mailslot\\%s", server, name);

	/*открываем существующую очередь*/
	HANDLE fd = CreateFile(path,           /*имя очереди*/
						   GENERIC_WRITE,  /*режим доступа*/
		                   FILE_SHARE_READ,/*режим совместного использования (серверу надо будет читать очередь тоже)*/
						   NULL,           /*доступ по AD*/
						   OPEN_EXISTING,  /*открывать существующий или ошибка*/
						   0,              /*атрибуты файла*/
						   NULL            /*особые атрибуты*/);
	while(getchar()!='\n');

	BOOL isRepeat = TRUE;
	while(isRepeat) {
		printf("Введите сообщение для очереди (для выхода - quit): ");
		char buf[LEN];
		gets(buf);

		if(!strcmp(buf, "quit"))
			isRepeat = FALSE;
		else {
			size_t len = 0;
			BOOL isOk = WriteFile(fd, buf, strlen(buf) + 1, &len, NULL); //если сервер погаснит, то сообщения будут лететь в никуда без ошибок
			if(!isOk)
				show_err("Ошибка записи сообщения в очередь");
			else if(strlen(buf)!=len-1)
				show_err("Не удалось отправить сообщение ЦЕЛИКОМ");
		}
	}


	/*закрываем дескриптор очереди*/
	CloseHandle(fd);

	return res;
}