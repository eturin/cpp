#ifndef SERVER_H
#define SERVER_H

#include "Common.h"
#include "Object.h"

#include "AdminServer.h"

class Server: public Object{
private:
	//удаляемые методы
	//inline virtual TM1V gethNewObject() const noexcept override ;
	//inline virtual bool deleteObject() noexcept override;
	//inline virtual bool makeDuplicate() override;
public:
	//конструктор
	Server(const AdminServer &Server);
	//запрещаем конструкторы копирования и переноса
	Server(const Server &) = delete;
	Server(Server &&) = delete;
	//запрещаем операторы
	Server & operator=(const Server &) = delete;
	Server & operator=(Server &&) = delete;
	//деструктор
	virtual ~Server() noexcept override;
	
	//проверка соединения
	bool isConnected() noexcept;
	inline virtual bool exist(bool isPublic = true) noexcept override {
		return isConnected();
	}
	//подключение
	bool connect(const char *ServerName, TM1_INDEX ServerNameLen=0, const char *UserName=nullptr, TM1_INDEX UserNameLen=0, const char *Password=nullptr, TM1_INDEX PasswordLen=0);
	//отключение
	bool disConnect();
	
	//получение кол-ва кубов
	inline TM1_INDEX getCountCubes() const {
		return getListCount(TM1ServerCubes());
	}
	//показать кубы
	inline std::string showCubes()const {
		return std::move(showList(TM1ServerCubes()));
	}
	
	//получение кол-ва измерений сервера
	inline TM1_INDEX getCountDimensions() const {		
		return getListCount(TM1ServerDimensions());
	}
	//показать измерения
	inline std::string showDimensions() const {
		return std::move(showList(TM1ServerDimensions()));
	}
};


#endif