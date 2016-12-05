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

#endif