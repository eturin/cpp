#define _CRT_SECURE_NO_WARNINGS
#include "Cube.h"
#include "Dimension.h"

Cube::Cube(const Server &server, const char * CubeName, TM1_INDEX CubeNameLen) :server(server){
	hUser = server.gethUser();

	//создаем пулы значений 
	hPool = server.gethPool();
	
	//получаем хендл куба
	if (CubeName != nullptr) 
		hCube = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerCubes(), CubeName, vCubeName, CubeNameLen);			
}

Cube::~Cube() noexcept {
	//для каждого пула следует вызывать этот метод
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
	//должно быть не менее 2-х измерений
	TM1_INDEX cnt = (TM1_INDEX)Dimensions.size();
	if (cnt >= 2) {
		//создаем массив измерений создаваемого куба
		TM1V tmp[20] = { 0 };
		TM1V vArrayOfDimensions = makeArray(hUser, hPool, cnt, tmp);
			
		//заполняем массив
		for (TM1_INDEX i = 0; i < cnt;++i) {
			const std::string strName = Dimensions[i];
			//выделяем место в пуле под значения				
			TM1V vName, hDimension = getObjectByName(hUser, hPool, server.gethServer(), TM1ServerDimensions(), strName.c_str(), vName);				
			if (hDimension == nullptr) {
				std::ostringstream sout;
				sout << "На сервере " << server.getServerName() << " отсутствует измерение с именем " << strName << "\n";
				throw std::exception(sout.str().c_str());
			}
			//укладываем значение в массив	
			TM1ValArraySet(vArrayOfDimensions, hDimension, i+1);
		} 

		//пробуем создать
		hNewCube = TM1CubeCreate(hPool, server.gethServer(), vArrayOfDimensions);
		if (TM1ValType(hUser, hNewCube) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "Не удалось создать новый куб перед регистрацией:\n\t";
			getLastError(sout,hNewCube, true);
			hNewCube = nullptr;
			throw std::exception(sout.str().c_str());
		}
		
		return true;
	}else 
		throw std::exception("В кубе должно быть не менее двух измерений");
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
		return false; //новый куб или его изменения не инициализированы
	else if (CubeName != nullptr) {
		TM1V hCube = registerObject(hUser, hPool, server.gethServer(), hNewCube, CubeName, vCubeName, CubeNameLen);
		if (hCube != nullptr)
			this->hCube = hCube;
		else
			return false; //не удалось зарегистрировать куб
	}else if (vCubeName != nullptr) {
		TM1V hCube = registerObject(hUser, hPool, server.gethServer(), hNewCube, vCubeName);
		if (hCube != nullptr)
			this->hCube = hCube;
		else
			return false; //не удалось зарегистрировать куб
	}
	return true;
}

TM1_INDEX Cube::getCountDimensions()const{	
	return getCountObjects(hUser,hPool,hCube,TM1CubeDimensions());
}

std::string Cube::showDimensions()const{
	return showObjects(hUser, hPool, hCube, TM1CubeDimensions(), "измерений");
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
	return showObjects(hUser, hPool, hCube, TM1CubeViews(), "представлений");
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
	//выделяем место в пуле под значение
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//заполнение массива
	for (TM1_INDEX i=0, cnt = getCountDimensions(); i < cnt; ++i)
		TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	//выделяем место в пуле под значение
	TM1V hVal = TM1CubeCellValueGet(hPool, hCube, vArrayElements);
	if (TM1ValType(hUser, hVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "Не удалось получить значение ячейки:\n\t";
		getLastError(sout, hVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hVal;
}

TM1V Cube::setCellValue(const std::vector<Dimension*> & Dimensions, double val)const{
	//выделяем место в пуле под значение
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//заполнение массива
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i=0; i < cnt; ++i)
		if(Dimensions[i]!=nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i+1);
		

	//выделяем место в пуле под значение
	TM1V vVal = TM1ValReal(hPool, val);

	//выделяем место в пуле под значение
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hCube, vArrayElements, vVal);
	//проверяем успешность записи
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()){
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			return nullptr;
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "В кубе "<<getCubeName()<<" не удалось установить значение ячейки: ";
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
	//выделяем место в пуле под значение
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//заполнение массива
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i = 0; i < cnt; ++i)
		if (Dimensions[i] != nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	
	//выделяем место в пуле под значение
	TM1V vVal = TM1ValString(hPool, const_cast<char*>(val), 0);
	if(vVal==0)
		throw std::exception("не удалось получить значение в пуле");

	//выделяем место в пуле под значение
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hCube, vArrayElements, vVal);
	//проверяем успешность записи
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()) {
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			throw std::exception("Не удалось установить значение ячейки");
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "В кубе " << getCubeName() << " не удалось установить значение ячейки: ";
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
