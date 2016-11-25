#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <stdio.h>
#include <string.h>

#define LEN 256

int show_err(const char * msg);

int StartClient() {
	

	printf("������� ��� �������: ");
	char buf_1[LEN];
	scanf("%s", buf_1);
	printf("������� ��� ������������ ������ \\\\%s\\pipe\\<���?>: ", buf_1);
	char buf_2[LEN];
	scanf("%s", buf_2);
	char path[LEN];
	sprintf(path, "\\\\%s\\pipe\\%s", buf_1, buf_2);

	printf("���������� � %s...", path);
	/*���� ��������� ����� ����������*/
	WaitNamedPipe(path, NMPWAIT_WAIT_FOREVER);

	/*��������� ������������ �����*/
	HANDLE fd = CreateFile(path,                              /*��� ������*/
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
		while(getchar() != '\n');
		
		BOOL bRes = TRUE;
		DWORD len_in = 0, len_out = 0, cnt = 1, flags = 0;
		/*��������� ������� ������*/
		bRes = GetNamedPipeInfo(fd,       /*��������� ������*/
								&flags,   /*����� ���� ������*/
								&len_out, /*������� ��������� ������*/
								&len_in,  /*������� �������� ������*/
								&cnt      /*���������� ����������*/);
		if(!bRes)
			show_err("������ ��������� ������� ������");
		else
			printf("�������� ������������ ������:\n\t������� ��������� ������: %d ����\n\t������� �������� ������: %d ����\n\t���������� ����������: %d\n", len_out, len_in, cnt);


		do {
			printf("������� �������: ");
			gets(buf_1);

			/*������ � �����*/
			DWORD len = 0;
			bRes = WriteFile(fd,                /*���������� ������*/
							 buf_1,             /*������������ ������*/
							 strlen(buf_1) + 1, /*������ ������������ ������ (���� �� ���� ����� ������, �� �������� ��������� ���������� ����)*/
							 &len,              /*���������� �������� ����*/
							 NULL);

			if(!bRes || (strlen(buf_1) + 1 != len)) {
				show_err("������ ������ � �����");				
				continue;
			} 
									
			/*������ �� ������*/
			bRes = ReadFile(fd,       /*���������� ������*/
							buf_2,    /*����������� ������*/
							LEN,      /*������ ��� ������ (���� �� ���� ����� ������, �� ��������� ��������� ���������� ����)*/
							&len,     /*���������� ��������� ����*/
							NULL);

			if(!bRes || 0 == len) {
				show_err("������ ������ �� ������");			
				continue;
			} else {
				buf_2[len] = '\0';
				printf("��������: %s\n", buf_2);
			}
		} while(strcmp(buf_1, "quit"));

		/*��������� ���������� ������*/
		CloseHandle(fd);
	}
	return 0;
}