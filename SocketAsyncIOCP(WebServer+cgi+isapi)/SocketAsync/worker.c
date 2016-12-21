#include "error.h"
#include "socket.h"
#include "server.h"
#include "req.h"
#include "client.h"
#include "worker.h"



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

/*замена символа в строке*/
char* replace_char(char* str, char in, char out) {
	char * p = str;

	while(*p != '\0') {
		if(*p == in)
			*p = out;
		++p;
	}

	return str;
}

char * create_env(const struct Client * pcln, const struct Worker * pwrk) {
	struct Req * preq = pcln->preq;

	/*формируем переменные окружения (каждая пара заканчивается '\0' и в конце всех еще один '\0')*/
	char * pEnv = malloc(3 * MAX_HEAD_HTTP), *pTemp = pEnv;
	memset(pEnv, 0, 3 * MAX_HEAD_HTTP); //весь заголовок сообщения клиента входит одну из переменных

	/*передаем переменные среды, доступные серверу*/
	extern char ** environ;	
	for(char ** env = environ; *env != NULL;++env){
		strcat(pTemp, *env);		
		pTemp += strlen(pTemp) + 1;		
	}

	if(preq != NULL) {
		/*формируем переменные окружения на основе заголовков*/
		for(struct hTab const * s = preq->pHeader; s != NULL; s = (struct hTab const*)(s->hh.next)) {
			if(!strcmp(s->key, "HOST")) {
				char * pos = strchr(s->val, ':');
				if(pos != NULL) {
					strcat(pTemp, "SERVER_NAME=");     
					strncat(pTemp, s->val, pos - s->val);
					pTemp += strlen(pTemp) + 1;
					
					strcat(pTemp, "SERVER_PORT=");
					strcat(pTemp, pos+1);
					pTemp += strlen(pTemp) + 1;					
				} else {
					strcat(pTemp, "SERVER_NAME=");
					strcat(pTemp, s->val);
					pTemp += strlen(pTemp) + 1;

					strcat(pTemp, "SERVER_PORT=80");					
					pTemp += strlen(pTemp) + 1;
				}
				strcat(pTemp, "SERVER_PORT_SECURE=0");
				pTemp += strlen(pTemp) + 1;
				
				pos=strstr(pcln->data, "\r\n");
				char *b = pcln->data;
				for(char * t = b; t != pos; ++t)
					if(*t == ' ')
						b = t;
				strcat(pTemp, "SERVER_PROTOCOL=");
				strncat(pTemp, b+1, pos - b-1);
				pTemp += strlen(pTemp) + 1;

				strcat(pTemp, "HTTP_VERSION=");
				strncat(pTemp, b + 1 + 5, pos - b - 1-5);
				pTemp += strlen(pTemp) + 1;
			} 

			strcat(pTemp, "HTTP_");
			strcat(pTemp, s->key);
			/*заменим некоторые символы*/
			replace_char(pTemp, '-', '_');
			strcat(pTemp, "=");
			strcat(pTemp, s->val);
			pTemp += strlen(pTemp) + 1;			
		}
		/*GET или POST*/
		strcat(pTemp, "REQUEST_METHOD=");
		strcat(pTemp, preq->cmd);
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "AUTH_TYPE=");     pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "AUTH_TYPE=");     pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "AUTH_PASSWORD="); pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "AUTH_USER=");     pTemp += strlen(pTemp) + 1;
		
		strcat(pTemp, "HTTPS=off");             pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "HTTPS_KEYSIZE=");        pTemp += strlen(pTemp) + 1; 
		strcat(pTemp, "HTTPS_SECRETKEYSIZE=");  pTemp += strlen(pTemp) + 1; 
		strcat(pTemp, "HTTPS_SERVER_ISSUER=");  pTemp += strlen(pTemp) + 1; 
		strcat(pTemp, "HTTPS_SERVER_SUBJECT="); pTemp += strlen(pTemp) + 1; 
		
		strcat(pTemp, "CERT_COOKIE=");       pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "CERT_FLAGS=");        pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "CERT_ISSUER=");       pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "CERT_SERIALNUMBER="); pTemp += strlen(pTemp) + 1;
		strcat(pTemp, "CERT_SUBJECT=");      pTemp += strlen(pTemp) + 1;
				
		sprintf(pTemp, "CONTENT_LENGTH=%u", preq->body_length);
		pTemp += strlen(pTemp) + 1;

		sprintf(pTemp, "CONTENT_LENGTH_AVAILABLE=%u",preq->cur);		
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "CONTENT_TYPE=");
		const struct hTab *s = htab_find(preq->pHeader, "CONTENT-TYPE", 0);
		if(s != NULL) 
			strcat(pTemp, s->val);
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "GATEWAY_INTERFACE=CGI/1.1"); pTemp += strlen(pTemp) + 1;
		
		sprintf(pTemp, "REMOTE_ADDR=%d.%d.%d.%d", pcln->addr.sin_addr.S_un.S_un_b.s_b1, pcln->addr.sin_addr.S_un.S_un_b.s_b2, pcln->addr.sin_addr.S_un.S_un_b.s_b3, pcln->addr.sin_addr.S_un.S_un_b.s_b4);
		pTemp += strlen(pTemp) + 1;
		sprintf(pTemp, "REMOTE_HOST=%d.%d.%d.%d", pcln->addr.sin_addr.S_un.S_un_b.s_b1, pcln->addr.sin_addr.S_un.S_un_b.s_b2, pcln->addr.sin_addr.S_un.S_un_b.s_b3, pcln->addr.sin_addr.S_un.S_un_b.s_b4);		
		pTemp += strlen(pTemp) + 1;
		sprintf(pTemp, "REMOTE_PORT=%d", pcln->addr.sin_port);
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "SERVER_SOFTWARE=");
		strcat(pTemp, pcln->psrv->name);
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "ALL_RAW=");
		strcat(pTemp, strstr(pcln->data,"\r\n")+2);
		pTemp += strlen(pTemp) + 1;
											  
		strcat(pTemp, "HTTP_URL=");
		strcat(pTemp, preq->url);
		pTemp += strlen(pTemp) + 1;
				  
		strcat(pTemp, "INSTANCE_ID=1");
		pTemp += strlen(pTemp) + 1;
		
		strcat(pTemp, "SCRIPT_NAME=");
		strcat(pTemp, pwrk->path);
		strcat(pTemp, ".exe");
		pTemp += strlen(pTemp) + 1;
		
		char * param_begin=strchr(preq->url, '?');
		size_t len = strlen(preq->url);
		if(param_begin != NULL)
			len = param_begin - preq->url;

		strcat(pTemp, "QUERY_STRING=");
		if(param_begin != NULL)
			strcat(pTemp, param_begin+1);
		else
			strcat(pTemp, preq->url);
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "PATH_INFO=");		
		strncat(pTemp, preq->url, len);		
		pTemp += strlen(pTemp) + 1;
		
		strcat(pTemp, "PATH_TRANSLATED=");
		strcat(pTemp, pcln->psrv->work_path);		
		strncat(pTemp, preq->url, len);
		replace_char(pTemp, '/', '\\');
		pTemp += strlen(pTemp) + 1;

		strcat(pTemp, "APPL_PHYSICAL_PATH=");
		strcat(pTemp, pcln->psrv->work_path);
		strcat(pTemp, pwrk->name);
		strcat(pTemp, "\\");
		pTemp += strlen(pTemp) + 1;

		//LOCAL_ADDR=127.0.0.1
		//LOGON_USER=		
				  		
		strcat(pTemp, "REMOTE_USER=");          pTemp += strlen(pTemp) + 1;	
		strcat(pTemp, "UNMAPPED_REMOTE_USER="); pTemp += strlen(pTemp) + 1;				 
	}

	return pEnv;
}

