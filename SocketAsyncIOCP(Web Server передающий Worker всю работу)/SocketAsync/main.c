#include "server.h"
#include "error.h"
#include <locale.h>

int main(int argc, char **argv) {
	/*�����������*/
	setlocale(LC_ALL, "russian");

	/*���������� �� ���������� ��������� ������*/
	if(argc>1) {
		int i = 1;
		while(i<argc)
			if(!strcmp(argv[i], "-d")) {
				++i;
				//mbstowcs(work_path, argv[i], strlen(argv[i]) + 1);
				strcpy(work_path, argv[i]);
				++i;
			} else if(!strcmp(argv[i], "-h")
					  || !strcmp(argv[i], "/?")
					  || !strcmp(argv[i], "/h")
					  || !strcmp(argv[i], "/help")) {
				printf("����������� ��������� �� ���������� �����������:\n"
					   "\t-h\n\t\t��� ��������� ���� �������\n"
					   "\t-d \"<���� � �������� � �������������>\"\n\t\t��� �������� �������� � ����������� �������������\n");
				system("pause");
				return 0;
			} else
				++i;
	}
	/*�������� ���������� �� ��������� ������*/
	if(strlen(work_path) == 0)
		/*������� ������ ���������*/
		//mbstowcs(work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
		memcpy(work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
	else if(work_path[strlen(work_path) - 1] != '\\')
		//work_path[wcslen(work_path) - 1] = L'\\';
		work_path[strlen(work_path) - 1] = '\\';

	if(!SetCurrentDirectory(work_path)) {
		show_err("�� ������� ����� � ������� � �������������", TRUE);
		return 1;
	}

	/*������ �������*/
	start_server();

	system("pause");
	return 0;
}