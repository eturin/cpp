#include <windows.h>
#include <locale.h>
#include <tlhelp32.h>

#include <stdio.h>
#include <tchar.h>


/*функция печати ошибки*/
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
		_tprintf(_TEXT("%s:\nномер ошибки %d\n"), msg, no);

	return no;
}

/*перебираем модули конкретного процесса*/
DWORD PrintModuleList(DWORD CONST dwProcessId) {
	/*получаем слепок всех модулей конкретного процесса*/
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if(INVALID_HANDLE_VALUE == hSnapshot) 
		return show_err(_TEXT("Не удалось получить список модулей процесса"));	

	/*получаем сведения о первом модуле из слепка*/
	MODULEENTRY32 meModuleEntry = {sizeof(MODULEENTRY32), 0}; //первое поле содержит размер структуры dwSize=sizeof(MODULEENTRY32), что принимающая функция помала с каким типом имеет дело	
	Module32First(hSnapshot, &meModuleEntry);
	do {
		_tprintf(_TEXT("  Адрес начала: %08X, размер в байтах: %08X, имя: %s, путь: %s\r\n"), meModuleEntry.modBaseAddr, meModuleEntry.modBaseSize, meModuleEntry.szModule, meModuleEntry.szExePath);
		
	} while(Module32Next(hSnapshot, &meModuleEntry));//получаем следующий модуль из слепка

	CloseHandle(hSnapshot);
}

/*перебираем все процессы*/
DWORD PrintProcessList() {
	/*получаем слепок:
	  TH32CS_SNAPPROCESS - включает все процессы
	  TH32CS_SNAPMODULE  - включает все модули КОНКРЕТНОГО процесса (здесь мы 0 передали вторым параметром)
	  TH32CS_SNAPTHREAD  - включает все потоки в системе*/
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnapshot) 
		return show_err(_TEXT("Не удалось получить список всех процессов"));		
	
	/*получаем сведения о первом процессе из слепка*/
	PROCESSENTRY32 peProcessEntry = {sizeof(PROCESSENTRY32), 0}; //первое поле содержит размер структуры dwSize=sizeof(PROCESSENTRY32), что принимающая функция помала с каким типом имеет дело		
	Process32First(hSnapshot, &peProcessEntry);
	do {
		_tprintf(_TEXT("===ID: %08X, имя: %s, всего потоков: %d===\r\n"), peProcessEntry.th32ProcessID, peProcessEntry.szExeFile, peProcessEntry.cntThreads);
		/*получение сведений о всех модулях конкретного процесса*/
		PrintModuleList(peProcessEntry.th32ProcessID);
	} while(Process32Next(hSnapshot, &peProcessEntry)); //получаем следующий процесс из слепка

	CloseHandle(hSnapshot);
}

int main() {
	setlocale(LC_ALL, "");
	PrintProcessList();

	ExitProcess(0);
}