#define _CRT_SECURE_NO_WARNINGS


#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

#define LEN 256

int show_err(const char*);

int main() {
	setlocale(LC_ALL, "russian");

	/*спрашиваем имя именованного канала*/
	printf("Укажите имя канала \\\\.\\pipe\\<имя>\n\tимя: ");
	char buf[LEN], path[LEN];
	scanf("%s", buf);
	/*строим локальное имя (имя своего компьютера заменили точкой)*/
	sprintf(path, "\\\\.\\pipe\\%s", buf);

	/*создаем в ядре OS именованный канал*/
	HANDLE fd = INVALID_HANDLE_VALUE;
	fd=CreateNamedPipe(path,                        /*имя канала*/
		               PIPE_ACCESS_DUPLEX,          /* режим открытия канала (в данном случае - канал двунаправленный )*/
					   PIPE_TYPE_BYTE | PIPE_WAIT,  /* режим работы канала (в данном случае - передача байт и блокирующий)*/
					   PIPE_UNLIMITED_INSTANCES,    /* максимальное количество реализаций канала (т.е. сколько может быть клиентов)*/
		               LEN,                         /*размер выходного буфера в байтах (придется ждать освобождения)*/
		               LEN-10,                      /*размер входного буфера в байтах (придется ждать освобождения)*/
		               0,                           /*время ожидания в миллисекундах (для блокирующего режима работы NMPWAIT_USE_DEFAULT_WAIT)*/
					   NULL                         /*адрес структуры с особыми атрибутами*/);
	if(fd == INVALID_HANDLE_VALUE)
		show_err("Ошибка создания именованного канала");
	else{ 
		/*подключаемся к именованному каналу (заблокируется в блокирующим режиме)*/
		if(!ConnectNamedPipe(fd /*дескриптор именованного канала*/, NULL/*адрес структуры OVERLAPPED (для асинхронного режима)*/)) 
			show_err("Статус");
		else {
			BOOL bRes = TRUE;
			while(bRes) {
				DWORD len = 0;
				bRes=ReadFile(fd, buf, sizeof(buf), &len, NULL);      //блокируется на время чтения
				printf("Получено: %s\n",buf);
				bRes=WriteFile(fd, buf, strlen(buf) + 1, &len, NULL); //блокируется на время записи				
				FlushFileBuffers(fd);
			}			
			/*отключаемся от именованного канала*/
			DisconnectNamedPipe(fd);
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