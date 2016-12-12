#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <HttpExt.h>

#include <stdio.h>
#include <string.h>

/*для отладки нужно цеплять w3wp.exe*/

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVersion) {
	//Sleep(5000);
	pVersion->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);

	lstrcpyn(pVersion->lpszExtensionDesc, "My ISAPI module", HSE_MAX_EXT_DLL_NAME_LEN);

	return TRUE; 
}

DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB) {
	//Sleep(5000);

	/*это наш буфер, где мы сформируем ответ клиенту*/
	CHAR szBuff[4096];
	CHAR szTempBuf[4096];

	/*пока нет статуса результата (вроде 200 или 404)*/
	pECB->dwHttpStatusCode = 0;

	/*формируем ответ*/
	wsprintf(szBuff,"Content-Type: text/html\r\n\r\n"
			        "<HTML><HEAD><TITLE>Simple ISAPI Extension</TITLE></HEAD>\n"
			        "<BODY BGCOLOR=#FFFFFF><H2>Hello from ISAPI Extension!</H2>\n");
	
	strcat(szBuff, "<HR>");
	wsprintf(szTempBuf, "<P>Extension Version: %d.%d\n", HIWORD(pECB->dwVersion), LOWORD(pECB->dwVersion));
	strcat(szBuff, szTempBuf);
	wsprintf(szTempBuf, "<BR>Method: %s(Метод)\n",       pECB->lpszMethod);
	strcat(szBuff, szTempBuf);
	wsprintf(szTempBuf, "<BR>QueryString: %s\n",         pECB->lpszQueryString);
	strcat(szBuff, szTempBuf);
	wsprintf(szTempBuf, "<BR>PathTranslated: %s\n",      pECB->lpszPathTranslated);
	strcat(szBuff, szTempBuf);
	wsprintf(szTempBuf, "<BR>TotalBytes: %d\n",          pECB->cbTotalBytes);
	strcat(szBuff, szTempBuf);
	wsprintf(szTempBuf, "<BR>ContentType: %s\n",         pECB->lpszContentType);
	strcat(szBuff, szTempBuf);

	strcat(szBuff, "<HR><P><B>Server Variables:</B><BR>\n");
	DWORD dwSize = 4096;
	pECB->GetServerVariable(pECB->ConnID, (LPSTR)"ALL_HTTP", (LPVOID)szTempBuf, &dwSize);
	strcat(szBuff, szTempBuf);

	strcat(szBuff, "</BODY></HTML>");

	/*отправляем клиенту*/
	if(!pECB->ServerSupportFunction(pECB->ConnID,HSE_REQ_SEND_RESPONSE_HEADER, NULL, NULL, (LPDWORD)szBuff)) {
		return HSE_STATUS_ERROR;
	} else {
		/*новый статус (теперь уже 200)*/
		pECB->dwHttpStatusCode = 200;
	}
	return HSE_STATUS_SUCCESS;
}