#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>

typedef FARPROC(WINAPI *LPMessageBox)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef FARPROC(WINAPI *LPLoadLibrary)(LPCWSTR);
typedef FARPROC(WINAPI *LPGetProcAddress)(HMODULE,LPCSTR);
typedef BOOL (WINAPI *LPFreeLibrary)(HMODULE);

/*�������� ���� ��� �������� �������*/
typedef struct _InjectData {
	TCHAR            title[50];
	TCHAR            msg[50];
	LPMessageBox     Message;
	LPLoadLibrary    LoadLibrary;
	LPGetProcAddress GetProcAddress;
	LPFreeLibrary    FreeLibrary;
} InjectData, *PInjectData;

/*����� ����� � �����*/
DWORD InjectionMain(LPVOID lpParams) {
	PInjectData info = (PInjectData)lpParams;

	/*����������� ��������� ����������*/
	HINSTANCE Hinstance = info->LoadLibraryW(_TEXT("user32.dll"));
	LPMessageBox proc = (LPMessageBox)info->GetProcAddress(Hinstance, "MessageBoxW");
	
	//TCHAR msg[] = {L'-', L'-', L'H', L'e', L'l', L'l', L'o', L'-', L'-',L'\0'};
	for(int i = 0; i < 2;++i)
		info->Message(NULL, info->msg, info->title, MB_OK);		        

	info->FreeLibrary(Hinstance);

	return 0;
}


/*������� ������ ������*/
DWORD show_err(LPCTSTR msg) {
	LPTSTR str_err;
	DWORD no = GetLastError();
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_FROM_HMODULE
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), &str_err, 0, NULL)) {
		_tprintf(_TEXT("%s: %d\n%s\n"), msg, no, str_err);
		LocalFree(str_err);
	} else
		_tprintf(_TEXT("%s:\n����� ������ %d\n"), msg, no);

	return no;
}

/*��������� �������������� ������� ��������*/
DWORD getProcessID(TCHAR * pName) {
	DWORD ret = 0;

	/*������� ��������� �� ������ ���� ��������� � ���� OS*/
	HANDLE snapHandle;
	if((snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
		show_err(_TEXT("�� ������� �������� ������ ���� ���������"));
		return ret;
	}

	/*�������� ������ ������� �� ������*/
	PROCESSENTRY32 processEntry = {sizeof(PROCESSENTRY32)}; //��������� �������� � ���� ��������� ����� � ������ � ������ ����	
	Process32First(snapHandle, &processEntry);
	do {
		/*�����������, ����� ������ ������ ������� �� ��� �����*/
		if(_tcscmp(processEntry.szExeFile, pName) == 0) {
			ret = processEntry.th32ProcessID;
			break;
		}
	} while(Process32Next(snapHandle, &processEntry)); //�������� ��������� �������

	/*��������� ��������� �� ������ � ����*/
	CloseHandle(snapHandle);

	return ret;
}

/*���������/���������� ������ ����������*/
BOOL setPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL isOn) {
	TOKEN_PRIVILEGES tp = {1}; // tp.PrivilegeCount = 1;
	if(!LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid)) {
		show_err(_TEXT("�� ������� ����� ���������� �� �����"));
		return FALSE;
	}

	tp.Privileges[0].Attributes = isOn ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;

	if(!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL)) {
		show_err(_TEXT("�� ������� ����������/����� ����������"));
		return FALSE;
	}

	return TRUE;
}

