#define _CRT_SECURE_NO_WARNINGS


#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

#define LEN 256

int show_err(const char*);

int main() {
	setlocale(LC_ALL, "russian");

	/*���������� ��� ������������ ������*/
	printf("������� ��� ������ \\\\.\\pipe\\<���>\n\t���: ");
	char buf[LEN], path[LEN];
	scanf("%s", buf);
	/*������ ��������� ��� (��� ������ ���������� �������� ������)*/
	sprintf(path, "\\\\.\\pipe\\%s", buf);

	/*������� � ���� OS ����������� �����*/
	HANDLE fd = INVALID_HANDLE_VALUE;
	fd=CreateNamedPipe(path,                        /*��� ������*/
		               PIPE_ACCESS_DUPLEX,          /* ����� �������� ������ (� ������ ������ - ����� ��������������� )*/
					   PIPE_TYPE_BYTE | PIPE_WAIT,  /* ����� ������ ������ (� ������ ������ - �������� ���� � �����������)*/
					   PIPE_UNLIMITED_INSTANCES,    /* ������������ ���������� ���������� ������ (�.�. ������� ����� ���� ��������)*/
		               LEN,                         /*������ ��������� ������ � ������ (�������� ����� ������������)*/
		               LEN-10,                      /*������ �������� ������ � ������ (�������� ����� ������������)*/
		               0,                           /*����� �������� � ������������� (��� ������������ ������ ������ NMPWAIT_USE_DEFAULT_WAIT)*/
					   NULL                         /*����� ��������� � ������� ����������*/);
	if(fd == INVALID_HANDLE_VALUE)
		show_err("������ �������� ������������ ������");
	else{ 
		/*������������ � ������������ ������ (������������� � ����������� ������)*/
		if(!ConnectNamedPipe(fd /*���������� ������������ ������*/, NULL/*����� ��������� OVERLAPPED (��� ������������ ������)*/)) 
			show_err("������");
		else {
			BOOL bRes = TRUE;
			while(bRes) {
				DWORD len = 0;
				bRes=ReadFile(fd, buf, sizeof(buf), &len, NULL);      //����������� �� ����� ������
				printf("��������: %s\n",buf);
				bRes=WriteFile(fd, buf, strlen(buf) + 1, &len, NULL); //����������� �� ����� ������				
				FlushFileBuffers(fd);
			}			
			/*����������� �� ������������ ������*/
			DisconnectNamedPipe(fd);
		}
		
		/*��������� ���������� ������*/
		CloseHandle(fd);
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