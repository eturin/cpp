#include "error.h" 

HANDLE fd_err=INVALID_HANDLE_VALUE;

HANDLE open_log() {
	if(fd_err == INVALID_HANDLE_VALUE) {
		char path[] = "\\\\.\\mailslot\\log";
		/*��������� ������������ �������*/
		fd_err = CreateFile(path,           /*��� �������*/
							GENERIC_WRITE,  /*����� �������*/
							FILE_SHARE_READ,/*����� ����������� ������������� (������� ���� ����� ������ ������� ����)*/
							NULL,           /*������ �� AD*/
							OPEN_EXISTING,  /*��������� ������������ ��� ������*/
							0,              /*�������� �����*/
							NULL            /*������ ��������*/);
	}

	return fd_err;
}

int show_err_wsa(const char * msg) {
	int      no = WSAGetLastError();
	char     str_err[10000] = {0};
	char     log[100000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		sprintf(log,"%s: %d\n%s\n", msg, no, str_err); 
	else
		sprintf(log,"%s:\n����� ������ %d\n", msg, no);

	printf(log);
	DWORD len = 0;
	BOOL isOk = WriteFile(open_log(), log, strlen(log) + 1, &len, NULL); //���� ������ ��������, �� ��������� ����� ������ � ������ ��� ������

	return no;
}

int show_err(const char * msg, BOOL check_err) {
	int      no = 0;
	char     log[100000] = {0};
	if(check_err) {
		no = GetLastError();
		char     str_err[10000] = {0};
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
			sprintf(log, "%s: %d\n%s\n", msg, no, str_err);
		else
			sprintf(log, "%s:\n����� ������ %d\n", msg, no);
	}else
		sprintf(log, "%s\n", msg);

	printf(log);
	DWORD len = 0;
	BOOL isOk = WriteFile(open_log(), log, strlen(log) + 1, &len, NULL); //���� ������ ��������, �� ��������� ����� ������ � ������ ��� ������

	return no;
}
