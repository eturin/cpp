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
	/*режим текущий режим работы*/
	char type;

	/*тип воркера*/
	char name[LEN_WORKER];

	/*дескрипторы каналов STDIN, STDOUT и STDERR*/
	struct FD {
		HANDLE fd_r, fd_w;  /*дескрипторы каналов*/
		HANDLE iocp;        /*порт канала worker*/
	} fd[3];

	/*сведения о процессе*/
	PROCESS_INFORMATION procInf;

	/*настройки процесса*/
	STARTUPINFO sti;

	/*текущий клиент*/
	struct Client *pcln;	

	struct Worker * next;
};

/*инициализация worker*/
struct Worker * init_Worker(const char *, size_t, struct Client *, LPVOID);

/*освобождение ресурсов worker*/
struct Worker * release_Worker(struct Worker *);

BOOL work(struct Client *, LPVOID);

#endif