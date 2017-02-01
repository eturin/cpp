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
		while(i < argc) {
			if(!strcmp(argv[i], "-d")) 				
				strcpy(psrv->work_path, argv[++i]);				
			else if(!strcmp(argv[i], "-p") && !sscanf(argv[++i], "%d", &psrv->port))
				printf("�� ������� �������� �������� ���� �� ���������� ���������� ������\n");
			else if(!strcmp(argv[i], "-sp") && !sscanf(argv[++i], "%d", &psrv->sport))
				printf("�� ������� �������� ���� ������������ ��������� �� ���������� ���������� ������\n");
			else if(!strcmp(argv[i], "-ip") && !sscanf(argv[++i], "%d", &psrv->iport))
				printf("�� ������� �������� �������� ���� isapi �� ���������� ���������� ������\n");
			else if(!strcmp(argv[i], "-sf"))
				strcpy(psrv->cert_path, argv[++i]);
			else if(!strcmp(argv[i], "-sn"))
				strcpy(psrv->cert_name, argv[++i]);
			else if(!strcmp(argv[i], "-ss"))
				strcpy(psrv->cert_path, argv[++i]);
			else if(!strcmp(argv[i], "-h")
					  || !strcmp(argv[i], "/?")
					  || !strcmp(argv[i], "/h")
					  || !strcmp(argv[i], "/help")) {
				printf("����������� ��������� �� ���������� �����������:\n"
					   "  -h\n"
					   "\t��� ��������� ���� �������\n"
					   "  -p <����� �����>\n"
					   "\t�������������� �������� �������� ����, ��� �� ������������� ��������� (����������� 12001)\n"
					   "  -sp <����� �����>\n"
					   "\t�������������� �������� �������� ����, ��� ������������� ��������� (����������� 12443)\n"
					   "  -sf \"<���� � ����� ������� �����������>\"\n"
					   "\t��� ��������� �������������� ���������� ��������� ����������:\n"
					   "\t\t1) ������ ���������� � ������ ���������(����� ���� ������)\n"
					   "\t\t\tmakecert -pe -ss my\n"
					   "\t\t2) ������������ ���������� ������ � �������� ������ � ���� ��� ������� ���������\n"
					   "\t\t3) ������� ���������� �� ���������(�� �����������)\n"
					   "  -sn \"<�� ���� ������� ����������>\"\n"
					   "  -ss \"<��� ���������>\"\n"
					   "\t���������� ����� ���� ���������� � ������ ���������\n"
					   "\t\tmakecert -n \"CN=<�� ���� ������� ����������>\" -pe -ss <��� ���������>\n"
					   "  -ip <����� �����>\n"
					   "\t���� �������������� �������� isapi (����������� �� ������� ������ ��������� �����)\n"
					   "  -d \"<���� � �������� � �������������>\"\n"
					   "\t��� �������� �������� � �����������-�������������\n");
				//system("pause");
				return 0;
			} 
			
			++i;
		}
	}
	/*�������� ���������� �� ��������� ������*/
	if(strlen(psrv->work_path) == 0)
		/*������� ������ ���������*/		
		memcpy(psrv->work_path, *argv, strrchr(*argv, '\\') - *argv + 1);
	else if(psrv->work_path[strlen(psrv->work_path) - 1] != '\\')
		psrv->work_path[strlen(psrv->work_path) - 1] = '\\';

	if(!SetCurrentDirectory(psrv->work_path)) {
		show_err("(master)�� ������� ����� � ������� � �������������", TRUE);
		return 1;
	}

	if(psrv->port == 0)
		psrv->port = 12001;
	if(psrv->sport == 0)
		psrv->sport = 12443;
	if(psrv->iport == 0)
		psrv->iport = psrv->port+1;
	if(psrv->port == psrv->sport
	   || psrv->port == psrv->iport
	   || psrv->sport == psrv->iport
	   || psrv->port<=0
	   || psrv->sport<=0
	   || psrv->iport<=0) {
		printf("��������� �������������� ����� ������ �� �����:\n\t�������� ����: %d\n\t�������� ����: %d\n\t���� isapi: %d\n", psrv->port, psrv->sport, psrv->iport);
		return 1;
	}
	
	/*������������� �����*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) 
		show_err_wsa("(master)������ ������������� �����");		
	else {
		/*������ �������*/
		start_server(psrv);

		/*������������ �����*/
		WSACleanup();
	}
	
	/*����������� ������� �������*/
	free(psrv);

	//system("pause");
	return 0;
}