char * create_env_isapi(const struct Client * pcln, const struct Worker * pwrk) {
	/*формируем переменные окружения (каждая пара заканчивается '\0' и в конце всех еще один '\0')*/
	char * pEnv = malloc(3 * MAX_HEAD_HTTP), *pTemp = pEnv;
	memset(pEnv, 0, 3 * MAX_HEAD_HTTP); //много переменных окружения у мастер процесса

	/*передаем переменные среды, доступные серверу*/
	extern char ** environ;
	for(char ** env = environ; *env != NULL; ++env) {
		strcat(pTemp, *env);
		pTemp += strlen(pTemp) + 1;
	}
	
	sprintf(pTemp, "ISAPI_PARH=%s", pcln->psrv->work_path);	
	pTemp += strlen(pTemp) + 1;

	sprintf(pTemp, "ISAPI_PORT=%d", pcln->psrv->port+1);
	pTemp += strlen(pTemp) + 1;

	return pEnv;
}

struct Worker * init_Worker(const char *name, size_t len, struct Client * pcln, LPVOID iocp) {
	/*признак успеха*/
	BOOL isOk = TRUE;

	struct Worker * pwrk = malloc(sizeof(struct Worker));
	memset(pwrk, 0, sizeof(struct Worker)); //!!!обязательно!!!
	for(int i = 0; i < 3; ++i)
		pwrk->fd[i].fd_r = pwrk->fd[i].fd_w = INVALID_HANDLE_VALUE;

