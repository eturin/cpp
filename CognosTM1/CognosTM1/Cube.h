#ifndef CUBE_H
#define CUBE_H

#include "Common.h"
#include "Object.h"

#include "Server.h"
#include <queue>
#include <string>
#include <vector>

class Dimension;

class Cube: public Object{
private:
	//свойства
	const Server &server; 	
	std::vector<std::string> Dimensions;	
public:
	//конструктор
	Cube(const Server &server, const char * CubeName=nullptr, TM1_INDEX CubeNameLen=0);
	//запрещаем конструкторы
	Cube(const Cube &)             = delete;
	Cube(Cube &&)                  = delete;
	//запрещаем операторы
	Cube & operator=(const Cube &) = delete;
	Cube & operator=(Cube &&)      = delete;
		
	//проверка опубликован-ли куб
	virtual bool exist() noexcept override;
	//создание пустого куба
	bool   makeNew();
	
	//добавление опубликованного измерения в куб
	void   addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//публикация куба
	bool   registerCube(const char * CubeName=nullptr, TM1_INDEX CubeNameLen = 0);
		
	//получени кол-ва измерений
	inline TM1_INDEX getCountDimensions()const {
		return getListCount(TM1CubeDimensions());
	}
	//получение строки с измерениями
	inline std::string showDimensions()const {
		return std::move(showList(TM1CubeDimensions()));
	}
	//получение измерения по индексу
	inline TM1V getDimensionByIndex(TM1_INDEX i)const  {
		return getListItemByIndex(i, TM1DimensionSubsets());
	}
	//получение измерения по имени
	inline TM1V getDimensionByName(const char *NameDimension, TM1_INDEX NameDimensionLen = 0)const {
		TM1V vName;
		return getListItemByName(TM1DimensionSubsets(), NameDimension, vName, NameDimensionLen);
	}

	//получение кол-ва представлений
	inline TM1_INDEX getCountViews()const {
		return getListCount(TM1CubeViews());
	}
	//получение строки с именами представлений
	inline std::string showViews()const {
		return std::move(showList(TM1CubeViews()));
	}
	//получение представления по индексу
	inline TM1V getViewByIndex(TM1_INDEX i)const {
		return getListItemByIndex(i, TM1CubeViews());
	}
	//получение представления по имени
	inline TM1V getViewByName(const char *NameView, TM1_INDEX NameViewLen = 0)const {
		TM1V vName;
		return getListItemByName(TM1CubeViews(), NameView, vName, NameViewLen);
	}
		
	//получение значение ячейки куба
	TM1V getCellValue(const std::vector<Dimension*> & Dimensions)const;
	//установка значения ячейки куба
	TM1V setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, const char * val, TM1_INDEX valLen = 0)const;
	TM1V setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, double val)const;

	//получение сервера
	inline const Server& getServer()const noexcept {
		return server;
	}
};

#endif