#define _CRT_SECURE_NO_WARNINGS
#include "Cube.h"
#include "Dimension.h"

Cube::Cube(const Server &server, const char * CubeName, TM1_INDEX CubeNameLen, bool isPublic) :Object(server),server(server){
	//�������� ����� ����
	if (CubeName != nullptr) 
		hObject = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerCubes(), CubeName, vName, CubeNameLen, isPublic);
}
Cube::Cube(const Server &server, TM1_INDEX i, bool isPublic) : Object(server), server(server) {
	if (0 < i && i <= server.getCountCubes())
		hObject = utilities::getObjectByIndex(hUser, hPool, server.gethObject(), TM1ServerCubes(), i, isPublic);
	else
		throw std::exception("�� ������� ��� ���� � ����� �������.");
}

bool Cube::exist(bool isPublic) noexcept {
	if(vName!=nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerCubes(), vName, isPublic);
	return hObject != nullptr;
}

bool Cube::makeNew() {
	//������ ���� �� ����� 2-� ���������
	TM1_INDEX cnt = (TM1_INDEX)Dimensions.size();
	if (cnt >= 2) {
		//������� ������ ��������� ������������ ����
		TM1V tmp[20] = { 0 };
		TM1V vArrayOfDimensions = utilities::makeArray(hUser, hPool, cnt, tmp);
			
		//��������� ������
		for (TM1_INDEX i = 0; i < cnt;++i) {
			const std::string strName = Dimensions[i];
			//�������� ����� � ���� ��� ��������				
			TM1V vName, hDimension = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerDimensions(), strName.c_str(), vName);
			if (hDimension == nullptr) {
				std::ostringstream sout;
				sout << "�� ������� " << server.getName() << " ����������� ��������� � ������ " << strName << "\n";
				throw std::exception(sout.str().c_str());
			}
			//���������� �������� � ������	
			TM1ValArraySet(vArrayOfDimensions, hDimension, i+1);
		} 

		//������� �������
		hNewObject = TM1CubeCreate(hPool, server.gethObject(), vArrayOfDimensions);
		if (TM1ValType(hUser, hNewObject) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "�� ������� ������� ����� ��� ����� ������������:\n\t";
			getLastError(sout, hNewObject, true);
			hNewObject = nullptr;
			throw std::exception(sout.str().c_str());
		}
		
		return true;
	}else 
		throw std::exception("� ���� ������ ���� �� ����� ���� ���������");
}

void Cube::addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen) {
	Dimensions.push_back(std::string(DimensionName, DimensionName + (DimensionNameLen ? DimensionNameLen : std::strlen(DimensionName))));
}

bool Cube::registerCube(bool isPublic, const char * CubeName, TM1_INDEX CubeNameLen) {
	if (hNewObject == nullptr)
		return false; //����� ��� ��� ��� ��������� �� ����������������
	else if (CubeName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, server.gethObject(), hNewObject, isPublic, CubeName, vName, CubeNameLen);
		if (hObject != nullptr)
			this->hObject = hObject;
		else
			return false; //�� ������� ���������������� ���
	}else if (vName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, server.gethObject(), hNewObject, isPublic, vName);
		if (hObject != nullptr)
			this->hObject = hObject;
		else
			return false; //�� ������� ���������������� ���
	}
	return true;
}


TM1V Cube::getCellValue(const std::vector<Dimension*> & Dimensions)const{
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = utilities::makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//���������� �������
	for (TM1_INDEX i=0, cnt = getCountDimensions(); i < cnt; ++i)
		TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	//�������� ����� � ���� ��� ��������
	TM1V hVal = TM1CubeCellValueGet(hPool, hObject, vArrayElements);
	if (TM1ValType(hUser, hVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "�� ������� �������� �������� ������:\n\t";
		getLastError(sout, hVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hVal;
}

TM1V Cube::setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, double val)const{
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = utilities::makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//���������� �������
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i=0; i < cnt; ++i)
		if(Dimensions[i]!=nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i+1);
		

	//�������� ����� � ���� ��� ��������
	TM1V vVal = TM1ValReal(hPool, val);

	//�������� ����� � ���� ��� ��������
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hObject, vArrayElements, vVal);
	//��������� ���������� ������
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()){
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			return nullptr;
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "� ���� "<<getName()<<" �� ������� ���������� �������� ������: ";
		for (TM1_INDEX i = 0; i < cnt; ++i)
			if (Dimensions[i] != nullptr) {
				/*sout << Dimensions[i]->getElementVal() << " "*/;
			}else
				sout << "null ";
		sout << val<<"\n\t";
		getLastError(sout, hNewVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hNewVal;	
}

TM1V Cube::setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, const char * val, TM1_INDEX valLen)const {
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = utilities::makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//���������� �������
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i = 0; i < cnt; ++i)
		if (Dimensions[i] != nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	
	//�������� ����� � ���� ��� ��������
	TM1V vVal = TM1ValString(hPool, const_cast<char*>(val), valLen);
	
	//�������� ����� � ���� ��� ��������
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hObject, vArrayElements, vVal);
	//��������� ���������� ������
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()) {
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			throw std::exception("�� ������� ���������� �������� ������");
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "� ���� " << getName() << " �� ������� ���������� �������� ������: ";
		for (TM1_INDEX i = 0; i < cnt; ++i)
			if (Dimensions[i] != nullptr) 
				/*sout << Dimensions[i]->getElementVal() << " "*/;
			else
				sout << "null ";
		sout << val << "\n\t";
		getLastError(sout, hNewVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hNewVal;
}


