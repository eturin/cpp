#ifndef DIMENSION_H
#define DIMENSION_H

#include "Common.h"
#include "Server.h"
#include "Cube.h"


class Dimension{
private:
	//свойства
	const Server &server;
	TM1U hUser          = nullptr;  //TM1 API session handle
	TM1P hPool          = nullptr;  //дескриптор пула значений
	TM1V hDimension     = nullptr;  //дескриптор измерения в пуле API
	TM1V hNewDimension  = nullptr;  //дескриптор создаваемого измерения в пуле API
	mutable TM1V vDimensionName = nullptr;  //имя измерения в пуле API
	TM1V hElement       = nullptr;  //дескриптор элемента в пуле API
	TM1V vElement       = nullptr;  //значение элемента в пуле API
	//методы
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept;
public:
	//создание экземпляра на основе опубликованного измерения по имени
	Dimension(const Cube &cube, const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//создание экземпляра на основе опубликованного измерения по индексу
	Dimension(const Cube &cube, TM1_INDEX i);
	//создание экземпляра для нового измерения или измерения опубликованного на сервере
	Dimension(const Server &server, const char * DimensionName, TM1_INDEX DimensionNameLen = 0) noexcept;
	//запрещаем конструкторы
	Dimension(const Dimension &) = delete;
	Dimension(Dimension &&) = delete;
	//запрещаем операторы
	Dimension & operator=(const Dimension &) = delete;
	Dimension & operator=(Dimension &&) = delete;
	//деструктор
	~Dimension() noexcept;

	//получение имени измерения
	const char * getDimensionName()const;
	//удаление опубликованного измерения
	bool deleteDimension() noexcept;
	
	//проверака доступности для чтения
	bool isRead()const noexcept;
	//проверака доступности для записи
	bool isWrite()const noexcept;
	//проверка опубликовано-ли измерение
	bool exist() noexcept;
	//создание пустого измерения
	void makeNew() noexcept;
	//копирование на основе опубликованного измерения
	bool makeDuplicate();
	//добавление элемента в состав измерения
	bool addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0);
	//проверка корректности измерения
	bool check()const;
	//публикация измерения
	bool registerDimension(const char * DimensionName = nullptr, TM1_INDEX DimensionNameLen = 0);
	
	bool  setElementVal(const char * strVal);
	bool  setElementVal(double val);
	char *getElementVal()const noexcept;
	TM1V  gethElement()const noexcept;

	//получение кол-ва подмножеств
	TM1_INDEX getCountSubsets()const;
	//получение строки с именами подмножеств
	std::string showSubsets()const;
	//получение подмножества по индексу
	TM1V getSubsetByIndex(TM1_INDEX i)const noexcept;
	//получение подмножества по имени
	TM1V getSubsetByName(const char *NameSubset)const noexcept;

	//это возможные значения измерения
	TM1_INDEX getCountElements()const;
	std::string showElements()const;
	TM1V getElementByIndex(TM1_INDEX i)const noexcept;
	TM1V getElementByName(const char *NameElement)const noexcept;

	TM1V gethDimension()const noexcept;
};

#endif