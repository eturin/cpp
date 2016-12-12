#include "server.h"
#include "worker.h"
#include "req.h"
#include "client.h"
#include "error.h"
#include "http_parser.h"

/* Раскодирование строки "%xx"*/
char DecodeHex(char *str) {
	char ch;

	// Обрабатываем старший разряд
	if(str[0] >= 'A')
		ch = ((str[0] & 0xdf) - 'A') + 10;
	else
		ch = str[0] - '0';

	// Сдвигаем его влево на 4 бита
	ch <<= 4;

	// Обрабатываем младший разряд и складываем
	// его со старшим
	if(str[1] >= 'A')
		ch += ((str[1] & 0xdf) - 'A') + 10;
	else
		ch += str[1] - '0';

	// Возвращаем результат перекодировки
	return ch;
}
/* Раскодирование строки из кодировки URL*/
void DecodeStr(char *szString) {
	int src;
	int dst;
	char ch;

	// Цикл по строке
	for(src = 0, dst = 0; szString[src]; src++, dst++) {
		// Получаем очередной символ перекодируемой строки
		ch = szString[src];

		// Заменяем символ "+" на пробел
		ch = (ch == '+') ? ' ' : ch;

		// Сохраняем результат
		szString[dst] = ch;

		// Обработка шестнадцатеричных кодов вида "%xx"
		if(ch == '%') {
			// Выполняем преобразование строки "%xx"
			// в код символа
			szString[dst] = DecodeHex(&szString[src + 1]);
			src += 2;
		}
	}

	// Закрываем строку двоичным нулем
	szString[dst] = '\0';
}
/*выделение URL*/
int call_request_url_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	req->url = malloc((len + 1)*sizeof(char));
	strncpy(req->url, buf, len);
	req->url[len] = '\0';
	
	//DecodeStr(req->url);

	return 0;
}
/*выделение имени заголовка*/
int call_header_field_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	++(req->head_cnt);
	if(req->head_cnt>1)
		req->head = realloc(req->head, sizeof(struct Head)*req->head_cnt);
	else
		req->head = malloc(sizeof(struct Head)*req->head_cnt);

	char * field = req->head[req->head_cnt - 1].field = malloc((len + 1)*sizeof(char));
	req->head[req->head_cnt - 1].value = NULL;

	strncpy(field, buf, len);
	field[len] = '\0';
	return 0;
}
/*выделение значения заголовка*/
int call_header_value_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	char * value = req->head[req->head_cnt - 1].value = malloc((len + 1)*sizeof(char));

	strncpy(value, buf, len);
	value[len] = '\0';

	if(!strcmp(req->head[req->head_cnt - 1].field, "Content-Length"))
		sscanf(value, "%d", &req->body_length);

	return 0;
}
/*выделение статуса ответа*/
int call_response_status_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	req->status = malloc((len + 1)*sizeof(char));
	strncpy(req->status, buf, len);
	req->status[len] = '\0';
	return 0;
}
/*выделение тела*/
int call_body_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	req->body = malloc((len + 1)*sizeof(char));
	strncpy(req->body, buf, len);
	req->body[len] = '\0';
	return 0;
}

int call_message_begin_cb(http_parser *p) { return 0; }
int call_headers_complete_cb(http_parser *p) { return 0; }
int call_message_complete_cb(http_parser *p) { return 0; }
int call_chunk_header_cb(http_parser *p) { return 0; }
int call_chunk_complete_cb(http_parser *p) { return 0; }

