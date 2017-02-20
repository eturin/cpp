#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>


/*функция печати ошибки*/
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
		_tprintf(_TEXT("%s:\nномер ошибки %d\n"), msg, no);

	return no;
}

/*получение идентификатора нужного процесса*/
DWORD getProcessID(TCHAR * pName) {
	DWORD ret = 0;

	/*получим указатель на слепок всех процессов в ядре OS*/
	HANDLE snapHandle;
	if((snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
		show_err(_TEXT("Не удалось получить список всех процессов"));
		return ret;
	}

	/*получаем первый процесс из слепка*/
	PROCESSENTRY32 processEntry = {sizeof(PROCESSENTRY32)}; //требуется сообщить о типе структуре через её размер в первом поле	
	Process32First(snapHandle, &processEntry);
	do {
		/*остановимся, когда найдем нужный процесс по его имени*/
		if(_tcscmp(processEntry.szExeFile, pName) == 0) {
			ret = processEntry.th32ProcessID;
			break;
		}
	} while(Process32Next(snapHandle, &processEntry)); //получаем следующий процесс

	/*закрываем указатель на объект в ядре*/
	CloseHandle(snapHandle);

	return ret;
}

/*включение/выключение нужной привелегии*/
BOOL setPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL isOn) {
	TOKEN_PRIVILEGES tp = {1}; // tp.PrivilegeCount = 1;
	if(!LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid)) {
		show_err(_TEXT("Не удалось найти привелегию по имени"));
		return FALSE;
	}

	tp.Privileges[0].Attributes = isOn ? SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;

	if(!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL)) {
		show_err(_TEXT("Не удалось установить/снять привилегию"));
		return FALSE;
	}

	return TRUE;
}

int main() {
	setlocale(LC_ALL, "russian");
	/*получаем указатель текущего процесса*/
	HANDLE hCurrentProc = GetCurrentProcess();
	
	/*получаем идентификатор нужного процесса по его имени*/
	//DWORD pid = getProcessID(_TEXT("1cv8c.exe"));
	DWORD pid = getProcessID(_TEXT("1cv8.exe"));
	//DWORD pid = getProcessID(_TEXT("notepad.exe"));
	//DWORD pid = getProcessID(_TEXT("Skype.exe"));
	//DWORD pid = getProcessID(_TEXT("ИсследованиеПроцессов.exe"));
	if(pid == 0) {
		_tprintf(_TEXT("Не удалось найти нужный процесс.\n"));
		return 1;
	}
	/*открываем другой процесс*/
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, /*флаг доступа*/
							   FALSE,              /*признак наследования получаемого дескриптора*/
							   pid);               /*идентификатор открываемого процесса*/

	if(hProc == NULL) {
		show_err(_TEXT("Не удалось открыть требуемый процесс"));
		return 1;
	}

	LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	if(addr == NULL) {
		show_err(_TEXT("Не удалось получить адрес функции загружающей dll"));
		return 1;
	}

	/*ЗАПИШЕМ НУЖНЫЕ НАМ ДАННЫЕ В ПАМЯТЬ ПРОЦЕССА И ЗАПУСТИМ В НЕМ ПОТОК*/
	
	/*резервируем регион данных памяти в виртуальном адресном пространстве конкретного процесса*/
	LPVOID lpParams = VirtualAllocEx(hProc, NULL, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	/*запись в регион данных нашей структуры*/
	TCHAR buffer[] = _TEXT("C:\\Users\\etyurin\\Documents\\Visual Studio 2013\\ProjectsCPP\\Внедрение DLL\\Debug\\Внедрение DLL.dll");
	DWORD dwWritten = 0;
	if(WriteProcessMemory(hProc, lpParams, buffer, sizeof(buffer), &dwWritten) == 0) {
		show_err(_TEXT("Ошибка записи в регион данных памяти"));
		return 1;
	}

	/*запускаем поток в удаленном процессе с указанием параметра*/
	DWORD ThreadID = 0;
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 16 * 1024, (LPTHREAD_START_ROUTINE)addr, lpParams, 0, &ThreadID);
	if(hThread == NULL) {
		show_err(_TEXT("Не удалось создать удаленный поток"));
		return 1;
	}

	/*Закрываем все указатели*/
	CloseHandle(hThread);
	CloseHandle(hProc);	
	CloseHandle(hCurrentProc);

	return 0;
}
