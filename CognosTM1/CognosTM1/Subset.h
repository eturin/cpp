#ifndef SUBSET_H
#define SUBSET_H

#include "Common.h"
#include "Object.h"

#include "Dimension.h"

class Subset: public Object {
private:
	//свойства
	const Dimension &dimension;
	
public:
	//создание экземпляра на основе опубликованного подмножества по имени
	Subset(const Dimension &dimension, const char * SubsetName, TM1_INDEX SubsetLen = 0);
	//создание экземпляра на основе опубликованного подмножества по индексу
	Subset(const Dimension &dimension, TM1_INDEX i);
	//запрещаем конструкторы
	Subset(const Subset &)             = delete;
	Subset(Subset &&)                  = delete;
	//запрещаем операторы
	Subset & operator=(const Subset &) = delete;
	Subset & operator=(Subset &&)      = delete;
		
	//проверка опубликовано-ли измерение
	virtual bool exist() noexcept override;
	//создание пустого подмножества
	void makeNew() noexcept;
	//создание обновляемого на основе MDX подмножества
	void makeNewWithMDX(const char * Expression, TM1_INDEX ExpressionLen = 0);
	
	//добавление элемента в состав подмножества
	bool addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0);
	
	//публикация 
	bool registerSubset(const char * SubsetName = nullptr, TM1_INDEX SubsetNameLen = 0);
	

	//количество элементов в подмножестве
	inline TM1_INDEX getCountElements(TM1V hObject = nullptr)const {
		return getListCount(TM1SubsetElements(), hObject);
	}
	//получение строки с именами элементов
	inline std::string showElements()const {
		return std::move(showList(TM1SubsetElements()));
	}

	//добавление всех элементов измерения
	bool addAllElements();
	
};

#endif 

