#include <windows.h>
#include <locale.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

DWORD show_err(LPCTSTR msg) {
	LPTSTR str_err;
	DWORD no = GetLastError();	
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
//					| FORMAT_MESSAGE_FROM_HMODULE
					| FORMAT_MESSAGE_ALLOCATE_BUFFER
					| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), &str_err, 0, NULL)) {
		_tprintf(_TEXT("%s: %d\n%s\n"), msg, no, str_err);
		LocalFree(str_err);
	} else
		_tprintf(_TEXT("%s:\nномер ошибки %d\n"), msg, no);	

	return no;
}

DWORD proc(LPCTSTR dir, DWORD * pn) {
	TCHAR path[MAX_PATH] = {0};
	if(S_OK != StringCchCopy(path, MAX_PATH, dir)) 
		return show_err(_TEXT("Не удалось сформировать путь к каталогу"));
		
	if(path[_tcslen(path)] != _T('\\') && S_OK != StringCchCat(path, MAX_PATH, _TEXT("\\*"))) 
		return show_err(_TEXT("Не удалось сформировать путь с маской"));
	else if(S_OK != StringCchCat(path, MAX_PATH, _TEXT("*")))
		return show_err(_TEXT("Не удалось сформировать путь с маской"));

	/*фрагмент пути без маски*/
	DWORD len = _tcslen(path)-sizeof(TCHAR);
	
	/*найдем первый файл*/
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(path, &ffd);
	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			if(!_tcscmp(ffd.cFileName, _TEXT(".")))
				continue;
			if(!_tcscmp(ffd.cFileName, _TEXT(".."))) 
				continue;
			
			/*исходное имя файла*/
			_tprintf(_TEXT("%s -->"), ffd.cFileName);
			TCHAR path_to_file_from[MAX_PATH] = {0};
			if(S_OK != StringCchCopyN(path_to_file_from, MAX_PATH, path, len)) {
				show_err(_TEXT("Не удалось сформировать полный путь к исходному файлу"));
				continue;
			}
			if(S_OK != StringCchCat(path_to_file_from, MAX_PATH, ffd.cFileName)) {
				show_err(_TEXT("Не удалось сформировать полный путь к исходному файлу"));
				continue;
			}
			/*вложенная директория*/
			if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				proc(path_to_file_from, pn);
			} else {
				/*формируем новое имя*/
				TCHAR path_to_file_to[MAX_PATH] = {0};
				if(S_OK != StringCchCopyN(path_to_file_to, MAX_PATH, path, len)) {
					show_err(_TEXT("Не удалось сформировать новое имя файла"));
					continue;
				}
				TCHAR tmp[MAX_PATH];
				_sntprintf_s(tmp, MAX_PATH, _TRUNCATE, _TEXT("(%d) "), *pn);
				if(S_OK != StringCchCat(path_to_file_to, MAX_PATH, tmp)) {
					show_err(_TEXT("Не удалось сформировать новое имя файла"));
					continue;
				}
				if(S_OK != StringCchCat(path_to_file_to, MAX_PATH, ffd.cFileName)) {
					show_err(_TEXT("Не удалось сформировать новое имя файла"));
					continue;
				}

				/*переименовываем файл*/
				if(!MoveFile(path_to_file_from, path_to_file_to)) {
					show_err(_TEXT("Не удалось переименовать файла"));
					continue;
				}

				++*pn;
			}
			
		} while(FindNextFile(hFind, &ffd));
		FindClose(hFind);
	}

	return S_OK;
}

int main() {
	/*локализация*/
	setlocale(LC_ALL, "rus");

	/*путь к каталогу и маска поиска файлов*/
	LPTSTR dir = _TEXT("e:\\myCert");	
	
	DWORD n = 0;
	proc(dir, &n);
	
	system("pause");
	return 0;
}