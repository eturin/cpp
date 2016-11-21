#define _CRT_SECURE_NO_WARNINGS


#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

#define LEN 250

struct Share {
	int a, b;
	char str[LEN - 4 * 2];
};

int show_err(const char*);

int main() {
	setlocale(LC_ALL, "russian");

	printf("������� ��� ������� ����������� ������ � ���� OS: ");	
	char name[LEN];
	scanf("%s",name);
	
	/*��������� ������������ ������ ����� ������ �� ������*/
	HANDLE fd=OpenFileMapping(FILE_MAP_READ,FALSE,name);
	if(fd == NULL)
		show_err("�� ������� ������� ������ ����� ������ � ���� OS");
	else {
		/*���������� ������ ����� ������ �� ���������*/
		struct Share * psh = MapViewOfFile(fd, FILE_MAP_READ, 0, 0, 0 /*���������� ���� �����*/);

		/*������ ����� ������ (!!!��������� ������� �� �����!!!)*/
		while(getchar() != 'q')
			printf("a=%d, b=%d, str=%s\n", psh->a, psh->b, psh->str);

		/*����������� �� ����� ������*/
		UnmapViewOfFile(psh);
	}

	/*��������� ����������*/
	CloseHandle(fd); //���������� ��������� ������ ����� ������, ����� ���������� ��������� (� �� ����, ����� ���� ����� ������������ ������)

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