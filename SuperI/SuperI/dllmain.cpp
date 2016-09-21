// dllmain.cpp : Defines the entry point for the DLL application.
#include "Base.h"
#include "class.hpp"

#ifndef __linux__
#include <windows.h>
#endif //__linux__
#ifndef __linux__
BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  ) {
	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
#endif //__linux__

/*ПРОЧИЕ МЕТОДЫ*/
/*Конвертация wchar_t* -> WCHAR_T* */
size_t wchar_to_WCHAR(WCHAR_T * &Dest, const wchar_t* Source, size_t len) {
	if(len == 0) {//если размер задан, то и место уже зарезервировано
		len = wchar_len(Source) + 1;
		Dest = new WCHAR_T[len];
	}
	memset(Dest, 0, len*sizeof(WCHAR_T));	
	for(size_t i = 0; i < len && Source[i]; ++i)
		Dest[i] = (WCHAR_T)Source[i];			

	return len;
}
/*Конвертация wchar_t* -> char* */
size_t wchar_to_char(char * &Dest, const wchar_t* Source, size_t len) {
	/*if(len == 0) {//если размер задан, то и место уже зарезервировано
		len = wchar_len(Source) +1;
		Dest = new char[len];
	}
	wcstombs(Dest, Source, len);
	Dest[len - 1] = '\0';*/
	
	if(len == 0) {
		len = WideCharToMultiByte(
			1251,   // Code page
			0,      // Default replacement of illegal chars
			Source, // Multibyte characters string
			-1,     // Number of unicode chars is not known
			NULL,   // No buffer yet, allocate it later
			0,      // No buffer
			NULL,   // Use system default
			NULL    // We are not interested whether the default char was used
			);
		if(len == 0)
			return 0;
		else
			Dest = new char[len]; 
	}

	len = WideCharToMultiByte(
		1251,    // Code page
		0,       // Default replacement of illegal chars
		Source,  // Multibyte characters string
		-1,      // Number of unicode chars is not known
		Dest,    // Output buffer
		len,     // buffer size
		NULL,    // Use system default
		NULL     // We are not interested whether the default char was used
		);

	if(len == 0) {
		delete[] Dest;
		return 0;
	}

	return len;
}
/*Конвертация WCHAR_T** -> wchar_t* */
size_t WCHAR_to_wchar(wchar_t * &Dest, const WCHAR_T* Source, size_t len) {
	if(len==0){//если размер задан, то и место уже зарезервировано
		len = WCHAR_len(Source) + 1;
		Dest = new wchar_t[len];
	}
	memset(Dest, 0, len*sizeof(wchar_t));
	for(size_t i = 0; i < len && Source[i]; ++i)
		Dest[i] = (wchar_t)Source[i];

	return len;
}
/*Конвертация WCHAR_T** -> char* */
size_t WCHAR_to_char(char * &Dest, const WCHAR_T* Source, size_t len) {
	wchar_t * temp;
	WCHAR_to_wchar(temp, Source);
	len=wchar_to_char(Dest, temp,len);
	delete[] temp;

	return len;
}
/*Вычисление длинны строки WCHAR_T* */
size_t WCHAR_len(const WCHAR_T* Source) {
	size_t res = 0;
	while(Source[res]) 	++res;

	return res;
}
/*Вычисление длинны строки wchar_t* */
size_t wchar_len(const wchar_t* Source) {
	size_t res = 0;
	while(Source[res]) 	++res;

	return res;
}


/*Список доступных типов*/
static const wchar_t Class_Names[] = L"myClass"; //|OtherClass1|OtherClass2

/*ЭКСПОРТИРУЕМЫЕ МЕТОДЫ*/
/*Получение экземпляра по имени (регистрируются в RegisterExtensionAs тем же именем)*/
long GetClassObject(const WCHAR_T* ex_name, Base** pInterface) {
	if(!*pInterface) {
		wchar_t * name = nullptr;
		WCHAR_to_wchar(name, ex_name);
		if(!wcscmp(name, L"myClass"))
			*pInterface = new myClass();
		delete[] name;
		return (long)*pInterface;
	}
	return 0;
}
/*Уничтожение экземпляра*/
long DestroyObject(Base** pIntf) {
	if(!*pIntf)
		return -1;

	delete *pIntf;
	*pIntf = 0;
	return 0;
}
/*Получение списка возможных типов*/
const WCHAR_T* GetClassNames() {
	static WCHAR_T* names;
	wchar_to_WCHAR(names, Class_Names);

	return names;
}