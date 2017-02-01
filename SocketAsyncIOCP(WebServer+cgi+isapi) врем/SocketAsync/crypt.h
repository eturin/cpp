#ifndef CRYPT_H
#define CRYPT_H

#include "common.h"

#pragma comment(lib, "Crypt32.lib") 
#include <Wincrypt.h>
#include <schannel.h>
//#include <wintrust.h>
#define SECURITY_WIN32
#include <security.h>

#pragma comment(lib, "Secur32.lib") 

#define IO_BUFFER_SIZE  0x10000

extern SecurityFunctionTable* tab;

struct Crypt {	
	CtxtHandle hContext;
	BOOL is_InitContext; /**/
	BOOL is_Connected;   /*закрытое соединение установлено*/
	BOOL is_Crypted;     /*сообщение уже зашифровано*/
	BOOL is_Response;    /*передача сформированного ответа сервера клиенту*/
	BOOL is_Clousing;    /*идет закрытие соединения*/
	SecPkgContext_StreamSizes head_Sizes;

	char* IO_Buffer;
	DWORD IO_Buffer_Size, IO_Buffer_cur;
		
};

void * release_Crypt(struct Client*);

struct Client;

HRESULT CreateCredentials(char *, char *, PCredHandle);
int crypt_check(struct Client*);
void crypt_clear(void * ext);



#endif