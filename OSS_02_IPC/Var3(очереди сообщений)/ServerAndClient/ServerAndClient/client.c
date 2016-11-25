#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>

#define LEN 256

int show_err(const char * msg);

/*������ ������ ����� � �������, �� �� ����� ������ �� �� (���� ���������� ����� ������������ ���� ��������)
  ����������������� ���� ����� \\*\mailslot\[����]���������
*/
int start_client() {
	int res = 0;

	/*����������� ��� ������� ���������*/
	printf("������� ��� �������: ");
	char server[LEN];
	scanf("%s", server);
	printf("������� ��� ������� ��������� \\\\%s\\mailslot\\<���?>: ",server);
	char name[LEN];
	scanf("%s", name);
	char path[LEN];
	sprintf(path, "\\\\%s\\mailslot\\%s", server, name);

	/*��������� ������������ �������*/
	HANDLE fd = CreateFile(path,           /*��� �������*/
						   GENERIC_WRITE,  /*����� �������*/
		                   FILE_SHARE_READ,/*����� ����������� ������������� (������� ���� ����� ������ ������� ����)*/
						   NULL,           /*������ �� AD*/
						   OPEN_EXISTING,  /*��������� ������������ ��� ������*/
						   0,              /*�������� �����*/
						   NULL            /*������ ��������*/);
	while(getchar()!='\n');

	BOOL isRepeat = TRUE;
	while(isRepeat) {
		printf("������� ��������� ��� ������� (��� ������ - quit): ");
		char buf[LEN];
		gets(buf);

		if(!strcmp(buf, "quit"))
			isRepeat = FALSE;
		else {
			size_t len = 0;
			BOOL isOk = WriteFile(fd, buf, strlen(buf) + 1, &len, NULL); //���� ������ ��������, �� ��������� ����� ������ � ������ ��� ������
			if(!isOk)
				show_err("������ ������ ��������� � �������");
			else if(strlen(buf)!=len-1)
				show_err("�� ������� ��������� ��������� �������");
		}
	}


	/*��������� ���������� �������*/
	CloseHandle(fd);

	return res;
}