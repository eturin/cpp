#define _CRT_SECURE_NO_WARNINGS


#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

#define LEN 256
#define CNT 10

/*��������� ���������� ������������ ������*/
#define CONNECTING_STATE 0 //��������� ����������
#define READING_STATE 1    //����� � ������
#define WRITING_STATE 2    //����� � ������

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

	/*���������� ��� ������������ ������*/
	printf("������� ��� ������ \\\\.\\pipe\\<���>\n\t���: ");
	char buf[LEN], path[LEN];
	scanf("%s", buf);
	/*������ ��������� ��� (��� ������ ���������� �������� ������)*/
	sprintf(path, "\\\\.\\pipe\\%s", buf);

	/*������� � ���� OS ��������� ���������� ������������ ������*/
	PIPEINST pipes[CNT];
	/*!!!!������������ �������������!!!!*/
	memset(pipes, 0, CNT*sizeof(PIPEINST));

	HANDLE hEvents[CNT];
	for(int i = 0; i < CNT; ++i) {
		/*��������� ��������� �������*/
		hEvents[i] = CreateEvent(NULL,   /*������� ������*/
								 TRUE,	 /*��� ������ TRUE - ������*/
								 TRUE,	 /*��������� ��������� TRUE - ����������*/
								 NULL	 /*��� �������*/);
		if(hEvents[i] == NULL) {
			show_err("�� ������� ������� ��������� �������");
			return 1;
		}

		pipes[i].Overlap.hEvent = hEvents[i];


		pipes[i].fd_pipe = CreateNamedPipe(path,                        /*��� ������*/
										   PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,/*����� �������� ������ (� ������ ������ - ����� ��������������� � ����������� )*/
										   PIPE_TYPE_BYTE | PIPE_WAIT,  /*����� ������ ������ (� ������ ������ - �������� ���� � �����������)*/
										   CNT,                         /*������������ ���������� ���������� ������ (�.�. ������� ����� ���� ��������)*/
										   LEN,                         /*������ ��������� ������ � ������ (�������� ����� ������������)*/
										   LEN,                         /*������ �������� ������ � ������ (�������� ����� ������������)*/
										   5000,                        /*����� �������� � ������������� (��� ������������ ������ ������ NMPWAIT_USE_DEFAULT_WAIT)*/
										   NULL                         /*����� ��������� � ������� ����������*/);
		if(pipes[i].fd_pipe == INVALID_HANDLE_VALUE) {
			show_err("������ �������� ������������ ������");
			return 1;
		} else {
			/*������������ � ������������ ������ (������������� � ����������� ������)*/
			pipes[i].wait_connect = Connect(pipes[i].fd_pipe, &pipes[i].Overlap);
			/*��������� ������*/
			pipes[i].state = pipes[i].wait_connect ? CONNECTING_STATE : READING_STATE;
		}
	}
			
	while(TRUE) {
		DWORD i = WaitForMultipleObjects(CNT,    /*���������� ������� � �������*/
			                            hEvents,/*������ �������*/
			                            FALSE,  /*�� ��������� ���� ������� ������������*/
			                            INFINITE/*����� ����������*/);
		/*�������� ������ ������������ �������*/
		i -= WAIT_OBJECT_0;

		DWORD len;
		BOOL isOk;
		/*���� ������� ���������� � ���� ����������� ������*/
		if(pipes[i].wait_connect) {
			/*��������� ��������� ��������� ����������� �������� �� ������� �������*/
			isOk = GetOverlappedResult(pipes[i].fd_pipe, /*���������� ������*/
									   &pipes[i].Overlap,/*OVERLAPPED*/
									   &len,             /*�������� ����*/
									   FALSE             /*�� �����*/);
			/*���������, ����� ����������� �������� ���������*/
			switch(pipes[i].state) {
				case CONNECTING_STATE:
					/*����� ��������� � �������� ������ ����������*/
					if(!isOk) {
						show_err("�� ������� ������� ��������� ����������� ��������");
						return 0;
					}					
					pipes[i].state = READING_STATE; //��������� ����������� �������� ����� �������
					break;

				case READING_STATE:
					/*����� ������ �������� ������*/
					if(!isOk || len == 0) {
						DisconnectAndReconnect(&pipes[i]);//�� ������� ���������
						continue;
					}
					printf("��������: %s\n", pipes[i].buf_in);
					pipes[i].len_in = len;					
					pipes[i].state = WRITING_STATE; //��������� ����������� �������� ����� �������
					break;

				case WRITING_STATE:
					/*����� ������ �������� ������*/
					if(!isOk || len != pipes[i].len_out) {
						DisconnectAndReconnect(&pipes[i]); //�� ������� ��������
						continue;
					}					
					pipes[i].state = READING_STATE; //��������� ����������� �������� ����� �������
					break;
			}
		}

		/*���������� ��������� ����������� ��������*/
		switch(pipes[i].state) {
			case READING_STATE:
				/*����� ���������������� �������� ������������ ������*/
				isOk = ReadFile(pipes[i].fd_pipe, /*���������� ���������� ������������ ������*/
								pipes[i].buf_in,  /*����� ��� ������ ������ �� �������*/
								LEN*sizeof(char), /*������ ������*/
								&pipes[i].len_in, /*���������� �������*/
								&pipes[i].Overlap /*��������� ������������ ���������� ��������*/);

				/*�� ������ ����� �� ��������� ������ �� �������*/ 
				if(isOk && pipes[i].len_in != 0) {
					pipes[i].wait_connect = FALSE; //�� ���� ���������� 
					pipes[i].state = WRITING_STATE;//����� ����� ��������
					printf("��������: %s\n", pipes[i].buf_in);
					continue;
				}

				/*����������� �������� ��� �� ���������*/
				if(!isOk && GetLastError() == ERROR_IO_PENDING) {
					pipes[i].wait_connect = TRUE;  //����� ����� ����������
					continue;
				}

				/*������ ������ ��� ������ ����������*/
				show_err("������");
				DisconnectAndReconnect(&pipes[i]);
				break;				

			case WRITING_STATE:
				/*����� ���������������� �������� ����������� ������*/
				pipes[i].len_out = pipes[i].len_in;
				memcpy(pipes[i].buf_out, pipes[i].buf_in, pipes[i].len_in); //����� ���������� ����� ��, ��� ��� ��������
				

				isOk = WriteFile(pipes[i].fd_pipe,             /*���������� ������*/
								 pipes[i].buf_out,             /*������������ ������*/
								 pipes[i].len_out*sizeof(char),/*������ ������������ ������*/
								 &len,                         /*���������� ���������� ����*/
						         &pipes[i].Overlap             /*��������� ����������� ��������*/);

				/*� ����� ����� �� ���������� ������*/
				if(isOk && len == pipes[i].len_out*sizeof(char)) {
					pipes[i].wait_connect = FALSE; //�� ���� ����������
					pipes[i].state = READING_STATE;//����� ����� ���������
					continue;
				}

				/*� ������ �� �������� ������ ��-�� ��� �� ��������������� ����������*/
				if(!isOk && (GetLastError() == ERROR_IO_PENDING)) {
					pipes[i].wait_connect = TRUE; //���� ����������
					continue;
				}

				/*������ ������ ��� ������ ����������*/
				show_err("������");
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
		printf("%s:\n����� ������ %d\n", msg, no);

	return no;
}

BOOL Connect(HANDLE fd, OVERLAPPED *po) {
	BOOL isOk = FALSE;

	/*������������ � ������, �� �� ���� (������ ������� 0)*/
	if(ConnectNamedPipe(fd /*���������� ������������ ������*/,
		                po /*����� ��������� OVERLAPPED (��� ������������ ������)*/)) {
		show_err("������ ����������� � ������������ ������");
		return FALSE;
	}

	/*����������� ���������*/
	switch(GetLastError()) {		
		case ERROR_IO_PENDING:
			                  /*������� ����������*/
			                  isOk = TRUE;
			                  break;			
		case ERROR_PIPE_CONNECTED:
			                  /*���� �����������*/
			                  if(SetEvent(po->hEvent)) break;
		default:
							  /*������ ������*/
							  show_err("������ ������ ����������� � ������������ ������");
	}

	return isOk;
}

VOID DisconnectAndReconnect(PIPEINST* pipe) {
	
	/*����������� �� ������������ ������*/
	if(!DisconnectNamedPipe(pipe->fd_pipe)) 
		show_err("�� ������� ����������� �� ������������ ������");
	
	/*����� ������������ � ������������ ������*/
	pipe->wait_connect = Connect(pipe->fd_pipe,&pipe->Overlap);
	pipe->state = pipe->wait_connect ?	CONNECTING_STATE : READING_STATE;
}