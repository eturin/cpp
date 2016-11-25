#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "htab.h"

/*количество реализаций именованного канала*/
#define CNT 10
/*размер вуфера*/
#define LEN 256

/*типы ожиданий*/
#define WAITING_STATE 0
#define READING_STATE 1
#define WRITING_STATE 2

/*структура для отдельной реализации канала*/
struct pipeInfo {
	//дескриптор реализации
	HANDLE fd;
	//буферы обмена
	char buf_in[LEN];
	size_t len_in;
	char buf_out[LEN];
	size_t len_out;
	//структура асинхронного вызова
	OVERLAPPED Overlap;
	//ожидание асинхронной операции
	BOOL wait;
	//тип ожидаемой команды
	int type;
	
	//указатель на хешь таблицу этого соединения
	struct hTab * ht;
};

int show_err(const char * msg);
BOOL Connect(struct pipeInfo * pipe);
void Reconnect(struct pipeInfo * pipe);
int Loop(struct pipeInfo * pipe, HANDLE * hEvent);
void ReadDATA(struct pipeInfo * pipe);
void Work(struct pipeInfo * pipe);
void WriteDATA(struct pipeInfo * pipe);


int StartServer() {
	int isOk = 0;

	/*запрашиваем имя канала*/
	printf("Укажите имя именованного канала (\\\\.\\pipe\\<имя?>): ");
	char buf[LEN];
	scanf("%s", buf);
	
	char path[LEN];
	sprintf(path, "\\\\.\\pipe\\%s", buf);
	
	/*инициализация структуры доступа к каналу (для всех)*/
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	/*инициализируем реализации именованного канала*/
	struct pipeInfo pipe[CNT];
	memset(pipe, 0, CNT*sizeof(struct pipeInfo));
	HANDLE hEvent[CNT]; //дескрипторы событий, связанных с каждым каналом
	for(size_t i = 0; i < CNT; ++i) {
		/*создаем реализацию канала в ядре OS*/
		pipe[i].fd=CreateNamedPipe(path,                        /*имя канала*/
						           PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,/*режим открытия канала (в данном случае - канал двунаправленный и АСИНХРОННЫЙ )*/
						           PIPE_TYPE_BYTE | PIPE_WAIT,  /*режим работы канала (в данном случае - передача байт и блокирующий)*/
						           CNT,                         /*максимальное количество реализаций канала (т.е. сколько может быть клиентов)*/
						           LEN,                         /*размер выходного буфера в байтах (придется ждать освобождения)*/
						           LEN,                         /*размер входного буфера в байтах (придется ждать освобождения)*/
						           0,                           /*время ожидания в миллисекундах (для блокирующего режима работы NMPWAIT_USE_DEFAULT_WAIT)*/
								   &sa                          /*адрес структуры с особыми атрибутами*/);
		if(pipe[i].fd == INVALID_HANDLE_VALUE) {
			show_err("Не удалось получить дескриптор реализации именованного канала");
			continue;
		}
		
		/*создаем событие, которое будет обслуживать асинхронные команды этой реализации канала*/
		hEvent[i] = CreateEvent(NULL,   /*атрибут защиты*/
								 TRUE,	 /*тип сброса TRUE - ручной*/
								 TRUE,	 /*начальное состояние TRUE - сигнальное*/
								 NULL	 /*имя события (если указать, то уникальное значение)*/);
		if(hEvent[i] == NULL) {
			show_err("Не удалось получить дескриптор события");
			continue;
		} else {
			pipe[i].Overlap.hEvent = hEvent[i];
		}
		
		/*подключаемся к реализации канала*/		
		Connect(&pipe[i]);				
	}

	/*реализация обработки событий асинхронных команд*/
	isOk=Loop(pipe,hEvent);

	/*закрываем все дескрипторы (сами бы закрылись, когда процесс завершится)*/
	for(size_t i = 0; i < CNT; ++i) {
		releas_hTab(pipe[i].ht);
		DisconnectNamedPipe(pipe[i].fd);
		CloseHandle(pipe[i].fd);
		CloseHandle(hEvent[i]);
	}
	free(sa.lpSecurityDescriptor);

	return isOk;
}

