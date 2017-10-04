#ifndef ADMINSERVER_H
#define ADMINSERVER_H

#include "Common.h"

class AdminServer{
private:		
	TM1U hUser = nullptr;       //TM1 API session handle
	TM1P hPool = nullptr;       //���������� ���� ��������
	
	//������
	TM1_INDEX getLastError(TM1V val, bool isShow=false)const;
public:
	//������������
	AdminServer(char * AdminHost, char * pathToSert = nullptr);
	AdminServer(const AdminServer &) = delete;
	//���������
	AdminServer & operator=(const AdminServer &) = delete;
	//�����������
	~AdminServer();

	//������	
	TM1U gethUser()const;

	//������ TM1
	int getVersion() const;
	//��� ����� ������� (������������ �� ���������)
	char * getAdminServer() const;
	//���-�� ��� ������
	TM1_INDEX getCountServers() const;
	//������� ������ ��� ������
	void showServers()const;
	

};



#endif