#ifndef SOCKET_H
#define SOCKET_H

#include "common.h"

extern int flag;

/*��������� ��������� ������������� � ������ socet */
int set_repitable(int);

/*������� �����*/
void close_socket(int);

#endif