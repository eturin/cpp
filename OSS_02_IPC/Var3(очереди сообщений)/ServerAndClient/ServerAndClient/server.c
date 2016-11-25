#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>

#define LEN 256

int show_err(const char * msg);
int loop(HANDLE fd);

/*������ ������ ������ � �� ����� ������ � �������*/
int start_server() {
	int res=0;

	/*����������� ��� ������� ���������*/
	printf("������� ��� ������� ��������� \\\\.\\mailslot\\<���?>: ");
	char name[LEN];
	scanf("%s",name);
	char path[LEN];
	sprintf(path, "\\\\.\\mailslot\\%s", name);

	/*������������� ��������� ������� � ������ (��� ����)*/
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	/*������� � ���� OS ������� ��������� (��������� ������������� �������� ������ � ��������� ����������������� �����, �.�. ���� ������ ����� �������� ��������� ������������ ���� ��������)*/
	HANDLE fd = CreateMailslot(path,                  /*��� ������� ���������*/
						       0,                     /*������������ ������ ������ ��������� (0-��� �����������, �� ������-����������� �� ������ 400����)*/
						       MAILSLOT_WAIT_FOREVER, /*������� �������� ��� ������ (���� �� ������ ����������� ������)*/
						       &sa                    /*����� �������*/);

	if(fd == INVALID_HANDLE_VALUE) {
		show_err("�� ������� ������� ����������� �������");
		return 1;
	}

	res=loop(fd);

	CloseHandle(fd);

	return res;
}


int loop(HANDLE fd) {
	int res = 0;
	
	BOOL isRepeat=TRUE;
	while(isRepeat) {
		char buf[LEN];
		size_t len = 0;
		BOOL isOk = ReadFile(fd,buf,LEN,&len,NULL);
		if(!isOk || len == 0)
			show_err("������ ������ �� �������");
		else {
			buf[len] = '\0';
			printf("��������� ���������: %s\n",buf);

			if(!strcmp(buf, "QUIT"))
				isRepeat = FALSE;
		}
	}

	return res;
}