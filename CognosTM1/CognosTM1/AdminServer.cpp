#define  _CRT_SECURE_NO_WARNINGS
#include "AdminServer.h"
#include <cstring>
#include <sstream>


AdminServer::AdminServer(const char * AdminHost, const char * pathToSert) {
	if (AdminHost == nullptr)
		throw std::exception("AdminHost не может быть nullptr");
	
	//все TM1 Applications начинают с  TM1APIInitialize() и  TM1SystemOpen().
	TM1APIInitialize();     //инициализация памяти API
	hUser = TM1SystemOpen();//получение TM1 API session handle

	//устанавливаем сертификат соединению (должен лежать в папке SSL, рядом с dll или exe)
	if(pathToSert!=nullptr)
		TM1SystemSetAdminSSLCertAuthority(hUser, const_cast<char*>(pathToSert));

	//вызов TM1SystemAdminHostSet для установки AdminServer, к которому будем подключаться
	TM1SystemAdminHostSet(hUser, const_cast<char*>(AdminHost));

	//создаем пулы значений 
	hPool = TM1ValPoolCreate(hUser);
}

AdminServer::~AdminServer() noexcept {
	//для каждого пула следует вызывать этот метод
	TM1ValPoolDestroy(hPool); 
	hPool = nullptr;

	//освобождение ресурсов
	TM1SystemClose(hUser);

	//освобождение памяти API
	TM1APIFinalize();		
}

TM1U AdminServer::gethUser()const noexcept {
	return hUser;
}

int AdminServer::getVersion()const noexcept {
	return TM1SystemVersionGet(); //ok
}

//получение имени AdminHost из API
const char * AdminServer::getAdminServer()const noexcept {
	return TM1SystemAdminHostGet(hUser); //ok
}

TM1_INDEX AdminServer::getCountServers()const noexcept {
	//получение(обновление) информации с Admin-сервера
	TM1SystemServerReload(hUser); //ok
		
	return TM1SystemServerNof(hUser);//ok
}

std::string AdminServer::showServers()const noexcept {
	std::ostringstream sout;
	TM1_INDEX cnt = getCountServers();
	for (TM1_INDEX i = 0; i < cnt; ++i){	
		char * strServerName = TM1SystemServerName(hUser, i+1);//ok
		if (strServerName == nullptr)
			continue;
		TM1V hServer = TM1SystemServerHandle(hUser, strServerName);//!!!расход памяти!!!
		if (hServer)
			sout << "    [CONNECTED] " << strServerName << std::endl;
		else
			sout << "[NOT CONNECTED] " << strServerName << std::endl;
	}
	return sout.str();
}

TM1_INDEX AdminServer::getLastError(std::ostringstream &sout, TM1V val, bool isShow)const noexcept {
	return ::getLastError(sout, hUser,val,isShow);
}