#ifndef ADMINSERVER_H
#define ADMINSERVER_H

#include "Common.h"
#include <string>

class AdminServer{
private:		
	TM1U hUser = nullptr;       //TM1 API session handle
	TM1P hPool = nullptr;       //���������� ���� ��������
	
	//������
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow=false)const noexcept;
public:
	//������������
	AdminServer(const char * AdminHost, const char * pathToSert = nullptr);
	//��������� ������������ ����������� � ��������
	AdminServer(const AdminServer &) = delete;
	AdminServer(AdminServer &&)      = delete;
	//��������� ���������
	AdminServer & operator=(const AdminServer &) = delete;
	AdminServer & operator=(AdminServer &&) = delete;
	//�����������
	virtual ~AdminServer() noexcept;

	//��������� ����������� ������
	inline TM1U gethUser()const noexcept {return hUser;}

	//��������� ������ TM1 API
	int getVersion() const noexcept;
	//��������� ����� �����-������� (������������ �� ���������)
	const char * getAdminServer() const noexcept;
	//��������� ���-�� ������������������ ��������
	TM1_INDEX getCountServers() const noexcept;
	//������� ������ ��� ������
	std::string showServers()const noexcept;
};



#endif