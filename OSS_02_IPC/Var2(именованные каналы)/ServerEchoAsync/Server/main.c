#define _CRT_SECURE_NO_WARNINGS


#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

#define LEN 256
#define CNT 10

/*состояние реализации именованного канала*/
#define CONNECTING_STATE 0 //установка соединения
#define READING_STATE 1    //готов к чтению
#define WRITING_STATE 2    //готов к записи

typedef struct {
	OVERLAPPED Overlap;
	HANDLE fd_pipe;
	
	char  buf_in[LEN];
	DWORD len_in;

	char  buf_out[LEN];
	DWORD len_out;

	DWORD state;
	BOOL  wait_connect;
} PIPEINST;

int show_err(const char*);
BOOL Connect(HANDLE,OVERLAPPED*);
VOID DisconnectAndReconnect(PIPEINST* pipe);

int main() {
	setlocale(LC_ALL, "russian");

	/*спрашиваем имя именованного канала*/
	printf("Укажите имя канала \\\\.\\pipe\\<имя>\n\tимя: ");
	char buf[LEN], path[LEN];
	scanf("%s", buf);
	/*строим локальное имя (имя своего компьютера заменили точкой)*/
	sprintf(path, "\\\\.\\pipe\\%s", buf);

	/*создаем в ядре OS несколько реализаций именованного канала*/
	PIPEINST pipes[CNT];
	/*!!!!обязательная инициализация!!!!*/
	memset(pipes, 0, CNT*sizeof(PIPEINST));

	HANDLE hEvents[CNT];
	for(int i = 0; i < CNT; ++i) {
		/*формируем структуру события*/
		hEvents[i] = CreateEvent(NULL,   /*атрибут защиты*/
								 TRUE,	 /*тип сброса TRUE - ручной*/
								 TRUE,	 /*начальное состояние TRUE - сигнальное*/
								 NULL	 /*имя обьекта*/);
		if(hEvents[i] == NULL) {
			show_err("Не удалось создать структуру события");
			return 1;
		}

		pipes[i].Overlap.hEvent = hEvents[i];


		pipes[i].fd_pipe = CreateNamedPipe(path,                        /*имя канала*/
										   PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,/*режим открытия канала (в данном случае - канал двунаправленный и АСИНХРОННЫЙ )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*режим работы канала (в данном случае - передача байт и блокирующий)*/
										   CNT,                         /*максимальное количество реализаций канала (т.е. сколько может быть клиентов)*/
										   LEN,                         /*размер выходного буфера в байтах (придется ждать освобождения)*/
										   LEN,                         /*размер входного буфера в байтах (придется ждать освобождения)*/
										   5000,                        /*время ожидания в миллисекундах (для блокирующего режима работы NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*адрес структуры с особыми атрибутами*/);
		if(pipes[i].fd_pipe == INVALID_HANDLE_VALUE) {
			show_err("Ошибка создания именованного канала");
			return 1;
		} else {
			/*подключаемся к именованному каналу (заблокируется в блокирующим режиме)*/
			pipes[i].wait_connect = Connect(pipes[i].fd_pipe, &pipes[i].Overlap);
			/*состояние канала*/
			pipes[i].state = pipes[i].wait_connect ? CONNECTING_STATE : READING_STATE;
		}
	}
			
	while(TRUE) {
		DWORD i = WaitForMultipleObjects(CNT,    /*количество событий в массиве*/
			                            hEvents,/*массив событий*/
			                            FALSE,  /*не требовать всех событий одновременно*/
			                            INFINITE/*ждать бесконечно*/);
		/*получаем индекс сработавшего события*/
		i -= WAIT_OBJECT_0;

		DWORD len;
		BOOL isOk;
		/*Если ожидали соединение в этом именованном канале*/
		if(pipes[i].wait_connect) {
			/*извлекаем результат последней асинхронной операции по индексу события*/
			isOk = GetOverlappedResult(pipes[i].fd_pipe, /*дескриптор канала*/
									   &pipes[i].Overlap,/*OVERLAPPED*/
									   &len,             /*передано байт*/
									   FALSE             /*не ждать*/);
			/*определим, какая асинхронная операция выполнена*/
			switch(pipes[i].state) {
				case CONNECTING_STATE:
					/*канал находился в ожидание нового соединения*/
					if(!isOk) {
						show_err("Не удалось извлечь результат асинхронной операции");
						return 0;
					}					
					pipes[i].state = READING_STATE; //следующая асинхронная операция будет чтением
					break;

				case READING_STATE:
					/*канал ожидал операцию чтения*/
					if(!isOk || len == 0) {
						DisconnectAndReconnect(&pipes[i]);//не удалось прочитать
						continue;
					}
					printf("Получено: %s\n", pipes[i].buf_in);
					pipes[i].len_in = len;					
					pipes[i].state = WRITING_STATE; //следующая асинхронная операция будет записью
					break;

				case WRITING_STATE:
					/*канал ожидал операцию записи*/
					if(!isOk || len != pipes[i].len_out) {
						DisconnectAndReconnect(&pipes[i]); //не удалось записать
						continue;
					}					
					pipes[i].state = READING_STATE; //следующая асинхронная операция будет чтением
					break;
			}
		}

		/*сформируем следующую асинхронную операцию*/
		switch(pipes[i].state) {
			case READING_STATE:
				/*нужно зарегистрировать операцию асинхронного чтения*/
				isOk = ReadFile(pipes[i].fd_pipe, /*дескриптор реализации именованного канала*/
								pipes[i].buf_in,  /*буфер для приема данных от клиента*/
								LEN*sizeof(char), /*размер буфера*/
								&pipes[i].len_in, /*фактически принято*/
								&pipes[i].Overlap /*структура асинхронного выполнения операции*/);

				/*из канала сразу же прочитаны данные от клиента*/ 
				if(isOk && pipes[i].len_in != 0) {
					pipes[i].wait_connect = FALSE; //не ждем соединения 
					pipes[i].state = WRITING_STATE;//будем ждать отправку
					printf("Получено: %s\n", pipes[i].buf_in);
					continue;
				}

				/*асинхронная операция еще не выполнена*/
				if(!isOk && GetLastError() == ERROR_IO_PENDING) {
					pipes[i].wait_connect = TRUE;  //будем ждать соединения
					continue;
				}

				/*прочие ошибки или разрыв соединения*/
				show_err("Прочее");
				DisconnectAndReconnect(&pipes[i]);
				break;				

			case WRITING_STATE:
				/*нужно зарегистрировать операцию асинхронной записи*/
				pipes[i].len_out = pipes[i].len_in;
				memcpy(pipes[i].buf_out, pipes[i].buf_in, pipes[i].len_in); //будем отправлять назад то, что нам прислали
				

				isOk = WriteFile(pipes[i].fd_pipe,             /*дескриптор канала*/
								 pipes[i].buf_out,             /*отправляемые данные*/
								 pipes[i].len_out*sizeof(char),/*размер отправляемых данных*/
								 &len,                         /*фактически отправлено байт*/
						         &pipes[i].Overlap             /*структура асинхронной отправки*/);

				/*в канал сразу же отправлены данные*/
				if(isOk && len == pipes[i].len_out*sizeof(char)) {
					pipes[i].wait_connect = FALSE; //не ждем соединения
					pipes[i].state = READING_STATE;//будем ждать получения
					continue;
				}

				/*в канала не записаны данные из-за ЕЩЕ НЕ УСТАНОВИВШЕГОСЯ СОЕДИНЕНИЯ*/
				if(!isOk && (GetLastError() == ERROR_IO_PENDING)) {
					pipes[i].wait_connect = TRUE; //ждем соединение
					continue;
				}

				/*прочие ошибки или разрыв соединения*/
				show_err("Прочее");
				DisconnectAndReconnect(&pipes[i]);
				break;
		}		
				
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

BOOL Connect(HANDLE fd, OVERLAPPED *po) {
	BOOL isOk = FALSE;

	/*подключаемся к каналу, но не ждем (должна вернуть 0)*/
	if(ConnectNamedPipe(fd /*дескриптор именованного канала*/,
		                po /*адрес структуры OVERLAPPED (для асинхронного режима)*/)) {
		show_err("Ошибка подключения к именованному каналу");
		return FALSE;
	}

	/*анализируем результат*/
	switch(GetLastError()) {		
		case ERROR_IO_PENDING:
			                  /*ожидаем соединения*/
			                  isOk = TRUE;
			                  break;			
		case ERROR_PIPE_CONNECTED:
			                  /*есть подключение*/
			                  if(SetEvent(po->hEvent)) break;
		default:
							  /*другие ошибки*/
							  show_err("Прочие ошибки подключения к именованному каналу");
	}

	return isOk;
}

VOID DisconnectAndReconnect(PIPEINST* pipe) {
	
	/*отключаемся от именованного канала*/
	if(!DisconnectNamedPipe(pipe->fd_pipe)) 
		show_err("Не удалось отключиться от именованного канала");
	
	/*снова подключаемся к именованному каналу*/
	pipe->wait_connect = Connect(pipe->fd_pipe,&pipe->Overlap);
	pipe->state = pipe->wait_connect ?	CONNECTING_STATE : READING_STATE;
}