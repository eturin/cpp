#ifndef COMMON_H
#define COMMON_H

#define WINVER 0x0601 /*API Windows 7*/
#include <Windows.h>
#include <clocale>


#include "myTm1api.h" //предварительно требуется Windows.h

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

namespace utilities {

#define NAME_LEN 128 //максиммальная длина наименования
	BOOL initDLL();
	BOOL releasDLL();

	wchar_t *CodePageToUnicode(int codePage, const char *src);
	char * UnicodeToCodePage(int codePage, const wchar_t *src);

	TM1_INDEX getLastError(std::ostringstream & sout, TM1U hUser, TM1V val, bool isShow = false) noexcept;

	TM1_INDEX   getCountObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, bool isPublic);
	
	std::string showObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, bool isPublic);	

	TM1V        getObjectByIndex(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1_INDEX i, bool isPublic) noexcept;
	TM1V        getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, const char *strName, TM1V & vName, TM1_INDEX NameLen = 0, bool isPublic=true)noexcept;
	TM1V        getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V vName, bool isPublic) noexcept;
	TM1V        getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, double val, TM1V & vVal, bool isPublic) noexcept;
	
	TM1V        getObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType);
	bool        setObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V val);

	TM1V	    registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, bool isPublic, const char *strName, TM1V & vName, TM1_INDEX NameLen = 0);
	TM1V	    registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, bool isPublic, TM1V vName);
	

	TM1V	    duplicateObject(TM1U hUser, TM1P hPool, TM1V hObject);

	bool	    deleteObject(TM1U hUser, TM1P hPool, TM1V hObject) noexcept;

	TM1V        makeArray(TM1U hUser, TM1P hPool, TM1_INDEX arraySize, TM1V* initArray);
}
#endif