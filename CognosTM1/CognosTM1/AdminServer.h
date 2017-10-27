#ifndef ADMINSERVER_H
#define ADMINSERVER_H

#include "Common.h"
#include <string>

class AdminServer{
private:		
	TM1U hUser = nullptr;       //TM1 API session handle
	TM1P hPool = nullptr;       //дескриптор пула значений
	
	//методы
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow=false)const noexcept;
public:
	//конструкторы
	AdminServer(const char * AdminHost, const char * pathToSert = nullptr);
	//запрещаем конструкторы копирования и переноса
	AdminServer(const AdminServer &) = delete;
	AdminServer(AdminServer &&)      = delete;
	//запрещаем операторы
	AdminServer & operator=(const AdminServer &) = delete;
	AdminServer & operator=(AdminServer &&) = delete;
	//деструкторы
	virtual ~AdminServer() noexcept;

	//получение дескриптора сессии
	inline TM1U gethUser()const noexcept {return hUser;}

	//получение версии TM1 API
	int getVersion() const noexcept;
	//получение имени админ-сервера (освобождение не требуется)
	const char * getAdminServer() const noexcept;
	//получение кол-ва зарегистрированных серверов
	TM1_INDEX getCountServers() const noexcept;
	//вывести список баз данных
	std::string showServers()const noexcept;
};



#endif