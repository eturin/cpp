#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"

Server::Server(AdminServer &Server){
	hUser = Server.gethUser();
	if (hUser == nullptr)
		throw std::exception("AdminServer �� ���������������");

	//������� ��� ��� �������� 
	hPool = TM1ValPoolCreate(hUser);
}

Server::~Server(){
	//��� ������� ���� ������� �������� ���� �����
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

char * Server::getServerName()const{	
	return vServerName!=nullptr ? TM1ValStringGet(hUser, vServerName) : nullptr; //ok
}

bool Server::isConnected(){
	if (vServerName != nullptr){
		hServer = TM1SystemServerHandle(hUser, getServerName()); //!!!������ ������!!!
		return hServer != nullptr;
	}else
		return false;
}

bool Server::connect(char *ServerName, TM1_INDEX ServerNameLen, char *UserName, TM1_INDEX UserNameLen, char *Password, TM1_INDEX PasswordLen){
	if (hServer == nullptr || !isConnected()){
		//�������� ����� � ���� ��� ��������
		vServerName = TM1ValString(hPool, ServerName, ServerNameLen);

		//������������ 
		if (UserName != nullptr){
			//�������� ����� � ���� ��� ��������
			TM1V vUserName = TM1ValString(hPool, UserName, UserNameLen);
			TM1V vPassword = TM1ValString(hPool, Password, PasswordLen);
			hServer = TM1SystemServerConnect(hPool, vServerName, vUserName, vPassword);
		}
		else
			//�������� ����� � ���� ��� ��������
			hServer = TM1SystemServerConnectIntegratedLogin(hPool, vServerName);

		//��������� ���������� ����������
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
		//�������� ����� � ���� ��� ��������
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
	
	std::cerr << "��� ���������� � ��������: " << getServerName() << std::endl;
	return 0;
}

void Server::showCubes()const{
	if (hServer!=nullptr)	
		showObjects(hUser, hPool, hServer, TM1ServerCubes(),"�����");		
	else
		std::cerr << "��� ���������� � �������� " << std::endl;
}

TM1_INDEX Server::getCountDimensions()const{
	return getCountObjects(hUser, hPool, hServer, TM1ServerDimensions());	
}


void Server::showDimensions()const{
	if (hServer!=nullptr)
		showObjects(hUser, hPool, hServer, TM1ServerDimensions(),"��������� �� �������");
	else
		std::cerr << "��� ���������� � �������� " << std::endl;

}

TM1_INDEX Server::getLastError(TM1V val, bool isShow)const{
	return ::getLastError(hUser, val, isShow);
}

