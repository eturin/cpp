#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS
#define WINVER 0x0601 /*API Windows 7*/

#define _GNU_SOURCE
#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*признаки типа*/
#define SERVER       0
#define CLIENT       1
#define WORKER       2
#define WORKER_ISAPI 3


/*состояние сокета*/
#define READ                0
#define WRITE               1
#define WAIT                2
#define READ_WORKER         3
#define WRITE_WORKER        4
#define READ_WORKER_ERR     5
#define READ_TO_SEND_WORKER 6

/*максимальный размер входящего сообщения (64k)*/
#define MAX_HEAD_HTTP 65536
#define MAX_BODY_HTTP 65536

/*максимальный размер буфера обмена асинхронной операции*/
#define LEN 10

#endif