#include "Common.h"

TM1_INDEX getLastError(std::ostringstream & sout, TM1U hUser, TM1V val, bool isShow) noexcept {
	TM1_INDEX rc = TM1ValErrorCode(hUser, val);

	if (rc && isShow){
		char * str = TM1ValErrorString(hUser, val);
		sout << "Error[" << rc << "]: " << str << std::endl;
	}

	return rc;
}

TM1_INDEX getCountObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType){
	//�������� ����� � ���� ��� ��������
	TM1V hCount = TM1ObjectListCountGet(hPool, hParrentObject, vType);
	//��������� ���������� ��������� ���-�� ��������
	if (TM1ValType(hUser, hCount) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "�� ������� �������� ���-�� ��������:\n\t";
		getLastError(sout, hUser, hCount, true);		
		throw std::exception(sout.str().c_str());
	}else
		return TM1ValIndexGet(hUser, hCount);
}

std::string showObjects(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, const char * str){
	TM1_INDEX cnt = getCountObjects(hUser, hPool, hParrentObject, vType);
	std::ostringstream sout;	
	for (TM1_INDEX i = 0; i < cnt; ++i){
		TM1V hObject = getObjectByIndex(hUser, hPool, hParrentObject, vType, i + 1);
		//��������� ���������� ��������� ������� �� �������
		if (hObject!=nullptr){			
			TM1V hName = getObjectProperty(hUser, hPool, hObject, TM1ObjectName());
			if (hName)			
				sout << TM1ValStringGet(hUser, hName) << std::endl;
		}
	}
	return sout.str();
}

TM1V getObjectByIndex(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1_INDEX i) noexcept {
	//�������� ����� � ���� ��� ��������
	TM1V ind = TM1ValIndex(hPool, i);
	if (ind == 0) {
		//�� ������� �������� ������ � ���� 
		return nullptr;
	}

	TM1V hObject = TM1ObjectListHandleByIndexGet(hPool, hParrentObject, vType, ind);
	//��������� ���������� ��������� ������� �� �������
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "�� ������� �������� ������ �� �������:" << i << "\n\t";
		getLastError(sout, hUser, hObject, true);
		return nullptr;
	}
	return hObject;
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, const char *strName, TM1V & vName, TM1_INDEX NameLen) noexcept {
	//�������� ����� � ���� ��� ��������
	vName = TM1ValString(hPool, const_cast<char*>(strName), NameLen);
	if (vName == 0) {
		//�� ������� �������� ��� � ����
		return nullptr;
	}
				
	return getObjectByName(hUser,hPool,hParrentObject, vType,vName);
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V vName) noexcept {
	TM1V hObject = TM1ObjectListHandleByNameGet(hPool, hParrentObject, vType, vName);
	//��������� ���������� ��������� ������� �� �����
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� �������� ������ �� �����:" << (vName != nullptr ? TM1ValStringGet(hUser, vName):"") << "\n\t";
		getLastError(sout, hUser, hObject, true);
		return nullptr;
	}

	return hObject;
}

TM1V getObjectByName(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, double val, TM1V & vVal) noexcept {
	//�������� ����� � ���� ��� ��������
	vVal = TM1ValReal(hPool, val);
	if (vVal == 0) {
		//�� ������� �������� ��� � ���� 
		vVal = nullptr;
		return nullptr;
	}

	TM1V hObject = TM1ObjectListHandleByNameGet(hPool, hParrentObject, vType, vVal);
	//��������� ���������� ��������� ������� �� �����
	if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� �������� ������ �� �����:" << val << "\n\t";
		getLastError(sout, hUser, hObject, true);
		return nullptr;
	}

	return hObject;
}

TM1V getObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType){
	//�������� ����� � ���� ��� ��������
	TM1V hProperty = TM1ObjectPropertyGet(hPool, hParrentObject, vType);
	//��������� ���������� ��������� ����������� ��������
	if (TM1ValType(hUser, hProperty) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "�� ������� �������� ��������:\n\t";
		getLastError(sout, hUser, hProperty, true);
		throw std::exception(sout.str().c_str());
	}
	return hProperty;
}

bool setObjectProperty(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V vType, TM1V val){
	TM1V vOk = TM1ObjectPropertySet(hPool, hParrentObject, vType, val);
	//��������� ���������� ��������� �������� ��������
	if (TM1ValType(hUser, vOk) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "�� ������� ���������� ��������:\n\t";
		getLastError(sout, hUser, vOk, true);
		throw std::exception(sout.str().c_str());
	}
	return true;
}

TM1V registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, const char *strName, TM1V & vName, TM1_INDEX NameLen) {
	//�������� ����� � ���� ��� ��������
	vName = TM1ValString(hPool, const_cast<char*>(strName), NameLen);
	if (vName == 0) 
		throw std::exception("�� ������� �������� ��� � ����");	

	return registerObject(hUser, hPool, hParrentObject, hObject, vName);
}

TM1V registerObject(TM1U hUser, TM1P hPool, TM1V hParrentObject, TM1V hObject, TM1V vName) {
	//�������� ����� � ���� ��� ��������
	TM1V hNewObject = TM1ObjectRegister(hPool, hParrentObject, hObject, vName);
	//��������� ���������� ��������� �������� ��������
	if (TM1ValType(hUser, hNewObject) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� ������������ ������:\n\t";
		getLastError(sout, hUser, hNewObject, true);
		throw std::exception(sout.str().c_str());
	}
	return hNewObject;
}

TM1V duplicateObject(TM1U hUser, TM1P hPool, TM1V hObject) {
	//�������� ����� � ���� ��� ��������
	TM1V hDuplicate = TM1ObjectDuplicate(hPool, hObject);
	//��������� ���������� �������� �����
	if (TM1ValType(hUser, hDuplicate) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� ����������� ������:\n\t";
		getLastError(sout, hUser, hDuplicate, true);
		throw std::exception(sout.str().c_str());
	}
	return hDuplicate;
}

bool deleteObject(TM1U hUser, TM1P hPool, TM1V hObject) noexcept {
	//������� ������
	TM1V vBool=TM1ObjectDelete(hPool, hObject);
	//��������� ���������� ��������
	if (TM1ValType(hUser, vBool) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� ������� ������:\n\t";
		getLastError(sout, hUser, vBool, true);
		return false;
	}else 
		return TM1ValBoolGet(hUser, vBool);
}

TM1V makeArray(TM1U hUser, TM1P hPool, TM1_INDEX arraySize, TM1V* initArray) {
	//�������� ����� � ���� ��� ��������
	TM1V vArrayElements = TM1ValArray(hPool, initArray, arraySize);
	//��������� ���������� �������� �������
	if (TM1ValType(hUser, vArrayElements) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� ������� ������ �������:"<< arraySize <<"\n\t";
		getLastError(sout, hUser, vArrayElements, true);
		throw std::exception(sout.str().c_str());
	}else if (TM1ValType(hUser, vArrayElements) == TM1ValTypeBool()) {
		std::ostringstream sout;
		sout << "�� ������� ������� ������ �������:" << arraySize << "\n";
		getLastError(sout, hUser, vArrayElements, true);
		throw std::exception(sout.str().c_str());
	}else
		return vArrayElements;
}