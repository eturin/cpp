#define _CRT_SECURE_NO_WARNINGS
#include "Cube.h"
#include "Dimension.h"

Cube::Cube(const Server &server, const char * CubeName, TM1_INDEX CubeNameLen) :server(server){
	hUser = server.gethUser();

	//������� ���� �������� 
	hPool = server.gethPool();
	
	//�������� ����� ����
	if (CubeName != nullptr) 
		hCube = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), CubeName, vCubeName, CubeNameLen);			
}

Cube::~Cube() noexcept {
	//��� ������� ���� ������� �������� ���� �����
	//TM1ValPoolDestroy(hPool);
	//hPool = nullptr;
}

const char * Cube::getCubeName()const noexcept {
	if (vCubeName != nullptr)
		return TM1ValStringGet(hUser, vCubeName);
	else
		return nullptr;	
}

bool Cube::deleteCube() noexcept {
	if (hCube != nullptr)
		return deleteObject(hUser, hPool, hCube);
	else
		return true;
}

const Server& Cube::getServer() const noexcept {
	return server;
}

bool Cube::exist() noexcept {
	if(vCubeName!=nullptr)
		hCube = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), vCubeName);
	return hCube != nullptr;
}

bool Cube::makeNew() {
	//������ ���� �� ����� 2-� ���������
	TM1_INDEX cnt = (TM1_INDEX)Dimensions.size();
	if (cnt >= 2) {
		//������� ������ ��������� ������������ ����
		TM1V tmp[20] = { 0 };
		TM1V vArrayOfDimensions = makeArray(hUser, hPool, cnt, tmp);
			
		//��������� ������
		for (TM1_INDEX i = 0; i < cnt;++i) {
			const std::string strName = Dimensions[i];
			//�������� ����� � ���� ��� ��������				
			TM1V vName, hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerDimensions(), strName.c_str(), vName);				
			if (hDimension == nullptr) {
				std::ostringstream sout;
				sout << "�� ������� " << server.getServerName() << " ����������� ��������� � ������ " << strName << "\n";
				throw std::exception(sout.str().c_str());
			}
			//���������� �������� � ������	
			TM1ValArraySet(vArrayOfDimensions, hDimension, i+1);
		} 

		//������� �������
		hNewCube = TM1CubeCreate(hPool, server.gethServer(), vArrayOfDimensions);
		if (TM1ValType(hUser, hNewCube) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "�� ������� ������� ����� ��� ����� ������������:\n\t";
			getLastError(sout,hNewCube, true);
			hNewCube = nullptr;
			throw std::exception(sout.str().c_str());
		}
		
		return true;
	}else 
		throw std::exception("� ���� ������ ���� �� ����� ���� ���������");
}

bool Cube::makeDuplicate() {
	if (hCube != nullptr) 
		hNewCube = duplicateObject(hUser, hPool, hCube);			
	
	return false;
}

void Cube::addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen) {
	Dimensions.push_back(std::string(DimensionName, DimensionName + DimensionNameLen));
}

bool Cube::registerCube(const char * CubeName, TM1_INDEX CubeNameLen) {
	if (hNewCube == nullptr)
		return false; //����� ��� ��� ��� ��������� �� ����������������
	else if (CubeName != nullptr) {
		TM1V hCube = registerObject(hUser, hPool, server.gethServer(), hNewCube, CubeName, vCubeName, CubeNameLen);
		if (hCube != nullptr)
			this->hCube = hCube;
		else
			return false; //�� ������� ���������������� ���
	}else if (vCubeName != nullptr) {
		TM1V hCube = registerObject(hUser, hPool, server.gethServer(), hNewCube, vCubeName);
		if (hCube != nullptr)
			this->hCube = hCube;
		else
			return false; //�� ������� ���������������� ���
	}
	return true;
}

TM1_INDEX Cube::getCountDimensions()const{	
	return getCountObjects(hUser,hPool,hCube,TM1CubeDimensions());
}

std::string Cube::showDimensions()const{
	return showObjects(hUser, hPool, hCube, TM1CubeDimensions(), "���������");
}

