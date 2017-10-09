#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"



Server::Server(const AdminServer &Server){
	hUser = Server.gethUser();
	if (hUser == nullptr)
		throw std::exception("AdminServer не инициализирован");

	//создаем пул дл€ значений (если пул спрашивать несколько раз, то будет высер)
	hPool = TM1ValPoolCreate(hUser);
}

Server::~Server() noexcept {
	//дл€ каждого пула следует вызывать этот метод
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

const char * Server::getServerName()const noexcept {
	return vServerName!=nullptr ? TM1ValStringGet(hUser, vServerName) : nullptr; //ok
}

bool Server::isConnected() noexcept {
	if (vServerName != nullptr){
		hServer = TM1SystemServerHandle(hUser, const_cast<char*>(getServerName())); //!!!расход пам€ти!!!
		if (TM1ValType(hUser, hServer) == TM1ValTypeBool())
			//не удалось проверить соединение
			hServer = nullptr;		
	}
	return hServer != nullptr;
}

bool Server::connect(const char *ServerName, TM1_INDEX ServerNameLen, const char *UserName, TM1_INDEX UserNameLen, const char *Password, TM1_INDEX PasswordLen){
	if (hServer == nullptr || !isConnected()){
		//выдел€ем место в пуле под значение
		vServerName = TM1ValString(hPool, const_cast<char*>(ServerName), ServerNameLen);
		if (vServerName == 0)
			throw std::exception("Ќе удалось получить им€ сервера в пуле.");

		//авторизуемс€ 
		if (UserName != nullptr){
			//выдел€ем место в пуле под значени€
			TM1V vUserName = TM1ValString(hPool, const_cast<char*>(UserName), UserNameLen), vPassword;
			if (vUserName == 0)
				throw std::exception("Ќе удалось получить им€ пользовател€ в пуле");
			else if(Password!=nullptr)
				vPassword = TM1ValString(hPool, const_cast<char*>(Password), PasswordLen);
			else
				vPassword = TM1ValString(hPool, "", 0);
			//подключение с именем пользовател€ и паролем
			hServer = TM1SystemServerConnect(hPool, vServerName, vUserName, vPassword);
		}else
			//подключение с интегрированной учетной записью
			hServer = TM1SystemServerConnectIntegratedLogin(hPool, vServerName);

		//провер€ем успешность соединени€
		if (TM1ValType(hUser, hServer) == TM1ValTypeError()){
			std::ostringstream sout;
			sout << "Ќе удалось подключитьс€ к серверу " << ServerName << ":\n\t";
			getLastError(sout, hServer, true);
			hServer = nullptr;
			std::cerr << sout.str() << std::endl;
			throw std::exception(sout.str().c_str());
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
			std::ostringstream sout;
			sout << "Ќе удалось выполнить отключение от сервера " << getServerName() << ":\n\t";
			getLastError(sout, vResult, true);
			std::cerr << sout.str() << std::endl;
			return false;			
		}
	}
	return true;
}

TM1U Server::gethUser()const noexcept {
	return hUser;
}

TM1V Server::gethServer()const noexcept {
	return hServer;
}

TM1P Server::gethPool()const noexcept {
	return hPool;
}

TM1_INDEX Server::getCountCubes()const{
	if (hServer!=nullptr)
		return getCountObjects(hUser, hPool, hServer, TM1ServerCubes());

	std::ostringstream sout;
	sout << "Ќет соединени€ с сервером: " << getServerName() << std::endl;
	throw std::exception(sout.str().c_str());
}

std::string Server::showCubes()const {
	if (hServer!=nullptr)	
		return showObjects(hUser, hPool, hServer, TM1ServerCubes(),"кубов");		
	
	std::ostringstream sout;
	sout << "Ќет соединени€ с сервером: " << getServerName() << std::endl;
	throw std::exception(sout.str().c_str());	
}

TM1_INDEX Server::getCountDimensions()const{
	return getCountObjects(hUser, hPool, hServer, TM1ServerDimensions());	
}


std::string Server::showDimensions()const{
	if (hServer!=nullptr)
		return showObjects(hUser, hPool, hServer, TM1ServerDimensions(),"измерений на сервере");
	
	std::ostringstream sout;
	sout << "Ќет соединени€ с сервером: " << getServerName() << std::endl;
	throw std::exception(sout.str().c_str());
}

TM1_INDEX Server::getLastError(std::ostringstream &sout, TM1V val, bool isShow)const noexcept {
	return ::getLastError(sout, hUser, val, isShow);
}

