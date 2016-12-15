#ifndef WORKER_H
#define WORKER_H

#include "common.h"

/*рабочий каталог*/
extern char work_path[];

/*максимальный размер имени worker*/
#define LEN_WORKER 50

struct Client;

/*процессы обработчиков*/
struct Worker {
	/*Признак типа*/
	char type;

	/*имя воркера*/
	char name[LEN_WORKER];
	/*относительный путь до программы*/
	char abs_path[256];
	/*относительный путь до программы*/
	char path[256];

	/*дескрипторы каналов STDIN, STDOUT и STDERR*/
	struct FD {
		HANDLE fd_r, fd_w;  /*дескрипторы каналов*/
		HANDLE iocp;        /*порт канала worker*/
		DWORD size;         /*max объем данных    */
		DWORD len;          /*общий объем данных  */
		DWORD cur;          /*сколько уже передано*/
		char * data;        /*передаваемые/получаемые данные*/
		
		/*структура, для выполнения асинхронных вызовов*/
		struct overlapped_inf_w {
			WSAOVERLAPPED overlapped;
			char type;
			CRITICAL_SECTION * pcs; /*критическая секция этого клиента (для управления потоками)*/
		} overlapped_inf;
	} fd[3];

	/*сведения о процессе*/
	PROCESS_INFORMATION procInf;

	/*настройки процесса*/
	STARTUPINFO sti;

	/*текущий клиент*/
	struct Client *pcln;		
};

/*инициализация worker*/
struct Worker * init_Worker(const char *, size_t, struct Client *, LPVOID);

/*освобождение ресурсов worker*/
struct Worker * release_Worker(struct Worker *);

BOOL work(struct Client *, LPVOID);

#endif