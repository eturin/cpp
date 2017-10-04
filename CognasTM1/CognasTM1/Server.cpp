#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"

Server::Server(AdminServer &Server){
	hUser = Server.gethUser();
	if (hUser == nullptr)
		throw std::exception("AdminServer не инициализирован");

	//создаем пул дл€ значений 
	hPool = TM1ValPoolCreate(hUser);
}

Server::~Server(){
	//дл€ каждого пула следует вызывать этот метод
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

char * Server::getServerName()const{	
	return vServerName!=nullptr ? TM1ValStringGet(hUser, vServerName) : nullptr; //ok
}

bool Server::isConnected(){
	if (vServerName != nullptr){
		hServer = TM1SystemServerHandle(hUser, getServerName()); //!!!расход пам€ти!!!
		return hServer != nullptr;
	}else
		return false;
}

bool Server::connect(char *ServerName, TM1_INDEX ServerNameLen, char *UserName, TM1_INDEX UserNameLen, char *Password, TM1_INDEX PasswordLen){
	if (hServer == nullptr || !isConnected()){
		//выдел€ем место в пуле под значение
		vServerName = TM1ValString(hPool, ServerName, ServerNameLen);

		//авторизуемс€ 
		if (UserName != nullptr){
			//выдел€ем место в пуле под значени€
			TM1V vUserName = TM1ValString(hPool, UserName, UserNameLen);
			TM1V vPassword = TM1ValString(hPool, Password, PasswordLen);
			hServer = TM1SystemServerConnect(hPool, vServerName, vUserName, vPassword);
		}
		else
			//выдел€ем место в пуле под значение
			hServer = TM1SystemServerConnectIntegratedLogin(hPool, vServerName);

		//провер€ем успешность соединени€
		if (TM1ValType(hUser, hServer) == TM1ValTypeError()){
			getLastError(hServer, true);
			hServer = nullptr;
		}

		return hServer != nullptr;
	}

	return false;
}

bool Server::disConnect(){
	if (isConnected()){		
		//выдел€ем место в пуле под значение
		TM1V vResult = TM1SystemServerDisconnect(hPool, vServerName);
		if (!TM1ValBoolGet(hUser, vResult)){
			getLastError(vResult, true);
			return false;
		}
	}
	return true;
}

TM1U Server::gethUser()const{
	return hUser;
}

TM1V Server::gethServer()const{
	return hServer;
}

TM1_INDEX Server::getCountCubes()const{
	if (hServer!=nullptr)
		return getCountObjects(hUser, hPool, hServer, TM1ServerCubes());		
	
	std::cerr << "Ќет соединени€ с сервером: " << getServerName() << std::endl;
	return 0;
}

void Server::showCubes()const{
	if (hServer!=nullptr)	
		showObjects(hUser, hPool, hServer, TM1ServerCubes(),"кубов");		
	else
		std::cerr << "Ќет соединени€ с сервером " << std::endl;
}

TM1_INDEX Server::getCountDimensions()const{
	return getCountObjects(hUser, hPool, hServer, TM1ServerDimensions());	
}


void Server::showDimensions()const{
	if (hServer!=nullptr)
		showObjects(hUser, hPool, hServer, TM1ServerDimensions(),"измерений на сервере");
	else
		std::cerr << "Ќет соединени€ с сервером " << std::endl;

}

TM1_INDEX Server::getLastError(TM1V val, bool isShow)const{
	return ::getLastError(hUser, val, isShow);
}

