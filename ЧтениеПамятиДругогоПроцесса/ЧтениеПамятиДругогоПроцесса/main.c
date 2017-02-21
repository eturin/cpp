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

	/*получаем ключ доступа нашего процесса*/
	HANDLE hToken;
	if(!OpenProcessToken(hCurrentProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		show_err(_TEXT("OpenProcessToken Error"));
		return 1;
	} else if(!setPrivilege(hToken, SE_DEBUG_NAME, TRUE)) /*добавляем нашему процессу привелегию отладки, чтоб писать в память другого процесса*/
		return 1;

	/*получаем идентификатор нужного процесса по его имени*/
	DWORD pid = getProcessID(_TEXT("1cv8c.exe"));
	//DWORD pid = getProcessID(_TEXT("1cv8.exe"));
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

	/*ПРОЧИТАЕМ НУЖНЫЕ НАМ ДАННЫЕ*/
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	
	/*буфер под считываемый регион*/
	PBYTE Buffer = malloc(65536);

	/*выбеляем память в адресном пространстве процесса*/
	LPVOID p = VirtualAllocEx(hProc, NULL, 65536, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(NULL == p)
		return show_err(_TEXT("Не удалось выделить память в виртуальном адресном пространстве процесса"));
	else
		_tprintf(_TEXT("Aдрес выделенного региона %x\n"), p);

	LPVOID lpMem = 0;	
	while(lpMem < si.lpMaximumApplicationAddress) {
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQueryEx(hProc, lpMem, &mbi, sizeof(MEMORY_BASIC_INFORMATION));   // Узнаем о текущем регионе памяти.

		if(mbi.Protect & PAGE_READWRITE 
		   || mbi.Protect & PAGE_WRITECOPY
		   || mbi.Protect & PAGE_READONLY) {
			
			/*считываю весь найденный регион*/
			DWORD dwRead = 0;
			if(!ReadProcessMemory(hProc, mbi.BaseAddress, Buffer, mbi.RegionSize, &dwRead))
				return show_err(_TEXT("Не удалось прочитать регион памяти процесса"));

			_tprintf(_TEXT("Начальный адрес просматриваемого региона %x\n"), mbi.BaseAddress);
						
				
			///*пишем в память процесса (так ради понта)*/
			//if(mbi.Protect & PAGE_READWRITE
			//   || mbi.Protect & PAGE_WRITECOPY) {
				//DWORD dwWritten = 0;
				//if(!WriteProcessMemory(hProc, p, Buffer, dwRead, &dwWritten))
				//	return show_err(_TEXT("Ошибка записи в регион данных памяти"));						
			//}
		}

		/*переход к следующему региону*/
		lpMem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);

	}

	/*освобождаем память удаленного процесса*/
	VirtualFree(p, 0, MEM_RELEASE);

	/*освобождаем память у себя*/
	free(Buffer);
	
	/*Закрываем все указатели*/	
	CloseHandle(hProc);
	CloseHandle(hToken);
	CloseHandle(hCurrentProc);

	return 0;
}