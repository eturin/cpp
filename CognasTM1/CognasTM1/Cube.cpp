#define _CRT_SECURE_NO_WARNINGS
#include "Cube.h"
#include "Dimension.h"

Cube::Cube(const Server &server, char * CubeName, TM1_INDEX CubeNameLen) :server(server){
	hUser = server.gethUser();

	//������� ���� �������� 
	hPool = TM1ValPoolCreate(hUser);
	
	//�������� ����� ����
	if (CubeName != nullptr) 
		hCube = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), CubeName, vCubeName, CubeNameLen);			
}
//Cube::Cube(const Server &server, char * CubeName, TM1_INDEX CubeNameLen, std::queue<std::string> &qDimensions) :server(server) {
//	hUser = server.gethUser();
//
//	//������� ���� �������� 
//	hPool = TM1ValPoolCreate(hUser);
//
//	//�������� ����� ���� (�� ������, ���� �� ��� ����������)
//	hCube = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), CubeName, vCubeName, CubeNameLen);
//	if (hCube == nullptr) {
//		//������ ���� �� ����� 2-� ���������
//		if (qDimensions.size() >= 2) {
//			//������� ������ ��������� ������������ ����
//			TM1V tmp[20] = { 0 };
//			TM1V vArrayOfDimensions = makeArray(hUser, hPool, qDimensions.size(), tmp);
//			if(vArrayOfDimensions ==nullptr)
//				throw std::exception("��� �� ����� ���� ������ (�� ������� ������� ������)");
//			
//			//��������� ������
//			TM1_INDEX i = 1;
//			do{
//				const std::string strName = qDimensions.front();
//				qDimensions.pop();
//				//�������� ����� � ���� ��� ��������				
//				TM1V vName, hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerDimensions(), const_cast<char*>(strName.c_str()), vName);
//				if(hDimension==nullptr)
//					throw std::exception("����������� ���������");
//				//���������� �������� � ������	
//				TM1ValArraySet(vArrayOfDimensions, hDimension, i++);
//			} while (!qDimensions.empty());
//
//			//������� �������
//			hCube = TM1CubeCreate(hPool, server.gethServer(), vArrayOfDimensions);
//			if (TM1ValType(hUser, hCube) == TM1ValTypeError()) {
//				getLastError(hCube, true);
//				hCube = nullptr;
//				this->~Cube();
//				throw std::exception("��� �� ����� ���� ������");
//			}
//			//� ����������������
//			hCube = registerObject(hUser, hPool, server.gethServer(), hCube, vCubeName);
//		}else 
//			throw std::exception("��� �������� ���� ��������� �� ����� ���� ���������");		
//	}
//}

Cube::~Cube(){
	//��� ������� ���� ������� �������� ���� �����
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

char * Cube::getCubeName()const{	
	return TM1ValStringGet(hUser, vCubeName);
}

bool Cube::deleteCube() {
	return deleteObject(hUser, hPool, hCube);
}

const Server& Cube::getServer() const {
	return server;
}

bool Cube::exist() {
	if(vCubeName!=nullptr)
		hCube = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), vCubeName);
	return hCube != nullptr;
}

bool Cube::makeNew() {
	if (hCube == nullptr) {
		//������ ���� �� ����� 2-� ���������
		TM1_INDEX cnt = Dimensions.size();
		if (cnt >= 2) {
			//������� ������ ��������� ������������ ����
			TM1V tmp[20] = { 0 };
			TM1V vArrayOfDimensions = makeArray(hUser, hPool, cnt, tmp);
			if (vArrayOfDimensions == nullptr)
				return false;

			//��������� ������
			for (TM1_INDEX i = 0; i < cnt;++i) {
				const std::string strName = Dimensions[i];
				//�������� ����� � ���� ��� ��������				
				TM1V vName, hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerDimensions(), const_cast<char*>(strName.c_str()), vName);				
				if (hDimension == nullptr)
					throw std::exception("����������� ���������");				
				//���������� �������� � ������	
				TM1ValArraySet(vArrayOfDimensions, hDimension, i+1);
			} 

			//������� �������
			hNewCube = TM1CubeCreate(hPool, server.gethServer(), vArrayOfDimensions);
			if (TM1ValType(hUser, hNewCube) == TM1ValTypeError()) {
				getLastError(hNewCube, true);
				hNewCube = nullptr;
				return false;
			}

			return true;
		}
	}
	return false;
}

