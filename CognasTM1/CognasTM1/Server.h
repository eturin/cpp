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
	TM1_INDEX getLastError(TM1V val, bool isShow = false)const;
public:
	//конструктор
	Server(AdminServer &Server);

	//деструктор
	~Server();

	//методы
	char * getServerName()const;	
	bool connect(char *ServerName, TM1_INDEX ServerNameLen=0, char *UserName = nullptr, TM1_INDEX UserNameLen=0, char *Password = nullptr, TM1_INDEX PasswordLen=0);
	bool disConnect();
	bool isConnected();
	TM1U gethUser()const;
	TM1V gethServer()const;

	//кол-во кубов
	TM1_INDEX getCountCubes() const;
	//вывести спиок кубов
	void showCubes()const;
	//кол-во измерений куба
	TM1_INDEX getCountDimensions() const;
	void showDimensions() const;
};


#endif