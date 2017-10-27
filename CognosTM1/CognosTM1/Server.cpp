#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"

Server::Server(const AdminServer &Server):Object(Server.gethUser(), TM1ValPoolCreate(Server.gethUser())){}

Server::~Server() noexcept {
	//дл€ каждого пула следует вызывать этот метод
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

bool Server::isConnected() noexcept {
	if (vName == nullptr 
		&& !uploadName())
		return false;
		
	if (vName != nullptr) {
		//!!!расход пам€ти!!!
		hObject = TM1SystemServerHandle(hUser, const_cast<char*>(getName()));
		if (TM1ValType(hUser, hObject) == TM1ValTypeBool())
			//не удалось проверить соединение
			hObject = nullptr;

		return hObject != nullptr;
	}else
		return false;
}

bool Server::connect(const char *ServerName, TM1_INDEX ServerNameLen, const char *UserName, TM1_INDEX UserNameLen, const char *Password, TM1_INDEX PasswordLen){
	if (hObject == nullptr || !isConnected()){
		if(ServerName==nullptr)
			throw std::exception("ƒл€ установки соединени€ с сервером, нужно указать им€ сервера");

		//выдел€ем место в пуле под значение
		vName = TM1ValString(hPool, const_cast<char*>(ServerName), ServerNameLen);
		
		//авторизуемс€ 
		if (UserName != nullptr){
			//выдел€ем место в пуле под значени€
			TM1V vUserName = TM1ValString(hPool, const_cast<char*>(UserName), UserNameLen), vPassword;
			if(Password!=nullptr)
				vPassword = TM1ValString(hPool, const_cast<char*>(Password), PasswordLen);
			else
				vPassword = TM1ValString(hPool, "", 0);
			//подключение с именем пользовател€ и паролем
			hObject = TM1SystemServerConnect(hPool, vName, vUserName, vPassword);
		}else
			//подключение с интегрированной учетной записью
			hObject = TM1SystemServerConnectIntegratedLogin(hPool, vName);

		//провер€ем успешность соединени€
		if (TM1ValType(hUser, hObject) == TM1ValTypeError()){
			std::ostringstream sout;
			sout << "Ќе удалось подключитьс€ к серверу " << ServerName << ":\n\t";
			getLastError(sout, hObject, true);
			hObject = nullptr;
			std::cerr << sout.str() << std::endl;
			throw std::exception(sout.str().c_str());
		}

		return hObject != nullptr;
	}

	return false;
}

bool Server::disConnect(){
	if (isConnected()){		
		//выдел€ем место в пуле под значение
		TM1V vResult = TM1SystemServerDisconnect(hPool, vName);
		if (!TM1ValBoolGet(hUser, vResult)){
			std::ostringstream sout;
			sout << "Ќе удалось выполнить отключение от сервера " << getName() << ":\n\t";
			getLastError(sout, vResult, true);
			std::cerr << sout.str() << std::endl;
			return false;			
		}
		hObject = nullptr;
	}
	return true;
}