bool Cube::makeDuplicate() {
	if (hCube != nullptr) {
		hNewCube = duplicateObject(hUser, hPool, hCube);
		if (hNewCube != nullptr)			
			return true;		
	}
	return false;
}

void Cube::addDimension(char * DimensionName, TM1_INDEX DimensionNameLen) {
	Dimensions.push_back(std::string(DimensionName, DimensionName + DimensionNameLen));
}

bool Cube::registerCube(char * CubeName, TM1_INDEX CubeNameLen) {
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

void Cube::showDimensions()const{
	showObjects(hUser, hPool, hCube, TM1CubeDimensions(), "���������");
}

TM1V Cube::getDimensionByIndex(TM1_INDEX i)const{
	return getObjectByIndex(hUser, hPool, hCube, TM1CubeDimensions(), i);	
}

TM1V Cube::getDimensionByName(char *NameDimension)const{
	TM1V vName;
	return getObjectByName(hUser, hPool, hCube, TM1CubeDimensions(), NameDimension, vName);
}

TM1_INDEX Cube::getCountViews()const{
	return getCountObjects(hUser, hPool, hCube, TM1CubeViews());
}

void Cube::showViews()const{
	showObjects(hUser, hPool, hCube, TM1CubeViews(), "�������������");
}

TM1V Cube::getViewByIndex(TM1_INDEX i)const{
	return getObjectByIndex(hUser, hPool, hCube, TM1CubeViews(), i);
}

TM1V Cube::getViewByName(char *NameView)const{
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
	TM1V vArrayElements = makeArray(hUser, hPool, Dimensions.size(), tmp);
	if (vArrayElements==nullptr)
		return nullptr; //������ ������� �� �������	

	//���������� �������
	for (TM1_INDEX i=0, cnt = getCountDimensions(); i < cnt; ++i)
		TM1ValArraySet(vArrayElements, Dimensions[i]->getElement(), i + 1);
	//�������� ����� � ���� ��� ��������
	TM1V hVal = TM1CubeCellValueGet(hPool, hCube, vArrayElements);
	if (TM1ValType(hUser, hVal) == TM1ValTypeError()){
		getLastError(hVal, true);
		return nullptr;
	}
	return hVal;
}

TM1V Cube::setCellValue(const std::vector<Dimension*> & Dimensions, double val)const{
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, Dimensions.size(), tmp);
	if (vArrayElements == nullptr)
		return nullptr; //������ ������� �� �������	

	//���������� �������
	for (TM1_INDEX i=0, cnt = getCountDimensions(); i < cnt; ++i)
		if(Dimensions[i]!=nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->getElement(), i+1);
		

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
		getLastError(hNewVal, true);
		return nullptr;
	}
	return hNewVal;	
}

TM1V Cube::setCellValue(const std::vector<Dimension*> & Dimensions, char * val)const {
	//�������� ����� � ���� ��� ��������
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, Dimensions.size(), tmp);
	if (vArrayElements == nullptr)
		return nullptr; //������ ������� �� �������	

	//���������� �������
	for (TM1_INDEX i = 0, cnt = getCountDimensions(); i < cnt; ++i)
		if (Dimensions[i] != nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->getElement(), i + 1);
		

	//�������� ����� � ���� ��� ��������
	TM1V vVal = TM1ValString(hPool, val, 0);

	//�������� ����� � ���� ��� ��������
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hCube, vArrayElements, vVal);
	//��������� ���������� ������
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()) {
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			return nullptr;
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()) {
		getLastError(hNewVal, true);
		return nullptr;
	}
	return hNewVal;
}

TM1P Cube::gethPool()const{
	return hPool;
}

TM1U Cube::gethUser()const{
	return hUser;
}

TM1V Cube::gethCube()const{
	return hCube;
}

TM1_INDEX Cube::getLastError(TM1V val, bool isShow)const{
	return ::getLastError(hUser, val, isShow);
}
