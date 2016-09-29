#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#define _GNU_SOURCE
#include <stdio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib") 
#include <Windows.h>
#include <locale.h>
#include <string.h>


#define LEN 100
int show_err(const wchar_t *);

int main() {
	setlocale(LC_ALL, "Russian");
	/*�������������*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 0), &ws) != NO_ERROR) {
		show_err(L"������ ������������� �����");
		return -1;
	}
	
	/*�������� ���������� ������*/
	int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sfd == -1) {
		show_err(L"�� ������� ���������� ������");		
		return 1;
	}

	/*��������� ����� � ������� ������� (� ������� �����������)*/
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(80);
	addr.sin_addr.s_addr = inet_addr("87.245.163.3");
	
	int res = connect(sfd, (struct sockaddr*)&addr, sizeof(addr));
	if(res == -1) {
		show_err(L"������ ����������"); 
		closesocket(sfd);
		return 2;
	}

	char data[100 * LEN] = "GET / HTTP/1.1\r\nHost: 87.245.163.3\r\nAccept-Encoding: gzip, deflate, sdch\r\n\r\n";
	size_t len = strlen(data);
	

	/*��������*/
	int size = send(sfd, data, len + 1, 0);
	if(size == -1) {
		show_err(L"������ ��������");
	} else 
		printf("���������� %d ����:\n%s\n", size, data);
	

	/*���������*/
	memset(data, '\0', 10 * LEN);
	while(1) {
		size = recv(sfd, data, 10 * LEN-1, 0);
		if(size==0)
			break;
		else if(size < 0 && show_err(L"������ ���������")!=0) 			
			break;
		else  {
			data[size] = '\0';			
			printf(data);
		}
	}

	/*��������� ����������*/
	closesocket(sfd);
	WSACleanup();
	
	system("pause");
	return 0;
}

int show_err(const wchar_t * msg) {
	int      no = WSAGetLastError();
	wchar_t  str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		wprintf(L"%s:\n%s\n", msg, str_err);
	else
		wprintf(L"%s:\n����� ������ %d\n", msg, no);	
	
	return no;
}