#define _CRT_SECURE_NO_WARNINGS
#include "Dimension.h"

Dimension::Dimension(const Cube &cube, const char * DimensionName, TM1_INDEX DimensionNameLen):server(cube.getServer()){
	hUser = cube.gethUser();

	//������� ���� �������� 
	hPool = cube.gethPool();

	//�������� ���������� ���������
	hDimension = getObjectByName(hUser, hPool, cube.gethCube(), TM1CubeDimensions(), DimensionName, vDimensionName, DimensionNameLen);
	if (hDimension == nullptr){
		hDimension = nullptr;
		this->~Dimension();
		std::ostringstream sout;
		sout << "��������� � ������ " << DimensionName << " �� ������� � ����� " << cube.getCubeName() << std::endl;
		throw std::exception(sout.str().c_str());
	}	
}
Dimension::Dimension(const Cube &cube, TM1_INDEX i) : server(cube.getServer()) {
	hUser = cube.gethUser();

	//������� ���� �������� 
	hPool = cube.gethPool();

	//�������� ���������� ���������
	if (0 < i && i <= cube.getCountDimensions()) 
		hDimension = getObjectByIndex(hUser, hPool, cube.gethCube(), TM1CubeDimensions(), i);
	else{
		this->~Dimension();
		std::ostringstream sout;
		sout << "��������� � �������� " << i << " �� ������� � ����� " << cube.getCubeName() << std::endl;
		throw std::exception(sout.str().c_str());
	}	
}
Dimension::Dimension(const Server & server, const char * DimensionName, TM1_INDEX DimensionNameLen) noexcept :server(server)  {
	hUser = server.gethUser();

	//������� ���� �������� 
	hPool = server.gethPool();

	//�������� ���������� ���������
	hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerDimensions(), DimensionName, vDimensionName, DimensionNameLen);
}

Dimension::~Dimension() noexcept {
	//��� ������� ���� ������� �������� ���� �����
	//TM1ValPoolDestroy(hPool);
	//hPool = nullptr;
}

const char * Dimension::getDimensionName()const {
	if (vDimensionName == nullptr)
		vDimensionName = getObjectProperty(hUser, hPool, hDimension, TM1ObjectName());

	return vDimensionName != nullptr ? TM1ValStringGet(hUser, vDimensionName) : nullptr;
}

bool Dimension::deleteDimension() noexcept {
	return deleteObject(hUser, hPool, hDimension);
}

TM1_INDEX Dimension::getCountSubsets()const{
	return getCountObjects(hUser, hPool, hDimension, TM1DimensionSubsets());
}

std::string Dimension::showSubsets()const{
	return showObjects(hUser, hPool, hDimension, TM1DimensionSubsets(), getDimensionName());
}

TM1V Dimension::getSubsetByIndex(TM1_INDEX i)const noexcept {
	return getObjectByIndex(hUser, hPool, hDimension, TM1DimensionSubsets(), i);
}

TM1V Dimension::getSubsetByName(const char *NameSubset)const noexcept {
	TM1V vName;
	return getObjectByName(hUser, hPool, hDimension, TM1DimensionSubsets(), NameSubset, vName);
}

TM1_INDEX Dimension::getCountElements()const{
	return getCountObjects(hUser, hPool, hDimension, TM1DimensionElements());
}

std::string Dimension::showElements()const{
	return showObjects(hUser, hPool, hDimension, TM1DimensionElements(), getDimensionName());
}

TM1V Dimension::getElementByIndex(TM1_INDEX i)const noexcept {
	return getObjectByIndex(hUser, hPool, hDimension, TM1DimensionElements(), i);
}

TM1V Dimension::getElementByName(const char *NameElement)const noexcept {
	TM1V vName;
	return getObjectByName(hUser, hPool, hDimension, TM1DimensionElements(), const_cast<char*>(NameElement), vName);
}

bool Dimension::isRead()const noexcept {
	return bool(TM1ValObjectCanRead(hUser, hDimension));
}

