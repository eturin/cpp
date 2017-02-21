#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <stdio.h>
#include <tlhelp32.h>

typedef FARPROC(WINAPI *LPMessageBox)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef FARPROC(WINAPI *LPLoadLibrary)(LPCWSTR);
typedef FARPROC(WINAPI *LPGetProcAddress)(HMODULE,LPCSTR);
typedef BOOL (WINAPI *LPFreeLibrary)(HMODULE);

/*описание типы для поточной функции*/
typedef struct _InjectData {
	TCHAR            title[50];
	TCHAR            msg[50];
	LPMessageBox     Message;
	LPLoadLibrary    LoadLibrary;
	LPGetProcAddress GetProcAddress;
	LPFreeLibrary    FreeLibrary;
} InjectData, *PInjectData;

/*точка входа в поток*/
DWORD InjectionMain(LPVOID lpParams) {
	PInjectData info = (PInjectData)lpParams;

	/*динамически загружаем библиотеку*/
	HINSTANCE Hinstance = info->LoadLibraryW(_TEXT("user32.dll"));
	LPMessageBox proc = (LPMessageBox)info->GetProcAddress(Hinstance, "MessageBoxW");
	
	//TCHAR msg[] = {L'-', L'-', L'H', L'e', L'l', L'l', L'o', L'-', L'-',L'\0'};
	for(int i = 0; i < 2;++i)
		info->Message(NULL, info->msg, info->title, MB_OK);		        

	info->FreeLibrary(Hinstance);

	return 0;
}


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

	/*получаем ключ доступа нашего процесса*/
	HANDLE hToken;
	if(!OpenProcessToken(hCurrentProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		show_err(_TEXT("OpenProcessToken Error"));
		return 1;
	} else if(!setPrivilege(hToken, SE_DEBUG_NAME, TRUE)) /*добавляем нашему процессу привелегию отладки, чтоб писать в память другого процесса*/
		return 1;

	/*получаем идентификатор нужного процесса по его имени*/
	//DWORD pid = getProcessID(_TEXT("1cv8c.exe"));
	//DWORD pid = getProcessID(_TEXT("1cv8.exe"));
	//DWORD pid = getProcessID(_TEXT("notepad.exe"));
	DWORD pid = getProcessID(_TEXT("Skype.exe"));
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

	/*динамически загружаем библиотеку*/
	HINSTANCE Hinstance = LoadLibrary(_TEXT("user32.dll"));
	/*получаем указатель на функцию, которую хотим вызвать*/
	InjectData injectData = {_TEXT("Test"), _TEXT("Привет"), NULL};
	injectData.Message = (LPMessageBox)GetProcAddress(Hinstance, "MessageBoxW");
	/*выгружаем библиотеку*/
	FreeLibrary(Hinstance);

	/*динамически загружаем библиотеку*/
	Hinstance = LoadLibrary(_TEXT("Kernel32.dll"));
	/*получаем указатель на функцию, которую хотим вызвать*/
	injectData.LoadLibrary    = (LPLoadLibrary)GetProcAddress(Hinstance, "LoadLibraryW");
	injectData.GetProcAddress = (LPGetProcAddress)GetProcAddress(Hinstance, "GetProcAddress");
	injectData.FreeLibrary    = (LPFreeLibrary)GetProcAddress(Hinstance, "FreeLibrary");
	/*выгружаем библиотеку*/
	FreeLibrary(Hinstance);

	/*ЗАПИШЕМ НУЖНЫЕ НАМ ДАННЫЕ И КОД В ПАМЯТЬ ПРОЦЕССА И ЗАПУСТИМ В НЕМ ПОТОК*/


	/*резервируем исполняемый регион памяти в виртуальном адресном пространстве конкретного процесса*/
	LPVOID lpProc = VirtualAllocEx(hProc, NULL, (char*)show_err - (char*)InjectionMain, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	/*запись в исполняемый регион кода нашей функции (размер с избытком)*/
	DWORD dwWritten = 0;
	if(WriteProcessMemory(hProc, lpProc, InjectionMain, (char*)show_err - (char*)InjectionMain, &dwWritten) == 0) {
		show_err(_TEXT("Ошибка записи в исполняемый регион памяти"));
		return 1;
	}

	/*резервируем регион данных памяти в виртуальном адресном пространстве конкретного процесса*/
	LPVOID lpParams = VirtualAllocEx(hProc, NULL, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	/*запись в регион данных нашей структуры*/
	dwWritten = 0;
	if(WriteProcessMemory(hProc, lpParams, &injectData, sizeof(injectData), &dwWritten) == 0) {
		show_err(_TEXT("Ошибка записи в регион данных памяти"));
		return 1;
	}

	/*запускаем поток в удаленном процессе с указанием параметра*/
	DWORD ThreadID = 0;
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 16 * 1024, (LPTHREAD_START_ROUTINE)lpProc, lpParams, 0, &ThreadID);
	if(hThread == NULL) {
		show_err(_TEXT("Не удалось создать удаленный поток"));
		return 1;
	}
	/*дожидаемся завершения потока*/
	WaitForSingleObject(hThread, INFINITE);
	/*освобождаем память удаленного процесса*/
	VirtualFree(lpProc, 0, MEM_RELEASE);
	VirtualFree(lpParams, 0, MEM_RELEASE);
	/*Закрываем все указатели*/
	CloseHandle(hThread);
	CloseHandle(hProc);
	CloseHandle(hToken);
	CloseHandle(hCurrentProc);

	return 0;
}