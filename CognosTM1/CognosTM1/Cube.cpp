#define _CRT_SECURE_NO_WARNINGS
#include "Cube.h"
#include "Dimension.h"

Cube::Cube(const Server &server, const char * CubeName, TM1_INDEX CubeNameLen, bool isPublic) :Object(server),server(server){
	//получаем хендл куба
	if (CubeName != nullptr) 
		hObject = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerCubes(), CubeName, vName, CubeNameLen, isPublic);
}
Cube::Cube(const Server &server, TM1_INDEX i, bool isPublic) : Object(server), server(server) {
	if (0 < i && i <= server.getCountCubes())
		hObject = utilities::getObjectByIndex(hUser, hPool, server.gethObject(), TM1ServerCubes(), i, isPublic);
	else
		throw std::exception("Ќа сервере нет куба с таким номером.");
}

bool Cube::exist(bool isPublic) noexcept {
	if(vName!=nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerCubes(), vName, isPublic);
	return hObject != nullptr;
}

bool Cube::makeNew() {
	//должно быть не менее 2-х измерений
	TM1_INDEX cnt = (TM1_INDEX)Dimensions.size();
	if (cnt >= 2) {
		//создаем массив измерений создаваемого куба
		TM1V tmp[20] = { 0 };
		TM1V vArrayOfDimensions = utilities::makeArray(hUser, hPool, cnt, tmp);
			
		//заполн€ем массив
		for (TM1_INDEX i = 0; i < cnt;++i) {
			const std::string strName = Dimensions[i];
			//выдел€ем место в пуле под значени€				
			TM1V vName, hDimension = utilities::getObjectByName(hUser, hPool, server.gethObject(), TM1ServerDimensions(), strName.c_str(), vName);
			if (hDimension == nullptr) {
				std::ostringstream sout;
				sout << "Ќа сервере " << server.getName() << " отсутствует измерение с именем " << strName << "\n";
				throw std::exception(sout.str().c_str());
			}
			//укладываем значение в массив	
			TM1ValArraySet(vArrayOfDimensions, hDimension, i+1);
		} 

		//пробуем создать
		hNewObject = TM1CubeCreate(hPool, server.gethObject(), vArrayOfDimensions);
		if (TM1ValType(hUser, hNewObject) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "Ќе удалось создать новый куб перед регистрацией:\n\t";
			getLastError(sout, hNewObject, true);
			hNewObject = nullptr;
			throw std::exception(sout.str().c_str());
		}
		
		return true;
	}else 
		throw std::exception("¬ кубе должно быть не менее двух измерений");
}

void Cube::addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen) {
	Dimensions.push_back(std::string(DimensionName, DimensionName + (DimensionNameLen ? DimensionNameLen : std::strlen(DimensionName))));
}

bool Cube::registerCube(bool isPublic, const char * CubeName, TM1_INDEX CubeNameLen) {
	if (hNewObject == nullptr)
		return false; //новый куб или его изменени€ не инициализированы
	else if (CubeName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, server.gethObject(), hNewObject, isPublic, CubeName, vName, CubeNameLen);
		if (hObject != nullptr)
			this->hObject = hObject;
		else
			return false; //не удалось зарегистрировать куб
	}else if (vName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, server.gethObject(), hNewObject, isPublic, vName);
		if (hObject != nullptr)
			this->hObject = hObject;
		else
			return false; //не удалось зарегистрировать куб
	}
	return true;
}


TM1V Cube::getCellValue(const std::vector<Dimension*> & Dimensions)const{
	//выдел€ем место в пуле под значение
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = utilities::makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//заполнение массива
	for (TM1_INDEX i=0, cnt = getCountDimensions(); i < cnt; ++i)
		TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	//выдел€ем место в пуле под значение
	TM1V hVal = TM1CubeCellValueGet(hPool, hObject, vArrayElements);
	if (TM1ValType(hUser, hVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "Ќе удалось получить значение €чейки:\n\t";
		getLastError(sout, hVal, true);
		throw std::exception(sout.str().c_str());
	}
	return hVal;
}

TM1V Cube::setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, double val)const{
	//выдел€ем место в пуле под значение
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = utilities::makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//заполнение массива
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i=0; i < cnt; ++i)
		if(Dimensions[i]!=nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i+1);
		

	//выдел€ем место в пуле под значение
	TM1V vVal = TM1ValReal(hPool, val);

	//выдел€ем место в пуле под значение
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hObject, vArrayElements, vVal);
	//провер€ем успешность записи
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()){
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			return nullptr;
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()){
		std::ostringstream sout;
		sout << "¬ кубе "<<getName()<<" не удалось установить значение €чейки: ";
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
	//выдел€ем место в пуле под значение
	TM1V tmp[20] = { 0 };
	TM1V vArrayElements = utilities::makeArray(hUser, hPool, (TM1_INDEX)Dimensions.size(), tmp);
	
	//заполнение массива
	TM1_INDEX cnt = getCountDimensions();
	for (TM1_INDEX i = 0; i < cnt; ++i)
		if (Dimensions[i] != nullptr)
			TM1ValArraySet(vArrayElements, Dimensions[i]->gethElement(), i + 1);
	
	//выдел€ем место в пуле под значение
	TM1V vVal = TM1ValString(hPool, const_cast<char*>(val), valLen);
	
	//выдел€ем место в пуле под значение
	TM1V hNewVal = TM1CubeCellValueSet(hPool, hObject, vArrayElements, vVal);
	//провер€ем успешность записи
	if (TM1ValType(hUser, hNewVal) == TM1ValTypeBool()) {
		TM1_BOOL rc = TM1ValBoolGet(hUser, hNewVal);
		if (rc == 0)
			throw std::exception("Ќе удалось установить значение €чейки");
		else
			return hNewVal;
	}else if (TM1ValType(hUser, hNewVal) == TM1ValTypeError()) {
		std::ostringstream sout;
		sout << "¬ кубе " << getName() << " не удалось установить значение €чейки: ";
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


