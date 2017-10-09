#ifndef SERVER_H
#define SERVER_H

#include "Common.h"
#include "AdminServer.h"

class Server{
private:
	//свойства
	TM1U hUser       = nullptr;       //TM1 API session handle
	TM1P hPool       = nullptr;       //дескриптор пула значений
	TM1V hServer     = nullptr;       //дескриптор сервера в пуле API
	TM1V vServerName = nullptr;       //имя сервера в пуле API
	//методы
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept;
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
	~Server() noexcept;

	//получение имени сервера (освобождение не требуется)
	const char * getServerName()const noexcept;
	//проверка соединения
	bool isConnected() noexcept;
	//подключение
	bool connect(const char *ServerName, TM1_INDEX ServerNameLen=0, const char *UserName = nullptr, TM1_INDEX UserNameLen=0, const char *Password = nullptr, TM1_INDEX PasswordLen=0);
	//отключение
	bool disConnect();
	//получение дескриптора сессии
	TM1U gethUser()const noexcept;
	//получение дескриптора сервера
	TM1V gethServer()const noexcept;
	//получение дескриптора пула
	TM1P gethPool()const noexcept;


	//получение кол-ва кубов
	TM1_INDEX getCountCubes() const;
	//показать кубы
	std::string showCubes()const;
	//получение кол-ва измерений сервера
	TM1_INDEX getCountDimensions() const;
	//показать измерения
	std::string showDimensions() const;
};


#endif