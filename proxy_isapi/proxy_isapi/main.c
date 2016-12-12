#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/

#include <Windows.h>
#include <HttpExt.h>
#include <locale.h>
#include <stdio.h>

/*для отладки нужно цеплять w3wp.exe*/
 
HINSTANCE hModule = NULL;
FARPROC fGetExtensionVersion= NULL;
FARPROC fHttpExtensionProc  = NULL;
FARPROC fTerminateExtension = NULL;

FARPROC fWriteClient           = NULL;
FARPROC fReadClient            = NULL;
FARPROC fGetServerVariable     = NULL;
FARPROC fServerSupportFunction = NULL;

int show_err(const char * msg, BOOL check_err) {
	int      no = 0;
	if(check_err) {
		no = GetLastError();
		char     str_err[10000] = {0};
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
			printf("%s: %d\n%s\n", msg, no, str_err);
		else
			printf("%s:\nномер ошибки %d\n", msg, no);
	} else
		printf("%s\n", msg);

	return no;
}

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVersion) {
	Sleep(15000);

	/*локализация*/
	setlocale(LC_ALL, "russian");

	/*переход в каталог wsisapi.dll, т.к. она работает не самостоятельно*/
	char path[] = "C:\\Program Files (x86)\\1cv82\\8.2.17.143\\bin\\";
	if(!SetCurrentDirectory(path)) {
		show_err("Не удалось зайти в каталог с dll", TRUE);
		return FALSE;
	}

	hModule = LoadLibrary("wsisapi.dll");
	if(hModule == NULL)
		show_err("Не удалось загрузить dll", TRUE);
	else {

		fGetExtensionVersion = GetProcAddress((HMODULE)hModule, "GetExtensionVersion");
		fHttpExtensionProc   = GetProcAddress((HMODULE)hModule, "HttpExtensionProc");
		fTerminateExtension  = GetProcAddress((HMODULE)hModule, "TerminateExtension");
		BOOL ret =  fGetExtensionVersion(pVersion);
		return ret;
	}

	return FALSE;		
}

BOOL WINAPI GetServerVariable(HCONN hConn, LPSTR lpszVariableName, LPVOID lpvBuffer, LPDWORD lpdwSize) {
	BOOL ret = fGetServerVariable(hConn, lpszVariableName, lpvBuffer, lpdwSize);

	//if (!strcmp(lpszVariableName, "APPL_PHYSICAL_PATH")) {
	//	char  tmp[] = "C:\\inetpub\\wwwroot\\my\\";
	//	/*первый раз запрашивает размер*/
	//	if (*lpdwSize)
	//		strcpy(lpvBuffer, tmp);
	//	*lpdwSize = strlen(tmp) + 1;
	//}else if (!strcmp(lpszVariableName, "HTTP_1C-APPLICATIONNAME")) {
	//	char  tmp[] = "C:\\inetpub\\wwwroot\\my\\";
	//	/*первый раз запрашивает размер*/
	//	if (*lpdwSize)
	//		strcpy(lpvBuffer, tmp);
	//	*lpdwSize = strlen(tmp) + 1;
	//}
	//else if (!strcmp(lpszVariableName, "HTTP_ACCEPT")) {
	//	char  tmp[] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
	//	/*первый раз запрашивает размер*/
	//	if (*lpdwSize)
	//		strcpy(lpvBuffer, tmp);
	//	*lpdwSize = strlen(tmp) + 1;
	//}

	return ret;
}
BOOL WINAPI ReadClient(HCONN ConnID, LPVOID lpvBuffer, LPDWORD lpdwSize) {
	BOOL ret = fReadClient(ConnID, lpvBuffer, lpdwSize);

	return TRUE;
}
BOOL WINAPI WriteClient(HCONN ConnID, LPVOID Buffer, LPDWORD lpdwBytes, DWORD dwReserved) {
	BOOL ret = fWriteClient(ConnID, Buffer, lpdwBytes, dwReserved);

	return ret;
}

struct HSE_SEND_HEADER_EX_INFO {
	LPCSTR pszStatus;
	LPCSTR pszHeader;
	DWORD cchStatus;
	DWORD cchHeader;
	BOOL fKeepConn;
};

BOOL WINAPI ServerSupportFunction(HCONN hConn, DWORD dwHSERRequest, LPVOID lpvBuffer, LPDWORD lpdwSize, LPDWORD lpdwDataType) {
	if (dwHSERRequest == HSE_REQ_SEND_RESPONSE_HEADER_EX) {
		HSE_SEND_HEADER_EX_INFO * tmp = (HSE_SEND_HEADER_EX_INFO *)lpvBuffer;
	}

	BOOL ret = fServerSupportFunction(hConn, dwHSERRequest, lpvBuffer, lpdwSize, lpdwDataType);

	return ret;
}

DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB) {
	//Sleep(5000);

	fWriteClient = pECB->WriteClient;
	pECB->WriteClient = WriteClient;

	fReadClient = pECB->ReadClient;
	pECB->ReadClient = ReadClient; 

	fGetServerVariable = pECB->GetServerVariable;
	pECB->GetServerVariable = GetServerVariable;

	fServerSupportFunction = pECB->ServerSupportFunction;
	pECB->ServerSupportFunction = ServerSupportFunction;

	DWORD ret = fHttpExtensionProc(pECB);

	return ret;
}

BOOL  WINAPI   TerminateExtension(DWORD dwFlags) {
	BOOL ret = fTerminateExtension(dwFlags);
	FreeLibrary(hModule);

	return ret;
}