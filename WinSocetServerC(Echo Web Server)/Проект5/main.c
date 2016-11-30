#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#define _GNU_SOURCE
#include <stdio.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") 
#include <Windows.h>
#include <locale.h>
#include <string.h>

/*���������� �������� ���� �� ������ �� ���� ���*/
#define LEN 100 
/*������������ ������ ��������� ���������*/
#define MAX_HTTP_REQUEST 4096

int show_err(const wchar_t*);
int set_nonblock(int);
int set_repitable(int);
extern size_t make_response(char**,size_t*);

/*������ �������� � �� ��������*/
struct Client {	
	BOOL is_read;
	int sfd;
	int len;
	int cur;
	char * data;
	struct Client *next;	
};

int main() {
	setlocale(LC_ALL, "Russian");
	/*�������������*/
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != NO_ERROR) {
		show_err(L"������ ������������� �����");
		return -1;
	}
	
	/*�������� ���������� ������ ������*/
	int msfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(msfd == -1) {
		show_err(L"�� ������� ���������� ������");		
		return 1;
	}

	/*��������� ����� � ������� ������� (������� ����� ������������)*/
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(12001);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");	
	int res = bind(msfd, (struct sockaddr*)&addr, sizeof(addr));
	if(res == -1) {
		show_err(L"������ ���������� ������ ������ � ������� �������"); 
		closesocket(msfd);
		return 2;
	}

	/*�������� ��������� �������������*/
	res=set_repitable(msfd);
	if(res != 0) {
		show_err(L"�� ������� ���������� ����� ���������� ������������� ������ ������");
		closesocket(msfd);
		return 2;
	}
	/*������ ����� �� �����������*/
	res = set_nonblock(msfd);
	if(res == -1) {
		show_err(L"�� ������� ���������� ����� ���������� ������������� ������ ������");
		closesocket(msfd);
		return 3;
	}

	/*�������� ������� ������� �����*/
	res = listen(msfd, SOMAXCONN);
	if(res == -1) {
		show_err(L"�� ������� ������ ������������� �������� ������");
		closesocket(msfd);
		return 2;
	}

	/*������ �������� � �� ��������*/
	struct Client *root = NULL, *tail = root;
	
	/*�������������������*/	
	fd_set set_r, set_w;
	BOOL is_repeat = TRUE;
	while(is_repeat) {
		/*�������������*/
		FD_ZERO(&set_r);
		FD_ZERO(&set_w);
		
		/*������������ ������ �����*/
		FD_SET(msfd, &set_r);
		
		/*������������ ����� ������*/
		int max = msfd;
		struct Client * cur = root;
		while(cur != NULL) {
			if(cur->is_read)
				FD_SET(cur->sfd, &set_r);
			else
				FD_SET(cur->sfd, &set_w);
			if(max<cur->sfd)
				max = cur->sfd;
			cur = cur->next;
		}

		/*�������� (��� ������� ���������� �� ������� ���� ����, ������� ����� �������������� �������)*/
		select(max + 1, &set_r, &set_w, NULL, NULL);

		/*��������� ����� ������*/
		struct Client * last = NULL;
		cur = root;
		while(cur != NULL) {
			if(FD_ISSET(cur->sfd, &set_r)) {
				//��������� ���������� ���������� ���������
				char data[LEN];
				int len = recv(cur->sfd, data, LEN, 0);
				if(len == 0) {
					//��� ������ ������� �������� ���������� 
					show_err(L"������ ��������� ��������� �� �������");
					shutdown(cur->sfd, SD_BOTH);
					closesocket(cur->sfd);
					free(cur->data);
					if(last == NULL)
						root = cur->next;
					else
						last->next = cur->next;
					if(cur == tail)
						tail = last;
					free(cur);
					cur = last;
				} else if(len>0) {
					/*��������� ��������� ��������� � ��������� �������*/
					strncpy(cur->data + cur->len / sizeof(char), data, len);
					cur->len += len;
					cur->data[cur->len] = '\0';
					
					//!!!!������� ������!!!!!
					if(!strncmp(cur->data, "OFF",5))
						is_repeat = FALSE;
					else if(!strncmp(cur->data + cur->len-4, "\r\n\r\n", 4)) {
						cur->is_read = FALSE;//��� ��������� ��������
						//����������� �����
						make_response(&cur->data, &cur->len);
					}
				}
			} else if(FD_ISSET(cur->sfd, &set_w)) {
				//�������� ���������� ���������
				int len = send(cur->sfd, cur->data + cur->cur, LEN>(cur->len - cur->cur) ? cur->len - cur->cur : LEN, 0);
				if(len == SOCKET_ERROR) {
					//������ ����������
					show_err(L"������ �������� ��������� �������");
					shutdown(cur->sfd, SD_BOTH);
					closesocket(cur->sfd);
					free(cur->data);
					if(last == NULL)
						root = cur->next;
					else
						last->next = cur->next;
					if(cur == tail)
						tail = last;
					free(cur);
					cur = last;
				} else if(len>0) {
					cur->cur += len;
					 
					if(cur->cur == cur->len) {//��� ����������						
						cur->len = 0;
						cur->cur = 0;
						free(cur->data);
						cur->data = (char*)malloc(MAX_HTTP_REQUEST * sizeof(char));						
						cur->is_read = TRUE;
					}
				}
			}
			last = cur;
			if(cur != NULL)
				cur = cur->next;
		}

		/*��������� ������ �����*/
		if(FD_ISSET(msfd, &set_r)) {
			/*��������� ����� ���������� (������ ����� ������� �� �������)*/
			int sfd_slave = accept(msfd, NULL, 0);
			if(sfd_slave != -1) {
				set_nonblock(sfd_slave);
				struct Client * cur = (struct Client*)malloc(sizeof(struct Client));
				cur->sfd = sfd_slave;
				cur->next = NULL;
				cur->is_read = TRUE;
				cur->len = 0;
				cur->cur = 0;
				cur->data = (char*)malloc(MAX_HTTP_REQUEST * sizeof(char));
				cur->data[0] = '\0';
				if(root == NULL)
					root = tail = cur;
				else
					tail = tail->next = cur;
			}else
				show_err(L"�� ������� ������� ����������");
		}
	}

	/*����������� �������*/
	struct Client * cur = root;
	while(cur != NULL) {
		free(cur->data);
		shutdown(cur->sfd, SD_BOTH);
		closesocket(cur->sfd);
		struct Client *tmp = cur;
		cur = cur->next;
		free(tmp);
	}
	root = tail = NULL;

	/*��������� ���������� ������ ������*/
	shutdown(msfd, SD_BOTH);
	closesocket(msfd);
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

int set_nonblock(int fd) {
	int flags;
#if defined(O_NONBLOCK) 
	if(-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else 
	flags = 1;
	return ioctlsocket(fd, FIONBIO, &flags);
#endif 
}

int set_repitable(int sfd) {
	int optval = 1;
	return setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