BOOL Connect(struct pipeInfo * pipe) {
	int isOk = FALSE; //не ожидаем соединения (событие наступило или ошибка)

	/*инициализация хеш-таблицы*/
	pipe->ht = create_hTab(0);

	/*подключаемся к каналу, но не ждем (должна вернуть 0)*/
	if(ConnectNamedPipe(pipe->fd       /*дескриптор именованного канала*/,
						&pipe->Overlap /*адрес структуры OVERLAPPED (для асинхронного режима)*/)) {
		show_err("Ошибка подключения к именованному каналу");
		return FALSE;
	}

	/*анализируем результат*/
	switch(GetLastError()) {
		case ERROR_IO_PENDING:
			/*ожидаем соединения с клиентом*/
			isOk = TRUE;
			break;
		case ERROR_PIPE_CONNECTED:
			/*клиент подключился между CreateNamedPipe и ConnectNamedPipe (меняем состояние события на случившееся, или идем смотреть ошибки)*/
			if(SetEvent(pipe->Overlap.hEvent))
				break;
		default:
			/*другие ошибки*/
			show_err("Ошибка подключения к именованному каналу или смены состояния события");
	}

	/*ожидание завершения асинхронной операции*/
	pipe->wait = isOk;
	/*тип асинхронной операции*/
	pipe->type = isOk ? WAITING_STATE : READING_STATE;

	return isOk;
}

void Reconnect(struct pipeInfo * pipe) {
	/*отключаемся от реализации*/
	if(!DisconnectNamedPipe(pipe->fd))
		show_err("Не удалось отключиться от именованного канала");
	
	/*освобождение ресурсов хеш-таблицы*/
	releas_hTab(pipe->ht);

	/*снова подключаемся к именованному каналу*/
	Connect(pipe);	
}

void ReadDATA(struct pipeInfo * pipe) {
	/*запустим следующую асинхронную операцию*/
	pipe->type = READING_STATE;
	pipe->wait = FALSE; //асинхронная операция еще не запущена

	/*регистрируем операцию асинхронного чтения*/
	BOOL isOk = ReadFile(pipe->fd,         /*дескриптор реализации именованного канала*/
					     pipe->buf_in,     /*буфер для приема данных от клиента*/
					     LEN*sizeof(char), /*размер буфера*/
					     &pipe->len_in,    /*фактически принято*/
					     &pipe->Overlap    /*структура асинхронного выполнения операции*/);

	
	if(isOk && pipe->len_in != 0) {
		/*из канала сразу же прочитаны данные от клиента (обработка)*/
		Work(pipe);		
	}else if(!isOk && GetLastError() == ERROR_IO_PENDING) {
		/*асинхронная операция еще не выполнена*/
		pipe->wait = TRUE;  //будем ждать завершения!!!		
	} else {
		/*прочие ошибки или разрыв соединения*/
		show_err("Прочее");
		Reconnect(pipe);
	}	
}

void WriteDATA(struct pipeInfo * pipe) {
	/*запустим следующую асинхронную операцию*/
	pipe->type = WRITING_STATE;
	pipe->wait = FALSE; //асинхронная операция еще не запущена

	/*нужно зарегистрировать операцию асинхронной записи*/
	size_t len = 0;
	BOOL isOk = WriteFile(pipe->fd,                  /*дескриптор канала*/
						  pipe->buf_out,             /*отправляемые данные*/
						  pipe->len_out*sizeof(char),/*размер отправляемых данных*/
						  &len,                      /*фактически отправлено байт*/
						  &pipe->Overlap             /*структура асинхронной отправки*/);

	if(isOk && len == pipe->len_out*sizeof(char)) {
		/*в канал сразу же ушли данные*/
		ReadDATA(pipe);
	} else if(!isOk && (GetLastError() == ERROR_IO_PENDING)) {
		/*в канала не записаны данные из-за ЕЩЕ НЕ УСТАНОВИВШЕГОСЯ СОЕДИНЕНИЯ*/
		pipe->wait = TRUE; //будем ждать завершения!!!
	} else {
		/*прочие ошибки или разрыв соединения*/
		show_err("Прочее");
		Reconnect(pipe);
	}
}

int Loop(struct pipeInfo * pipe, HANDLE * hEvent) {
	while(TRUE) {
		/*ждем любое из событий, обслуживающих реализации именованного канала*/
		DWORD i = WaitForMultipleObjects(CNT,    /*размер массиве*/
										 hEvent, /*массив дескрипторов событий*/
										 FALSE,  /*не требовать всех событий одновременно*/
										 INFINITE/*ждать бесконечно*/);
		/*получаем индекс сработавшего события*/
		i -= WAIT_OBJECT_0;

		DWORD len;
		BOOL isOk;
		/*Если не ожидали завершения асинхронной операции, то ничего не делаем*/
		if(!pipe[i].wait)
			continue;
		
		/*извлекаем результат последней асинхронной операции по индексу события*/
		isOk = GetOverlappedResult(pipe[i].fd,      /*дескриптор канала*/
									&pipe[i].Overlap,/*OVERLAPPED*/
									&len,            /*передано или отправлено байт*/
									FALSE            /*не ждать*/);
		/*определим последнюю асинхронную операцию, связанную с этим событием*/
		switch(pipe[i].type) {
			case WAITING_STATE:
				/*новое соединение*/
				if(!isOk) {
					show_err("Асинхронное соединение с клиентом не удалось");
					Reconnect(&pipe[i]);
				}else
					//следующая асинхронная операция, которую нужно запустить
					ReadDATA(&pipe[i]);
				break;

			case READING_STATE:
				/*завершилось чтение из реализации именованного канала*/
				if(!isOk || len == 0) {
					//мы ждали, что клиент что-то отправит, а он отключился
					Reconnect(&pipe[i]);					
				} else {				
					pipe[i].len_in = len;					
					//обработка полученных данных
					Work(&pipe[i]);
				}
				break;

			case WRITING_STATE:
				/*завершилась запись в реализацию именованного канала*/
				if(!isOk || len != pipe[i].len_out) {
					//мы хотели что-то отправить, а клиент  отключился
					Reconnect(&pipe[i]);					
				} else
					//следующая асинхронная операция, которую нужно запустить
					ReadDATA(&pipe[i]);
				break;
		}				
	}
}

