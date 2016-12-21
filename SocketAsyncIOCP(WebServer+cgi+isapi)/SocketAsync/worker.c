#include "error.h"
#include "socket.h"
#include "server.h"
#include "req.h"
#include "client.h"
#include "worker.h"



BOOL create_pipe(LPVOID iocp, struct Worker *pwrk, int i, BOOL isIn,const char * msg) {
#define MAXLEN 1000
	/*��������� ��� ������*/
	char path[256];
	sprintf(path, "\\\\.\\pipe\\%s\\%d\\%s", pwrk->name, (unsigned)pwrk, msg);

	int lpMode = PIPE_NOWAIT;
	if(isIn) {		
		/*�������� ������������ ������*/
		pwrk->fd[i].fd_w = CreateNamedPipe(path,                        /*��� ������*/
										   PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,/*����� �������� ������ (� ������ ������ - ����� ���������������� � ����������� )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*����� ������ ������ (� ������ ������ - �������� ���� � �����������)*/
										   1,                           /*������������ ���������� ���������� ������ (�.�. ������� ����� ���� ��������)*/
										   MAX_HEAD_HTTP,               /*������ ��������� ������ � ������ (�������� ����� ������������)*/
										   MAX_HEAD_HTTP,               /*������ �������� ������ � ������ (�������� ����� ������������)*/
										   0,                           /*����� �������� � ������������� (��� ������������ ������ ������ NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*����� ��������� � ������� ����������*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� ������� ����������� ������ [out]", msg);
			show_err(msg_str, TRUE);			
			return FALSE;
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_w, (OVERLAPPED*)&pwrk->fd[i].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {			/*������������ � ������������ ������ (������������� � ����������� ������)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*��������� ������������ �����*/
		pwrk->fd[i].fd_r = CreateFile(path,                              /*��� ������*/
							          GENERIC_READ,                      /*����� �������*/
									  FILE_SHARE_READ | FILE_SHARE_WRITE,/*����� ����������� �������������*/
							          NULL,                              /*������ ��������*/
							          OPEN_EXISTING,                     /*��������� ������������ ��� ������*/
									  FILE_FLAG_WRITE_THROUGH,           /*�������� ����� (����� ������ �������������� �����������)*/
							          NULL                               /*������ ��������*/);
		if(pwrk->fd[i].fd_r == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;			
		}

		//DWORD mode = PIPE_NOWAIT;
		//if(!SetNamedPipeHandleState(pwrk->fd[i].fd_r,       /*���������� ������*/
		//	                        &mode,                  /* ����� ����������, � ������� ������ ����� ����� ������*/
		//	                        NULL,                   /* ����� ����������, � ������� ����������� ������������ ������ ������, ������������� � �����*/
		//	                        NULL                    /* ����� ������������ �������� ����� ��������� ������*/)) {
		//	char msg_str[MAXLEN];
		//	sprintf(msg_str, "%s �� ������� �������� �� ����������� ����� ������ ������ [out]", msg);
		//	show_err(msg_str, TRUE);
		//	return FALSE;
		//}
		
		/*�������� ������������ ��� ��� ������ ������, ������� ������ ���� � �������*/		
		if(!SetHandleInformation(pwrk->fd[i].fd_r,       /* ���������� ������*/
			                     HANDLE_FLAG_INHERIT,    /* ���������� ������ (����� ������������ � �������� �����������)*/
			                     1                       /* ����� �������� �������*/)) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� �������� ������������ ����������� ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}		

		/*��������� ����� ������ � ������ (����� ����������� �������� � ���� ������ ����� ������������ ��������� ����)*/
		pwrk->fd[i].iocp = CreateIoCompletionPort(pwrk->fd[i].fd_w,                  /*���������� ������*/
												  iocp,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
												  (ULONG_PTR)pwrk,                   /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
												  1                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
		if(pwrk->fd[i].iocp == NULL) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� ������� � ������ OS ���������� ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
	}else{
		/*�������� ������������ ������*/
		pwrk->fd[i].fd_r = CreateNamedPipe(path,                        /*��� ������*/
										   PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,/*����� �������� ������ (� ������ ������ - ����� ���������������� � ����������� )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*����� ������ ������ (� ������ ������ - �������� ���� � �����������)*/
										   1,                           /*������������ ���������� ���������� ������ (�.�. ������� ����� ���� ��������)*/
										   LEN,                         /*������ ��������� ������ � ������ (�������� ����� ������������)*/
										   LEN,                         /*������ �������� ������ � ������ (�������� ����� ������������)*/
										   0,                           /*����� �������� � ������������� (��� ������������ ������ ������ NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*����� ��������� � ������� ����������*/);
		if(pwrk->fd[i].fd_r == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� ������� ����������� ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_r, (OVERLAPPED*)&pwrk->fd[i].overlapped_inf) && ERROR_IO_PENDING != GetLastError()) {			/*������������ � ������������ ������ (������������� � ����������� ������)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*��������� ������������ �����*/
		pwrk->fd[i].fd_w = CreateFile(path,                              /*��� ������*/
									  GENERIC_WRITE,                     /*����� �������*/
									  FILE_SHARE_WRITE | FILE_SHARE_READ,/*����� ����������� �������������*/
									  NULL,                              /*������ ��������*/
									  OPEN_EXISTING,                     /*��������� ������������ ��� ������*/
									  FILE_FLAG_WRITE_THROUGH,           /*�������� ����� (����� ������ �������������� �����������)*/
									  NULL                               /*������ ��������*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}

		//DWORD mode = PIPE_READMODE_BYTE | PIPE_NOWAIT; //��. ��������� http://www.frolov-lib.ru/books/bsp/v27/ch2_3.htm
		//if(!SetNamedPipeHandleState(pwrk->fd[i].fd_w,       /*���������� ������*/
		//	                        &mode,                  /* ����� ����������, � ������� ������ ����� ����� ������*/
		//	                        NULL,                   /* ����� ����������, � ������� ����������� ������������ ������ ������, ������������� � �����*/
		//	                        NULL                    /* ����� ������������ �������� ����� ��������� ������*/)) {
		//	char msg_str[MAXLEN];
		//	sprintf(msg_str, "%s �� ������� �������� �� ����������� ����� ������ ������ [out]", msg);
		//	show_err(msg_str, TRUE);
		//	return FALSE;
		//}
		
		/*�������� ������������ ��� ��� ������ ������, ������� ������ ���� � �������*/
		if(!SetHandleInformation(pwrk->fd[i].fd_w,       /* ���������� ������*/
			                     HANDLE_FLAG_INHERIT,    /* ���������� ������ (����� ������������ � �������� �����������)*/
			                     1                       /* ����� �������� �������*/)) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� �������� ������������ ����������� ������ [in]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		
		/*��������� ����� ������ � ������ (����� ����������� �������� � ���� ������ ����� ������������ ��������� ����)*/
		pwrk->fd[i].iocp = CreateIoCompletionPort(pwrk->fd[i].fd_r,                  /*���������� ������*/
												  iocp,                              /*���������� ������������� ����� ���������� I/O (��� ����� � ��������� ��������)*/
												  (ULONG_PTR)pwrk,                   /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
												  1                                  /*����� ������������  ����������� ������� (0-�� ���������� ���� ����������)*/);
		if(pwrk->fd[i].iocp == NULL) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� ������� � ������ OS ���������� ������ [in]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
	}

	return TRUE;
}

/*������ ������� � ������*/
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

	/*��������� ���������� ��������� (������ ���� ������������� '\0' � � ����� ���� ��� ���� '\0')*/
	char * pEnv = malloc(3 * MAX_HEAD_HTTP), *pTemp = pEnv;
	memset(pEnv, 0, 3 * MAX_HEAD_HTTP); //���� ��������� ��������� ������� ������ ���� �� ����������

	/*�������� ���������� �����, ��������� �������*/
	extern char ** environ;	
	for(char ** env = environ; *env != NULL;++env){
		strcat(pTemp, *env);		
		pTemp += strlen(pTemp) + 1;		
	}

	if(preq != NULL) {
		/*��������� ���������� ��������� �� ������ ����������*/
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
			/*������� ��������� �������*/
			replace_char(pTemp, '-', '_');
			strcat(pTemp, "=");
			strcat(pTemp, s->val);
			pTemp += strlen(pTemp) + 1;			
		}
		/*GET ��� POST*/
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
	/*��������� ���������� ��������� (������ ���� ������������� '\0' � � ����� ���� ��� ���� '\0')*/
	char * pEnv = malloc(3 * MAX_HEAD_HTTP), *pTemp = pEnv;
	memset(pEnv, 0, 3 * MAX_HEAD_HTTP); //����� ���������� ��������� � ������ ��������

	/*�������� ���������� �����, ��������� �������*/
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
	/*������� ������*/
	BOOL isOk = TRUE;

	struct Worker * pwrk = malloc(sizeof(struct Worker));
	memset(pwrk, 0, sizeof(struct Worker)); //!!!�����������!!!
	for(int i = 0; i < 3; ++i)
		pwrk->fd[i].fd_r = pwrk->fd[i].fd_w = INVALID_HANDLE_VALUE;

	pwrk->procInf.hProcess = INVALID_HANDLE_VALUE;
	pwrk->procInf.hThread  = INVALID_HANDLE_VALUE;
	
	while(isOk) {
		/*���������� ���� � worker*/
		if(len == 0 || len >= LEN_WORKER) {
			isOk = FALSE;
			show_err("������ � ����� worker", FALSE);
			break;
		}
		memcpy(pwrk->path, name - 1, len + 1);
		memcpy(pwrk->name, name, len);
		if(strstr(pwrk->name, "isapi") != NULL) {
			pwrk->type = WORKER_ISAPI;
			/*��������� ����� � ������� ������� (������� ����� ������������)*/
			pwrk->addr.sin_port = htons(pcln->psrv->port + 1);
			pwrk->addr.sin_family = AF_INET;
			inet_pton(AF_INET, "127.0.0.1", &pwrk->addr.sin_addr);
		}else
			pwrk->type = WORKER;
				
		sprintf(pwrk->abs_path, "%s%s.exe", pcln->psrv->work_path, pwrk->name);

		/*������� ��� ��������� ������ (STDIN, STDOUT � STDERR)*/		
		pwrk->fd[0].overlapped_inf.type = WRITE_WORKER;
		pwrk->fd[0].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*������� ������*/
																   TRUE, /*��� ������ TRUE - ������*/
																   TRUE, /*��������� ��������� TRUE - ����������*/
																   NULL  /*��� �������*/);
		isOk = create_pipe(iocp, pwrk, 0, TRUE, "STDIN");
		if(!isOk) break;
		
		
		pwrk->fd[1].overlapped_inf.type = READ_WORKER;
		pwrk->fd[1].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*������� ������*/
																   TRUE, /*��� ������ TRUE - ������*/
																   TRUE, /*��������� ��������� TRUE - ����������*/
																   NULL  /*��� �������*/);
		isOk = create_pipe(iocp, pwrk, 1, FALSE, "STDOUT");
		if(!isOk) break;
		
		
		pwrk->fd[2].overlapped_inf.type = READ_WORKER_ERR;
		pwrk->fd[2].overlapped_inf.overlapped.hEvent = CreateEvent(NULL, /*������� ������*/
																   TRUE, /*��� ������ TRUE - ������*/
																   TRUE, /*��������� ��������� TRUE - ����������*/
																   NULL  /*��� �������*/);
		isOk = create_pipe(iocp, pwrk, 2, FALSE, "STDERR");
		if(!isOk) break;
		
		/*������� �������� �������, �������� ������� ����������� stdin, stdout � stderr*/
		pwrk->sti.cb = sizeof(STARTUPINFO);			// ������� ������
		pwrk->sti.dwFlags = STARTF_USESTDHANDLES;   //!!!�����������!!!, ���� ����������� ��������� �����������
		pwrk->sti.hStdInput  = pwrk->fd[0].fd_r;
		pwrk->sti.hStdOutput = pwrk->fd[1].fd_w;
		pwrk->sti.hStdError  = pwrk->fd[2].fd_w;

		/*��������� ���������� ��������� ������ ������� worker (������ ���� ������������� '\0' � � ����� ���� ��� ���� '\0')*/
		char * pEnv = NULL;
		if(pwrk->type == WORKER)
			pEnv = create_env(pcln, pwrk);
		else
			pEnv = create_env_isapi(pcln, pwrk);

		isOk = CreateProcess(pwrk->abs_path,/*���� � ���������*/
							 "",            /*��������� ���������� ������*/
							 NULL,          /*������ AD ��� ������ ��������*/
							 NULL,          /*������ AD ��� ������ ������*/
							 TRUE,          /*���� ������������ �������� �������� (��������� ����������� �����������)*/
							 0,             /*����� �������� �������� ��������*/
							 pEnv,          /*��������� �� ���� ���������� ���������, ��� �������� (NULL - ������ ��� � ��������)*/
							 NULL,          /*������� ���� ��� ������� (NULL - ������ ��� � ��������)*/
							 &pwrk->sti,    /*������������ ��� ��������� ������� ��������, �������� ������������ ���� � ���������*/
							 &pwrk->procInf /*�������� � �������� (���� ����������)*/);
		/*������� ������ � ����������� ���������*/
		free(pEnv);
		if(!isOk) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "�� ������� ������� �������� �������: %s ", pwrk->abs_path);
			show_err(msg_str, TRUE);						
			break;
		}		

		/*�������� ��������� ����������� ��������� ��������*/
		CloseHandle(pwrk->fd[0].fd_r); pwrk->fd[0].fd_r = INVALID_HANDLE_VALUE;
		CloseHandle(pwrk->fd[1].fd_w); pwrk->fd[1].fd_w = INVALID_HANDLE_VALUE;
		CloseHandle(pwrk->fd[2].fd_w); pwrk->fd[2].fd_w = INVALID_HANDLE_VALUE;

		/*���� ��� �����, ���� �� ���������� � ��������� �����������*/
		break;
	}

	/*��������� ���������� worker ������ �������*/
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
			show_err("������ ������� ����������� �������� ReadFile [STDOUT]", TRUE);
			isOk = FALSE;
		}

		//STDIN	(!!!����� ���������, �.�. �������������� ����� �������!!!)
		if(pwrk->type == WORKER) {
			pwrk->fd[0].size = pcln->size;
			pwrk->fd[0].len  = pcln->len;
			pwrk->fd[0].data = pcln->data;
			pcln->data = NULL;
			pcln->DataBuf.buf = NULL;
			pcln->cur = pcln->len = pcln->size;
		}
		
		//if(pwrk->type == WORKER_ISAPI) {
		//	/*����� ���������� ������ ����*/
		//	pwrk->fd[0].cur = strstr(pwrk->fd[0].data, "\r\n\r\n") - pwrk->fd[0].data + 4;
		//} else
		//	pwrk->fd[0].cur = 0;
	}
	
	if(!isOk) {
		if(pwrk->fd[0].data != NULL) {
			/*!!!���������� ����� ������� � �������� ���������!!!*/
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
		/*������� ������� � ��������� ��� �����������*/
		if(pwrk->procInf.hProcess != INVALID_HANDLE_VALUE) {
			/*�������� ��� �������� ��������� ��������*/
			GetExitCodeProcess(pwrk->procInf.hProcess, &rc);
			if(rc == STILL_ACTIVE) /*������� ���������*/;
				
			TerminateProcess(pwrk->procInf.hProcess, NO_ERROR);
			/*CloseHandle(pwrk->procInf.hProcess);
			CloseHandle(pwrk->procInf.hThread);*/
		}

		if(pwrk->pcln != NULL) {
			/*����� ��������� ������� �� ������ ������� 500*/
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
					
					/*������ ������*/
					pwrk->pcln->preq = pars_http(pwrk->pcln->data, &pwrk->pcln->len);
					if(pwrk->pcln->preq->cur>0 && pwrk->pcln->preq->body_length==0){
						/*����� ���������� ��� CONTENT-LENGTH*/
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
			
			/*��������� ����������� ��������*/
			pwrk->pcln->overlapped_inf.type = WRITE;
			start_async(pwrk->pcln, 0, pwrk->pcln->psrv->iocp, &pwrk->pcln->overlapped_inf);		
		} 

		/*��������� �����������*/
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
			/*��������� ��������� ���� �������, ������������� ����������� ����������*/
			EnterCriticalSection(&pcln->psrv->cs);
			/*��� ��� ��������*/
			if(pcln->psrv->pWIsapi == NULL) {
				pcln->psrv->pWIsapi = init_Worker("isapi", 5, pcln, pcln->psrv->iocp);
				Sleep(2000); //����� ����������� �������� �����������
			}
			LeaveCriticalSection(&pcln->psrv->cs);
			if(pcln->psrv->pWIsapi == NULL) {
				isOk = FALSE;
				break;
			}
		}		
		/*������������ � ��������*/
		pcln->wsfd = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0/*WSA_FLAG_OVERLAPPED*/); //���� WSA_FLAG_OVERLAPPED ����� ��� �������� ������� ����������� �������� (���� ����� ���������� "��������" � ������ - ����� ������ SOCKET_ERROR � WSAGetLastError() ������� � ������� �������������� �������� - WSA_IO_PENDING)
		if(pcln->wsfd == -1) {
			show_err_wsa("�� ������� ���������� ������ ��� worker isapi");
			isOk = FALSE;
			break;		
		}	
		/*��������� ������ ������� �������� ����� ����������*/
		struct sockaddr_in addr;
		memcpy(&addr, &pcln->psrv->pWIsapi->addr, sizeof(struct sockaddr_in));
		if(0 != WSAConnect(pcln->wsfd, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in), NULL, NULL, NULL, NULL)) {
			show_err_wsa("�� ������� ������������ � worker isapi");			
			/*worker isapi �����*/
			EnterCriticalSection(&pcln->psrv->cs);
			if(0 != WSAConnect(pcln->wsfd, (const struct sockaddr*)&pcln->psrv->pWIsapi->addr, sizeof(pcln->psrv->pWIsapi->addr), NULL, NULL, NULL, NULL)) {
				pcln->psrv->pWIsapi = release_Worker(pcln->psrv->pWIsapi);
			}
			LeaveCriticalSection(&pcln->psrv->cs);
			isOk = FALSE;
			break;
		}

		/*���������� ������*/
		WSABUF buf;
		buf.buf = (char*)&pcln->len;
		buf.len = sizeof(pcln->len);
		DWORD len = 0;
		if(0 != WSASend(pcln->wsfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
			show_err_wsa("�� ������� ��������� worker isapi ������ ���������");
			/*�� ������� �������� ��� ���������� �� ��*/
			isOk = FALSE;
			break;
		}

		/*���������� ������*/		
		buf.buf = pcln->data;
		buf.len = pcln->len;
		len = 0;
		if(0 != WSASend(pcln->wsfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
			show_err_wsa("�� ������� ��������� worker isapi ���������");
			/*�� ������� �������� ��� ���������� �� ��*/
			isOk = FALSE;
			break;
		}

		/*���������� �����*/
		WSAPROTOCOL_INFOW inf;
		memset(&inf, 0, sizeof(WSAPROTOCOL_INFOW));
		if(0 != WSADuplicateSocketW(pcln->sfd, pcln->psrv->pWIsapi->procInf.dwProcessId, &inf)) {
			show_err_wsa("�� ������� �������� ���������, ��� �������� ������ worker isapi");
			isOk = FALSE;
			break;
		}
		buf.buf = (char*)&inf;
		buf.len = sizeof(WSAPROTOCOL_INFOW);
		len = 0;		
		if(0 != WSASend(pcln->wsfd, &buf, 1, &len, 0, NULL, NULL) || buf.len != len) {
			show_err_wsa("�� ������� ��������� worker isapi ���������� ������ �������");
			/*�� ������� �������� ��� ���������� �� ��*/
			isOk = FALSE;
			break;
		}

		/*���� ��������� ��������� (worker isapi �������� ����������)*/		
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
		/*��������� ����������� �������� ������ � ����� �������*/
		pcln->overlapped_inf.type = WRITE;
		start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
	} else {
		/*��������� ����������� �������� ������*/
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
		/*��������� ��������� ����� �������� ������ Worker isapi*/
		HANDLE hThread = CreateThread(NULL,                    /*���������� ������ (NULL - �� ����� ���� �����������)*/
									  0,                       /*��������� ������ ����� (0-����� �������� �����������)*/
									  send_isapi,              /*������� ������*/
									  pcln,                    /*�������� ������*/
									  DETACHED_PROCESS,        /*����� �������� (� ������ ������ ��������� �����, ������� �� ����� ����������� � ����������)*/
									  NULL                     /*������������� ������ (NULL - ������������� ������������ �� �����)*/);
		if(hThread == NULL) {
			show_err("�� ������� ��������� ��������� ����� �������� isapi", TRUE);
			make500(pcln);
			/*��������� ����������� �������� ������ � ����� �������*/
			pcln->overlapped_inf.type = WRITE;
			start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
		}
	} else {
		/*������ ������*/
		pcln->preq = pars_http(pcln->data, &pcln->len);

		if(pcln->preq == NULL || pcln->preq->url==NULL) {
			/*������ �� ������� ���������� 404*/
			make404(pcln);
			/*��������� ����������� �������� ������ � ����� �������*/
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
				/*worker �� �������*/
				make404(pcln);
				/*��������� ����������� �������� ������ � ����� �������*/
				pcln->overlapped_inf.type = WRITE;
				start_async((void*)pcln, 0, pcln->psrv->iocp, &pcln->overlapped_inf);
			} else {
				if(pcln->preq == NULL || pcln->preq->body_length == 0) {
					/*������� ����� ������, ��� worker*/
					pwrk->fd[0].data[pwrk->fd[0].len] = '\0';
					++pwrk->fd[0].len;
				}
				/*��������� ����������� �������� ������ � ����� STDIN worker*/
				start_async((void*)pwrk, 0, pwrk->pcln->psrv->iocp, &pwrk->fd[0].overlapped_inf);
			}
		}
	}

	return isOk;
}