	pwrk->procInf.hProcess = INVALID_HANDLE_VALUE;
	pwrk->procInf.hThread  = INVALID_HANDLE_VALUE;
	
	while(isOk) {
		/*сформируем путь к worker*/
		if(len == 0 || len >= LEN_WORKER) {
			isOk = FALSE;
			show_err("Ошибка в имени worker", FALSE);
			break;
		}
		memcpy(pwrk->path, name - 1, len + 1);
		memcpy(pwrk->name, name, len);
		if(strstr(pwrk->name, "isapi") != NULL) {
			pwrk->type = WORKER_ISAPI;
			/*связываем сокет с сетевым адресом (коротый хотим использовать)*/
			pwrk->addr.sin_port = htons(pcln->psrv->port + 1);
			pwrk->addr.sin_family = AF_INET;
			inet_pton(AF_INET, "127.0.0.1", &pwrk->addr.sin_addr);
		}else
			pwrk->type = WORKER;
				
		sprintf(pwrk->abs_path, "%s%s.exe", pcln->psrv->work_path, pwrk->name);

		/*создаем три анонимных канала (STDIN, STDOUT и STDERR)*/		
		pwrk->fd[0].overlapped_inf.type = WRITE_WORKER;
		pwrk->fd[0].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
																   TRUE, /*тип сброса TRUE - ручной*/
																   TRUE, /*начальное состояние TRUE - сигнальное*/
																   NULL  /*имя обьекта*/);
		isOk = create_pipe(iocp, pwrk, 0, TRUE, "STDIN");
		if(!isOk) break;
		
		
		pwrk->fd[1].overlapped_inf.type = READ_WORKER;
		pwrk->fd[1].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
																   TRUE, /*тип сброса TRUE - ручной*/
																   TRUE, /*начальное состояние TRUE - сигнальное*/
																   NULL  /*имя обьекта*/);
		isOk = create_pipe(iocp, pwrk, 1, FALSE, "STDOUT");
		if(!isOk) break;
		
		
		pwrk->fd[2].overlapped_inf.type = READ_WORKER_ERR;
		pwrk->fd[2].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*атрибут защиты*/
																   TRUE, /*тип сброса TRUE - ручной*/
																   TRUE, /*начальное состояние TRUE - сигнальное*/
																   NULL  /*имя обьекта*/);
		isOk = create_pipe(iocp, pwrk, 2, FALSE, "STDERR");
		if(!isOk) break;
		
		/*создаем дочерний процесс, указывая потомку дескрипторы stdin, stdout и stderr*/
		pwrk->sti.cb = sizeof(STARTUPINFO);			// указать размер
		pwrk->sti.dwFlags = STARTF_USESTDHANDLES;   //!!!обязательно!!!, чтоб использовал указанные дескрипторы
		pwrk->sti.hStdInput  = pwrk->fd[0].fd_r;
		pwrk->sti.hStdOutput = pwrk->fd[1].fd_w;
		pwrk->sti.hStdError  = pwrk->fd[2].fd_w;

		/*формируем переменные окружения только обычным worker (каждая пара заканчивается '\0' и в конце всех еще один '\0')*/
		char * pEnv = NULL;
		if(pwrk->type == WORKER)
			pEnv = create_env(pcln, pwrk);
		else
			pEnv = create_env_isapi(pcln, pwrk);

		isOk = CreateProcess(pwrk->abs_path,/*путь к программе*/
							 "",            /*параметры коммандной строки*/
							 NULL,          /*доступ AD для нового процесса*/
							 NULL,          /*доступ AD для нового потока*/
							 TRUE,          /*Флаг наследования текущего процесса (разрешаем наследовать дескрипторы)*/
							 0,             /*Флаги способов создания процесса*/
							 pEnv,          /*указатель на блок переменных окружения, для процесса (NULL - значит как у родителя)*/
							 NULL,          /*Текущий диск или каталог (NULL - значит как у родителя)*/
							 &pwrk->sti,    /*Используется для настройки свойств процесса, например расположения окон и заголовок*/
							 &pwrk->procInf /*сведения о процессе (сами заполнятся)*/);
		/*удаляем строку с переменными окружения*/
		free(pEnv);
		if(!isOk) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "Не удалось создать дочерний процесс: %s ", pwrk->abs_path);
			show_err(msg_str, TRUE);						
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
		memset(pwrk->fd[2].data, 0, MAX_HEAD_HTTP);
		start_async(pwrk, 0, pcln->psrv->iocp, &pwrk->fd[2].overlapped_inf);
				
		//STDOUT		
		pwrk->fd[1].len = 0;
		pwrk->fd[1].cur = 0;
		pwrk->fd[1].size = MAX_HEAD_HTTP;
		pwrk->fd[1].data = malloc(MAX_HEAD_HTTP);
		memset(pwrk->fd[1].data, 0, MAX_HEAD_HTTP);
		if(!ReadFile(pwrk->fd[1].fd_r, pwrk->fd[1].data + pwrk->fd[1].len, LEN, &len, (OVERLAPPED*)&pwrk->fd[1].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {
			show_err("Ошибка запуска асинхронной операции ReadFile [STDOUT]", TRUE);
			isOk = FALSE;
		}

		//STDIN	(!!!самым последним, т.к. переставляется буфер клиента!!!)
		if(pwrk->type == WORKER) {
			pwrk->fd[0].size = pcln->size;
			pwrk->fd[0].len  = pcln->len;
			pwrk->fd[0].data = pcln->data;
			pcln->data = NULL;
			pcln->DataBuf.buf = NULL;
			pcln->cur = pcln->len = pcln->size;
		}
		
		//if(pwrk->type == WORKER_ISAPI) {
		//	/*нужно передавать только тело*/
		//	pwrk->fd[0].cur = strstr(pwrk->fd[0].data, "\r\n\r\n") - pwrk->fd[0].data + 4;
		//} else
		//	pwrk->fd[0].cur = 0;
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
		if(pwrk->type == WORKER) 
			pwrk->pcln = pcln;
		pcln->overlapped_inf.type = WAIT;
	}

	pcln->pwrk = pwrk;
	
	return pwrk;
}
struct Worker * release_Worker(struct Worker *pwrk) {
	if(pwrk != NULL) {

		DWORD rc = 0;
		/*убиваем процесс и закрываем его дескрипторы*/
		if(pwrk->procInf.hProcess != INVALID_HANDLE_VALUE) {
			/*забираем код возврата дочернего процесса*/
			GetExitCodeProcess(pwrk->procInf.hProcess, &rc);
			if(rc == STILL_ACTIVE) /*процесс отламался*/;
				
			TerminateProcess(pwrk->procInf.hProcess, NO_ERROR);
			/*CloseHandle(pwrk->procInf.hProcess);
			CloseHandle(pwrk->procInf.hThread);*/
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

			if(rc == 0 && pwrk->pcln->len > 0) {
				if(pwrk->type == WORKER) 
					make200(pwrk->pcln);
				else {
					
					/*парсим запрос*/
					pwrk->pcln->preq = pars_http(pwrk->pcln->data, &pwrk->pcln->len);
					if(pwrk->pcln->preq->cur>0 && pwrk->pcln->preq->body_length==0){
						/*среди заголовков нет CONTENT-LENGTH*/
						make200(pwrk->pcln);
					//	char * tmp = pwrk->pcln->data;
					//	pwrk->pcln->size = pwrk->pcln->len + 30;
					//	char * data=pwrk->pcln->data = malloc(pwrk->pcln->size);
					//	memset(data, 0, pwrk->pcln->len + 30);
					//	char * p=strstr(tmp, "\r\n")+2;
					//	strncat(data, tmp, p - tmp);
					//	data += p - tmp ;
					//	sprintf(data, "Content-Length: %u\r\n", (pwrk->pcln->preq->cur));
					//	               
					//	pwrk->pcln->preq->body_length=pwrk->pcln->preq->cur;
					//	strcat(data, p);						
					//	free(tmp);
					//	pwrk->pcln->len = strlen(pwrk->pcln->data);
					}
				}
				
			} else if(rc == 0)
				make404(pwrk->pcln);
			else 
				make500(pwrk->pcln);
			
			/*запускаем асинхронные операции*/
			pwrk->pcln->overlapped_inf.type = WRITE;
			start_async(pwrk->pcln, 0, pwrk->pcln->psrv->iocp, &pwrk->pcln->overlapped_inf);		
		} 

		/*закрываем дескрипторы*/
		for(int i = 0; i < 3; ++i) {
			if(pwrk->fd[i].fd_r!=INVALID_HANDLE_VALUE) CloseHandle(pwrk->fd[i].fd_r);
			if(pwrk->fd[i].fd_w!=INVALID_HANDLE_VALUE) CloseHandle(pwrk->fd[i].fd_w);
			free(pwrk->fd[i].data);
			WSACloseEvent(pwrk->fd[i].overlapped_inf.overlapped.hEvent);								
		}
		
		free(pwrk);
	}

