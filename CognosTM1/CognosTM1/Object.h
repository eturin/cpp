#ifndef OBJECT_H
#define OBJECT_H

#include "Common.h"

class Object {
protected:
	TM1U hUser;                 //TM1 API дескриптор сессии
	TM1P hPool;                 //дескриптор пула значений
	TM1V hObject    = nullptr;  //дескриптор опубликованного объекта
	TM1V hNewObject = nullptr;  //дескриптор нового объекта
	TM1V vName      = nullptr;  //имя объекта в пуле

	//методы
	inline TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept { 
		return utilities::getLastError(sout, hUser, val, isShow);
	}
public:
	//конструктор
	inline Object(TM1U hUser, TM1P hPool=nullptr):hUser(hUser), hPool(hPool){
		if (hUser == nullptr)
			throw std::exception("AdminServer не инициализирован");
	}
	inline Object(const Object & other) : hUser(other.gethUser()), hPool(other.gethPool()) {
		if (hUser == nullptr)
			throw std::exception("AdminServer не инициализирован");
	}

	//деструктор
	virtual ~Object() = default;

	//получение дескриптора сессии
	inline TM1U gethUser() const noexcept { return hUser; }
	//получение дескриптора пула
	inline TM1P gethPool() const noexcept { return hPool; }
	//получение дескриптора опубликованного объекта
	inline TM1V gethObject() const noexcept { return hObject; }
	//получение дескриптора нового объекта
	inline virtual TM1V gethNewObject() const noexcept { return hNewObject; }

	//извлечение имени объекта из API по дескриптору
	inline bool uploadName() {
		if (hObject != nullptr) {
			vName = getObjectProperty(TM1ObjectName());
			return vName != nullptr;
		}else
			return false;
	}
	//получение имени объекта	
	inline const char* getName() const {
		if (vName != nullptr)
			return TM1ValStringGet(hUser, vName);		
		else
			return nullptr;
	}

	//проверака доступности на чтение
	inline bool isRead()const noexcept {
		return bool(TM1ValObjectCanRead(hUser, hObject));
	}
	//проверака доступности на запись
	inline bool isWrite()const noexcept {
		return bool(TM1ValObjectCanWrite(hUser, hObject));
	}
	
	//проверка опубликованности
	virtual bool exist(bool isPublic = true) noexcept = 0;
	//получение дескриптора родителя
	inline TM1V getParrent(TM1V hObject = nullptr)const {
		if (hObject || this->hObject)
			return utilities::getObjectProperty(hUser, hPool, hObject ? hObject : this->hObject, TM1ObjectParent());
		else
			throw std::exception("Объект не инициализирован");		
	}
	
	//удаление опубликованного объекта
	inline virtual bool deleteObject() noexcept {
		if (hObject != nullptr) {
			if(utilities::deleteObject(hUser, hPool, hObject))
				hObject = nullptr;
			return hObject == nullptr;
		}
		return true;
	}
	//создание нового объекта на основе опубликованного
	inline virtual bool makeDuplicate() {
		if (hObject != nullptr) {
			hNewObject = utilities::duplicateObject(hUser, hPool, hObject);
			return hNewObject != nullptr;
		}else
			return false;
	}

	//получить свойство объекта
	inline TM1V getObjectProperty(TM1V vType, TM1V hObject = nullptr) const {
		if (hObject || this->hObject)
			return utilities::getObjectProperty(hUser, hPool, hObject ? hObject : this->hObject, vType);
		else
			throw std::exception("Объект не инициализирован");
	}
	//установить свойство объекта
	/*inline*/ bool setObjectProperty(TM1V vType, TM1V val, TM1V hObject = nullptr) {
		if (hObject || this->hObject)
			return utilities::setObjectProperty(hUser, hPool, hObject ? hObject : this->hObject, vType, val);
		else
			throw std::exception("Объект не инициализирован");
	}
	
	//получение количества дочерних объектов определенного типа
	inline TM1_INDEX getListCount(TM1V vType, bool isPublic=true, TM1V hObject=nullptr)const {
		if (hObject || this->hObject)			
			return utilities::getCountObjects(hUser, hPool, hObject ? hObject:this->hObject, vType, isPublic);
		else
			throw std::exception("Объект не инициализирован");
	}
	
	//показать дочерние объекты
	inline std::string showList(TM1V vType, bool isPublic = true, TM1V hObject = nullptr)const {
		if (hObject || this->hObject)
			return std::move(utilities::showObjects(hUser, hPool, hObject ? hObject : this->hObject, vType, isPublic));
		else
			throw std::exception("Объект не инициализирован");
	}
	
	//получение дочернего объекта по имени
	inline TM1V getListItemByName(TM1V vType, const char *Name, TM1V &vName, TM1_INDEX NameLen, TM1V hObject = nullptr, bool isPublic = true) const{
		if (hObject || this->hObject)
			return utilities::getObjectByName(hUser, hPool, hObject ? hObject : this->hObject, vType, const_cast<char*>(Name), vName, NameLen, isPublic);
		else
			throw std::exception("Объект не инициализирован");
	}
	//получение дочернего объекта по имени из пула
	inline TM1V getListItemByName(const TM1V &vName, TM1V vType, TM1V hObject = nullptr, bool isPublic = true) const {
		if (hObject || this->hObject)
			return utilities::getObjectByName(hUser, hPool, hObject ? hObject : this->hObject, vType, vName, isPublic);
		else
			throw std::exception("Объект не инициализирован");
	}
	//получение дочернего объекта по индексу
	inline TM1V getListItemByIndex(TM1_INDEX i, TM1V vType, TM1V hObject = nullptr, bool isPublic=true) const{
		if (hObject || this->hObject)
			return utilities::getObjectByIndex(hUser, hPool, hObject ? hObject : this->hObject, vType, i, isPublic);
		else
			throw std::exception("Объект не инициализирован");
	}
};


#endif
