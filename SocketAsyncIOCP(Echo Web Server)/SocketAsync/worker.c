#include "worker.h"
#include "req.h"
#include "client.h"
#include "error.h"
#include "http_parser.h"



/*��������� URL*/
int call_request_url_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	req->url = malloc((len + 1)*sizeof(char));
	strncpy(req->url, buf, len);
	req->url[len] = '\0';
	return 0;
}
/*��������� ����� ���������*/
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
/*��������� �������� ���������*/
int call_header_value_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	char * value = req->head[req->head_cnt - 1].value = malloc((len + 1)*sizeof(char));

	strncpy(value, buf, len);
	value[len] = '\0';

	if(!strcmp(req->head[req->head_cnt - 1].field, "Content-Length"))
		sscanf(value, "%d", &req->body_length);

	return 0;
}
/*��������� ������� ������*/
int call_response_status_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *req = (struct Req*)parser->data;
	req->status = malloc((len + 1)*sizeof(char));
	strncpy(req->status, buf, len);
	req->status[len] = '\0';
	return 0;
}
/*��������� ����*/
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
	/*���������� ������ � ��� ���*/
	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);

	/*��������� �������� ���������� �������� ����*/
	struct Req *req = init_Req();
	parser.data = req;
	req->head_length = len;

	/*��� ������� ��������� ������, ��� ��������� ���������� http-���������*/
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

	/*��������� ������*/
	size_t parsed = http_parser_execute(&parser, &settings_null, data, len);

	/*���������� �� �������*/
	if(parsed != len)
		req = release_Req(req);

	return req;
}

