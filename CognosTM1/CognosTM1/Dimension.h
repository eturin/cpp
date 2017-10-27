#ifndef DIMENSION_H
#define DIMENSION_H

#include "Common.h"
#include "Object.h"

#include "Server.h"
#include "Cube.h"


class Dimension: public Object{
private:
	//свойства
	const Server &server;	
	TM1V hElement       = nullptr;  //дескриптор элемента в пуле API
	TM1V vElement       = nullptr;  //значение элемента в пуле API
	
public:
	//создание экземпл€ра на основе опубликованного измерени€ по имени
	Dimension(const Cube &cube, const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//создание экземпл€ра на основе опубликованного измерени€ по индексу
	Dimension(const Cube &cube, TM1_INDEX i);
	//создание экземпл€ра дл€ нового измерени€ или измерени€ опубликованного на сервере
	Dimension(const Server &server, const char * DimensionName, TM1_INDEX DimensionNameLen = 0) noexcept;
	//запрещаем конструкторы
	Dimension(const Dimension &)             = delete;
	Dimension(Dimension &&)                  = delete;
	//запрещаем операторы
	Dimension & operator=(const Dimension &) = delete;
	Dimension & operator=(Dimension &&)      = delete;
					
	//проверка опубликовано-ли измерение
	virtual bool exist() noexcept override;
	//создание пустого измерени€
	inline void makeNew() noexcept {
		hNewObject = TM1DimensionCreateEmpty(hPool, server.gethObject());
	}
	
	//добавление элемента в состав измерени€
	bool addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0);	
	//проверка корректности измерени€
	bool check()const;
	//публикаци€ измерени€
	bool registerDimension(const char * DimensionName = nullptr, TM1_INDEX DimensionNameLen = 0);
	
	bool  setElementVal(const char * strVal, TM1_INDEX strValLen = 0);
	bool  setElementVal(double val);	
	inline TM1V  gethElement()const noexcept { return hElement; }

	//получение кол-ва подмножеств
	inline TM1_INDEX getCountSubsets()const {
		return getListCount(TM1DimensionSubsets());
	}
	//получение строки с именами подмножеств
	inline std::string showSubsets()const {
		return std::move(showList(TM1DimensionSubsets()));
	}
	//получение подмножества по индексу
	inline TM1V getSubsetByIndex(TM1_INDEX i)const {
		return getListItemByIndex(i, TM1DimensionSubsets());
	}
	//получение подмножества по имени
	inline TM1V getSubsetByName(const char *NameSubset, TM1_INDEX NameSubsetLen=0)const {
		TM1V vName;
		return getListItemByName(TM1DimensionSubsets(), NameSubset, vName, NameSubsetLen);
	}

	//это возможные значени€ измерени€
	inline TM1_INDEX getCountElements()const {
		return getListCount(TM1DimensionElements());
	}
	//получение строки с именами элементов
	inline std::string showElements()const {
		return std::move(showList(TM1DimensionElements()));
	}
	//получение элемента по индексу
	inline TM1V getElementByIndex(TM1_INDEX i)const {
		return getListItemByIndex(i, TM1DimensionElements());
	}
	//получение элемента по имени
	inline TM1V getElementByName(const char *NameElement, TM1_INDEX NameElementLen=0)const {
		TM1V vName;
		return getListItemByName(TM1DimensionElements(), NameElement, vName, NameElementLen);
	}
		
	inline const Server & getServer() const noexcept { return server; }
};

#endif