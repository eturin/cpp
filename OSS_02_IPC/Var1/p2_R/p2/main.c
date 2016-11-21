#define _CRT_SECURE_NO_WARNINGS


#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

#define LEN 250

struct Share {
	int a, b;
	char str[LEN - 4 * 2];
};

int show_err(const char*);

int main() {
	setlocale(LC_ALL, "russian");

	printf("Укажите имя объекта разделяемой памяти в ядре OS: ");	
	char name[LEN];
	scanf("%s",name);
	
	/*Открываем существующий объект общей памяти на чтение*/
	HANDLE fd=OpenFileMapping(FILE_MAP_READ,FALSE,name);
	if(fd == NULL)
		show_err("Не удалось открыть объект общей памяти в ядре OS");
	else {
		/*проецируем объект общей памяти на структуру*/
		struct Share * psh = MapViewOfFile(fd, FILE_MAP_READ, 0, 0, 0 /*используем весь объем*/);

		/*читаем общую память (!!!осторожно мьютекс не стоит!!!)*/
		while(getchar() != 'q')
			printf("a=%d, b=%d, str=%s\n", psh->a, psh->b, psh->str);

		/*отключаемся от общей памяти*/
		UnmapViewOfFile(psh);
	}

	/*закрываем дескриптор*/
	CloseHandle(fd); //фактически закроется объект общей памяти, когда отключится последний (а до того, можно даже снова подключаться другим)

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