#ifndef ADMINSERVER_H
#define ADMINSERVER_H

#include "Common.h"

class AdminServer{
private:		
	TM1U hUser = nullptr;       //TM1 API session handle
	TM1P hPool = nullptr;       //дескриптор пула значений
	
	//методы
	TM1_INDEX getLastError(TM1V val, bool isShow=false)const;
public:
	//конструкторы
	AdminServer(char * AdminHost, char * pathToSert = nullptr);
	AdminServer(const AdminServer &) = delete;
	//операторы
	AdminServer & operator=(const AdminServer &) = delete;
	//деструкторы
	~AdminServer();

	//методы	
	TM1U gethUser()const;

	//версия TM1
	int getVersion() const;
	//имя админ сервера (освобождение не требуется)
	char * getAdminServer() const;
	//кол-во баз данных
	TM1_INDEX getCountServers() const;
	//вывести список баз данных
	void showServers()const;
	

};



#endif