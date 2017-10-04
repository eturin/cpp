#define _CRT_SECURE_NO_WARNINGS
#include "Dimension.h"

Dimension::Dimension(const Cube &cube, char * DimensionName, TM1_INDEX DimensionNameLen):server(cube.getServer()){
	hUser = cube.gethUser();

	//создаем пулы значений 
	hPool = TM1ValPoolCreate(hUser);

	//получаем дескриптор измерения
	hDimension = getObjectByName(hUser, hPool, cube.gethCube(), TM1CubeDimensions(), DimensionName, vDimensionName, DimensionNameLen);
	if (hDimension == nullptr){
		hDimension = nullptr;
		this->~Dimension();
		throw std::exception("Измерение не связано с кубом");
	}	
}
Dimension::Dimension(const Cube &cube, TM1_INDEX i) : server(cube.getServer()) {
	hUser = cube.gethUser();

	//создаем пулы значений 
	hPool = TM1ValPoolCreate(hUser);

	//получаем дескриптор измерения
	if (0 < i && i <= cube.getCountDimensions()) 
		hDimension = getObjectByIndex(hUser, hPool, cube.gethCube(), TM1CubeDimensions(), i);
	else{
		this->~Dimension();
		throw std::exception("Измерение с таким номером не найдено в кубе");
	}	
}
Dimension::Dimension(const Server &server, char * DimensionName, TM1_INDEX DimensionNameLen):server(server) {
	hUser = server.gethUser();

	//создаем пулы значений 
	hPool = TM1ValPoolCreate(hUser);

	//получаем дескриптор измерения
	hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerDimensions(), DimensionName, vDimensionName, DimensionNameLen);	
}
Dimension::~Dimension(){
	//для каждого пула следует вызывать этот метод
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

char * Dimension::getDimensionName(){
	if (vDimensionName == nullptr)
		vDimensionName = getObjectProperty(hUser, hPool, hDimension, TM1ObjectName());

	return vDimensionName != nullptr ? TM1ValStringGet(hUser, vDimensionName) : nullptr;
}

bool Dimension::deleteDimension() {
	return deleteObject(hUser, hPool, hDimension);
}

TM1_INDEX Dimension::getCountSubsets()const{
	return getCountObjects(hUser, hPool, hDimension, TM1DimensionSubsets());
}

void Dimension::showSubsets()const{
	showObjects(hUser, hPool, hDimension, TM1DimensionSubsets(), "подмножеств");
}

TM1V Dimension::getSubsetByIndex(TM1_INDEX i)const{
	return getObjectByIndex(hUser, hPool, hDimension, TM1DimensionSubsets(), i);
}

TM1V Dimension::getSubsetByName(char *NameSubset)const{
	TM1V vName;
	return getObjectByName(hUser, hPool, hDimension, TM1DimensionSubsets(), NameSubset, vName);
}

TM1_INDEX Dimension::getCountElements()const{
	return getCountObjects(hUser, hPool, hDimension, TM1DimensionElements());
}

void Dimension::showElements()const{
	showObjects(hUser, hPool, hDimension, TM1DimensionElements(), "элементов");
}

TM1V Dimension::getElementByIndex(TM1_INDEX i)const{
	return getObjectByIndex(hUser, hPool, hDimension, TM1DimensionElements(), i);
}

TM1V Dimension::getElementByName(char *NameElement)const{
	TM1V vName;
	return getObjectByName(hUser, hPool, hDimension, TM1DimensionElements(), NameElement, vName);
}

bool Dimension::isRead()const{
	return TM1ValObjectCanRead(hUser, hDimension);
}

bool Dimension::isWrite()const{
	return TM1ValObjectCanWrite(hUser, hDimension);
}

bool Dimension::exist() {
	if (vDimensionName != nullptr)
		hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), vDimensionName);
	return hDimension != nullptr;
}

void Dimension::makeNew() {
	//пробуем создать
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

bool Dimension::addElement(char * ElementName, TM1_INDEX ElementNameLen) {
	if (hNewDimension != nullptr) {
		//выделяем место в пуле под значения
		TM1V vName = TM1ValString(hPool, ElementName, ElementNameLen);
		TM1V hElement=TM1DimensionElementInsert(hPool, hNewDimension, TM1ObjectNull(), vName, TM1TypeElementSimple());
		//проверяем успешность добавления элемента
		if (TM1ValType(hUser, hElement) == TM1ValTypeError()) {
			getLastError(hElement, true);
			return false;
		}
		return true;
	}
	return false;
}

bool Dimension::check()const {
	if (hNewDimension != nullptr) {
		TM1V vBool = TM1DimensionCheck(hPool, hNewDimension);
		//проверяем успешность создания массива
		if (TM1ValType(hUser, vBool) == TM1ValTypeError()) {
			getLastError(vBool, true);
			return false;
		}else if (TM1ValType(hUser, vBool) == TM1ValTypeBool()
			&& TM1ValBoolGet(hUser,vBool)!=1) {
			getLastError(vBool, true);
			return false;
		}else
			return true;
	}
	return false;
}

bool Dimension::registerDimension(char * DimensionName, TM1_INDEX DimensionNameLen) {
	if (hNewDimension == nullptr)
		return false; //новое измерение не инициализировано
	else if (hDimension != nullptr
			&& (DimensionName==nullptr 
				|| DimensionName !=nullptr && std::strncpy(TM1ValStringGet(hUser,vDimensionName), DimensionName, DimensionNameLen)==0)){
		//делаем обновление, а не вставку нового
		TM1V hDimension =TM1DimensionUpdate(hPool, this->hDimension, hNewDimension);
		if (TM1ValType(hUser, hDimension) == TM1ValTypeError()) {
			getLastError(hDimension, true);
			return false;
		}
		this->hDimension = hDimension;
		return true;
	}else if (DimensionName != nullptr) {
		TM1V hDimension = registerObject(hUser, hPool, server.gethServer(), hNewDimension, DimensionName, vDimensionName, DimensionNameLen);
		if (hDimension != nullptr)
			this->hDimension = hDimension;
		else
			return false; //не удалось зарегистрировать измерение
	}else if (vDimensionName != nullptr) {
		TM1V hDimension = registerObject(hUser, hPool, server.gethServer(), hNewDimension, vDimensionName);
		if (hDimension != nullptr)
			this->hDimension = hDimension;
		else
			return false; //не удалось зарегистрировать измерение
	}
	
	return true;
}

bool Dimension::setElementVal(char * strVal){
	hElement = getObjectByName(hUser, hPool, hDimension, TM1DimensionElements(), strVal, vElement);	
	return hElement != nullptr;
}

bool Dimension::setElementVal(double val) {
	hElement = getObjectByName(hUser, hPool, hDimension, TM1DimensionElements(), val, vElement);
	return hElement != nullptr;
}

char * Dimension::getElementVal()const{
	return vElement != nullptr ? TM1ValStringGet(hUser, vElement) : nullptr;
}

TM1V Dimension::getElement()const{
	return hElement;
}

TM1_INDEX Dimension::getLastError(TM1V val, bool isShow)const {
	return ::getLastError(hUser, val, isShow);
}

TM1V Dimension::gethDimension()const {
	return hDimension;
}