struct Req * pars_http(const char* data, size_t len) {
	/*определяем парсер и его тип*/
	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);

	/*структура хранящая выделенные парсером поля*/
	struct Req *req = init_Req();
	parser.data = req;
	req->head_length = len;

	/*это функции обратного вызова, для выделение фрагментов http-сообщения*/
	static http_parser_settings settings_null = {.on_message_begin = call_message_begin_cb
		, .on_header_field = call_header_field_cb
		, .on_header_value = call_header_value_cb
		, .on_url = call_request_url_cb
		, .on_status = call_response_status_cb
		, .on_body = call_body_cb
		, .on_headers_complete = call_headers_complete_cb
		, .on_message_complete = call_message_complete_cb
		, .on_chunk_header = call_chunk_header_cb
		, .on_chunk_complete = call_chunk_complete_cb
	};

	/*запускаем парсер*/
	size_t parsed = http_parser_execute(&parser, &settings_null, data, len);

	/*распарсить не удалось*/
	if(parsed != len)
		req = release_Req(req);

	return req;
}

BOOL create_pipe(LPVOID iocp, struct Worker *pwrk, int i, BOOL isIn,const char * msg) {
#define MAXLEN 1000
	/*формируем имя канала*/
	char path[256];
	sprintf(path, "\\\\.\\pipe\\%s\\%d\\%s", pwrk->name, (unsigned)pwrk, msg);

	int lpMode = PIPE_NOWAIT;
	if(isIn) {		
		/*создание именованного канала*/
		pwrk->fd[i].fd_w = CreateNamedPipe(path,                        /*имя канала*/
										   PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,/*режим открытия канала (в данном случае - канал однонаправленный и АСИНХРОННЫЙ )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*режим работы канала (в данном случае - передача байт и блокирующий)*/
										   1,                           /*максимальное количество реализаций канала (т.е. сколько может быть клиентов)*/
										   MAX_HEAD_HTTP,               /*размер выходного буфера в байтах (придется ждать освобождения)*/
										   MAX_HEAD_HTTP,               /*размер входного буфера в байтах (придется ждать освобождения)*/
										   0,                           /*время ожидания в миллисекундах (для блокирующего режима работы NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*адрес структуры с особыми атрибутами*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось создать именованный канала [out]", msg);
			show_err(msg_str, TRUE);			
			return FALSE;
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_w, (OVERLAPPED*)&pwrk->fd[i].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {			/*подключаемся к именованному каналу (заблокируется в блокирующим режиме)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s серверу не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*открываем существующий канал*/
		pwrk->fd[i].fd_r = CreateFile(path,                              /*имя канала*/
							          GENERIC_READ,                      /*режим доступа*/
									  FILE_SHARE_READ | FILE_SHARE_WRITE,/*режим совместного использования*/
							          NULL,                              /*особые атрибуты*/
							          OPEN_EXISTING,                     /*открывать существующий или ошибка*/
									  FILE_FLAG_WRITE_THROUGH,           /*атрибуты файла (здесь отмена промежуточного кеширования)*/
							          NULL                               /*особые атрибуты*/);
		if(pwrk->fd[i].fd_r == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s клиенту не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;			
		}

		//DWORD mode = PIPE_NOWAIT;
		//if(!SetNamedPipeHandleState(pwrk->fd[i].fd_r,       /*дескриптор канала*/
		//	                        &mode,                  /* адрес переменной, в которой указан новый режим канала*/
		//	                        NULL,                   /* адрес переменной, в которой указывается максимальный размер пакета, передаваемого в канал*/
		//	                        NULL                    /* адрес максимальной задержки перед передачей данных*/)) {
		//	char msg_str[MAXLEN];
		//	sprintf(msg_str, "%s не удалось включить не блокирующий режим работы канала [out]", msg);
		//	show_err(msg_str, TRUE);
		//	return FALSE;
		//}
		
		/*включаем наследование для тех концов канала, которые должны быть у потомка*/		
		if(!SetHandleInformation(pwrk->fd[i].fd_r,       /* дескриптор канала*/
			                     HANDLE_FLAG_INHERIT,    /* заменяемые флажки (здесь наследование и закрытие дескриптора)*/
			                     1                       /* новые значения флажков*/)) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось включить наследование дескриптору канала [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}		

		/*связываем конец канала с портом (любая асинхронная операция с этим концом будет использовать указанный порт)*/
		pwrk->fd[i].iocp = CreateIoCompletionPort(pwrk->fd[i].fd_w,                  /*дескриптор сокета*/
												  iocp,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
												  (ULONG_PTR)pwrk,                   /*ключ завершения (параметр будет доступен при наступление события)*/
												  1                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
		if(pwrk->fd[i].iocp == NULL) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось связать с портом OS дескриптор канала [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
	}else{
		/*создание именованного канала*/
		pwrk->fd[i].fd_r = CreateNamedPipe(path,                        /*имя канала*/
										   PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,/*режим открытия канала (в данном случае - канал однонаправленный и АСИНХРОННЫЙ )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*режим работы канала (в данном случае - передача байт и блокирующий)*/
										   1,                           /*максимальное количество реализаций канала (т.е. сколько может быть клиентов)*/
										   LEN,                         /*размер выходного буфера в байтах (придется ждать освобождения)*/
										   LEN,                         /*размер входного буфера в байтах (придется ждать освобождения)*/
										   0,                           /*время ожидания в миллисекундах (для блокирующего режима работы NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*адрес структуры с особыми атрибутами*/);
		if(pwrk->fd[i].fd_r == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось создать именованный канала [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_r, (OVERLAPPED*)&pwrk->fd[i].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {			/*подключаемся к именованному каналу (заблокируется в блокирующим режиме)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s серверу не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*открываем существующий канал*/
		pwrk->fd[i].fd_w = CreateFile(path,                              /*имя канала*/
									  GENERIC_WRITE,                     /*режим доступа*/
									  FILE_SHARE_WRITE | FILE_SHARE_READ,/*режим совместного использования*/
									  NULL,                              /*особые атрибуты*/
									  OPEN_EXISTING,                     /*открывать существующий или ошибка*/
									  FILE_FLAG_WRITE_THROUGH,           /*атрибуты файла (здесь отмена промежуточного кеширования)*/
									  NULL                               /*особые атрибуты*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s клиенту не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}

		//DWORD mode = PIPE_READMODE_BYTE | PIPE_NOWAIT; //см. подробнее http://www.frolov-lib.ru/books/bsp/v27/ch2_3.htm
		//if(!SetNamedPipeHandleState(pwrk->fd[i].fd_w,       /*дескриптор канала*/
		//	                        &mode,                  /* адрес переменной, в которой указан новый режим канала*/
		//	                        NULL,                   /* адрес переменной, в которой указывается максимальный размер пакета, передаваемого в канал*/
		//	                        NULL                    /* адрес максимальной задержки перед передачей данных*/)) {
		//	char msg_str[MAXLEN];
		//	sprintf(msg_str, "%s не удалось включить не блокирующий режим работы канала [out]", msg);
		//	show_err(msg_str, TRUE);
		//	return FALSE;
		//}
		
		/*включаем наследование для тех концов канала, которые должны быть у потомка*/
		if(!SetHandleInformation(pwrk->fd[i].fd_w,       /* дескриптор канала*/
			                     HANDLE_FLAG_INHERIT,    /* заменяемые флажки (здесь наследование и закрытие дескриптора)*/
			                     1                       /* новые значения флажков*/)) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось включить наследование дескриптору канала [in]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		
		/*связываем конец канала с портом (любая асинхронная операция с этим концом будет использовать указанный порт)*/
		pwrk->fd[i].iocp = CreateIoCompletionPort(pwrk->fd[i].fd_r,                  /*дескриптор сокета*/
												  iocp,                              /*дескриптор существующего порта завершения I/O (для связи с имеющейся очередью)*/
												  (ULONG_PTR)pwrk,                   /*ключ завершения (параметр будет доступен при наступление события)*/
												  1                                  /*число одновременно  исполняемых потоков (0-по количеству ядер процессора)*/);
		if(pwrk->fd[i].iocp == NULL) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось связать с портом OS дескриптор канала [in]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
	}

	return TRUE;
}

struct Worker * init_Worker(const char *name, size_t len, struct Client * pcln, LPVOID iocp) {
	/*признак успеха*/
	BOOL isOk = TRUE;

	struct Worker * pwrk = malloc(sizeof(struct Worker));
	memset(pwrk, 0, sizeof(struct Worker)); //!!!обязательно!!!
	for(int i = 0; i < 3; ++i)
		pwrk->fd[i].fd_r = pwrk->fd[i].fd_w = INVALID_HANDLE_VALUE;
	pwrk->type = WORKER;
	pwrk->procInf.hProcess = INVALID_HANDLE_VALUE;
	pwrk->procInf.hThread = INVALID_HANDLE_VALUE;
	
	while(isOk) {
		/*сформируем путь к worker*/
		if(len == 0 || len >= LEN_WORKER) {
			isOk = FALSE;
			show_err("Ошибка в имени worker", FALSE);
			break;
		}
		memcpy(pwrk->name, name, len);
		char path[256];
		sprintf(path, "%s%s.exe", pcln->psrv->work_path, pwrk->name);

		/*создаем три анонимных канала*/
		//pwrk->fd[0].overlapped_inf.pcs = pcln->overlapped_inf.pcs; /*инициализация критической секции*/
		pwrk->fd[0].overlapped_inf.type = WRITE_WORKER;
		pwrk->fd[0].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
																   TRUE, /*тип сброса TRUE - ручной*/
																   TRUE, /*начальное состояние TRUE - сигнальное*/
																   NULL  /*имя обьекта*/);
		isOk = create_pipe(iocp, pwrk, 0, TRUE, "STDIN");
		if(!isOk) break;
		
		
		//pwrk->fd[1].overlapped_inf.pcs = malloc(sizeof(CRITICAL_SECTION));/*инициализация критической секции*/
		pwrk->fd[1].overlapped_inf.type = READ_WORKER;
		//InitializeCriticalSection(pwrk->fd[1].overlapped_inf.pcs);
		pwrk->fd[1].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
																   TRUE, /*тип сброса TRUE - ручной*/
																   TRUE, /*начальное состояние TRUE - сигнальное*/
																   NULL  /*имя обьекта*/);
		isOk = create_pipe(iocp, pwrk, 1, FALSE, "STDOUT");
		if(!isOk) break;
		
		
		//pwrk->fd[2].overlapped_inf.pcs = malloc(sizeof(CRITICAL_SECTION));/*инициализация критической секции*/
		pwrk->fd[2].overlapped_inf.type = READ_WORKER_ERR;
		//InitializeCriticalSection(pwrk->fd[2].overlapped_inf.pcs);
		pwrk->fd[2].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
																   TRUE, /*тип сброса TRUE - ручной*/
																   TRUE, /*начальное состояние TRUE - сигнальное*/
																   NULL  /*имя обьекта*/);
		isOk = create_pipe(iocp, pwrk, 2, FALSE, "STDERR");
		if(!isOk) break;
		
		/*создаем дочерний процесс*/
		pwrk->sti.cb = sizeof(STARTUPINFO);			// указать размер
		/*устанавливаем потомку дескрипторы stdin, stdout и stderr*/
		pwrk->sti.dwFlags = STARTF_USESTDHANDLES; //!!!обязательно!!!
		pwrk->sti.hStdInput  = pwrk->fd[0].fd_r;
		pwrk->sti.hStdOutput = pwrk->fd[1].fd_w;
		pwrk->sti.hStdError  = pwrk->fd[2].fd_w;

		isOk = CreateProcess(path,          /*путь к программе*/
							 "",            /*параметры коммандной строки*/
							 NULL,          /*доступ AD для нового процесса*/
							 NULL,          /*доступ AD для нового потока*/
							 TRUE,          /*Флаг наследования текущего процесса (разрешаем наследовать дескрипторы)*/
							 0,             /*Флаги способов создания процесса*/
							 NULL,          /*указатель на структуру переменных окружения, для процесса (NULL - значит как у родителя)*/
							 NULL,          /*Текущий диск или каталог (NULL - значит как у родителя)*/
							 &pwrk->sti,    /*Используется для настройки свойств процесса, например расположения окон и заголовок*/
							 &pwrk->procInf /*сведения о процессе (сами заполнятся)*/);
		if(!isOk) {
			show_err("Не удалось создать дочерний процесс", TRUE);
			break;
		}

		/*родитель закрывает дексрипторы дочернего процесса*/
		CloseHandle(pwrk->fd[0].fd_r); pwrk->fd[0].fd_r = INVALID_HANDLE_VALUE;
		CloseHandle(pwrk->fd[1].fd_w); pwrk->fd[1].fd_w = INVALID_HANDLE_VALUE;
		CloseHandle(pwrk->fd[2].fd_w); pwrk->fd[2].fd_w = INVALID_HANDLE_VALUE;

		/*цикл был нужен, чтоб не морочиться с условными операторами*/
		break;
	}

	/*готовимся отправлять worker данные клиента*/
	if(isOk) {
		//STDERR		
		pwrk->fd[2].len = 0;
		pwrk->fd[2].cur = 0;
		pwrk->fd[2].size = MAX_HEAD_HTTP;
		pwrk->fd[2].data = malloc(MAX_HEAD_HTTP);
		if(!ReadFile(pwrk->fd[2].fd_r, pwrk->fd[2].data, LEN, &len, (OVERLAPPED*)&pwrk->fd[2].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
			show_err("Ошибка запуска асинхронной операции ReadFile [STDERR]", TRUE);
		}
		
		//STDOUT		
		pwrk->fd[1].len = 0;
		pwrk->fd[1].cur = 0;
		pwrk->fd[1].size = MAX_HEAD_HTTP;
		pwrk->fd[1].data = malloc(MAX_HEAD_HTTP);
		if(!ReadFile(pwrk->fd[1].fd_r, pwrk->fd[1].data + pwrk->fd[1].len, LEN, &len, (OVERLAPPED*)&pwrk->fd[1].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
			show_err("Ошибка запуска асинхронной операции ReadFile [STDOUT]", TRUE);
			isOk = FALSE;
		}

		//STDIN	(!!!самым последним, т.к. переставляется буфер клиента!!!)
		pwrk->fd[0].size = pcln->size;
		pwrk->fd[0].len = pcln->len;
		pwrk->fd[0].cur = 0;
		pwrk->fd[0].data = pcln->data;
		pcln->data = NULL;
		pcln->DataBuf.buf = NULL;
		/*признак конца данных, для worker*/
		pwrk->fd[0].data[pwrk->fd[0].len] = '\0';
		++pwrk->fd[0].len;
	}
	
	if(!isOk) {
		if(pwrk->fd[0].data != NULL) {
			/*!!!возвращаем буфер клиента в исходное состояние!!!*/
			pcln->size = pwrk->fd[0].size;
			pcln->len  = pwrk->fd[0].len-1;			
			pcln->data = pwrk->fd[0].data;
			pwrk->fd[0].data = NULL;
			pcln->DataBuf.buf = pcln->data + pcln->cur;
		}
		pwrk = release_Worker(pwrk);
	} else {
		pwrk->pcln = pcln;
		pcln->overlapped_inf.type = WAIT;
	}

	pcln->pwrk = pwrk;
	
	return pwrk;
}
struct Worker * release_Worker(struct Worker *pwrk) {
	if(pwrk != NULL 
	   /*&& (pwrk->pcln == NULL || TryEnterCriticalSection(pwrk->pcln->overlapped_inf.pcs))*/) {

		DWORD rc = 0;
		/*убиваем процесс и закрываем его дескрипторы*/
		if(pwrk->procInf.hProcess != INVALID_HANDLE_VALUE) {
			/*забираем код возврата дочернего процесса*/
			GetExitCodeProcess(pwrk->procInf.hProcess, &rc);
			if(rc == STILL_ACTIVE) /*процесс отламался*/;
				
			TerminateProcess(pwrk->procInf.hProcess, NO_ERROR);
			CloseHandle(pwrk->procInf.hProcess);
			CloseHandle(pwrk->procInf.hThread);
		}

		if(pwrk->pcln != NULL) {
			/*нужно известить клиента об ошибке сервера 500*/
			free(pwrk->pcln->data);
			pwrk->pcln->size = pwrk->fd[1].size;
			pwrk->pcln->len  = pwrk->fd[1].len;
			pwrk->pcln->data = pwrk->fd[1].data;
			pwrk->fd[1].data = NULL;
			pwrk->pcln->cur  = 0;
			pwrk->pcln->DataBuf.buf = pwrk->pcln->data;
			
			pwrk->pcln->pwrk = NULL;

			if(rc == 0 && pwrk->pcln->len>0) 
				make200(pwrk->pcln);				
			else if(rc == 0)
				make404(pwrk->pcln);
			else 
				make500(pwrk->pcln);
			
			/*запускаем асинхронные операции*/
			pwrk->pcln->overlapped_inf.type = WRITE;
			start_async(pwrk->pcln, 0, pwrk->pcln->psrv->iocp, &pwrk->pcln->overlapped_inf);
			//LeaveCriticalSection(pwrk->pcln->overlapped_inf.pcs);
		} 

		/*закрываем дескрипторы*/
		for(int i = 0; i < 3; ++i) {
			CloseHandle(pwrk->fd[i].fd_r);
			CloseHandle(pwrk->fd[i].fd_w);
			WSACloseEvent(pwrk->fd[i].overlapped_inf.overlapped.hEvent);			
			free(pwrk->fd[i].data);			
		}
		//pwrk->fd[1].overlapped_inf.pcs = NULL; //это секция клиента, её не нужно удалять
		//if(pwrk->fd[1].overlapped_inf.pcs != NULL) {
		//	DeleteCriticalSection(pwrk->fd[1].overlapped_inf.pcs);
		//	free(pwrk->fd[1].overlapped_inf.pcs);
		//}
		//if(pwrk->fd[2].overlapped_inf.pcs != NULL) {
		//	DeleteCriticalSection(pwrk->fd[2].overlapped_inf.pcs);
		//	free(pwrk->fd[2].overlapped_inf.pcs);
		//}
		free(pwrk);
	}

	return NULL;
}

BOOL work(struct Client * pcln, LPVOID iocp) {
	BOOL isOk = TRUE;

	/*парсим запрос*/
	pcln->preq = pars_http(pcln->data, pcln->len);

	if(pcln->preq == NULL) {
		/*запрос не удалось распарсить 404*/
		make404(pcln);
	} else {
		char *begin = strchr(pcln->preq->url, '/') + 1;
		char *end = strchr(begin, '/');
		if(end == NULL)
			end = strchr(begin, '?');
		if(end == NULL)
			end = begin + strlen(begin);
		struct Worker * pwrk = init_Worker(begin, end - begin, pcln, iocp);		

		if(pwrk == NULL) {
			/*worker не запущен*/
			make404(pcln);
			/*запускаем асинхронную операцию записи в сокет клиента*/
			pcln->overlapped_inf.type = WRITE;
			start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
		} else
			/*запускаем асинхронную операцию записи в канал STDIN worker*/
			start_async((void*)pwrk, 0, pwrk->pcln->psrv->iocp, (struct overlapped_inf*)&pwrk->fd[0].overlapped_inf);		
	}
	
	return isOk;
}

