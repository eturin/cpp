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
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept;
public:
	//�����������
	Server(const AdminServer &Server);
	//��������� ������������ ����������� � ��������
	Server(const Server &) = delete;
	Server(Server &&) = delete;
	//��������� ���������
	Server & operator=(const Server &) = delete;
	Server & operator=(Server &&) = delete;
	//����������
	~Server() noexcept;

	//��������� ����� ������� (������������ �� ���������)
	const char * getServerName()const noexcept;
	//�������� ����������
	bool isConnected() noexcept;
	//�����������
	bool connect(const char *ServerName, TM1_INDEX ServerNameLen=0, const char *UserName = nullptr, TM1_INDEX UserNameLen=0, const char *Password = nullptr, TM1_INDEX PasswordLen=0);
	//����������
	bool disConnect();
	//��������� ����������� ������
	TM1U gethUser()const noexcept;
	//��������� ����������� �������
	TM1V gethServer()const noexcept;
	//��������� ����������� ����
	TM1P gethPool()const noexcept;


	//��������� ���-�� �����
	TM1_INDEX getCountCubes() const;
	//�������� ����
	std::string showCubes()const;
	//��������� ���-�� ��������� �������
	TM1_INDEX getCountDimensions() const;
	//�������� ���������
	std::string showDimensions() const;
};


#endif