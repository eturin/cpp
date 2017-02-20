#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>

typedef FARPROC(WINAPI *LPMessageBox)(HWND, LPCWSTR, LPCWSTR, UINT);

/*�������� ���� ��� �������� �������*/
typedef struct _InjectData {
	char          title[50];
	char          msg[50];
	LPMessageBox  MessageB;
} InjectData, *PInjectData;

/*����� ����� � �����*/
static DWORD WINAPI InjectionMain(LPVOID lpParams) {
	PInjectData info = (PInjectData)lpParams;
	info->MessageB(NULL, (LPCWSTR)info->msg, (LPCWSTR)info->title, MB_OK);

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
	setlocale(LC_ALL, "");
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
	DWORD pid = getProcessID(_TEXT("1cv8c.exe"));
	if(pid == 0) {
		_tprintf(_TEXT("�� ������� ����� ������ �������.\n"));
		return 1;
	}
	/*��������� ������ �������*/
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, /*���� �������*/
							   FALSE,              /*������� ������������ ����������� �����������*/
							   pid);               /*������������� ������������ ��������*/
	
	if(hProc==NULL) {
		show_err(_TEXT("�� ������� ������� ��������� �������"));
		return 1;
	}

	/*����������� ��������� ����������*/
	HINSTANCE userHinstance = LoadLibrary(_TEXT("user32.dll"));
	/*�������� ��������� �� �������, ������� ����� �������*/
	InjectData injectData = {"Test", "������", NULL};
	injectData.MessageB = (LPMessageBox)GetProcAddress(userHinstance, "MessageBoxA");
	/*��������� ����������*/
	FreeLibrary(userHinstance);

	/*������� ������ ��� ������ � ��� � ������ �������� � �������� � ��� �����*/
	
	DWORD ProcSize = 1024; //������ ������������ �������
	/*����������� ����������� ������ ������ � ����������� �������� ������������ ����������� ��������*/
	LPVOID lpProc   = VirtualAllocEx(hProc, NULL, ProcSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	/*������ � ����������� ������ ���� ����� ������� (������ � ��������)*/
	DWORD dwWritten=0;
	if(WriteProcessMemory(hProc, lpProc, InjectionMain, ProcSize, &dwWritten) == 0) {
		show_err(_TEXT("������ ������ � ����������� ������ ������"));
		return 1;
	}

	/*����������� ������ ������ ������ � ����������� �������� ������������ ����������� ��������*/
	LPVOID lpParams = VirtualAllocEx(hProc, NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
	/*������ � ������ ������ ����� ���������*/
	dwWritten = 0;
	if(WriteProcessMemory(hProc, lpParams, &injectData, sizeof(injectData), &dwWritten) == 0) {
		show_err(_TEXT("������ ������ � ������ ������ ������"));
		return 1;
	}

	/*��������� ����� � ��������� �������� � ��������� ���������*/	
	DWORD ThreadID=0;
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)lpProc, lpParams, 0, &ThreadID);
	if(hThread == NULL) {
		show_err(_TEXT("�� ������� ������� ��������� �����"));
		return 1;
	}

	/*��������� ��� ���������*/
	CloseHandle(hThread);
	CloseHandle(hProc);
	CloseHandle(hToken);
	CloseHandle(hCurrentProc);

	return 0;
}