	return NULL;
}

DWORD WINAPI send_isapi(LPVOID lpParameter) {
	struct Client * pcln = (struct Client *)lpParameter;

	BOOL isOk=TRUE;
	do{
		if(pcln->psrv->pWIsapi == NULL) {
			/*требуется запустить один процесс, обслуживающий загружаемые библиотеки*/
			EnterCriticalSection(&pcln->psrv->cs);
			/*еще раз проверим*/
			if(pcln->psrv->pWIsapi == NULL) {
				pcln->psrv->pWIsapi = init_Worker("isapi", 5, pcln, pcln->psrv->iocp);
				Sleep(2000); //дадим возможность процессу запуститься
			}
			LeaveCriticalSection(&pcln->psrv->cs);
			if(pcln->psrv->pWIsapi == NULL) {
				isOk = FALSE;
				break;
			}
		}		
		/*подключаемся к процессу*/
		pcln->wsfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0/*WSA_FLAG_OVERLAPPED*/); //флаг WSA_FLAG_OVERLAPPED нужен для быстрого запуска асинхронных операций (чтоб сразу возвращали "сведения" о старте - будут кидать SOCKET_ERROR и WSAGetLastError() сообщит о статусе продолжающейся операции - WSA_IO_PENDING)
		if(pcln->wsfd == -1) {
			show_err_wsa("Не получен дескриптор сокета для worker isapi");
			isOk = FALSE;
			break;		
		}	
		/*структура адреса изменит значение после соединения*/
		struct sockaddr_in addr;
		memcpy(&addr, &pcln->psrv->pWIsapi->addr, sizeof(struct sockaddr_in));
		if(0 != WSAConnect(pcln->wsfd, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in), NULL, NULL, NULL, NULL)) {
			show_err_wsa("Не удалось подключиться к worker isapi");			
			/*worker isapi лежит*/
			EnterCriticalSection(&pcln->psrv->cs);
			if(0 != WSAConnect(pcln->wsfd, (const struct sockaddr*)&pcln->psrv->pWIsapi->addr, sizeof(pcln->psrv->pWIsapi->addr), NULL, NULL, NULL, NULL)) {
				pcln->psrv->pWIsapi = release_Worker(pcln->psrv->pWIsapi);
			}
			LeaveCriticalSection(&pcln->psrv->cs);
			isOk = FALSE;
			break;
		}

		/*отправляем размер*/
		WSABUF buf;
		buf.buf = (char*)&pcln->len;
		buf.len = sizeof(pcln->len);
		DWORD len = 0;
		if(0 != WSASend(pcln->wsfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
			show_err_wsa("Не удалось отправить worker isapi размер сообщения");
			/*не удалось отправит или отправлено не всё*/
			isOk = FALSE;
			break;
		}

		/*отправляем данные*/		
		buf.buf = pcln->data;
		buf.len = pcln->len;
		len = 0;
		if(0 != WSASend(pcln->wsfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
			show_err_wsa("Не удалось отправить worker isapi сообщение");
			/*не удалось отправит или отправлено не всё*/
			isOk = FALSE;
			break;
		}

		/*отправляем сокет*/
		WSAPROTOCOL_INFOW inf;
		memset(&inf, 0, sizeof(WSAPROTOCOL_INFOW));
		if(0 != WSADuplicateSocketW(pcln->sfd, pcln->psrv->pWIsapi->procInf.dwProcessId, &inf)) {
			show_err_wsa("Не удалось получить структуру, для передачи сокета worker isapi");
			isOk = FALSE;
			break;
		}
		buf.buf = (char*)&inf;
		buf.len = sizeof(WSAPROTOCOL_INFOW);
		len = 0;		
		if(0 != WSASend(pcln->wsfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
			show_err_wsa("Не удалось отправить worker isapi дескриптор сокета клиента");
			/*не удалось отправит или отправлено не всё*/
			isOk = FALSE;
			break;
		}

		/*ждем окончания обработки (worker isapi разорвет соединение)*/		
		buf.buf = pcln->data;
		buf.len = pcln->len;
		len = 0;
		WSARecv(pcln->wsfd, &buf, 1, &len, &flag, NULL, NULL);
	} while(FALSE);
	
	pcln->wsfd = close_socket(pcln->wsfd);

	if(!isOk) {	
		if(pcln->psrv->pWIsapi!=NULL)
			show_err(pcln->psrv->pWIsapi->fd[1].data,FALSE); 
		make500(pcln);
		/*запускаем асинхронную операцию записи в сокет клиента*/
		pcln->overlapped_inf.type = WRITE;
		start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
	} else {
		/*запускаем асинхронную операцию чтения*/
		pcln->overlapped_inf.type = READ;
		start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
	}

	return 0;
}

BOOL work(struct Client * pcln, LPVOID iocp) {
	BOOL isOk = TRUE;

	char * begin = strstr(pcln->data, "/isapi");
	char * end = strstr(pcln->data, "\r\n");
	if(begin!=NULL && begin < end) {
		pcln->overlapped_inf.type = WAIT;
		/*запускаем отдельный поток передачи данных Worker isapi*/
		HANDLE hThread = CreateThread(NULL,                    /*дескриптор защиты (NULL - не может быть унаследован)*/
									  0,                       /*начальный размер стека (0-взять значение поумолчанию)*/
									  send_isapi,              /*функция потока*/
									  pcln,                    /*параметр потока*/
									  DETACHED_PROCESS,        /*опции создания (в данном случае создается поток, который не будет присоединен в дальнейшем)*/
									  NULL                     /*идентификатор потока (NULL - идентификатор возвращаться не будет)*/);
		if(hThread == NULL) {
			show_err("Не удалось запустить отдельный поток передачи isapi", TRUE);
			make500(pcln);
			/*запускаем асинхронную операцию записи в сокет клиента*/
			pcln->overlapped_inf.type = WRITE;
			start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
		}
	} else {
		/*парсим запрос*/
		pcln->preq = pars_http(pcln->data, &pcln->len);

		if(pcln->preq == NULL || pcln->preq->url==NULL) {
			/*запрос не удалось распарсить 404*/
			make404(pcln);
			/*запускаем асинхронную операцию записи в сокет клиента*/
			pcln->overlapped_inf.type = WRITE;
			start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
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
			} else {
				if(pcln->preq == NULL || pcln->preq->body_length == 0) {
					/*признак конца данных, для worker*/
					pwrk->fd[0].data[pwrk->fd[0].len] = '\0';
					++pwrk->fd[0].len;
				}
				/*запускаем асинхронную операцию записи в канал STDIN worker*/
				start_async((void*)pwrk, 0, pwrk->pcln->psrv->iocp, &pwrk->fd[0].overlapped_inf);
			}
		}
	}

	return isOk;
}

