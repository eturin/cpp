#include "server.h"
#include "error.h"
#include <locale.h>

int main(int argc, char **argv) {
	/*�����������*/
	setlocale(LC_ALL, "russian");
	/*�������������*/
	struct Server *psrv=init_Server();
	/*���������� �� ���������� ��������� ������*/
	if(argc>1) {
		int i = 1;
		while(i<argc)
			if(!strcmp(argv[i], "-d")) {
				++i;				
				strcpy(psrv->work_path, argv[i]);
				++i;
			}else if(!strcmp(argv[i], "-p")) {
				++i;				
				strcpy(psrv->work_path, argv[i]);
				++i;
			}else if(!strcmp(argv[i], "-h")
					  || !strcmp(argv[i], "/?")
					  || !strcmp(argv[i], "/h")
					  || !strcmp(argv[i], "/help")) {
				printf("����������� ��������� �� ���������� �����������:\n"
					   "\t-h\n\t\t��� ��������� ���� �������\n"
					   "\t-p\n\t\t�������������� ���� ������-������\n"
					   "\t-d \"<���� � �������� � �������������>\"\n\t\t��� �������� �������� � ����������� �������������\n");
				system("pause");
				return 0;
			} else
				++i;
	}
	/*�������� ���������� �� ��������� ������*/
	if(strlen(psrv->work_path) == 0)
		/*������� ������ ���������*/		
		memcpy(psrv->work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
	else if(psrv->work_path[strlen(psrv->work_path) - 1] != '\\')
		psrv->work_path[strlen(psrv->work_path) - 1] = '\\';

	if(!SetCurrentDirectory(psrv->work_path)) {
		show_err("�� ������� ����� � ������� � �������������", TRUE);
		return 1;
	}

	if(psrv->port == 0)
		psrv->port = 12001;

	/*������������� �����*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) 
		show_err_wsa("������ ������������� �����");		
	else {
		/*������ �������*/
		start_server(psrv);

		/*������������ �����*/
		WSACleanup();
	}
	
	/*����������� ������� �������*/
	free(psrv);

	system("pause");
	return 0;
}