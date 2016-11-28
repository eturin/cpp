#define _CRT_SECURE_NO_WARNINGS


#define WINDIR 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#define LEN 256

int show_err(const char * msg);


int main() {	
	/*setlocale(LC_ALL, "russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);*/


	/*����������� �������*/
	HANDLE fd_r_1, fd_w_1, fd_r_2, fd_w_2;
	fd_r_1 = fd_w_1 = fd_r_2 = fd_w_2 = INVALID_HANDLE_VALUE;

	while(TRUE) {
		/*������� ��� ��������� ������*/		
		BOOL isOk = CreatePipe(&fd_r_1,  /*���������� ��� ������*/
							   &fd_w_1,  /*���������� ��� ������*/
							   NULL,     /*������ AD*/
							   LEN         /*������ ������ � ������ (0-�������� OS �����������)*/);
		if(!isOk) {
			show_err("�� ������� ������� ��������� �����");
			break;
		} 

		isOk = CreatePipe(&fd_r_2,  /*���������� ��� ������*/
		                  &fd_w_2,  /*���������� ��� ������*/
		                  NULL,     /*������ AD*/
		                  LEN         /*������ ������ � ������ (0-�������� OS �����������)*/);

		if(!isOk) {
			show_err("�� ������� ������� ��������� �����");
			break;
		}

		/*������ �� ����������� �����, �� �������� ����� ������ (�.�. �� �����, ������� ����� ���������)*/
		int lpMode = PIPE_NOWAIT;
		isOk = SetNamedPipeHandleState(fd_r_2,
									   &lpMode,    /*�� �����������*/
									   NULL,       /*������������ ���������� ���� ����������� ����� ���������*/
									   NULL        /*������������ �������� �� ���� ���� ����� ���������*/);
		if(!isOk) {
			show_err("�� ������� �������� �� ����������� �����");
			break;
		}

		/*�������� ������������ ��� ��� ������ ������, ������� ������ ���� � �������*/
		isOk = SetHandleInformation(fd_r_1,                  /* ���������� ������*/
									HANDLE_FLAG_INHERIT ,    /* ���������� ������ (����� ������������ � �������� �����������)*/
									1                        /* ����� �������� �������*/);
		if(!isOk) {
			show_err("�� ������� ��������� ������������ ����������� ������");
			break;
		}
				
		isOk = SetHandleInformation(fd_w_2,                  /* ���������� ������*/
									HANDLE_FLAG_INHERIT,     /* ���������� ������ (����� ������������ � �������� �����������)*/
									1                        /* ����� �������� �������*/);
		if(!isOk) {
			show_err("�� ������� ��������� ������������ ����������� ������");
			break;
		} 

		
		/*DWORD flags;
		isOk = GetHandleInformation(fd_r_1,&flags);*/

		/*������� �������� �������*/
		STARTUPINFO sti;				        // ���������
		ZeroMemory(&sti, sizeof(STARTUPINFO));	// ��������
		sti.cb = sizeof(STARTUPINFO);			// ������� ������
		/*������������� ������� ����������� stdin, stdout � stderr*/
		sti.dwFlags = STARTF_USESTDHANDLES; //!!!�����������!!!
		sti.hStdInput  = fd_r_1;
		sti.hStdOutput = fd_w_2;
		sti.hStdError  = fd_w_2;

		PROCESS_INFORMATION procInf;
		isOk = CreateProcess("c:\\Windows\\system32\\cmd.exe",   /*���� � ���������*/
							 "",      /*��������� ���������� ������*/
							 NULL,    /*������ AD ��� ������ ��������*/
							 NULL,    /*������ AD ��� ������ ������*/
							 TRUE,    /*���� ������������ �������� �������� (��������� ����������� �����������)*/
							 CREATE_NEW_CONSOLE /*0*/, /*����� �������� �������� ��������*/
							 NULL,    /*��������� �� ��������� ���������� ���������, ��� �������� (NULL - ������ ��� � ��������)*/
							 NULL,    /*������� ���� ��� ������� (NULL - ������ ��� � ��������)*/
							 &sti,    /*������������ ��� ��������� ������� ��������, �������� ������������ ���� � ���������*/
							 &procInf /*�������� � �������� (���� ����������)*/);
		if(!isOk) {
			show_err("�� ������� ������� �������� �������");
			break;
		} 
			
		BOOL isRepeat = TRUE;
		while(isRepeat) {
			char buf[LEN];
			DWORD len = 0;
			
			while(TRUE) 
				if(ReadFile(fd_r_2, buf, LEN-2, &len, NULL)){
					buf[len] = '\0';
					printf(buf);
					/*�������� ����� �����*/
					if(buf[len - 1] == '>')
						break;					
				} else if(ERROR_BROKEN_PIPE == GetLastError()) {
					/*������ ������*/
					show_err("-->");
					isRepeat = FALSE;
					break;
				} else if(GetExitCodeProcess(procInf.hProcess, &len)){
					/*���������, ��� �������� ������� ���*/
					if(len == STILL_ACTIVE)
						break;
					else {
						printf("�������� ������� ���������� � ����� %d\n",len);
						isRepeat = FALSE;
						break;
					}
				} else
					show_err("�� ������� ��������� �������� �������");
											
			if(isRepeat) {
				//printf("\n������� �������: ");
				gets(buf);
				size_t len_ = strlen(buf);
				if(len_) {
					buf[len_++] = '\n';
					//buf[len + 2] = '\0';
					isOk = WriteFile(fd_w_1, buf, len_, &len, NULL);
					if(!isOk)
						show_err("������ ������ � �����");
					else if(ERROR_BROKEN_PIPE == GetLastError()) {
						show_err("������ ������ � �����");
						break;
					} else if(len_ != len)
						show_err("������ ������ � ����� ��� �������");
				}
			}
			
		}

		/*������� ������� � ��������� ��� ����������*/
		TerminateProcess(procInf.hProcess, NO_ERROR);		
		CloseHandle(procInf.hProcess);

		/*���� ����, ���� �� ���������� � ����������� ������� ����*/
		break; 		
	}

	/*��������� �����������*/
	CloseHandle(fd_w_1);
	CloseHandle(fd_r_2);
	CloseHandle(fd_r_1);
	CloseHandle(fd_w_2);

	system("pause");
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
