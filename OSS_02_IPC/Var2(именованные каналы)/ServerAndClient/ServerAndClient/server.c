#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "htab.h"

/*���������� ���������� ������������ ������*/
#define CNT 10
/*������ ������*/
#define LEN 256

/*���� ��������*/
#define WAITING_STATE 0
#define READING_STATE 1
#define WRITING_STATE 2

/*��������� ��� ��������� ���������� ������*/
struct pipeInfo {
	//���������� ����������
	HANDLE fd;
	//������ ������
	char buf_in[LEN];
	size_t len_in;
	char buf_out[LEN];
	size_t len_out;
	//��������� ������������ ������
	OVERLAPPED Overlap;
	//�������� ����������� ��������
	BOOL wait;
	//��� ��������� �������
	int type;
	
	//��������� �� ���� ������� ����� ����������
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

	/*����������� ��� ������*/
	printf("������� ��� ������������ ������ (\\\\.\\pipe\\<���?>): ");
	char buf[LEN];
	scanf("%s", buf);
	
	char path[LEN];
	sprintf(path, "\\\\.\\pipe\\%s", buf);
	
	/*������������� ��������� ������� � ������ (��� ����)*/
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	/*�������������� ���������� ������������ ������*/
	struct pipeInfo pipe[CNT];
	memset(pipe, 0, CNT*sizeof(struct pipeInfo));
	HANDLE hEvent[CNT]; //����������� �������, ��������� � ������ �������
	for(size_t i = 0; i < CNT; ++i) {
		/*������� ���������� ������ � ���� OS*/
		pipe[i].fd=CreateNamedPipe(path,                        /*��� ������*/
						           PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,/*����� �������� ������ (� ������ ������ - ����� ��������������� � ����������� )*/
						           PIPE_TYPE_BYTE | PIPE_WAIT,  /*����� ������ ������ (� ������ ������ - �������� ���� � �����������)*/
						           CNT,                         /*������������ ���������� ���������� ������ (�.�. ������� ����� ���� ��������)*/
						           LEN,                         /*������ ��������� ������ � ������ (�������� ����� ������������)*/
						           LEN,                         /*������ �������� ������ � ������ (�������� ����� ������������)*/
						           0,                           /*����� �������� � ������������� (��� ������������ ������ ������ NMPWAIT_USE_DEFAULT_WAIT)*/
								   &sa                          /*����� ��������� � ������� ����������*/);
		if(pipe[i].fd == INVALID_HANDLE_VALUE) {
			show_err("�� ������� �������� ���������� ���������� ������������ ������");
			continue;
		}
		
		/*������� �������, ������� ����� ����������� ����������� ������� ���� ���������� ������*/
		hEvent[i] = CreateEvent(NULL,   /*������� ������*/
								 TRUE,	 /*��� ������ TRUE - ������*/
								 TRUE,	 /*��������� ��������� TRUE - ����������*/
								 NULL	 /*��� ������� (���� �������, �� ���������� ��������)*/);
		if(hEvent[i] == NULL) {
			show_err("�� ������� �������� ���������� �������");
			continue;
		} else {
			pipe[i].Overlap.hEvent = hEvent[i];
		}
		
		/*������������ � ���������� ������*/		
		Connect(&pipe[i]);				
	}

	/*���������� ��������� ������� ����������� ������*/
	isOk=Loop(pipe,hEvent);

	/*��������� ��� ����������� (���� �� ���������, ����� ������� ����������)*/
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
	int isOk = FALSE; //�� ������� ���������� (������� ��������� ��� ������)

	/*������������� ���-�������*/
	pipe->ht = create_hTab(0);

	/*������������ � ������, �� �� ���� (������ ������� 0)*/
	if(ConnectNamedPipe(pipe->fd       /*���������� ������������ ������*/,
						&pipe->Overlap /*����� ��������� OVERLAPPED (��� ������������ ������)*/)) {
		show_err("������ ����������� � ������������ ������");
		return FALSE;
	}

	/*����������� ���������*/
	switch(GetLastError()) {
		case ERROR_IO_PENDING:
			/*������� ���������� � ��������*/
			isOk = TRUE;
			break;
		case ERROR_PIPE_CONNECTED:
			/*������ ����������� ����� CreateNamedPipe � ConnectNamedPipe (������ ��������� ������� �� �����������, ��� ���� �������� ������)*/
			if(SetEvent(pipe->Overlap.hEvent))
				break;
		default:
			/*������ ������*/
			show_err("������ ����������� � ������������ ������ ��� ����� ��������� �������");
	}

