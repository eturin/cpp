#include "worker.h"
#include "req.h"
#include "client.h"
#include "error.h"
#include "http_parser.h"



/*выделение URL*/
int call_request_url_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	req->url = malloc((len + 1)*sizeof(char));
	strncpy(req->url, buf, len);
	req->url[len] = '\0';
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
										   PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,/*режим открытия канала (в данном случае - канал двунаправленный и АСИНХРОННЫЙ )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*режим работы канала (в данном случае - передача байт и блокирующий)*/
										   1,                           /*максимальное количество реализаций канала (т.е. сколько может быть клиентов)*/
										   LEN,                         /*размер выходного буфера в байтах (придется ждать освобождения)*/
										   LEN,                         /*размер входного буфера в байтах (придется ждать освобождения)*/
										   0,                           /*время ожидания в миллисекундах (для блокирующего режима работы NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*адрес структуры с особыми атрибутами*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s не удалось создать именованный канала [out]", msg);
			show_err(msg_str, TRUE);			
			return FALSE;
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_w, &pwrk->pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {			/*подключаемся к именованному каналу (заблокируется в блокирующим режиме)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s серверу не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*открываем существующий канал*/
		pwrk->fd[i].fd_r = CreateFile(path,                              /*имя канала*/
							          GENERIC_READ,                      /*режим доступа*/
							          FILE_SHARE_READ,                   /*режим совместного использования*/
							          NULL,                              /*особые атрибуты*/
							          OPEN_EXISTING,                     /*открывать существующий или ошибка*/
							          0,                                 /*атрибуты файла*/
							          NULL                               /*особые атрибуты*/);
		if(pwrk->fd[i].fd_r == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s клиенту не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;			
		}
		
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
												  (ULONG_PTR)pwrk->pcln,             /*ключ завершения (параметр будет доступен при наступление события)*/
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
										   PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,/*режим открытия канала (в данном случае - канал двунаправленный и АСИНХРОННЫЙ )*/
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
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_r, &pwrk->pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {			/*подключаемся к именованному каналу (заблокируется в блокирующим режиме)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s серверу не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*открываем существующий канал*/
		pwrk->fd[i].fd_w = CreateFile(path,                              /*имя канала*/
									  GENERIC_WRITE,                     /*режим доступа*/
									  FILE_SHARE_WRITE,/*режим совместного использования*/
									  NULL,                              /*особые атрибуты*/
									  OPEN_EXISTING,                     /*открывать существующий или ошибка*/
									  0,                                 /*атрибуты файла*/
									  NULL                               /*особые атрибуты*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s клиенту не удалось подключиться к именованному каналу [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		
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
												  (ULONG_PTR)pwrk->pcln,             /*ключ завершения (параметр будет доступен при наступление события)*/
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
	pwrk->procInf.hProcess = INVALID_HANDLE_VALUE;
	pwrk->procInf.hThread = INVALID_HANDLE_VALUE;
	pwrk->type = WAIT;
	pwrk->pcln = pcln;

	while(isOk) {
		/*сформируем путь к worker*/
		if(len == 0 || len >= LEN_WORKER) {
			isOk = FALSE;
			show_err("Ошибка в имени worker", FALSE);
			break;
		}
		memcpy(pwrk->name, name, len);
		char path[256];
		sprintf(path, "%s%s.exe", work_path, pwrk->name);

		/*создаем три анонимных канала*/
		isOk = create_pipe(iocp, pwrk, 0, TRUE, "STDIN");
		if(!isOk) break;
		isOk = create_pipe(iocp, pwrk, 1, FALSE, "STDOUT");
		if(!isOk) break;
		isOk = create_pipe(iocp, pwrk, 2, FALSE, "STDERR");
		if(!isOk) break;

		/*DWORD flags;
		isOk = GetHandleInformation(fd_r_1,&flags);*/

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

	if(!isOk) 
		pcln->pwrk = pwrk = release_Worker(pwrk);
	else {
		/*готовимся отправлять worker данные клиента*/
		pcln->DataBuf.buf = pcln->data;
		pcln->cur = 0;
	}
	
	pcln->pwrk = pwrk;
	
	return pwrk;
}
struct Worker * release_Worker(struct Worker *pwrk) {
	if(pwrk != NULL) {
		/*убиваем процесс и закрываем его дескрипторы*/
		if(pwrk->procInf.hProcess != INVALID_HANDLE_VALUE) {
			TerminateProcess(pwrk->procInf.hProcess, NO_ERROR);
			CloseHandle(pwrk->procInf.hProcess);
			CloseHandle(pwrk->procInf.hThread);
		}

		/*закрываем дескрипторы*/
		for(int i = 0; i < 3; ++i) {
			CloseHandle(pwrk->fd[i].fd_r);
			CloseHandle(pwrk->fd[i].fd_w);
		}
				
		if(pwrk->type == READ) {
			/*worker выполнил задачу*/
			make200(pwrk->pcln);
			pwrk->pcln->pwrk = NULL;
		} else if(pwrk->type == WRITE && pwrk->pcln != NULL) {
			/*нужно известить клиента об ошибке сервера 500*/
			make500(pwrk->pcln);
			pwrk->pcln->pwrk = NULL;
		} else  if(pwrk->pcln != NULL) {
			/*нужно известить клиента о 404*/
			make404(pwrk->pcln);
			pwrk->pcln->pwrk = NULL;
		}

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
			end = begin + strlen(begin);
		struct Worker * pwrk = init_Worker(begin, end - begin, pcln, iocp);		

		if(pwrk == NULL) {
			/*worker не запущен*/
			make404(pcln);
		} else {
			/*свяжем worker с client и изменим режимы их работы (клиент ждет, данные отправляем worker)*/
			pcln->type = WAIT;
			pwrk->type = WRITE;	
			/*признак конца данных, для worker*/
			pcln->data[pcln->len] = '\0';
			++pcln->len;
		}
	}
	
	return isOk;
}

