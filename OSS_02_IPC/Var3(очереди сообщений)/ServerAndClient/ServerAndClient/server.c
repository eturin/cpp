#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>

#define LEN 256

int show_err(const char * msg);
int loop(HANDLE fd);

/*сервер только читает и не может писать в очередь*/
int start_server() {
	int res=0;

	/*запрашиваем имя очереди сообщений*/
	printf("Укажите имя очереди сообщений \\\\.\\mailslot\\<имя?>: ");
	char name[LEN];
	scanf("%s",name);
	char path[LEN];
	sprintf(path, "\\\\.\\mailslot\\%s", name);

	/*инициализация структуры доступа к каналу (для всех)*/
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	/*создаем в ядре OS очередь сообщений (допускают одновременную передачу данных и допускают широковещательный режим, т.е. один клиент может посылать сообщение одновременно всем серверам)*/
	HANDLE fd = CreateMailslot(path,                  /*имя очереди сообщений*/
						       0,                     /*максимальный размер одного сообщения (0-нет ограничений, но широко-вещательные не больше 400байт)*/
						       MAILSLOT_WAIT_FOREVER, /*таймаут ожидания при чтение (чтоб не читать бесконечные потоки)*/
						       &sa                    /*права доступа*/);

	if(fd == INVALID_HANDLE_VALUE) {
		show_err("Не удалось создать именованную очередь");
		return 1;
	}

	res=loop(fd);

	CloseHandle(fd);

	return res;
}


int loop(HANDLE fd) {
	int res = 0;
	
	BOOL isRepeat=TRUE;
	while(isRepeat) {
		char buf[LEN];
		size_t len = 0;
		BOOL isOk = ReadFile(fd,buf,LEN,&len,NULL);
		if(!isOk || len == 0)
			show_err("Ошибка чтения из очереди");
		else {
			buf[len] = '\0';
			printf("Прочитано сообщение: %s\n",buf);

			if(!strcmp(buf, "QUIT"))
				isRepeat = FALSE;
		}
	}

	return res;
}