#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <clocale>
#include "Tm1api.h" //предварительно требуется Windows.h
#include <iostream>
#include <cstring>

#define NAME_LEN 128 //максиммальная длина наименования

TM1_INDEX getLastError(TM1U hUser, TM1V val, bool isShow=false);

TM1_INDEX getCountObjects  (TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType);
void      showObjects      (TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, char * str);
TM1V      getObjectByIndex (TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1_INDEX i);
TM1V      getObjectByName  (TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, char *strName, TM1V & vName, TM1_INDEX NameLen=0);
TM1V      getObjectByName  (TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V vName);
TM1V      getObjectByName  (TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, double val, TM1V & vVal);

TM1V      getObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType);
bool      setObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V val);

TM1V	  registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, char *strName, TM1V & vName, TM1_INDEX NameLen = 0);
TM1V	  registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, TM1V vName);

TM1V	  duplicateObject(TM1U hUser, TM1P hPool, TM1V hObject);

bool	  deleteObject(TM1U hUser, TM1P hPool, TM1V hObject);

TM1V      makeArray(TM1U hUser, TM1P hPool, TM1_INDEX arraySize, TM1V* initArray);

#endif