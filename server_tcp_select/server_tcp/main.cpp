#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>

#include <iostream>                                                                                                              
#include <algorithm>              
#include <set>                                                                                                                   
#include <vector>                                                                                                                   
#include <string>                           

//структура для хранения сведений о клиентах
struct Conn_cl {
	int socket;
	sockaddr_in sa_client;
	bool operator<(const Conn_cl & other)  const { return socket < other.socket;	}
	bool operator==(const Conn_cl & other) const { return socket == other.socket;	}
	Conn_cl(int socket, sockaddr_in & sa_client):socket(socket), sa_client(sa_client) {};
	Conn_cl(int socket):socket(socket) {};
};

int set_nonblock(int fd);

int main() {
	std::set<Conn_cl> set_clients;
	//инициализация
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData))
		return -1;
	//формируем сокет                                                                                                        
	int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port   = htons(1111);
	inet_pton(AF_INET, "0.0.0.0", &(sa.sin_addr));
	
	//биндим                                                                                                                 
	if(bind(master_socket, (sockaddr*)&sa, sizeof(sa)))
		std::cout << "Error on bind.\n";
	else {
		//не блокирующий сокет                                                                                           
		set_nonblock(master_socket);
		if(listen(master_socket, SOMAXCONN)) //слушаем                                                                    
			std::cout << "Error on listen.\n";
		else {
			//повторное использование сокета                                                                         
			int optval = 1;
			setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval));
			//принимаем соединеия                                                                                    
			while(true) {
				fd_set set;//1024 бит                                                                            
				FD_ZERO(&set);
				FD_SET(master_socket, &set);
				for(auto it = set_clients.begin(); it != set_clients.end(); ++it)
					FD_SET(it->socket, &set);

				int max;
				if(set_clients.size() > 0) {
					max = std::max_element(set_clients.begin(), set_clients.end())->socket;
					max = master_socket < max ? max : master_socket;
				} else
					max = master_socket;

				//засыпаем                                                                                       
				select(max + 1, &set, NULL, NULL, NULL);

				//проснулись 
				std::vector<int>         v_to_del;
				std::vector<std::string> v_msg;
				for(auto it = set_clients.begin(); it != set_clients.end(); ++it)
					if(FD_ISSET(it->socket, &set)) {
						static char Buf[1024] = {0};
						int size = recv(it->socket, Buf, 1024, 0);
						if(size == 0 && errno != EAGAIN) {
							//закрываем                                                              
							shutdown(it->socket, SD_BOTH);
							closesocket(it->socket);									
							v_to_del.push_back(it->socket);
							//формируем сообщение
							std::string msg = "Del: ";
							char * ip = inet_ntoa(it->sa_client.sin_addr);
							msg += ip;
							msg += "\n\r";
							v_msg.push_back(msg);
						} else if(size > 0) {
							//формируем сообщение
							std::string msg = "From: ";
							char * ip = inet_ntoa(it->sa_client.sin_addr);
							msg += ip;
							msg += "\n\r";							
							for(int i = 0; i < size; ++i) 
								msg += Buf[i];
							msg += "\n\r";
							v_msg.push_back(msg);
							std::cout << msg;
						}
					}
				//отключение
				for(auto it = v_to_del.begin(); it != v_to_del.end(); ++it) {
					auto i=set_clients.find(*it);
					set_clients.erase(i);					
				}				
				v_to_del.clear();
				//отправка на всех
				for(auto it_msg = v_msg.begin(); it_msg != v_msg.end(); ++it_msg)
					for(auto it = set_clients.begin(); it != set_clients.end(); ++it)					
						send(it->socket, it_msg->c_str(), it_msg->size(), 0);
				v_msg.clear();
				//на мастере новое подключение                                                                   
				if(FD_ISSET(master_socket, &set)) {
					sockaddr_in sa_client;	
					socklen_t size_sa_client = sizeof(sa_client);
					int slave_socket = accept(master_socket, (sockaddr*)&sa_client, &size_sa_client);
					set_nonblock(slave_socket);
					std::string msg = "Add: ";
					char * ip = inet_ntoa(sa_client.sin_addr);
					msg += ip;
					msg += "\n\r";
					for(auto it = set_clients.begin(); it != set_clients.end(); ++it)
						send(it->socket, msg.c_str(), msg.size(), 0);
					set_clients.insert(Conn_cl(slave_socket, sa_client));
				}
			}
			//закрываем мастер сокет                                                                                          
			shutdown(master_socket, SD_BOTH);
			closesocket(master_socket);
		}
	}
	std::system("pause");
	return 0;
}

int set_nonblock(int fd) {
	u_long flags=1;
	return ioctlsocket(fd, FIONBIO, &flags);
}