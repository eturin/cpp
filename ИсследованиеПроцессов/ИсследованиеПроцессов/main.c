#include <windows.h>
#include <locale.h>
#include <tlhelp32.h>

#include <stdio.h>
#include <tchar.h>


/*������� ������ ������*/
DWORD show_err(LPCTSTR msg) {
	LPTSTR str_err;
	DWORD no = GetLastError();
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
//		| FORMAT_MESSAGE_FROM_HMODULE
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), &str_err, 0, NULL)) {
		_tprintf(_TEXT("%s: %d\n%s\n"), msg, no, str_err);
		LocalFree(str_err);
	} else
		_tprintf(_TEXT("%s:\n����� ������ %d\n"), msg, no);

	return no;
}

/*���������� ������ ����������� ��������*/
DWORD PrintModuleList(DWORD CONST dwProcessId) {
	/*�������� ������ ���� ������� ����������� ��������*/
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if(INVALID_HANDLE_VALUE == hSnapshot) 
		return show_err(_TEXT("�� ������� �������� ������ ������� ��������"));	

	/*�������� �������� � ������ ������ �� ������*/
	MODULEENTRY32 meModuleEntry = {sizeof(MODULEENTRY32), 0}; //������ ���� �������� ������ ��������� dwSize=sizeof(MODULEENTRY32), ��� ����������� ������� ������ � ����� ����� ����� ����	
	Module32First(hSnapshot, &meModuleEntry);
	do {
		_tprintf(_TEXT("  ����� ������: %08X, ������ � ������: %08X, ���: %s, ����: %s\r\n"), meModuleEntry.modBaseAddr, meModuleEntry.modBaseSize, meModuleEntry.szModule, meModuleEntry.szExePath);
		
	} while(Module32Next(hSnapshot, &meModuleEntry));//�������� ��������� ������ �� ������

	CloseHandle(hSnapshot);
}

/*���������� ��� ��������*/
DWORD PrintProcessList() {
	/*�������� ������:
	  TH32CS_SNAPPROCESS - �������� ��� ��������
	  TH32CS_SNAPMODULE  - �������� ��� ������ ����������� �������� (����� �� 0 �������� ������ ����������)
	  TH32CS_SNAPTHREAD  - �������� ��� ������ � �������*/
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnapshot) 
		return show_err(_TEXT("�� ������� �������� ������ ���� ���������"));		
	
	/*�������� �������� � ������ �������� �� ������*/
	PROCESSENTRY32 peProcessEntry = {sizeof(PROCESSENTRY32), 0}; //������ ���� �������� ������ ��������� dwSize=sizeof(PROCESSENTRY32), ��� ����������� ������� ������ � ����� ����� ����� ����		
	Process32First(hSnapshot, &peProcessEntry);
	do {
		_tprintf(_TEXT("===ID: %08X, ���: %s, ����� �������: %d===\r\n"), peProcessEntry.th32ProcessID, peProcessEntry.szExeFile, peProcessEntry.cntThreads);
		/*��������� �������� � ���� ������� ����������� ��������*/
		PrintModuleList(peProcessEntry.th32ProcessID);
	} while(Process32Next(hSnapshot, &peProcessEntry)); //�������� ��������� ������� �� ������

	CloseHandle(hSnapshot);
}

int main() {
	setlocale(LC_ALL, "");
	PrintProcessList();

	ExitProcess(0);
}