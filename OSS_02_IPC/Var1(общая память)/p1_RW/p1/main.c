#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#define LEN 250

struct Share {
	int a, b;
	char str[LEN-4*2];
};

int show_err(const char*);

int main(int argc, char ** argv) {
	setlocale(LC_ALL, "russian");

	/*����������� ��� �����, ������� ����� ���������� � ������*/
	printf("������� ��� ������������ �����: ");
	char name[LEN];
	gets(name);
		
	printf("������� ������� ������ ����� ������ � ���� OS: %s...", name);
	/*��������� ������ ����� ������ � ���� OS*/
	HANDLE fd = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, /*����� �������*/
								FALSE,                          /*������������*/
								name                            /*��� ������������� �������*/);
	
	HANDLE fd_file = INVALID_HANDLE_VALUE;
	if(fd == NULL) {		
		/*������� ��� ��������� ���� �� ����� � ������� ��������*/
		fd_file = CreateFile(name,                              /*��� �����*/
							 GENERIC_READ | GENERIC_WRITE,      /*����� �������*/
							 FILE_SHARE_READ | FILE_SHARE_WRITE,/*����� ����������� �������*/							 
							 NULL,                              /*NULL - ��� ������ ������������ ��������� ����������*/
							 OPEN_ALWAYS,                       /*������� ������������ ��� ������� �����*/
							 FILE_ATTRIBUTE_TEMPORARY,          /*�������� ����� (� ������ ������ OS ����� �������� ������ �� ����, ���� ���������� ���-������)*/
							 NULL                               /*��� ������� � ��������������� ����������*/);
		if(INVALID_HANDLE_VALUE != fd_file) {
			/*������ ������ �����*/
			SetFilePointer(fd_file, sizeof(struct Share), NULL, FILE_BEGIN); //�������� ��������� ������� ������� �� LEN:0 �� ������ �����
			if(!SetEndOfFile(fd_file)) {
				printf(" �������\n");
				show_err("�� ������� �������� ������ ����� �� �����");
			}else{			
				/*������� ������ ����� ������ � ���� OS*/
				fd = CreateFileMapping(fd_file,                         /*���������� �����, �� ������ �������� ��������� ����������� ������*/
									   NULL,                            /*NULL - ��� ������ ������������ ��������� ����������*/
									   PAGE_READWRITE,                  /*�������� ������*/
									   0,                               /*������� ����� �������*/
									   0,                               /*� ������� ����� ������� (������ ����� ���� �������, � ���� �������� ������ ��������� �����)*/
									   name					            /*��� ������������� �������*/);
				if(fd == NULL) {
					printf(" �������\n");
					show_err("�� ������� ������� ������ ����� ������ � ���� OS");
					fd = INVALID_HANDLE_VALUE;
				} else
					printf("  ������\n");
			}
		} else
			show_err("�� ������� ������� ���� �� �����");
	} else
		printf("  ������\n");

	if(fd != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize = GetFileSize(fd, NULL);
		/*���������� ����� ������ �� ���������*/
		struct Share * psh=NULL;
		if(NULL == (psh=MapViewOfFile(fd,                          /* ���������� ������� ����� ������ � ���� OS*/
			                       FILE_MAP_WRITE,                 /* ����� �������*/
			                       0,                              /* ������� DWORD �������� �� ������*/
			                       0,                              /* ������� DWORD �������� �� ������*/
			                       sizeof(struct Share)            /* ����� ������������ ������ */)))
			show_err("�� ������� ���������� ������ �� ���������");
		else {
			printf("������ � ����� ������.\n");
			/*���������� ����� ������*/
			do{
				printf("������� ����� ����� a � b: ");
				if(2 != scanf("%d %d", &psh->a, &psh->b))
					break;
				while(getchar() != '\n'); //�������� ������ �� �����
				printf("������� ������: ");
				gets(psh->str);
			} while(TRUE);

			/*���������� �� ����� ������*/
			UnmapViewOfFile(psh);
		}
	}


	/*��������� �����������*/
	CloseHandle(fd_file);
	CloseHandle(fd);//���������� ��������� ������ ����� ������, ����� ���������� ��������� (� �� ����, ����� ���� ����� ������������ ������)

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