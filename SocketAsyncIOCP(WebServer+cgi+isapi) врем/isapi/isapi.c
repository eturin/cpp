#include "isapi.h"
#include "../SocketAsync/req.h"
#include "client_isapi.h"
#include "server_isapi.h"
#include <ctype.h>

struct Isapi * init_isapi(const char *name, const char *path) {
	BOOL isOk = FALSE;
	struct Isapi *pIsapi = malloc(sizeof(struct Isapi));
	memset(pIsapi, 0, sizeof(struct Isapi));
	strcpy(pIsapi->path, path);
	strcpy(pIsapi->name, name);

	/*переход в каталог wsisapi.dll, т.к. она работает не самостоятельно*/	
	char dir[256] = {0};
	strcpy(dir, path);
	char * p=strrchr(dir, '\\');
	if(p==NULL)
		show_err("Не удалось определить каталог с dll на основе пути", FALSE);
	else if(*++p='\0',!SetCurrentDirectory(dir))
		show_err("(isapi)Не удалось зайти в каталог с dll", TRUE);
	else if(NULL == (pIsapi->hModule = LoadLibrary(pIsapi->path)))
		show_err("(isapi)Не удалось загрузить dll", TRUE);
	else {
		/*получаем адреса символов загруженной библиотеки*/
		pIsapi->fGetExtensionVersion = GetProcAddress((HMODULE)pIsapi->hModule, "GetExtensionVersion");
		pIsapi->fHttpExtensionProc   = GetProcAddress((HMODULE)pIsapi->hModule, "HttpExtensionProc");
		pIsapi->fTerminateExtension  = GetProcAddress((HMODULE)pIsapi->hModule, "TerminateExtension");
		isOk = pIsapi->fGetExtensionVersion(&pIsapi->Version);
	}

	if(!isOk)
		pIsapi = release_isapi(pIsapi);

	return pIsapi;
}

struct Isapi * release_isapi(struct Isapi * pIsapi) {
	if(pIsapi != NULL) {
		DWORD dwFlags = 1;
		if(pIsapi->fTerminateExtension)
			pIsapi->fTerminateExtension(dwFlags);
		FreeLibrary(pIsapi->hModule);
		free(pIsapi);
	}

	return NULL;
}

/*перевод в верхний регистр*/
char * convert_to_upper(char * b, char *e) {
	char * t = b;
	while(t != e) 
		*t++ = toupper(*t);
	return b;
}
/*перевод в нижний регистр*/
char * convert_to_lower(char * b, char *e) {
	char * t = b;
	while(t != e)
		*t++ = tolower(*t);
	return b;
}
struct Isapi * get_isapi(struct Client_isapi* pcln) {
	struct Isapi * pIsapi = NULL;

	struct Server_isapi* psrv = pcln->psrv;

	char *b = strstr(pcln->preq->url, "/isapi/"), *e = NULL;
	if(NULL != b ) {
		b += 7;				
		e = strchr(b, '/');
		if(NULL == e)
			e = strchr(b, '?');
		if(NULL == e)
			e = strchr(b, ' ');
		if(NULL == e)
			e = b+strlen(b);

		struct hTab *s = htab_find(psrv->hISAPI, convert_to_upper(b,e), e - b);
		if(s == NULL) {
			char path[256] = "isapi/";
			strncat(path, b, e - b + 1);
			strcat(path, "web.config");
			
			/*поиск в конфигурационном файле*/
			HANDLE  hFile = CreateFile(path,
									   GENERIC_READ,          /* открываем, чтоб читать*/
									   FILE_SHARE_READ,       /* читать можно и другим процессам*/
									   NULL,                  /*доступ не нужен*/
									   OPEN_EXISTING,         /*открываем только существующий файл*/
									   FILE_ATTRIBUTE_NORMAL, /*это обычный файл*/
									   NULL);                 

			if(hFile != INVALID_HANDLE_VALUE) {
				DWORD len = GetFileSize(hFile, NULL);
				char *buf = malloc(len+1);
				memset(buf, 0, len + 1);
				if(!ReadFile(hFile, buf, len, &len, NULL))
				   show_err("(isapi)Не удалось прочитать файл web.config", TRUE);
				else {
					CloseHandle(hFile);
					char * pos_b = strstr(convert_to_lower(buf, buf + len), "scriptprocessor=\"");
					if(pos_b==NULL)
						show_err("(isapi)Не удалось найти фрагмент <scriptprocessor=\"> в web.config", FALSE);
					else {
						pos_b += 17;
						char *pos_e = strstr(pos_b, "\"");
						if(pos_e == NULL)
							show_err("(isapi)Не удалось найти конец пути файла фрагмента <scriptprocessor=\"....\"> в web.config", FALSE);
						else {
							htab_add(&psrv->hISAPI, "edo", 0, pos_b, pos_e-pos_b);
						}
					}
				}
				free(buf);
			} else
				show_err("(isapi)Не удалось открыть файл web.config", TRUE);			
		}

		s = htab_find(psrv->hISAPI, convert_to_upper(b, e), e - b);
		if(s != NULL && s->pIsapi != NULL) 
			pIsapi = s->pIsapi;
		else if(s != NULL)
			pIsapi = s->pIsapi = init_isapi(s->key, s->val);
	}

