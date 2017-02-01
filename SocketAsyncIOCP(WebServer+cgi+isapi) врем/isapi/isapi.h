#ifndef ISAPI_H
#define ISAPI_H

#include "common_isapi.h"
#include <HttpExt.h>

struct Isapi {

	char name[256];
	char path[256];

	/*хендл динамической библиотеки*/
	HINSTANCE hModule;

	/*сведения предоставленные библиотекой динамической загрузки*/
	HSE_VERSION_INFO Version;

	/*адреса символов библиотеки*/
	FARPROC fGetExtensionVersion;
	FARPROC fHttpExtensionProc;
	FARPROC fTerminateExtension;

};


struct Client_isapi;

struct Isapi * init_isapi(const char *path, const char *name);
struct Isapi * release_isapi(struct Isapi * pIsapi);
struct Isapi * get_isapi(struct Client_isapi*);
DWORD call_HttpExtensionProc(struct Client_isapi *);

#endif