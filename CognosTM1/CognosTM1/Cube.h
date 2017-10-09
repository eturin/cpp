#ifndef CUBE_H
#define CUBE_H

#include "Common.h"
#include "Server.h"
#include <queue>
#include <string>
#include <vector>

class Dimension;

class Cube{
private:
	//свойства
	const Server &server; 
	TM1U hUser          = nullptr;  //TM1 API session handle
	TM1P hPool          = nullptr;  //дескриптор пула значений
	TM1V hCube          = nullptr;  //дескриптор куба в пуле API
	TM1V hNewCube       = nullptr;  //дескриптор создаваемого куба в пуле API
	TM1V vCubeName      = nullptr;  //имя сервера в пуле API
	TM1V vRule          = nullptr;  //дескриптор в пуле API	

	std::vector<std::string> Dimensions;
	//методы
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept;
public:
	//конструктор
	Cube(const Server &server, const char * CubeName=nullptr, TM1_INDEX CubeNameLen=0);
	//запрещаем конструкторы
	Cube(const Cube &) = delete;
	Cube(Cube &&) = delete;
	//запрещаем операторы
	Cube & operator=(const Cube &) = delete;
	Cube & operator=(Cube &&) = delete;
	//деструктор
	~Cube() noexcept;

	//получение имени куба
	const char * getCubeName()const noexcept;
	//проверка опубликован-ли куб
	bool   exist() noexcept;
	//создание пустого куба
	bool   makeNew();
	//копирование на основе опубликованного куба
	bool   makeDuplicate();
	//добавление опубликованного измерения в куб
	void   addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//публикация куба
	bool   registerCube(const char * CubeName=nullptr, TM1_INDEX CubeNameLen = 0);
	//удаление куба
	bool   deleteCube() noexcept;
	
	//получени кол-ва измерений
	TM1_INDEX getCountDimensions()const;
	//получение строки с измерениями
	std::string showDimensions()const;
	//получение измерения по индексу
	TM1V getDimensionByIndex(TM1_INDEX i)const noexcept;
	//получение измерения по имени
	TM1V getDimensionByName(const char *NameDimension)const noexcept;
	
	//получение кол-ва представлений
	TM1_INDEX getCountViews()const;
	//получение строки с именами представлений
	std::string showViews()const;
	//получение представления по индексу
	TM1V getViewByIndex(TM1_INDEX i)const noexcept;
	//получение представления по имени
	TM1V getViewByName(const char *NameView)const noexcept;
		
	TM1V getRule();	
	
	//получение значение ячейки куба
	TM1V getCellValue(const std::vector<Dimension*> & Dimensions)const;
	//установка значения ячейки куба
	TM1V setCellValue(const std::vector<Dimension*> & Dimensions, const char * val)const;
	TM1V setCellValue(const std::vector<Dimension*> & Dimensions, double val)const;

	//получение сервера
	const Server& getServer()const noexcept;
	//получение дескриптора пула
	TM1P gethPool()const noexcept;
	//получение дескриптора сессии
	TM1U gethUser()const noexcept;
	//получение дескриптора куба
	TM1V gethCube()const noexcept;

};

#endif