bool Dimension::isWrite()const noexcept {
	return bool(TM1ValObjectCanWrite(hUser, hDimension));
}

bool Dimension::exist() noexcept {
	if (vDimensionName != nullptr)
		hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), vDimensionName);
	return hDimension != nullptr;
}

void Dimension::makeNew() noexcept {
	//������� ������ ��������� �� �������
	hNewDimension = TM1DimensionCreateEmpty(hPool, server.gethServer());
}

bool Dimension::makeDuplicate() {
	if (hDimension != nullptr) {
		hNewDimension = duplicateObject(hUser, hPool, hDimension);
		if (hNewDimension != nullptr)
			return true;
	}
	return false;
}

bool Dimension::addElement(const char * ElementName, TM1_INDEX ElementNameLen) {
	if (hNewDimension == nullptr) 
		throw std::exception("������� ����� ��������� � ���������� ���������.");
	
	//�������� ����� � ���� ��� ��������
	TM1V vName = TM1ValString(hPool, const_cast<char*>(ElementName), ElementNameLen);
	if (vName == 0)
		throw std::exception("�� ������� �������� ��� �������� � ����");
			
	TM1V hElement=TM1DimensionElementInsert(hPool, hNewDimension, TM1ObjectNull(), vName, TM1TypeElementSimple());
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
	if (hNewDimension == nullptr) 
		throw std::exception("��������� ��������� �� ����������������.");
	
	TM1V vBool = TM1DimensionCheck(hPool, hNewDimension);
	//��������� ���������� �������� �������
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
	if (hNewDimension == nullptr)
		throw std::exception("��������� ��������� �� ����������������.");
	else if (hDimension != nullptr
			&& (DimensionName==nullptr 
				|| DimensionName !=nullptr && std::strncpy(TM1ValStringGet(hUser,vDimensionName), DimensionName, DimensionNameLen)==0)){
		//������ ����������, � �� ������� ������
		TM1V hDimension =TM1DimensionUpdate(hPool, this->hDimension, hNewDimension);
		if (TM1ValType(hUser, hDimension) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "�� ������� �������� �������������� ������ ��������� "<<getDimensionName()<<"\n\t";
			getLastError(sout, hDimension, true);
			throw std::exception(sout.str().c_str());
		}
		this->hDimension = hDimension;
		return true;
	}else if (DimensionName != nullptr) {
		TM1V hDimension = registerObject(hUser, hPool, server.gethServer(), hNewDimension, DimensionName, vDimensionName, DimensionNameLen);
		this->hDimension = hDimension;		
	}else if (vDimensionName != nullptr) {
		TM1V hDimension = registerObject(hUser, hPool, server.gethServer(), hNewDimension, vDimensionName);
		this->hDimension = hDimension;	
	}
	
	return true;
}

bool Dimension::setElementVal(const char * strVal){
	if(hDimension==nullptr)
		throw std::exception("������� �������� �������� ����� ������ � �������������� ����������");

	hElement = getObjectByName(hUser, hPool, hDimension, TM1DimensionElements(), strVal, vElement);	
	return hElement != nullptr;
}

bool Dimension::setElementVal(double val) {
	if (hDimension == nullptr)
		throw std::exception("������� �������� �������� ����� ������ � �������������� ����������");

	hElement = getObjectByName(hUser, hPool, hDimension, TM1DimensionElements(), val, vElement);
	return hElement != nullptr;
}

char * Dimension::getElementVal()const noexcept {
	return vElement != nullptr ? TM1ValStringGet(hUser, vElement) : nullptr;
}

TM1V Dimension::gethElement()const noexcept {
	return hElement;
}

TM1_INDEX Dimension::getLastError(std::ostringstream &sout, TM1V val, bool isShow)const noexcept {
	return ::getLastError(sout, hUser, val, isShow);
}

TM1V Dimension::gethDimension()const noexcept {
	return hDimension;
}