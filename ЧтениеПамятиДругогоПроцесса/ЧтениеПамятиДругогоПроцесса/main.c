#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>

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
	DWORD pid = getProcessID(_TEXT("1cv8c.exe"));
	//DWORD pid = getProcessID(_TEXT("1cv8.exe"));
	//DWORD pid = getProcessID(_TEXT("notepad.exe"));
	//DWORD pid = getProcessID(_TEXT("Skype.exe"));
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

	/*��������� ������ ��� ������*/
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	
	/*����� ��� ����������� ������*/
	PBYTE Buffer = malloc(65536);

	/*�������� ������ � �������� ������������ ��������*/
	LPVOID p = VirtualAllocEx(hProc, NULL, 65536, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(NULL == p)
		return show_err(_TEXT("�� ������� �������� ������ � ����������� �������� ������������ ��������"));
	else
		_tprintf(_TEXT("A���� ����������� ������� %x\n"), p);

	LPVOID lpMem = 0;	
	while(lpMem < si.lpMaximumApplicationAddress) {
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQueryEx(hProc, lpMem, &mbi, sizeof(MEMORY_BASIC_INFORMATION));   // ������ � ������� ������� ������.

		if(mbi.Protect & PAGE_READWRITE 
		   || mbi.Protect & PAGE_WRITECOPY
		   || mbi.Protect & PAGE_READONLY) {
			
			/*�������� ���� ��������� ������*/
			DWORD dwRead = 0;
			if(!ReadProcessMemory(hProc, mbi.BaseAddress, Buffer, mbi.RegionSize, &dwRead))
				return show_err(_TEXT("�� ������� ��������� ������ ������ ��������"));

			_tprintf(_TEXT("��������� ����� ���������������� ������� %x\n"), mbi.BaseAddress);
						
				
			///*����� � ������ �������� (��� ���� �����)*/
			//if(mbi.Protect & PAGE_READWRITE
			//   || mbi.Protect & PAGE_WRITECOPY) {
				//DWORD dwWritten = 0;
				//if(!WriteProcessMemory(hProc, p, Buffer, dwRead, &dwWritten))
				//	return show_err(_TEXT("������ ������ � ������ ������ ������"));						
			//}
		}

		/*������� � ���������� �������*/
		lpMem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);

	}

	/*����������� ������ ���������� ��������*/
	VirtualFree(p, 0, MEM_RELEASE);

	/*����������� ������ � ����*/
	free(Buffer);
	
	/*��������� ��� ���������*/	
	CloseHandle(hProc);
	CloseHandle(hToken);
	CloseHandle(hCurrentProc);

	return 0;
}