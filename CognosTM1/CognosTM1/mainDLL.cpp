#define _CRT_SECURE_NO_WARNINGS


#include <wchar.h>
#include <HttpExt.h>

#include "Context.h"

/*��� ������� ����� ������� w3wp.exe*/

//����� ����� � dll
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved){
	BOOL isOk = 1;
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH: //����� ������� ����� ������ � DLL
		isOk = initDLL();
		break;
	case DLL_THREAD_ATTACH:  //����� ����� �������� ������ � DLL

		break;
	case DLL_THREAD_DETACH:  //�����(���������?) ������������� �� DLL

		break;
	case DLL_PROCESS_DETACH: //�������(���� �� �������?) ������������� �� DLL

		break;
	}

	return isOk;
}


BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVersion) {	
	pVersion->dwExtensionVersion = MAKELONG(HSE_VERSION_MINOR, HSE_VERSION_MAJOR);

	lstrcpyn(pVersion->lpszExtensionDesc, "ISAPI module for Cognas TM1", HSE_MAX_EXT_DLL_NAME_LEN);

	return TRUE;
}

DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB) {
	std::string response = "Content-Type: text/html; charset=utf-8\r\n\r\n";
	if (std::strcmp(pECB->lpszMethod, "POST") == 0) {
		//��������� ���������� �������
		CHAR * buf = new CHAR[pECB->cbTotalBytes+1];
		std::memset(buf, 0, pECB->cbTotalBytes + 1);
		std::strncat(buf, (CHAR*)pECB->lpbData, pECB->cbTotalBytes);
		buf[pECB->cbTotalBytes] = '\0';
		
		//���������� ��������� ������
		DWORD total = pECB->cbTotalBytes - (DWORD)std::strlen(buf);
		char * b = buf+ pECB->cbTotalBytes- total;
		while (total) {
			DWORD len = total;
			if(!pECB->ReadClient(pECB->ConnID, b, &len))
				break; //������ ������
			total -= len;
			b += len;
		}

		
		//����������
		wchar_t *wText2 = CodePageToUnicode(65001, buf);
		delete[] buf;
		char *ansiText = UnicodeToCodePage(1251, wText2);
		delete[] wText2;
		//������				
		try {
			Context context(ansiText,
				false,
				R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognosTM1\Debug\ssl\tm1ca_v2.pem)");
			response += "OK";
		}catch (std::exception &e) {
			const char * buf = e.what();
			//����������
			wchar_t *wText = CodePageToUnicode(1251, buf);
			char *utf8Text = UnicodeToCodePage(65001, wText);
			delete[] wText;
			response += utf8Text;
			delete[] utf8Text;
		}

		//����������� �������
		delete[] ansiText;
		
	}else {
		const char * buf = R"({
	"adminServer":"as-msk-a0134",
	"server":"S2018",
	"login":"admin",
	"pwd":"",	
	
	"delete":[
		{"cubeName":"BDDS"},
		{"dimensionName":"������"},
		{"dimensionName":"������������"},
		{"dimensionName":"�������������"},
		{"dimensionName":"����"},
		{"dimensionName":"����������������"},
		{"dimensionName":"����������"}		
	],
	
	"create":[
		{"dimensionName":"������",
		 "elements":["0x11111111111111111111111111111111","0x22222222222222222222222222222222"]},
		 
		{"dimensionName":"������������",
		 "elements":["�4-0091362/1","�4-0091362/2"]},
		 
		{"dimensionName":"�������������",
		 "elements":["�����"]},
		 
		{"dimensionName":"����",
		 "elements":["18.02.2015","19.02.2015"]},
		 
		{"dimensionName":"����������������",
		 "elements":["05.03.2015","06.03.2015"]},
		 
		{"dimensionName":"����������",
		 "elements":["����� ������ � 48 �. ������","����� ������ � 49 �. ������"]},
		 
		{"cubeName":"BDDS",
		 "dimensions":["������","������������","�������������","����","����������������","����������"]}
	],
	
	"set":[
		{"cubeName":"BDDS",		
		"val":[
				{"1":"0x11111111111111111111111111111111",
				 "2":"�4-0091362/1",
				 "3":"�����",
				 "4":"18.02.2015",
				 "5":"05.03.2015",
				 "6":"����� ������ � 48 �. ������",
				 "v":54.00},
				{"1":"0x22222222222222222222222222222222",
				 "2":"�4-0091362/2",
				 "3":"�����",
				 "4":"19.02.2015",
				 "5":"06.03.2015",
				 "6":"����� ������ � 49 �. ������",
				 "v":12.00}
			]
		}
	]
})";
		//����������
		wchar_t *wText = CodePageToUnicode(1251, buf);
		char *utf8Text = UnicodeToCodePage(65001, wText);
		delete[] wText;
		response += utf8Text;
		delete[] utf8Text;
	}

	//���������� �������
	if (!pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, NULL, NULL, (LPDWORD)response.c_str())) {
		return HSE_STATUS_ERROR;
	}
	else {
		/*����� ������ (������ ��� 200)*/
		pECB->dwHttpStatusCode = 200;
	}
	return HSE_STATUS_SUCCESS;

	
	////Sleep(15000);

	///*��� ��� �����, ��� �� ���������� ����� �������*/
	//CHAR szBuff[4096];
	//CHAR szTempBuf[4096];

	///*���� ��� ������� ���������� (����� 200 ��� 404)*/
	//pECB->dwHttpStatusCode = 0;

	///*��������� �����*/
	//wsprintf(szBuff, "Content-Type: text/html\r\n\r\n"
	//	"<HTML><HEAD><TITLE>������ ���������� ISAPI</TITLE></HEAD>\n"
	//	"<BODY BGCOLOR=#FFFFFF><H2>Hi from ISAPI Extension!</H2>\n");

	//
	//strcat(szBuff, "<HR>");
	//wsprintf(szTempBuf, "<P>Extension Version: %d.%d\n", HIWORD(pECB->dwVersion), LOWORD(pECB->dwVersion));
	//strcat(szBuff, szTempBuf);
	//wsprintf(szTempBuf, "<BR>Method: %s(�����)\n", pECB->lpszMethod);
	//strcat(szBuff, szTempBuf);
	//wsprintf(szTempBuf, "<BR>QueryString: %s\n", pECB->lpszQueryString);
	//strcat(szBuff, szTempBuf);
	//wsprintf(szTempBuf, "<BR>PathTranslated: %s\n", pECB->lpszPathTranslated);
	//strcat(szBuff, szTempBuf);
	//wsprintf(szTempBuf, "<BR>TotalBytes: %d\n", pECB->cbTotalBytes);
	//strcat(szBuff, szTempBuf);
	//wsprintf(szTempBuf, "<BR>ContentType: %s\n", pECB->lpszContentType);
	//strcat(szBuff, szTempBuf);

	//strcat(szBuff, "<HR><P><B>Server Variables:</B><BR>\nALL_HTTP: ");
	//DWORD dwSize = 4096;
	//pECB->GetServerVariable(pECB->ConnID, (LPSTR)"ALL_HTTP", (LPVOID)szTempBuf, &dwSize);
	//strcat(szBuff, szTempBuf);

	//strcat(szBuff, "<BR>\nAPPL_PHYSICAL_PATH: ");
	//dwSize = 4096;
	//pECB->GetServerVariable(pECB->ConnID, (LPSTR)"APPL_PHYSICAL_PATH", (LPVOID)szTempBuf, &dwSize);
	//strcat(szBuff, szTempBuf);

	//strcat(szBuff, "<BR>\nHTTP_ACCEPT: ");
	//dwSize = 4096;
	//pECB->GetServerVariable(pECB->ConnID, (LPSTR)"HTTP_ACCEPT", (LPVOID)szTempBuf, &dwSize);
	//strcat(szBuff, szTempBuf);

	//strcat(szBuff, "</BODY></HTML>");

	///*���������� �������*/
	//if (!pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, NULL, NULL, (LPDWORD)szBuff)) {
	//	return HSE_STATUS_ERROR;
	//}
	//else {
	//	/*����� ������ (������ ��� 200)*/
	//	pECB->dwHttpStatusCode = 200;
	//}
	//return HSE_STATUS_SUCCESS;
}

BOOL  WINAPI   TerminateExtension(DWORD dwFlags) {
	
	return releasDLL();
}