void Work(struct pipeInfo * pipe) {
	/*
	  1) set ключ значение
		 Необходимо сохранить в памяти значение под указанным ключом
		 и записать в канал строку: acknowledged
	  2) get ключ
	     Если ключ имеется в хранилище, следует записать в канал строку в формате
	            found значение
	     В противном случает следует записать в канал строку missing.
	  3) list
		 Необходимо записать в канал строку, содержащую через пробел все
         имеющиеся в хранилище ключи.
	  4) delete ключ
         Если ключ присутствует в хранилище, следует записать в канал строку
         deleted, иначе — строку missing.
      5) quit
         Необходимо отключить именованный канал от клиента
	*/
	
	/*извлекаем команду*/
	char cmd[LEN];
	char str_key[LEN];
	char str_val[LEN];
	int res=sscanf(pipe->buf_in,"%s %s %s", cmd, str_key, str_val);

	/*обрабатываем команду*/
	if(res == 0) {
		sprintf(pipe->buf_out,"missing");
		pipe->len_out = strlen(pipe->buf_out)+1;
	} else if(!strcmp(cmd, "set")) {
		if(res == 3) {
			/*добавляем в хеш-таблицу ключ и значение*/
			size_t len = strlen(str_key) + 1;
			char * key = malloc(len*sizeof(char));
			strcpy(key, str_key);

			len = strlen(str_val) + 1;
			char * val = malloc(len*sizeof(char));
			strcpy(val, str_val);

			add_hTab(pipe->ht, key, val);

			sprintf(pipe->buf_out, "acknowledged");
			pipe->len_out = strlen(pipe->buf_out)+1;
		} else {
			/*не достаточно параметоров*/
			sprintf(pipe->buf_out, "missing");
			pipe->len_out = strlen(pipe->buf_out)+1;
		}
	} else if(!strcmp(cmd, "get")) {
		if(res == 2) {
			/*получаем значение из таблицы*/
			char * val = search_hTab(pipe->ht,str_key);			
			if(val!=NULL)
				sprintf(pipe->buf_out, "found %s", val);
			else
				sprintf(pipe->buf_out, "missing");

			pipe->len_out = strlen(pipe->buf_out)+1;
		} else {
			/*не достаточно параметоров*/
			sprintf(pipe->buf_out, "missing");
			pipe->len_out = strlen(pipe->buf_out)+1;
		}
	} else if(!strcmp(cmd, "list")) {
		/*стрираем*/
		memset(pipe->buf_out,0,LEN*sizeof(char));
		/*заполняем ключами*/
		for(size_t i = 0; i < pipe->ht->size;++i)
			if(pipe->ht->key_val[i].key!=NULL)
				sprintf(pipe->buf_out, "%s%s ", pipe->buf_out, pipe->ht->key_val[i].key);
		pipe->len_out = strlen(pipe->buf_out)+1;
	} else if(!strcmp(cmd, "delete")) {
		if(res == 2) {
			/*получаем значение из таблицы*/
			int res = del_hTab(pipe->ht, str_key);
			if(res)
				sprintf(pipe->buf_out, "deleted");
			else
				sprintf(pipe->buf_out, "missing");

			pipe->len_out = strlen(pipe->buf_out)+1;
		} else {
			/*не достаточно параметоров*/
			sprintf(pipe->buf_out, "missing");
			pipe->len_out = strlen(pipe->buf_out)+1;
		}
	} else if(!strcmp(cmd, "quit")) {
		/*отключаем клиента*/
		Reconnect(pipe);
		return;
	} else {
		sprintf(pipe->buf_out, "missing");
		pipe->len_out = strlen(pipe->buf_out)+1;
	}
	
	/*следующая асинхронная операция*/
	WriteDATA(pipe);
}