BOOL create_pipe(LPVOID iocp, struct Worker *pwrk, int i, BOOL isIn,const char * msg) {
#define MAXLEN 1000
	/*��������� ��� ������*/
	char path[256];
	sprintf(path, "\\\\.\\pipe\\%s\\%d\\%s", pwrk->name, (unsigned)pwrk, msg);

	int lpMode = PIPE_NOWAIT;
	if(isIn) {
		/*�������� ������������ ������*/
		pwrk->fd[i].fd_w = CreateNamedPipe(path,                        /*��� ������*/
										   PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,/*����� �������� ������ (� ������ ������ - ����� ��������������� � ����������� )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*����� ������ ������ (� ������ ������ - �������� ���� � �����������)*/
										   1,                           /*������������ ���������� ���������� ������ (�.�. ������� ����� ���� ��������)*/
										   LEN,                         /*������ ��������� ������ � ������ (�������� ����� ������������)*/
										   LEN,                         /*������ �������� ������ � ������ (�������� ����� ������������)*/
										   0,                           /*����� �������� � ������������� (��� ������������ ������ ������ NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*����� ��������� � ������� ����������*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s �� ������� ������� ����������� ������ [out]", msg);
			show_err(msg_str, TRUE);			
			return FALSE;
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_w, &pwrk->pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {			/*������������ � ������������ ������ (������������� � ����������� ������)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*��������� ������������ �����*/
		pwrk->fd[i].fd_r = CreateFile(path,                              /*��� ������*/
							          GENERIC_READ,                      /*����� �������*/
							          FILE_SHARE_READ,                   /*����� ����������� �������������*/
							          NULL,                              /*������ ��������*/
							          OPEN_EXISTING,                     /*��������� ������������ ��� ������*/
							          0,                                 /*�������� �����*/
							          NULL                               /*������ ��������*/);
		if(pwrk->fd[i].fd_r == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;			
		}
		
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
												  (ULONG_PTR)pwrk->pcln,             /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
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
										   PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,/*����� �������� ������ (� ������ ������ - ����� ��������������� � ����������� )*/
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
		} else if(!ConnectNamedPipe(pwrk->fd[i].fd_r, &pwrk->pcln->overlapped) && ERROR_IO_PENDING != GetLastError()) {			/*������������ � ������������ ������ (������������� � ����������� ������)*/
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		/*��������� ������������ �����*/
		pwrk->fd[i].fd_w = CreateFile(path,                              /*��� ������*/
									  GENERIC_WRITE,                     /*����� �������*/
									  FILE_SHARE_WRITE,/*����� ����������� �������������*/
									  NULL,                              /*������ ��������*/
									  OPEN_EXISTING,                     /*��������� ������������ ��� ������*/
									  0,                                 /*�������� �����*/
									  NULL                               /*������ ��������*/);
		if(pwrk->fd[i].fd_w == INVALID_HANDLE_VALUE) {
			char msg_str[MAXLEN];
			sprintf(msg_str, "%s ������� �� ������� ������������ � ������������ ������ [out]", msg);
			show_err(msg_str, TRUE);
			return FALSE;
		}
		
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
												  (ULONG_PTR)pwrk->pcln,             /*���� ���������� (�������� ����� �������� ��� ����������� �������)*/
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

struct Worker * init_Worker(const char *name, size_t len, struct Client * pcln, LPVOID iocp) {
	/*������� ������*/
	BOOL isOk = TRUE;

	struct Worker * pwrk = malloc(sizeof(struct Worker));
	memset(pwrk, 0, sizeof(struct Worker)); //!!!�����������!!!
	for(int i = 0; i < 3; ++i)
		pwrk->fd[i].fd_r = pwrk->fd[i].fd_w = INVALID_HANDLE_VALUE;
	pwrk->procInf.hProcess = INVALID_HANDLE_VALUE;
	pwrk->procInf.hThread = INVALID_HANDLE_VALUE;
	pwrk->type = WAIT;
	pwrk->pcln = pcln;

	while(isOk) {
		/*���������� ���� � worker*/
		if(len == 0 || len >= LEN_WORKER) {
			isOk = FALSE;
			show_err("������ � ����� worker", FALSE);
			break;
		}
		memcpy(pwrk->name, name, len);
		char path[256];
		sprintf(path, "%s%s.exe", work_path, pwrk->name);

		/*������� ��� ��������� ������*/
		isOk = create_pipe(iocp, pwrk, 0, TRUE, "STDIN");
		if(!isOk) break;
		isOk = create_pipe(iocp, pwrk, 1, FALSE, "STDOUT");
		if(!isOk) break;
		isOk = create_pipe(iocp, pwrk, 2, FALSE, "STDERR");
		if(!isOk) break;

		/*DWORD flags;
		isOk = GetHandleInformation(fd_r_1,&flags);*/

		/*������� �������� �������*/
		pwrk->sti.cb = sizeof(STARTUPINFO);			// ������� ������
		/*������������� ������� ����������� stdin, stdout � stderr*/
		pwrk->sti.dwFlags = STARTF_USESTDHANDLES; //!!!�����������!!!
		pwrk->sti.hStdInput  = pwrk->fd[0].fd_r;
		pwrk->sti.hStdOutput = pwrk->fd[1].fd_w;
		pwrk->sti.hStdError  = pwrk->fd[2].fd_w;

		isOk = CreateProcess(path,          /*���� � ���������*/
							 "",            /*��������� ���������� ������*/
							 NULL,          /*������ AD ��� ������ ��������*/
							 NULL,          /*������ AD ��� ������ ������*/
							 TRUE,          /*���� ������������ �������� �������� (��������� ����������� �����������)*/
							 0,             /*����� �������� �������� ��������*/
							 NULL,          /*��������� �� ��������� ���������� ���������, ��� �������� (NULL - ������ ��� � ��������)*/
							 NULL,          /*������� ���� ��� ������� (NULL - ������ ��� � ��������)*/
							 &pwrk->sti,    /*������������ ��� ��������� ������� ��������, �������� ������������ ���� � ���������*/
							 &pwrk->procInf /*�������� � �������� (���� ����������)*/);
		if(!isOk) {
			show_err("�� ������� ������� �������� �������", TRUE);
			break;
		}

		/*�������� ��������� ����������� ��������� ��������*/
		CloseHandle(pwrk->fd[0].fd_r); pwrk->fd[0].fd_r = INVALID_HANDLE_VALUE;
		CloseHandle(pwrk->fd[1].fd_w); pwrk->fd[1].fd_w = INVALID_HANDLE_VALUE;
		CloseHandle(pwrk->fd[2].fd_w); pwrk->fd[2].fd_w = INVALID_HANDLE_VALUE;

		/*���� ��� �����, ���� �� ���������� � ��������� �����������*/
		break;
	}

	if(!isOk) 
		pcln->pwrk = pwrk = release_Worker(pwrk);
	else {
		/*��������� ���������� worker ������ �������*/
		pcln->DataBuf.buf = pcln->data;
		pcln->cur = 0;
	}
	
	pcln->pwrk = pwrk;
	
	return pwrk;
}
struct Worker * release_Worker(struct Worker *pwrk) {
	if(pwrk != NULL) {
		/*������� ������� � ��������� ��� �����������*/
		if(pwrk->procInf.hProcess != INVALID_HANDLE_VALUE) {
			TerminateProcess(pwrk->procInf.hProcess, NO_ERROR);
			CloseHandle(pwrk->procInf.hProcess);
			CloseHandle(pwrk->procInf.hThread);
		}

		/*��������� �����������*/
		for(int i = 0; i < 3; ++i) {
			CloseHandle(pwrk->fd[i].fd_r);
			CloseHandle(pwrk->fd[i].fd_w);
		}
				
		if(pwrk->type == READ) {
			/*worker �������� ������*/
			make200(pwrk->pcln);
			pwrk->pcln->pwrk = NULL;
		} else if(pwrk->type == WRITE && pwrk->pcln != NULL) {
			/*����� ��������� ������� �� ������ ������� 500*/
			make500(pwrk->pcln);
			pwrk->pcln->pwrk = NULL;
		} else  if(pwrk->pcln != NULL) {
			/*����� ��������� ������� � 404*/
			make404(pwrk->pcln);
			pwrk->pcln->pwrk = NULL;
		}

		free(pwrk);
	}

	return NULL;
}

BOOL work(struct Client * pcln, LPVOID iocp) {
	BOOL isOk = TRUE;

	/*������ ������*/
	pcln->preq = pars_http(pcln->data, pcln->len);

	if(pcln->preq == NULL) {
		/*������ �� ������� ���������� 404*/
		make404(pcln);
	} else {
		char *begin = strchr(pcln->preq->url, '/') + 1;
		char *end = strchr(begin, '/');
		if(end == NULL)
			end = begin + strlen(begin);
		struct Worker * pwrk = init_Worker(begin, end - begin, pcln, iocp);		

		if(pwrk == NULL) {
			/*worker �� �������*/
			make404(pcln);
		} else {
			/*������ worker � client � ������� ������ �� ������ (������ ����, ������ ���������� worker)*/
			pcln->type = WAIT;
			pwrk->type = WRITE;	
			/*������� ����� ������, ��� worker*/
			pcln->data[pcln->len] = '\0';
			++pcln->len;
		}
	}
	
	return isOk;
}

