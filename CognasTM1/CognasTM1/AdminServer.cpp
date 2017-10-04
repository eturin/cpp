#define  _CRT_SECURE_NO_WARNINGS
#include "AdminServer.h"
#include <cstring>

AdminServer::AdminServer(char * AdminHost, char * pathToSert){
	if (AdminHost == nullptr)
		throw std::exception("AdminHost �� ����� ���� nullptr");
	
	//��� TM1 Applications �������� �  TM1APIInitialize() �  TM1SystemOpen().
	TM1APIInitialize();     //������������� ������ API
	hUser = TM1SystemOpen();//��������� TM1 API session handle

	//������������� ���������� ���������� (������ ������ � ����� SSL, ����� � dll ��� exe)
	if(pathToSert!=nullptr)
		TM1SystemSetAdminSSLCertAuthority(hUser, pathToSert);

	//����� TM1SystemAdminHostSet ��� ��������� AdminServer, � �������� ����� ������������
	TM1SystemAdminHostSet(hUser, AdminHost);

	//������� ���� �������� 
	hPool = TM1ValPoolCreate(hUser);
}

AdminServer::~AdminServer(){
	//��� ������� ���� ������� �������� ���� �����
	TM1ValPoolDestroy(hPool); 
	hPool = nullptr;

	//������������ ��������
	TM1SystemClose(hUser);

	//������������ ������ API
	TM1APIFinalize();		
}

TM1U AdminServer::gethUser()const{
	return hUser;
}

int AdminServer::getVersion()const{
	return TM1SystemVersionGet(); //ok
}

//��������� ����� AdminHost �� API
char * AdminServer::getAdminServer()const{	
	return TM1SystemAdminHostGet(hUser); //ok
}

TM1_INDEX AdminServer::getCountServers()const{
	//���������(����������) ���������� � Admin-�������
	TM1SystemServerReload(hUser); //ok
		
	return TM1SystemServerNof(hUser);//ok
}

void AdminServer::showServers()const{
	std::cout << "������  TM1    - " << getVersion() 
		    << "\nAdmin-������   - " << getAdminServer()<<std::endl;
	int cnt = getCountServers();
	std::cout << "����� �������� - " << cnt << std::endl;
	for (TM1_INDEX i = 0; i < cnt; ++i){	
		char * strServerName = TM1SystemServerName(hUser, i+1);//ok
		TM1V hServer = TM1SystemServerHandle(hUser, strServerName);//!!!������ ������!!!
		if (hServer)
			std::cout << "    [CONNECTED] " << strServerName << std::endl;
		else
			std::cout << "[NOT CONNECTED] " << strServerName << std::endl;
	}
}

TM1_INDEX AdminServer::getLastError(TM1V val, bool isShow)const{	
	return ::getLastError(hUser,val,isShow);
}