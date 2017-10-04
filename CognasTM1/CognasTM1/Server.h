#ifndef SERVER_H
#define SERVER_H

#include "Common.h"
#include "AdminServer.h"

class Server{
private:
	//��������
	TM1U hUser       = nullptr;       //TM1 API session handle
	TM1P hPool       = nullptr;       //���������� ���� ��������
	TM1V hServer     = nullptr;       //���������� ������� � ���� API
	TM1V vServerName = nullptr;       //��� ������� � ���� API
	//������
	TM1_INDEX getLastError(TM1V val, bool isShow = false)const;
public:
	//�����������
	Server(AdminServer &Server);

	//����������
	~Server();

	//������
	char * getServerName()const;	
	bool connect(char *ServerName, TM1_INDEX ServerNameLen=0, char *UserName = nullptr, TM1_INDEX UserNameLen=0, char *Password = nullptr, TM1_INDEX PasswordLen=0);
	bool disConnect();
	bool isConnected();
	TM1U gethUser()const;
	TM1V gethServer()const;

	//���-�� �����
	TM1_INDEX getCountCubes() const;
	//������� ����� �����
	void showCubes()const;
	//���-�� ��������� ����
	TM1_INDEX getCountDimensions() const;
	void showDimensions() const;
};


#endif