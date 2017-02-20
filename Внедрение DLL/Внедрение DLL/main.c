#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>


INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved) {
	/* open file */
	FILE * file;
	fopen_s(&file, "C:\\Users\\etyurin\\Documents\\Visual Studio 2013\\ProjectsCPP\\Внедрение DLL\\Debug\\temp.txt", "a+");
	SYSTEMTIME sm;
	GetSystemTime(&sm);	
	switch(Reason) {
		case DLL_PROCESS_ATTACH:
			fprintf(file, "%d:%d:%d DLL attach function called.\n", sm.wHour,sm.wMinute,sm.wSecond);
			//fprintf(file, "DLL attach function called.\n");
			MessageBoxW(NULL, _TEXT("Привет"), _TEXT("Как сам?"), MB_OK);
			break;
		case DLL_PROCESS_DETACH:
			fprintf(file, "%d:%d:%d DLL detach function called.\n", sm.wHour, sm.wMinute, sm.wSecond);
			//fprintf(file, "DLL detach function called.\n");
			break;
		case DLL_THREAD_ATTACH:
			fprintf(file, "%d:%d:%d DLL thread attach function called.\n", sm.wHour, sm.wMinute, sm.wSecond);
			//fprintf(file, "DLL thread attach function called.\n");
			MessageBoxW(NULL, _TEXT("Привет"), _TEXT("Как сам?"), MB_OK);
			break;
		case DLL_THREAD_DETACH:
			fprintf(file, "%d:%d:%d DLL thread detach function called.\n", sm.wHour, sm.wMinute, sm.wSecond);
			//fprintf(file, "DLL thread detach function called.\n");
			break;
	}
	/* close file */
	fclose(file);	
	return TRUE;
}