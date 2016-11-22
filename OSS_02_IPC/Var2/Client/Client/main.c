#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#define LEN 256
int show_err(const char*);

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	/*���������� ��� �������*/
	printf("��� �������: ");
	char server[LEN];
	scanf("%s",server);

	/*���������� ��� ������*/
	printf("������� ��� ������������ ������: \\\\%s\\pipe\\<���>\n\t���: ",server);
	char name[LEN];
	scanf("%s", name);
	while(getchar() != '\n');

	/*��������� �������� ���*/
	char path[LEN];
	sprintf(path, "\\\\%s\\pipe\\%s", server, name);

	printf("���������� � %s...",path);
	/*���� ��������� ����� ����������*/
	WaitNamedPipe(path, NMPWAIT_WAIT_FOREVER);

	/*��������� ������������ �����*/
	HANDLE fd=CreateFile(path,                              /*��� ������*/
						 GENERIC_READ | GENERIC_WRITE,      /*����� �������*/
						 FILE_SHARE_READ | FILE_SHARE_WRITE,/*����� ����������� �������������*/ 
		                 NULL,                              /*������ ��������*/ 
		                 // ������ 
		                 OPEN_EXISTING,                     /*��������� ������������ ��� ������*/
		                 0,                                 /*�������� �����*/
						 NULL                               /*������ ��������*/);  
	if(fd == INVALID_HANDLE_VALUE) {
		printf(" �������!\n");
		show_err("�� ������� ������� ������������ �����");
	} else {
		printf(" ok!\n");
		BOOL bRes = TRUE;
		DWORD len_in = 0, len_out = 0, cnt=1, flags=0;
		/*��������� ������� ������*/
		bRes=GetNamedPipeInfo(fd,       /*��������� ������*/
			&flags,   /*����� ���� ������*/
			&len_out, /*������� ��������� ������*/
			&len_in,  /*������� �������� ������*/
			&cnt      /*���������� ����������*/);
		if(!bRes)
			show_err("������ ��������� ������� ������");
		else
			printf("�������� ������������ ������:\n\t������� ��������� ������: %d ����\n\t������� �������� ������: %d ����\n\t���������� ����������: %d\n",len_out,len_in,cnt);


		while(bRes){
			char *buf=malloc(len_out*sizeof(char));
			printf("������� ���������: ");
			gets(buf);

			/*������ � �����*/
			DWORD len = 0;
			bRes = WriteFile(fd,              /*���������� ������*/
							 buf,             /*������������ ������*/
							 strlen(buf) + 1, /*������ ������������ ������ (���� �� ���� ����� ������, �� �������� ��������� ���������� ����)*/
							 &len,            /*���������� �������� ����*/
							 NULL);

			if(!bRes || (strlen(buf) + 1 != len)) {
				show_err("������ ������ � �����");
				free(buf);
				break;
			} else 
				printf("����������.\n");
			
			free(buf);
			buf = malloc(len_in*sizeof(char));
			/*������ �� ������*/
			bRes = ReadFile(fd,         /*���������� ������*/
				            buf,        /*����������� ������*/
							len_in,     /*������ ��� ������ (���� �� ���� ����� ������, �� ��������� ��������� ���������� ����)*/
				            &len,       /*���������� ��������� ����*/
				            NULL);  

			if(!bRes || 0 == len) {
				show_err("������ ������ �� ������");
				free(buf);
				break;
			} else {
				buf[len] = '\0';
				printf("��������: %s\n", buf);
			}
			
			free(buf);			
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