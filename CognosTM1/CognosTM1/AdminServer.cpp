#define  _CRT_SECURE_NO_WARNINGS
#include "AdminServer.h"
#include <cstring>
#include <sstream>


AdminServer::AdminServer(const char * AdminHost, const char * pathToSert) {
	if (AdminHost == nullptr)
		throw std::exception("AdminHost �� ����� ���� nullptr");
	
	//��� TM1 Applications �������� �  TM1APIInitialize() �  TM1SystemOpen().
	TM1APIInitialize();     //������������� ������ API
	hUser = TM1SystemOpen();//��������� TM1 API session handle

	//������������� ���������� ���������� (������ ������ � ����� SSL, ����� � dll ��� exe)
	if(pathToSert!=nullptr)
		TM1SystemSetAdminSSLCertAuthority(hUser, const_cast<char*>(pathToSert));

	//����� TM1SystemAdminHostSet ��� ��������� AdminServer, � �������� ����� ������������
	TM1SystemAdminHostSet(hUser, const_cast<char*>(AdminHost));

	//������� ���� �������� 
	hPool = TM1ValPoolCreate(hUser);
}

AdminServer::~AdminServer() noexcept {
	//��� ������� ���� ������� �������� ���� �����
	TM1ValPoolDestroy(hPool); 
	hPool = nullptr;

	//������������ ��������
	TM1SystemClose(hUser);

	//������������ ������ API
	TM1APIFinalize();		
}

TM1U AdminServer::gethUser()const noexcept {
	return hUser;
}

int AdminServer::getVersion()const noexcept {
	return TM1SystemVersionGet(); //ok
}

//��������� ����� AdminHost �� API
const char * AdminServer::getAdminServer()const noexcept {
	return TM1SystemAdminHostGet(hUser); //ok
}

TM1_INDEX AdminServer::getCountServers()const noexcept {
	//���������(����������) ���������� � Admin-�������
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
		TM1V hServer = TM1SystemServerHandle(hUser, strServerName);//!!!������ ������!!!
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