	/*�������� ���������� ����������� ��������*/
	pipe->wait = isOk;
	/*��� ����������� ��������*/
	pipe->type = isOk ? WAITING_STATE : READING_STATE;

	return isOk;
}

void Reconnect(struct pipeInfo * pipe) {
	/*����������� �� ����������*/
	if(!DisconnectNamedPipe(pipe->fd))
		show_err("�� ������� ����������� �� ������������ ������");
	
	/*������������ �������� ���-�������*/
	releas_hTab(pipe->ht);

	/*����� ������������ � ������������ ������*/
	Connect(pipe);	
}

void ReadDATA(struct pipeInfo * pipe) {
	/*�������� ��������� ����������� ��������*/
	pipe->type = READING_STATE;
	pipe->wait = FALSE; //����������� �������� ��� �� ��������

	/*������������ �������� ������������ ������*/
	BOOL isOk = ReadFile(pipe->fd,         /*���������� ���������� ������������ ������*/
					     pipe->buf_in,     /*����� ��� ������ ������ �� �������*/
					     LEN*sizeof(char), /*������ ������*/
					     &pipe->len_in,    /*���������� �������*/
					     &pipe->Overlap    /*��������� ������������ ���������� ��������*/);

	
	if(isOk && pipe->len_in != 0) {
		/*�� ������ ����� �� ��������� ������ �� ������� (���������)*/
		Work(pipe);		
	}else if(!isOk && GetLastError() == ERROR_IO_PENDING) {
		/*����������� �������� ��� �� ���������*/
		pipe->wait = TRUE;  //����� ����� ����������!!!		
	} else {
		/*������ ������ ��� ������ ����������*/
		show_err("������");
		Reconnect(pipe);
	}	
}

void WriteDATA(struct pipeInfo * pipe) {
	/*�������� ��������� ����������� ��������*/
	pipe->type = WRITING_STATE;
	pipe->wait = FALSE; //����������� �������� ��� �� ��������

	/*����� ���������������� �������� ����������� ������*/
	size_t len = 0;
	BOOL isOk = WriteFile(pipe->fd,                  /*���������� ������*/
						  pipe->buf_out,             /*������������ ������*/
						  pipe->len_out*sizeof(char),/*������ ������������ ������*/
						  &len,                      /*���������� ���������� ����*/
						  &pipe->Overlap             /*��������� ����������� ��������*/);

	if(isOk && len == pipe->len_out*sizeof(char)) {
		/*� ����� ����� �� ���� ������*/
		ReadDATA(pipe);
	} else if(!isOk && (GetLastError() == ERROR_IO_PENDING)) {
		/*� ������ �� �������� ������ ��-�� ��� �� ��������������� ����������*/
		pipe->wait = TRUE; //����� ����� ����������!!!
	} else {
		/*������ ������ ��� ������ ����������*/
		show_err("������");
		Reconnect(pipe);
	}
}

int Loop(struct pipeInfo * pipe, HANDLE * hEvent) {
	while(TRUE) {
		/*���� ����� �� �������, ������������� ���������� ������������ ������*/
		DWORD i = WaitForMultipleObjects(CNT,    /*������ �������*/
										 hEvent, /*������ ������������ �������*/
										 FALSE,  /*�� ��������� ���� ������� ������������*/
										 INFINITE/*����� ����������*/);
		/*�������� ������ ������������ �������*/
		i -= WAIT_OBJECT_0;

		DWORD len;
		BOOL isOk;
		/*���� �� ������� ���������� ����������� ��������, �� ������ �� ������*/
		if(!pipe[i].wait)
			continue;
		
		/*��������� ��������� ��������� ����������� �������� �� ������� �������*/
		isOk = GetOverlappedResult(pipe[i].fd,      /*���������� ������*/
									&pipe[i].Overlap,/*OVERLAPPED*/
									&len,            /*�������� ��� ���������� ����*/
									FALSE            /*�� �����*/);
		/*��������� ��������� ����������� ��������, ��������� � ���� ��������*/
		switch(pipe[i].type) {
			case WAITING_STATE:
				/*����� ����������*/
				if(!isOk) {
					show_err("����������� ���������� � �������� �� �������");
					Reconnect(&pipe[i]);
				}else
					//��������� ����������� ��������, ������� ����� ���������
					ReadDATA(&pipe[i]);
				break;

			case READING_STATE:
				/*����������� ������ �� ���������� ������������ ������*/
				if(!isOk || len == 0) {
					//�� �����, ��� ������ ���-�� ��������, � �� ����������
					Reconnect(&pipe[i]);					
				} else {				
					pipe[i].len_in = len;					
					//��������� ���������� ������
					Work(&pipe[i]);
				}
				break;

			case WRITING_STATE:
				/*����������� ������ � ���������� ������������ ������*/
				if(!isOk || len != pipe[i].len_out) {
					//�� ������ ���-�� ���������, � ������  ����������
					Reconnect(&pipe[i]);					
				} else
					//��������� ����������� ��������, ������� ����� ���������
					ReadDATA(&pipe[i]);
				break;
		}				
	}
}