int main() {
	setlocale(LC_ALL, "russian");
	/*�������� ��������� �������� ��������*/
	HANDLE hCurrentProc = GetCurrentProcess();

	/*�������� ���� ������� ������ ��������*/
	HANDLE hToken;
	if(!OpenProcessToken(hCurrentProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		show_err(_TEXT("OpenProcessToken Error"));
		return 1;
	} else if(!setPrivilege(hToken, SE_DEBUG_NAME, TRUE)) /*��������� ������ �������� ���������� �������, ���� ������ � ������ ������� ��������*/
		return 1;

	/*�������� ������������� ������� �������� �� ��� �����*/
	//DWORD pid = getProcessID(_TEXT("1cv8c.exe"));
	//DWORD pid = getProcessID(_TEXT("1cv8.exe"));
	//DWORD pid = getProcessID(_TEXT("notepad.exe"));
	DWORD pid = getProcessID(_TEXT("Skype.exe"));
	//DWORD pid = getProcessID(_TEXT("���������������������.exe"));
	if(pid == 0) {
		_tprintf(_TEXT("�� ������� ����� ������ �������.\n"));
		return 1;
	}
	/*��������� ������ �������*/
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, /*���� �������*/
							   FALSE,              /*������� ������������ ����������� �����������*/
							   pid);               /*������������� ������������ ��������*/

	if(hProc == NULL) {
		show_err(_TEXT("�� ������� ������� ��������� �������"));
		return 1;
	}

	/*����������� ��������� ����������*/
	HINSTANCE Hinstance = LoadLibrary(_TEXT("user32.dll"));
	/*�������� ��������� �� �������, ������� ����� �������*/
	InjectData injectData = {_TEXT("Test"), _TEXT("������"), NULL};
	injectData.Message = (LPMessageBox)GetProcAddress(Hinstance, "MessageBoxW");
	/*��������� ����������*/
	FreeLibrary(Hinstance);

	/*����������� ��������� ����������*/
	Hinstance = LoadLibrary(_TEXT("Kernel32.dll"));
	/*�������� ��������� �� �������, ������� ����� �������*/
	injectData.LoadLibrary    = (LPLoadLibrary)GetProcAddress(Hinstance, "LoadLibraryW");
	injectData.GetProcAddress = (LPGetProcAddress)GetProcAddress(Hinstance, "GetProcAddress");
	injectData.FreeLibrary    = (LPFreeLibrary)GetProcAddress(Hinstance, "FreeLibrary");
	/*��������� ����������*/
	FreeLibrary(Hinstance);

	/*������� ������ ��� ������ � ��� � ������ �������� � �������� � ��� �����*/


	/*����������� ����������� ������ ������ � ����������� �������� ������������ ����������� ��������*/
	LPVOID lpProc = VirtualAllocEx(hProc, NULL, (char*)show_err - (char*)InjectionMain, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	/*������ � ����������� ������ ���� ����� ������� (������ � ��������)*/
	DWORD dwWritten = 0;
	if(WriteProcessMemory(hProc, lpProc, InjectionMain, (char*)show_err - (char*)InjectionMain, &dwWritten) == 0) {
		show_err(_TEXT("������ ������ � ����������� ������ ������"));
		return 1;
	}

	/*����������� ������ ������ ������ � ����������� �������� ������������ ����������� ��������*/
	LPVOID lpParams = VirtualAllocEx(hProc, NULL, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	/*������ � ������ ������ ����� ���������*/
	dwWritten = 0;
	if(WriteProcessMemory(hProc, lpParams, &injectData, sizeof(injectData), &dwWritten) == 0) {
		show_err(_TEXT("������ ������ � ������ ������ ������"));
		return 1;
	}

	/*��������� ����� � ��������� �������� � ��������� ���������*/
	DWORD ThreadID = 0;
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 16 * 1024, (LPTHREAD_START_ROUTINE)lpProc, lpParams, 0, &ThreadID);
	if(hThread == NULL) {
		show_err(_TEXT("�� ������� ������� ��������� �����"));
		return 1;
	}
	/*���������� ���������� ������*/
	WaitForSingleObject(hThread, INFINITE);
	/*����������� ������ ���������� ��������*/
	VirtualFree(lpProc, 0, MEM_RELEASE);
	VirtualFree(lpParams, 0, MEM_RELEASE);
	/*��������� ��� ���������*/
	CloseHandle(hThread);
	CloseHandle(hProc);
	CloseHandle(hToken);
	CloseHandle(hCurrentProc);

	return 0;
}