	return pIsapi;
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

BOOL fill_env(struct Client_isapi * pcln) {
	struct Req * preq = pcln->preq;		

	/*передаем переменные среды, доступные серверу*/
	extern char ** environ;
	for(char ** env = environ; *env != NULL; ++env) {
		char *b = strchr(*env, '=')+1;
		htab_add(&pcln->pEnv, *env, b - *env-1, b, strlen(b));
	}
		
	/*формируем переменные окружения на основе заголовков*/
	for(struct hTab const * s = preq->pHeader; s != NULL; s = (struct hTab const*)(s->hh.next)) {
		if(!strcmp(s->key, "HOST")) {
			char * pos = strchr(s->val, ':');
			if(pos != NULL) {
				htab_add(&pcln->pEnv, "SERVER_NAME", 0, s->val, pos - s->val);
				htab_add(&pcln->pEnv, "SERVER_PORT", 0, pos + 1, 0);
			} else {
				htab_add(&pcln->pEnv, "SERVER_NAME", 0, s->val, 0);
				htab_add(&pcln->pEnv, "SERVER_PORT", 0, "80", 0);
			}
			htab_add(&pcln->pEnv, "SERVER_PORT_SECURE", 0, "0", 0);
					
			pos = strstr(pcln->data, "\r\n");
			char *b = pos;
			while(b != pcln->data && *b != ' ') --b;			
			htab_add(&pcln->pEnv, "SERVER_PROTOCOL", 0, b + 1, pos - b - 1);
			b = strchr(b,'/');
			htab_add(&pcln->pEnv, "HTTP_VERSION", 0, b + 1, pos - b - 1);					
		}

		char * tmp = malloc(strlen(s->key) + 6);
		sprintf(tmp, "HTTP_%s", s->key);
		/*заменим некоторые символы*/
		replace_char(tmp, '-', '_');				
		htab_add(&pcln->pEnv, tmp, 0, s->val, 0);
		/*удаляем выделенную память*/
		free(tmp);
	}
	/*GET или POST*/
	htab_add(&pcln->pEnv, "REQUEST_METHOD", 0, preq->cmd, 0);
			
	htab_add(&pcln->pEnv, "AUTH_TYPE"    , 0, "", 0);
	htab_add(&pcln->pEnv, "AUTH_PASSWORD", 0, "", 0);
	htab_add(&pcln->pEnv, "AUTH_USER"    , 0, "", 0);
						
	htab_add(&pcln->pEnv, "HTTPS"               , 0, "off", 0);
	htab_add(&pcln->pEnv, "HTTPS_KEYSIZE"       , 0, ""   , 0);
	htab_add(&pcln->pEnv, "HTTPS_SECRETKEYSIZE" , 0, ""   , 0);
	htab_add(&pcln->pEnv, "HTTPS_SERVER_ISSUER" , 0, ""   , 0);
	htab_add(&pcln->pEnv, "HTTPS_SERVER_SUBJECT", 0, ""   , 0);

			
	htab_add(&pcln->pEnv, "CERT_COOKIE"      , 0, "", 0);
	htab_add(&pcln->pEnv, "CERT_FLAGS"       , 0, "", 0);
	htab_add(&pcln->pEnv, "CERT_ISSUER"      , 0, "", 0);
	htab_add(&pcln->pEnv, "CERT_SERIALNUMBER", 0, "", 0);
	htab_add(&pcln->pEnv, "CERT_SUBJECT"     , 0, "", 0);

	{
		char tmp[100] = {0};
		sprintf(tmp, "%u", preq->body_length);
		htab_add(&pcln->pEnv, "CONTENT_LENGTH", 0, tmp, 0);

		sprintf(tmp, "%u", preq->cur);
		htab_add(&pcln->pEnv, "CONTENT_LENGTH_AVAILABLE", 0, tmp, 0);
	}
						
	const struct hTab *s = htab_find(preq->pHeader, "CONTENT-TYPE", 0);
	if(s != NULL)
		htab_add(&pcln->pEnv, "CONTENT_TYPE", 0, s->val, 0);
	else
		htab_add(&pcln->pEnv, "CONTENT_TYPE", 0, "", 0);
			
	htab_add(&pcln->pEnv, "GATEWAY_INTERFACE", 0, "CGI/1.1", 0);

	{
		char tmp[100] = {0};
		sprintf(tmp, "REMOTE_ADDR=%d.%d.%d.%d", pcln->addr.sin_addr.S_un.S_un_b.s_b1, pcln->addr.sin_addr.S_un.S_un_b.s_b2, pcln->addr.sin_addr.S_un.S_un_b.s_b3, pcln->addr.sin_addr.S_un.S_un_b.s_b4);
		htab_add(&pcln->pEnv, "REMOTE_ADDR", 0, tmp, 0);
		htab_add(&pcln->pEnv, "REMOTE_HOST", 0, tmp, 0);
						
		sprintf(tmp, "%d", pcln->addr.sin_port);
		htab_add(&pcln->pEnv, "REMOTE_PORT", 0, tmp, 0);				
	}

	htab_add(&pcln->pEnv, "SERVER_SOFTWARE", 0, pcln->psrv->name, 0);
			
	htab_add(&pcln->pEnv, "ALL_RAW" , 0, strstr(pcln->data, "\r\n") + 2, 0);			
	htab_add(&pcln->pEnv, "HTTP_URL", 0, preq->url, 0);
			
	htab_add(&pcln->pEnv, "INSTANCE_ID", 0, "1", 0);

	{
		char *e = strstr(preq->url, pcln->pIsapi->name);
		e += strlen(pcln->pIsapi->name);
		htab_add(&pcln->pEnv, "SCRIPT_NAME", 0, preq->url, e - preq->url);
	}

	{
		char * param_begin = strchr(preq->url, '?');
		size_t len = strlen(preq->url);
		if(param_begin != NULL)
			len = param_begin - preq->url;
		if(param_begin != NULL)
			htab_add(&pcln->pEnv, "QUERY_STRING", 0, param_begin + 1, 0);
		else
			htab_add(&pcln->pEnv, "QUERY_STRING", 0, "", 0);

		htab_add(&pcln->pEnv, "PATH_INFO", 0, preq->url, len);

		char tmp[256] = {0};
		strcat(tmp, pcln->psrv->work_path);
		strncat(tmp, preq->url+1, len-1);
		replace_char(tmp, '/', '\\');
		htab_add(&pcln->pEnv, "PATH_TRANSLATED", 0, tmp, 0);

		tmp[0] = '\0';
		sprintf(tmp, "%sisapi\\%s\\", pcln->psrv->work_path, pcln->pIsapi->name);
		htab_add(&pcln->pEnv, "APPL_PHYSICAL_PATH", 0, tmp, 0);
	}

	//LOCAL_ADDR=127.0.0.1
	//LOGON_USER=		
	htab_add(&pcln->pEnv, "REMOTE_USER"         , 0, "", 0);
	htab_add(&pcln->pEnv, "UNMAPPED_REMOTE_USER", 0, "", 0);			
		

	return TRUE;
	
}

BOOL WINAPI GetServerVariable(HCONN hConn, LPSTR lpszVariableName, LPVOID lpvBuffer, LPDWORD lpdwSize) {
	const struct hTab * s = htab_find(((const struct Client_isapi *)hConn)->pEnv, lpszVariableName,0);
	if(s != NULL) {
		if(*lpdwSize != 0) {
			strcpy(lpvBuffer, s->val);
		}
		*lpdwSize = strlen(s->val)+1;
	}

	return s != NULL;
}
BOOL WINAPI ReadClient(HCONN hConn, LPVOID lpvBuffer, LPDWORD lpdwSize) {
	if(!SendAndRecv(FALSE, ((struct Client_isapi *)hConn)->sfd, (struct Client_isapi *)hConn, (char*)lpvBuffer, *lpdwSize, FALSE)) {
		show_err("(isapi)Ошибка извлечения данных из сокета по требованию загружаемой библиотеки", FALSE);
		return FALSE;
	}else
		return TRUE;
}
BOOL WINAPI WriteClient(HCONN hConn, LPVOID lpvBuffer, LPDWORD lpdwBytes, DWORD dwReserved) {
	BOOL ret = TRUE;

	struct Client_isapi * pcln = (struct Client_isapi *)hConn;
	if(pcln->data == NULL) {
		pcln->size = *lpdwBytes;
		pcln->data = malloc(pcln->size);		
	} else {
		pcln->size += *lpdwBytes;
		pcln->data = realloc(pcln->data, pcln->size);		
	}

	memset(pcln->data + pcln->len, 0, pcln->size - pcln->len);
	memcpy(pcln->data + pcln->len, lpvBuffer, *lpdwBytes);
	pcln->len += *lpdwBytes;
	
	return ret;
}

BOOL Send(struct Client_isapi * pcln, const char * msg) {
	BOOL ret = TRUE;

	if(pcln->data != NULL) {
		pcln->preq = release_Req(pcln->preq);
		/*разбор данных ответа клиенту*/
		pcln->preq = pars_http(pcln->data, &pcln->len);
		if(pcln->preq != NULL) {
			if(pcln->preq->body_length == 0 && pcln->preq->cur > 0) {
				/*загружаемая библиотека не вернула размер тела сообщения*/
				char * tmp = pcln->data;
				size_t len_ = pcln->len;

				//добавляем запаса из 50байт на заголовок с длинной тела сообщения
				pcln->size = pcln->len + 50;
				pcln->data = malloc(pcln->size);
				memset(pcln->data, 0, pcln->size);

				//возбмем весь заголовок и один перевод строки
				char *e = strstr(tmp, "\r\n\r\n") + 2;
				pcln->len = e - tmp;
				memcpy(pcln->data, tmp, pcln->len);
				len_ -= pcln->len;

				//добавим заголовок с длинной тела сообщения
				char str[50] = {0};
				pcln->preq->body_length = pcln->preq->cur;
				sprintf(str, "Content-Length: %u\r\n", pcln->preq->body_length);
				size_t len = strlen(str);
				memcpy(pcln->data + pcln->len, str, len);
				pcln->len += len;

				memcpy(pcln->data + pcln->len, e, len_);
				pcln->len += len_;

				free(tmp);
			}

			if(!SendAndRecv(TRUE, pcln->sfd, pcln, pcln->data, pcln->len, FALSE))
				show_err(msg,FALSE);
		} else {
			char * format500 = "HTTP/1.1 500 ERROR\r\n"
				               "Version: HTTP/1.1\r\n"
				               "Cache-Control: no-cache\r\n"
				               "Content-Type: text/html\r\n"
				               "Connection: close\r\n"
				               "Content-Length: 0 \r\n\r\n";
			
			if(!SendAndRecv(TRUE, pcln->sfd, pcln, format500, strlen(format500), FALSE))
				show_err(msg, FALSE);
		}

		free(pcln->data);
		pcln->data = NULL;
	}

	return ret;
}

BOOL WINAPI ServerSupportFunction(HCONN hConn, DWORD dwHSERRequest, LPVOID lpvBuffer, LPDWORD lpdwSize, LPDWORD lpdwDataType) {
	BOOL ret = TRUE;

	struct Client_isapi * pcln = (struct Client_isapi *)hConn;
	if(dwHSERRequest == HSE_REQ_SEND_RESPONSE_HEADER_EX) {
		HSE_SEND_HEADER_EX_INFO * tmp = (HSE_SEND_HEADER_EX_INFO *)lpvBuffer;
		
		if(pcln->data != NULL) 
			Send(pcln, "(isapi)Ошибка отправки промежуточного сообщения из ISAPI клиенту");		

		pcln->size = tmp->cchStatus + tmp->cchHeader + 12;
		pcln->data = malloc(pcln->size);
		memset(pcln->data, 0, pcln->size);

		sprintf(pcln->data, "HTTP/1.1 %s\r\n%s", tmp->pszStatus, tmp->pszHeader);		
		pcln->len = pcln->size-1;
	}

	return ret;
}

DWORD call_HttpExtensionProc(struct Client_isapi * pcln) {

	/*формируем хеш-таблицу переменных окружения*/
	fill_env(pcln);
	/*данные клиента больше не нужны*/
	free(pcln->data);
	pcln->data = NULL;
	pcln->len = pcln->size = pcln->cur = 0;

	/*формируем структуру для вызова isapi*/
	EXTENSION_CONTROL_BLOCK ecb;
	memset(&ecb, 0, sizeof(EXTENSION_CONTROL_BLOCK));

	/*размер типа в байтах*/
	ecb.cbSize = sizeof(EXTENSION_CONTROL_BLOCK);

	/*текущий статус*/
	ecb.dwHttpStatusCode = 200;

	/*версия спецификации isapi*/
	ecb.dwVersion = 524288;/*здесь какая-то константа*/

	/*уникальный идентификатор, для транзакции*/
	ecb.ConnID = pcln;

	/*сообщение от isapi, для логирования, связанное с транзакцией (фиксированный массив символов размера HSE_LOG_BUFFER_LEN)*/
	//ecb.lpszLogData;
	
	/*метод GET или POST*/
	struct hTab * s = htab_find(pcln->pEnv, "REQUEST_METHOD",0);
	ecb.lpszMethod = s->val;

	/*данные, принятые от удаленного пользователя методом GET*/
	s = htab_find(pcln->pEnv, "QUERY_STRING",0);
	ecb.lpszQueryString = s->val;

	/*виртуальный путь к программному файлу библиотеки DLL*/
	s = htab_find(pcln->pEnv, "PATH_INFO",0);
	ecb.lpszPathInfo = s->val;

	/*физический путь к программному файлу библиотеки DLL расширения ISAPI*/
	s = htab_find(pcln->pEnv, "PATH_TRANSLATED",0);
	ecb.lpszPathTranslated = s->val;

	/*общее количество байт данных, которое необходимо получить от удаленного пользователя*/
	s = htab_find(pcln->pEnv, "CONTENT_LENGTH",0);
	ecb.cbTotalBytes = atol(s->val);

	/*размер блока данных, полученных автоматически от браузера посетителя сервера (48kb)*/
	s = htab_find(pcln->pEnv, "CONTENT_LENGTH_AVAILABLE",0);
	ecb.cbAvailable = atol(s->val);

	/*блок автоматически загруженных данных*/	
	ecb.lpbData = pcln->preq->body;

	/*содержит тип принятых данных*/
	s = htab_find(pcln->pEnv, "CONTENT_TYPE",0);
	ecb.lpszContentType = s->val;
	
	/*функции обратных вызовов для isapi*/
	ecb.WriteClient           = WriteClient;	
	ecb.ReadClient            = ReadClient;
	ecb.GetServerVariable     = GetServerVariable;
	ecb.ServerSupportFunction = ServerSupportFunction;
	
	/*вызываем*/
	DWORD ret = pcln->pIsapi->fHttpExtensionProc(&ecb);

	Send(pcln, "Ошибка отправки окончательного ответа от ISAPI");

	/*сообщение от динамической библиотеки отправляем в лог*/
	if(ecb.lpszLogData[0] != '\0')
		show_err(ecb.lpszLogData, FALSE);	
	

	return ret;
}