void Work(struct pipeInfo * pipe) {
	/*
	  1) set ���� ��������
		 ���������� ��������� � ������ �������� ��� ��������� ������
		 � �������� � ����� ������: acknowledged
	  2) get ����
	     ���� ���� ������� � ���������, ������� �������� � ����� ������ � �������
	            found ��������
	     � ��������� ������� ������� �������� � ����� ������ missing.
	  3) list
		 ���������� �������� � ����� ������, ���������� ����� ������ ���
         ��������� � ��������� �����.
	  4) delete ����
         ���� ���� ������������ � ���������, ������� �������� � ����� ������
         deleted, ����� � ������ missing.
      5) quit
         ���������� ��������� ����������� ����� �� �������
	*/
	
	/*��������� �������*/
	char cmd[LEN];
	char str_key[LEN];
	char str_val[LEN];
	int res=sscanf(pipe->buf_in,"%s %s %s", cmd, str_key, str_val);

	/*������������ �������*/
	if(res == 0) {
		sprintf(pipe->buf_out,"missing");
		pipe->len_out = strlen(pipe->buf_out)+1;
	} else if(!strcmp(cmd, "set")) {
		if(res == 3) {
			/*��������� � ���-������� ���� � ��������*/
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
			/*�� ���������� �����������*/
			sprintf(pipe->buf_out, "missing");
			pipe->len_out = strlen(pipe->buf_out)+1;
		}
	} else if(!strcmp(cmd, "get")) {
		if(res == 2) {
			/*�������� �������� �� �������*/
			char * val = search_hTab(pipe->ht,str_key);			
			if(val!=NULL)
				sprintf(pipe->buf_out, "found %s", val);
			else
				sprintf(pipe->buf_out, "missing");

			pipe->len_out = strlen(pipe->buf_out)+1;
		} else {
			/*�� ���������� �����������*/
			sprintf(pipe->buf_out, "missing");
			pipe->len_out = strlen(pipe->buf_out)+1;
		}
	} else if(!strcmp(cmd, "list")) {
		/*��������*/
		memset(pipe->buf_out,0,LEN*sizeof(char));
		/*��������� �������*/
		for(size_t i = 0; i < pipe->ht->size;++i)
			if(pipe->ht->key_val[i].key!=NULL)
				sprintf(pipe->buf_out, "%s%s ", pipe->buf_out, pipe->ht->key_val[i].key);
		pipe->len_out = strlen(pipe->buf_out)+1;
	} else if(!strcmp(cmd, "delete")) {
		if(res == 2) {
			/*�������� �������� �� �������*/
			int res = del_hTab(pipe->ht, str_key);
			if(res)
				sprintf(pipe->buf_out, "deleted");
			else
				sprintf(pipe->buf_out, "missing");

			pipe->len_out = strlen(pipe->buf_out)+1;
		} else {
			/*�� ���������� �����������*/
			sprintf(pipe->buf_out, "missing");
			pipe->len_out = strlen(pipe->buf_out)+1;
		}
	} else if(!strcmp(cmd, "quit")) {
		/*��������� �������*/
		Reconnect(pipe);
		return;
	} else {
		sprintf(pipe->buf_out, "missing");
		pipe->len_out = strlen(pipe->buf_out)+1;
	}
	
	/*��������� ����������� ��������*/
	WriteDATA(pipe);
}