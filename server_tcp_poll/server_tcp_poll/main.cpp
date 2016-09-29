#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>

#include <vector>                                                                                                                   
#include <set>   
#include <string>                           

const int MAX_POLLS = 2048;
int set_nonblock(int);
int show_err(const wchar_t *);

int main() {
	//�������������
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		show_err(L"������ ������������� �����");
		return -1;
	}
	
	//��������� �����                                                                                                        
	int m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5555);
	inet_pton(AF_INET, "0.0.0.0", &(saddr.sin_addr));

	//������                                                                                                                 
	if(bind(m_socket, (sockaddr*)&saddr, sizeof(saddr))) 
		show_err(L"Error on bind");		
	else {
		//�� ����������� �����                                                                                           
		set_nonblock(m_socket);
		if(listen(m_socket, SOMAXCONN))  //�������                                                                    
			show_err(L"Error on listen");
		else {
			//��������� ������������� ������                                                                         
			int optval = 1;
			setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
			
			//������ ������
			std::set<int> s_so�kets;

			while(true) {
				pollfd mpfd[MAX_POLLS];
				//��������� ������ �����
				mpfd[0].fd = m_socket;
				mpfd[0].events = POLLIN;
				//��������� ������
				int k = 1;
				for(auto it = s_so�kets.begin(); it != s_so�kets.end(); ++it) {
					mpfd[k].fd       = *it;
					mpfd[k++].events = POLLIN;
				}

				//��������                                                                                       
				WSAPoll(mpfd, s_so�kets.size()+1,-1);

				//���������� 
				if(mpfd[0].revents&POLLIN) {
					//���������� �������
					sockaddr_in saddr_client;
					socklen_t size_saddr_client = sizeof(saddr_client);
					int s_socket = accept(m_socket, (sockaddr*)&saddr_client, &size_saddr_client);
					set_nonblock(s_socket);
					s_so�kets.insert(s_socket);
				}
				std::vector<int>         v_to_del;
				for(int i = 1, l = s_so�kets.size() + 1; i<l; ++i)
					if(mpfd[i].revents & POLLIN) {
						//������ ���������
						char buf[512] = {0};
						int cnt = recv(mpfd[i].fd, buf, 511, 0);
						if(cnt == 0 && errno != EAGAIN) {
							//���������                                                              
							shutdown(mpfd[i].fd, SD_BOTH);
							closesocket(mpfd[i].fd);
							v_to_del.push_back(mpfd[i].fd);							
						} else if(cnt > 0) {
							//������� �� �������
							buf[511] = '\0';
							printf(buf);
						}
					}
				//����������
				for(auto it = v_to_del.begin(); it != v_to_del.end(); ++it) {
					auto i = s_so�kets.find(*it);
					s_so�kets.erase(i);
				}
				v_to_del.clear();			
			}
			//��������� ������ �����                                                                                          
			shutdown(m_socket, SD_BOTH);
			closesocket(m_socket);
		}
	}
	WSACleanup();

	std::system("pause");
	return 0;
}

int set_nonblock(int fd) {
	u_long flags = 1;
	return ioctlsocket(fd, FIONBIO, &flags);
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