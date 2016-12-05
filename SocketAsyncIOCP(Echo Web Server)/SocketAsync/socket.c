#include "socket.h"

int flag = 0;

int set_repitable(int sfd) {
	int optval = 1;
	return setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
}

void close_socket(int fd) {
	shutdown(fd, SD_BOTH);
	closesocket(fd);
}