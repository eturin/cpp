#define  _CRT_SECURE_NO_WARNINGS
#include "AdminServer.h"
#include <cstring>

AdminServer::AdminServer(char * AdminHost, char * pathToSert){
	if (AdminHost == nullptr)
		throw std::exception("AdminHost не может быть nullptr");
	
	//все TM1 Applications начинают с  TM1APIInitialize() и  TM1SystemOpen().
	TM1APIInitialize();     //инициализация памяти API
	hUser = TM1SystemOpen();//получение TM1 API session handle

	//устанавливаем сертификат соединению (должен лежать в папке SSL, рядом с dll или exe)
	if(pathToSert!=nullptr)
		TM1SystemSetAdminSSLCertAuthority(hUser, pathToSert);

	//вызов TM1SystemAdminHostSet для установки AdminServer, к которому будем подключаться
	TM1SystemAdminHostSet(hUser, AdminHost);

	//создаем пулы значений 
	hPool = TM1ValPoolCreate(hUser);
}

AdminServer::~AdminServer(){
	//для каждого пула следует вызывать этот метод
	TM1ValPoolDestroy(hPool); 
	hPool = nullptr;

	//освобождение ресурсов
	TM1SystemClose(hUser);

	//освобождение памяти API
	TM1APIFinalize();		
}

TM1U AdminServer::gethUser()const{
	return hUser;
}

int AdminServer::getVersion()const{
	return TM1SystemVersionGet(); //ok
}

//получение имени AdminHost из API
char * AdminServer::getAdminServer()const{	
	return TM1SystemAdminHostGet(hUser); //ok
}

TM1_INDEX AdminServer::getCountServers()const{
	//получение(обновление) информации с Admin-сервера
	TM1SystemServerReload(hUser); //ok
		
	return TM1SystemServerNof(hUser);//ok
}

void AdminServer::showServers()const{
	std::cout << "Версия  TM1    - " << getVersion() 
		    << "\nAdmin-сервер   - " << getAdminServer()<<std::endl;
	int cnt = getCountServers();
	std::cout << "Всего серверов - " << cnt << std::endl;
	for (TM1_INDEX i = 0; i < cnt; ++i){	
		char * strServerName = TM1SystemServerName(hUser, i+1);//ok
		TM1V hServer = TM1SystemServerHandle(hUser, strServerName);//!!!расход памяти!!!
		if (hServer)
			std::cout << "    [CONNECTED] " << strServerName << std::endl;
		else
			std::cout << "[NOT CONNECTED] " << strServerName << std::endl;
	}
}

TM1_INDEX AdminServer::getLastError(TM1V val, bool isShow)const{	
	return ::getLastError(hUser,val,isShow);
}