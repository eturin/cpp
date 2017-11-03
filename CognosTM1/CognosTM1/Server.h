#ifndef SERVER_H
#define SERVER_H

#include "Common.h"
#include "Object.h"

#include "AdminServer.h"

class Server: public Object{
private:
	//��������� ������
	//inline virtual TM1V gethNewObject() const noexcept override ;
	//inline virtual bool deleteObject() noexcept override;
	//inline virtual bool makeDuplicate() override;
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
	virtual ~Server() noexcept override;
	
	//�������� ����������
	bool isConnected() noexcept;
	inline virtual bool exist(bool isPublic = true) noexcept override {
		return isConnected();
	}
	//�����������
	bool connect(const char *ServerName, TM1_INDEX ServerNameLen=0, const char *UserName=nullptr, TM1_INDEX UserNameLen=0, const char *Password=nullptr, TM1_INDEX PasswordLen=0);
	//����������
	bool disConnect();
	
	//��������� ���-�� �����
	inline TM1_INDEX getCountCubes() const {
		return getListCount(TM1ServerCubes());
	}
	//�������� ����
	inline std::string showCubes()const {
		return std::move(showList(TM1ServerCubes()));
	}
	
	//��������� ���-�� ��������� �������
	inline TM1_INDEX getCountDimensions() const {		
		return getListCount(TM1ServerDimensions());
	}
	//�������� ���������
	inline std::string showDimensions() const {
		return std::move(showList(TM1ServerDimensions()));
	}
};


#endif