TM1V Cube::getDimensionByIndex(TM1_INDEX i)const noexcept {
	return getObjectByIndex(hUser, hPool, hCube, TM1CubeDimensions(), i);	
}

TM1V Cube::getDimensionByName(const char *NameDimension)const noexcept {
	TM1V vName;
	return getObjectByName(hUser, hPool, hCube, TM1CubeDimensions(), NameDimension, vName);
}

TM1_INDEX Cube::getCountViews()const{
	return getCountObjects(hUser, hPool, hCube, TM1CubeViews());
}

std::string Cube::showViews()const{
	return showObjects(hUser, hPool, hCube, TM1CubeViews(), "�������������");
}

TM1V Cube::getViewByIndex(TM1_INDEX i)const noexcept {
	return getObjectByIndex(hUser, hPool, hCube, TM1CubeViews(), i);
}

TM1V Cube::getViewByName(const char *NameView)const noexcept {
	TM1V vName;
	return getObjectByName(hUser, hPool, hCube, TM1CubeViews(), NameView, vName);
}

TM1V Cube::getRule(){
	if(vRule==nullptr)
		vRule = getObjectProperty(hUser ,hPool, hCube, TM1CubeRule());
	return vRule;
}

TM1V Cube::getCellValue(const std::vector<Dimension*> & Dimensions)const{
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//���������� �������
	for (TM1_INDEX i=0, cnt = getCountDimensions(); i < cnt; ++i)
		TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	//�������� ����� � ���� ��� ��������
	TM1V hVal = TM1CubeCellValueGet(hPool, hCube, vArrayElements);
	if (TM1ValType(hUser, hVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "�� ������� �������� �������� ������:\n\t";
		getLastError(sout, hVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hVal;
}

TM1V Cube::setCellValue(const std::vector<Dimension*> & Dimensions, double val)const{
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//���������� �������
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i=0; i < cnt; ++i)
		if(Dimensions[i]!=nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i+1);
		

	//�������� ����� � ���� ��� ��������
	TM1V vVal = TM1ValReal(hPool, val);

	//�������� ����� � ���� ��� ��������
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hCube, vArrayElements, vVal);
	//��������� ���������� ������
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()){
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			return nullptr;
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "� ���� "<<getCubeName()<<" �� ������� ���������� �������� ������: ";
		for (TM1_INDEX i = 0; i < cnt; ++i)
			if (Dimensions[i] != nullptr) {
				sout << Dimensions[i]->getElementVal() << " ";
			}else
				sout << "null ";
		sout << val<<"\n\t";
		getLastError(sout, hNewVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hNewVal;	
}

TM1V Cube::setCellValue(const std::vector<Dimension*> & Dimensions, const char * val)const {
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//���������� �������
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i = 0; i < cnt; ++i)
		if (Dimensions[i] != nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	
	//�������� ����� � ���� ��� ��������
	TM1V vVal = TM1ValString(hPool, const_cast<char*>(val), 0);
	if(vVal==0)
		throw std::exception("�� ������� �������� �������� � ����");

	//�������� ����� � ���� ��� ��������
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hCube, vArrayElements, vVal);
	//��������� ���������� ������
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()) {
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			throw std::exception("�� ������� ���������� �������� ������");
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "� ���� " << getCubeName() << " �� ������� ���������� �������� ������: ";
		for (TM1_INDEX i = 0; i < cnt; ++i)
			if (Dimensions[i] != nullptr) 
				sout << Dimensions[i]->getElementVal() << " ";
			else
				sout << "null ";
		sout << val << "\n\t";
		getLastError(sout, hNewVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hNewVal;
}

TM1P Cube::gethPool()const noexcept {
	return hPool;
}

TM1U Cube::gethUser()const noexcept {
	return hUser;
}

TM1V Cube::gethCube()const noexcept {
	return hCube;
}

TM1_INDEX Cube::getLastError(std::ostringstream &sout, TM1V val, bool isShow)const noexcept {
	return ::getLastError(sout, hUser, val, isShow);
}
