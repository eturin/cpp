#define _CRT_SECURE_NO_WARNINGS
#include "Dimension.h"
#include "Subset.h"

Dimension::Dimension(const Cube &cube, const char * DimensionName, TM1_INDEX DimensionNameLen):Object(cube),server(cube.getServer()){
	//�������� ���������� ���������
	hObject = utilities::getObjectByName(hUser, hPool, cube.gethObject(), TM1CubeDimensions(), DimensionName, vName, DimensionNameLen);
	if (hObject == nullptr){
		this->~Dimension();
		std::ostringstream sout;
		sout << "��������� � ������ " << DimensionName << " �� ������� � ����� " << cube.getName() << std::endl;
		throw std::exception(sout.str().c_str());
	}	
}
Dimension::Dimension(const Cube &cube, TM1_INDEX i) :Object(cube), server(cube.getServer()) {
	//�������� ���������� ���������
	if (0 < i && i <= cube.getCountDimensions()) 
		hObject = utilities::getObjectByIndex(hUser, hPool, cube.gethObject(), TM1CubeDimensions(), i);
	else{
		this->~Dimension();
		std::ostringstream sout;
		sout << "��������� � �������� " << i << " �� ������� � ����� " << cube.getName() << std::endl;
		throw std::exception(sout.str().c_str());
	}	
}
Dimension::Dimension(const Server & server, const char * DimensionName, TM1_INDEX DimensionNameLen) noexcept :Object(server), server(server)  {
	//�������� ���������� ���������
	hObject = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerDimensions(), DimensionName, vName, DimensionNameLen);
}

bool Dimension::exist() noexcept {
	if (vName != nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1CubeDimensions(), vName);
	return hObject != nullptr;
}

bool Dimension::addElement(const char * ElementName, TM1_INDEX ElementNameLen) {
	if (hNewObject == nullptr) 
		throw std::exception("������� �������� ������ � ����� ���������.");
	
	//�������� ����� � ���� ��� ��������
	TM1V vName = TM1ValString(hPool, const_cast<char*>(ElementName), ElementNameLen);
				
	TM1V hElement=TM1DimensionElementInsert(hPool, hNewObject, TM1ObjectNull(), vName, TM1TypeElementSimple());
	//��������� ���������� ���������� ��������
	if (TM1ValType(hUser, hElement) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "�� ������� �������� ������� " << ElementName << " � ������ ���������:\n\t";
		getLastError(sout, hElement, true);
		throw std::exception(sout.str().c_str());
	}

	return true;	
}

bool Dimension::check()const {
	if (hNewObject == nullptr) 
		throw std::exception("����� ������ ��������� �� ���������������.");
	
	TM1V vBool = TM1DimensionCheck(hPool, hNewObject);
	//��������� ���������
	if (TM1ValType(hUser, vBool) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "��������� ������ �� ���������:\n\t";
		getLastError(sout,vBool, true);
		throw std::exception(sout.str().c_str());
	}else if (TM1ValType(hUser, vBool) == TM1ValTypeBool()
		&& TM1ValBoolGet(hUser,vBool)!=1) {
		throw std::exception("��������� ������ �� ���������.");
	}
	
	return true;	
}

bool Dimension::registerDimension(const char * DimensionName, TM1_INDEX DimensionNameLen) {
	if (hNewObject == nullptr)
		throw std::exception("����� ������ ��������� �� ���������������.");
	else if (hObject != nullptr
			&& (DimensionName==nullptr 
				|| DimensionName !=nullptr && std::strncmp(TM1ValStringGet(hUser,vName), DimensionName, DimensionNameLen ? DimensionNameLen: std::strlen(DimensionName))==0)){
		//������ ����������, � �� ������� ������
		TM1V hObject =TM1DimensionUpdate(hPool, this->hObject, hNewObject);
		if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "�� ������� �������� �������������� ������ ��������� "<<getName()<<"\n\t";
			getLastError(sout, hObject, true);
			throw std::exception(sout.str().c_str());
		}
		this->hObject = hObject;
		return true;
	}else if (DimensionName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, server.gethObject(), hNewObject, DimensionName, vName, DimensionNameLen);
		this->hObject = hObject;
	}else if (vName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, server.gethObject(), hNewObject, vName);
		this->hObject = hObject;
	}
		
	return true;
}

bool Dimension::setElementVal(const char * strVal, TM1_INDEX strValLen){
	if(hObject==nullptr)
		throw std::exception("������� �������� �������� ����� ������ � �������������� ����������");
		
	hElement = getListItemByName(TM1DimensionElements(), strVal, vElement, strValLen);
	return hElement != nullptr;
}

bool Dimension::setElementVal(double val) {
	if (hObject == nullptr)
		throw std::exception("������� �������� �������� ����� ������ � �������������� ����������");
	vElement = TM1ValReal(hPool, val);
	hElement = getListItemByName(vElement, TM1DimensionElements());
	
	return hElement != nullptr;
}




