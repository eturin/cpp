#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"

Server::Server(const AdminServer &Server):Object(Server.gethUser(), TM1ValPoolCreate(Server.gethUser())){}

Server::~Server() noexcept {
	//��� ������� ���� ������� �������� ���� �����
	TM1ValPoolDestroy(hPool);
	hPool = nullptr;
}

bool Server::isConnected() noexcept {
	if (vName == nullptr 
		&& !uploadName())
		return false;
		
	if (vName != nullptr) {
		//!!!������ ������!!!
		hObject = TM1SystemServerHandle(hUser, const_cast<char*>(getName()));
		if (TM1ValType(hUser, hObject) == TM1ValTypeBool())
			//�� ������� ��������� ����������
			hObject = nullptr;

		return hObject != nullptr;
	}else
		return false;
}

bool Server::connect(const char *ServerName, TM1_INDEX ServerNameLen, const char *UserName, TM1_INDEX UserNameLen, const char *Password, TM1_INDEX PasswordLen){
	if (hObject == nullptr || !isConnected()){
		if(ServerName==nullptr)
			throw std::exception("��� ��������� ���������� � ��������, ����� ������� ��� �������");

		//�������� ����� � ���� ��� ��������
		vName = TM1ValString(hPool, const_cast<char*>(ServerName), ServerNameLen);
		
		//������������ 
		if (UserName != nullptr){
			//�������� ����� � ���� ��� ��������
			TM1V vUserName = TM1ValString(hPool, const_cast<char*>(UserName), UserNameLen), vPassword;
			if(Password!=nullptr)
				vPassword = TM1ValString(hPool, const_cast<char*>(Password), PasswordLen);
			else
				vPassword = TM1ValString(hPool, "", 0);
			//����������� � ������ ������������ � �������
			hObject = TM1SystemServerConnect(hPool, vName, vUserName, vPassword);
		}else
			//����������� � ��������������� ������� �������
			hObject = TM1SystemServerConnectIntegratedLogin(hPool, vName);

		//��������� ���������� ����������
		if (TM1ValType(hUser, hObject) == TM1ValTypeError()){
			std::ostringstream sout;
			sout << "�� ������� ������������ � ������� " << ServerName << ":\n\t";
			getLastError(sout, hObject, true);
			hObject = nullptr;
			std::cerr << sout.str() << std::endl;
			throw std::exception(sout.str().c_str());
		}

		return hObject != nullptr;
	}

	return false;
}

bool Server::disConnect(){
	if (isConnected()){		
		//�������� ����� � ���� ��� ��������
		TM1V vResult = TM1SystemServerDisconnect(hPool, vName);
		if (!TM1ValBoolGet(hUser, vResult)){
			std::ostringstream sout;
			sout << "�� ������� ��������� ���������� �� ������� " << getName() << ":\n\t";
			getLastError(sout, vResult, true);
			std::cerr << sout.str() << std::endl;
			return false;			
		}
		hObject = nullptr;
	}
	return true;
}


