#ifndef SOCKET_H
#define SOCKET_H

#include "common.h"

extern int flag;

/*разрешить повторное присоединение к сокету socet */
int set_repitable(int);

/*закрыть сокет*/
void close_socket(int);

#endif