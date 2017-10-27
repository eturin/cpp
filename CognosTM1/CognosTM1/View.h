#ifndef VIEW_H
#define VIEW_H

#include "Common.h"
#include "Object.h"

#include <vector>

class Cube;

class View: public Object {
private:
	//свойства
	const Cube &cube;
	
	std::vector<std::string> Titles;
	std::vector<std::string> Columns;
	std::vector<std::string> Rows;
	std::vector<std::string> Elements;	
public:
	//создание экземпляра на основе опубликованного представления по имени
	View(const Cube &cube, const char * ViewName = nullptr, TM1_INDEX ViewNameLen = 0);
	//создание экземпляра на основе опубликованного представления по индексу
	View(const Cube &cube, TM1_INDEX i);
	//запрещаем конструкторы
	View(const View &) = delete;
	View(View &&) = delete;
	//запрещаем операторы
	View & operator=(const View &) = delete;
	View & operator=(View &&) = delete;
		
	//проверка опубликовано-ли представление
	virtual bool exist() noexcept override;
	//создание пустого представления
	void makeNew();
	
	//публикация представления
	bool registerView(const char * ViewName = nullptr, TM1_INDEX ViewNameLen = 0);
	
	//добавление измерения в заголовок
	inline void addTitle(const char * DimensionName, TM1_INDEX DimensionNameLen = 0) {
		if (DimensionNameLen)
			Titles.push_back(std::string(DimensionName, DimensionNameLen));
		else
			Titles.push_back(DimensionName);
	}
	//добавление измерения в колонки
	inline void addColumn(const char * DimensionName, TM1_INDEX DimensionNameLen = 0) {
		if (DimensionNameLen)
			Columns.push_back(std::string(DimensionName, DimensionNameLen));
		else
			Columns.push_back(DimensionName);
	}
	//добавление измерения в строки
	inline void addRow(const char * DimensionName, TM1_INDEX DimensionNameLen = 0) {
		if (DimensionNameLen)
			Rows.push_back(std::string(DimensionName, DimensionNameLen));
		else
			Rows.push_back(DimensionName);
	}
	
	//подавление нулей
	inline bool setSuppressZeroes(bool val = true, TM1V hObject = nullptr) {
		return setObjectProperty(TM1ViewSuppressZeroes(), TM1ValBool(hPool, val), hObject);
	}
	inline bool getSuppressZeroes(TM1V hObject = nullptr) const{
		return TM1ValBoolGet(hUser, getObjectProperty(TM1ViewSuppressZeroes(), hObject));
	}
	//пропуск нулей
	inline bool setSkipZeroes(bool val = true, TM1V hObject = nullptr) {
		return setObjectProperty(TM1ViewExtractSkipZeroes(), TM1ValBool(hPool, val), hObject);
	}
	inline bool getSkipZeroes(TM1V hObject = nullptr) const{
		return TM1ValBoolGet(hUser, getObjectProperty(TM1ViewExtractSkipZeroes(), hObject));
	}
	//пропуск итоговых полей
	inline void setSkipConsolidated(bool val = true, TM1V hObject = nullptr) {
		setObjectProperty(TM1ViewExtractSkipConsolidatedValues(), TM1ValBool(hPool, val), hObject);
	}
	inline bool getSkipConsolidated(TM1V hObject = nullptr) const {
		return TM1ValBoolGet(hUser, getObjectProperty(TM1ViewExtractSkipConsolidatedValues(), hObject));
	}
	//пропуск правил
	inline void setSkipRule(bool val = true, TM1V hObject = nullptr) {
		setObjectProperty(TM1ViewExtractSkipRuleValues(), TM1ValBool(hPool, val), hObject);
	}
	inline bool getSkipRule(TM1V hObject = nullptr) const{
		return TM1ValBoolGet(hUser, getObjectProperty(TM1ViewExtractSkipRuleValues(), hObject));
	}

	//добавление элементов отбора
	inline void addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0) {
		if (ElementNameLen)
			Elements.push_back(std::string(ElementName, ElementNameLen));
		else
			Elements.push_back(ElementName);
	}

	std::string show() const;	
};

#endif
