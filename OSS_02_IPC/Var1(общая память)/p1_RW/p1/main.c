#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#define LEN 250

struct Share {
	int a, b;
	char str[LEN-4*2];
};

int show_err(const char*);

int main(int argc, char ** argv) {
	setlocale(LC_ALL, "russian");

	/*запрашиваем имя файла, который будем отображать в память*/
	printf("Укажите имя разделяемого файла: ");
	char name[LEN];
	gets(name);
		
	printf("Пробуем открыть объект общей памяти в ядре OS: %s...", name);
	/*открываем объект общей памяти в ядре OS*/
	HANDLE fd = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, /*режим доступа*/
								FALSE,                          /*наследование*/
								name                            /*имя проецируемого объекта*/);
	
	HANDLE fd_file = INVALID_HANDLE_VALUE;
	if(fd == NULL) {		
		/*создаем или открываем файл на диске в текущем каталоге*/
		fd_file = CreateFile(name,                              /*имя файла*/
							 GENERIC_READ | GENERIC_WRITE,      /*режим доступа*/
							 FILE_SHARE_READ | FILE_SHARE_WRITE,/*режим совместного доступа*/							 
							 NULL,                              /*NULL - это запрет наследования дочерними процессами*/
							 OPEN_ALWAYS,                       /*открыть существующий или создать новый*/
							 FILE_ATTRIBUTE_TEMPORARY,          /*атрибуты файла (в данном случае OS будет избегать записи на диск, если достаточно кеш-памяти)*/
							 NULL                               /*нет шаблона с дополнительными атрибутами*/);
		if(INVALID_HANDLE_VALUE != fd_file) {
			/*задаем размер файла*/
			SetFilePointer(fd_file, sizeof(struct Share), NULL, FILE_BEGIN); //сдвигаем указатель текущей позиции на LEN:0 от начала файла
			if(!SetEndOfFile(fd_file)) {
				printf(" неудача\n");
				show_err("Не удалось изменить размер файла на диске");
			}else{			
				/*создаем объект общей памяти в ядре OS*/
				fd = CreateFileMapping(fd_file,                         /*дескриптор файла, на основе которого создается разделяемая память*/
									   NULL,                            /*NULL - это запрет наследования дочерними процессами*/
									   PAGE_READWRITE,                  /*атрибуты защиты*/
									   0,                               /*старшее слово размера*/
									   0,                               /*и младшее слово размера (размер может быть огромен, а нули означают размер исходного файла)*/
									   name					            /*имя проецируемого объекта*/);
				if(fd == NULL) {
					printf(" неудача\n");
					show_err("Не удалось создать объект общей памяти в ядре OS");
					fd = INVALID_HANDLE_VALUE;
				} else
					printf("  создан\n");
			}
		} else
			show_err("Не удалось создать файл на диске");
	} else
		printf("  открыт\n");

	if(fd != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize = GetFileSize(fd, NULL);
		/*проецируем общую память на структуру*/
		struct Share * psh=NULL;
		if(NULL == (psh=MapViewOfFile(fd,                          /* дескриптор объекта общей памяти в ядре OS*/
			                       FILE_MAP_WRITE,                 /* режим доступа*/
			                       0,                              /* старшее DWORD смещения от начала*/
			                       0,                              /* младшее DWORD смещения от начала*/
			                       sizeof(struct Share)            /* число отображаемых байтов */)))
			show_err("Не удалось отобразить память на структуру");
		else {
			printf("Запись в общую память.\n");
			/*заполнение общей памяти*/
			do{
				printf("Укажите целые числа a и b: ");
				if(2 != scanf("%d %d", &psh->a, &psh->b))
					break;
				while(getchar() != '\n'); //забираем строку до конца
				printf("Укажите строку: ");
				gets(psh->str);
			} while(TRUE);

			/*отключение от общей памяти*/
			UnmapViewOfFile(psh);
		}
	}


	/*закрываем дескрипторы*/
	CloseHandle(fd_file);
	CloseHandle(fd);//фактически закроется объект общей памяти, когда отключится последний (а до того, можно даже снова подключаться другим)

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