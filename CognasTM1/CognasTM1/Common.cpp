#include "Common.h"

TM1_INDEX getLastError(TM1U hUser, TM1V val, bool isShow){
	TM1_INDEX rc = TM1ValErrorCode(hUser, val);

	if (rc && isShow){
		char * str = TM1ValErrorString(hUser, val);
		std::cerr << "Error[" << rc << "]: " << str << std::endl;
	}

	return rc;
}

TM1_INDEX getCountObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType){
	//�������� ����� � ���� ��� ��������
	TM1V hCount = TM1ObjectListCountGet(hPool, hParrentObject, vType);
	//��������� ���������� ��������� ���-�� ��������
	if (TM1ValType(hUser, hCount) == TM1ValTypeError()){
		getLastError(hUser, hCount, true);
		return 0;
	}
	else
		return TM1ValIndexGet(hUser, hCount);
}

void showObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, char * str){
	TM1_INDEX cnt = getCountObjects(hUser, hPool, hParrentObject, vType);
	std::cout << "����� ���-�� " << str << " " << cnt << ":" << std::endl;
	for (TM1_INDEX i = 0; i < cnt; ++i){
		TM1V hObject = getObjectByIndex(hUser, hPool, hParrentObject, vType, i + 1);
		//��������� ���������� ��������� ������� �� �������
		if (TM1ValType(hUser, hObject) == TM1ValTypeError()){
			getLastError(hUser, hObject, true);			
		}else{
			TM1V hName = getObjectProperty(hUser, hPool, hObject, TM1ObjectName());
			if (hName)			
				std::cout << TM1ValStringGet(hUser, hName) << std::endl;
		}
	}
}

TM1V getObjectByIndex(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1_INDEX i){
	//�������� ����� � ���� ��� ��������
	TM1V ind = TM1ValIndex(hPool, i);
	TM1V hObject = TM1ObjectListHandleByIndexGet(hPool, hParrentObject, vType, ind);
	//��������� ���������� ��������� ������� �� �������
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()){
		getLastError(hUser, hObject, true);
		return nullptr;
	}
	return hObject;
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, char *strName, TM1V & vName, TM1_INDEX NameLen){
	//�������� ����� � ���� ��� ��������
	vName = TM1ValString(hPool, strName, NameLen);
				
	return getObjectByName(hUser,hPool,hParrentObject, vType,vName);
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V vName) {
	TM1V hObject = TM1ObjectListHandleByNameGet(hPool, hParrentObject, vType, vName);
	//��������� ���������� ��������� ������� �� �����
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
		getLastError(hUser, hObject, true);
		return nullptr;
	}

	return hObject;
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, double val, TM1V & vVal) {
	//�������� ����� � ���� ��� ��������
	vVal = TM1ValReal(hPool, val);

	TM1V hObject = TM1ObjectListHandleByNameGet(hPool, hParrentObject, vType, vVal);
	//��������� ���������� ��������� ������� �� �����
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
		getLastError(hUser, hObject, true);
		return nullptr;
	}

	return hObject;
}

TM1V getObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType){
	//�������� ����� � ���� ��� ��������
	TM1V hProperty = TM1ObjectPropertyGet(hPool, hParrentObject, vType);
	//��������� ���������� ��������� ����������� ��������
	if (TM1ValType(hUser, hProperty) == TM1ValTypeError()){
		getLastError(hUser, hProperty, true);
		return nullptr;
	}
	return hProperty;
}

bool setObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V val){
	TM1V vOk = TM1ObjectPropertySet(hPool, hParrentObject, vType, val);
	//��������� ���������� ��������� �������� ��������
	if (TM1ValType(hUser, vOk) == TM1ValTypeError()){
		getLastError(hUser, vOk, true);
		return false;
	}
	return true;
}

TM1V registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, char *strName, TM1V & vName, TM1_INDEX NameLen) {
	//�������� ����� � ���� ��� ��������
	vName = TM1ValString(hPool, strName, NameLen);

	return registerObject(hUser, hPool, hParrentObject, hObject, vName);
}

TM1V registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, TM1V vName) {
	//�������� ����� � ���� ��� ��������
	TM1V hNewObject = TM1ObjectRegister(hPool, hParrentObject, hObject, vName);
	//��������� ���������� ��������� �������� ��������
	if (TM1ValType(hUser, hNewObject) == TM1ValTypeError()) {
		getLastError(hUser, hNewObject, true);
		return nullptr;
	}
	return hNewObject;
}

TM1V duplicateObject(TM1U hUser, TM1P hPool, TM1V hObject) {
	//�������� ����� � ���� ��� ��������
	TM1V hDuplicate = TM1ObjectDuplicate(hPool, hObject);	//��������� ���������� �������� �����
	if (TM1ValType(hUser, hDuplicate) == TM1ValTypeError()) {
		getLastError(hUser, hDuplicate, true);
		return nullptr;
	}
	return hDuplicate;
}

bool deleteObject(TM1U hUser, TM1P hPool, TM1V hObject) {
	//������� ������
	TM1V vBool=TM1ObjectDelete(hPool, hObject);
	//��������� ���������� ��������
	if (TM1ValType(hUser, vBool) == TM1ValTypeError()) {
		getLastError(hUser, vBool, true);
		return false;
	}else 
		return TM1ValBoolGet(hUser, vBool);
}

TM1V makeArray(TM1U hUser, TM1P hPool, TM1_INDEX arraySize, TM1V* initArray) {
	//�������� ����� � ���� ��� ��������
	TM1V vArrayElements = TM1ValArray(hPool, initArray, arraySize);
	//��������� ���������� �������� �������
	if (TM1ValType(hUser, vArrayElements) == TM1ValTypeError()) {
		getLastError(hUser, vArrayElements, true);
		return nullptr;
	}else if (TM1ValType(hUser, vArrayElements) == TM1ValTypeBool()) {
		getLastError(hUser, vArrayElements, true);
		return nullptr;
	}else
		return vArrayElements;
}