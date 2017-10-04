#define _CRT_SECURE_NO_WARNINGS

#include "Context.h"

Context::Context(char * str, bool isFile, char * strSerPath):strSerPath(strSerPath){
	rapidjson::Reader reader;
	if (isFile){
		std::ifstream fin(str);
		if (fin.is_open()){
			rapidjson::IStreamWrapper isw(fin);
			reader.Parse(isw, *this);			
			fin.close();			
		}
	}else{
		rapidjson::StringStream ss(str);
		reader.Parse(ss, *this);
	}
}

Context::~Context(){
	for (int i = 0, len = useDimensions.size(); i < len; ++i)
		delete useDimensions[i];

	delete dimension, cube ,server, adminServer;
	dimension   = nullptr;
	cube        = nullptr;
	server      = nullptr;
	adminServer = nullptr;	
}

bool Context::Key(const char* str, rapidjson::SizeType length, bool copy) {
	strKey.erase(strKey.rfind('/') + 1, std::string::npos);
	strKey += str;	
	return true;
}

bool Context::String(const char* str, rapidjson::SizeType length, bool copy) {
	if (strKey == "/adminServer"){
		//�������� ��� �����-�������		
		adminServer = new AdminServer(const_cast<char*>(str), strSerPath);
		delete server;
		server      = new Server(*adminServer);
	}else if (strKey == "/server"){
		//�������� ��� �������
		strServer    = str;		
	}else if (strKey == "/login"){
		//�������� ��� ������������
		strLogin = str;		
	}else if (strKey == "/pwd") {
		//������� ������
		strPwd = str;
	}else if(strKey == "/create/dimensionName"){
		//[�������� ��������] �������� ��� ���������
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			delete dimension;
			dimension = new Dimension(*server, const_cast<char*>(str), length);
			dimension->makeNew();
		}else
			throw std::exception("���������� �� �����������");
	}else if (strKey == "/create/elements") {
		//[�������� ��������] �������� ��� ��������
		if (dimension==nullptr)
			throw std::exception("����������� ���������");
		else if(!dimension->addElement(const_cast<char*>(str), length))
			throw std::exception("�� ������� �������� ������� � ����������� ���������");		
	}else if (strKey == "/delete/dimensionName") {
		//[�������� ��������] �������� ��� ���������
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			try {
				Dimension dimension(*server, const_cast<char*>(str), length);
				if(!dimension.deleteDimension())
					throw std::exception("�� ������� ������� ���������");
			}catch (std::exception &e) {
				//��������� ��� �� �������
			}
		}else
			throw std::exception("���������� �� �����������");
	}else if (strKey == "/create/cubeName") {
		//[�������� ��������] �������� ��� ����
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			delete cube;
			cube = new Cube(*server, const_cast<char*>(str), length);
		}else
			throw std::exception("���������� �� �����������");
	}else if (strKey == "/create/dimensions") {
		//[�������� ��������] �������� ��� ��������� (��� ����)
		cube->addDimension(const_cast<char*>(str), length);
	}else if (strKey == "/delete/cubeName") {
		//[�������� ��������] �������� ��� ���������
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			try {
				Cube cube(*server, const_cast<char*>(str), length);
				cube.deleteCube();
			}catch (std::exception &e) {
				//���� ��� �� �������
			}
		}else
			throw std::exception("���������� �� �����������");
	}else if (strKey == "/set/cubeName"){
		//[�������� ������ � ���] �������� ��� ���� 		
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			delete cube;
			cube = new Cube(*server, const_cast<char*>(str), length);
			for (int i = 0, len = useDimensions.size(); i < len;++i)
				delete useDimensions[i];
			useDimensions.resize(cube->getCountDimensions());
		}else
			throw std::exception("���������� �� �����������");
	}else if (strKey =="/set/val/v") {
		//[�������� ������ � ���] �������� �������� 		
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			//���������� � ���			
			cube->setCellValue(useDimensions, const_cast<char*>(str));
		}else
			throw std::exception("���������� �� �����������");
	}else if (strKey.find("/set/val")==0) {
		//[�������� ������ � ���] �������� ��������� 		
		if (cube!=nullptr) {
			//��������� �������� ��������� �� ������ � �����
			auto pos=strKey.rfind('/');
			int cnt=0;
			std::sscanf(strKey.substr(pos + 1, std::string::npos).c_str(), "%d", &cnt);
			if (cnt>useDimensions.size())
				throw std::exception("� ���� ��� ������� ���������");
			else if(useDimensions[cnt - 1]==nullptr)
				useDimensions[cnt - 1] = new Dimension(*cube, cnt);

			useDimensions[cnt - 1]->setElementVal(const_cast<char*>(str));

		}else
			throw std::exception("��� ���������� �������� ���������, ����� ������� ���");
	}

	return true;
}
bool Context::Null(){
	return true;
}
bool Context::Bool(bool b) {
	return true;
}
bool Context::Int(int i) {
	return true;
}
bool Context::Uint(unsigned u) {
	return true;
}
bool Context::Int64(int64_t i)  {
	return true;
}
bool Context::Uint64(uint64_t u) {
	return true;
}
bool Context::Double(double d) {
	if (strKey == "/set/val/v") {
		//[�������� ������ � ���] �������� �������� 		
		if (cube!=nullptr) {
			//���������� � ���
			cube->setCellValue(useDimensions, d);
		}else
			throw std::exception("���, ��� ������������ ��������, �� ���������������");
	}else if (strKey.find("/set/val") == 0) {
		//[�������� ������ � ���] �������� ��������� 		
		if (server->isConnected() //���������/������������ ����������
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			//��������� �������� ��������� �� ������ � �����
			auto pos = strKey.rfind('/');
			int cnt = 0;
			std::sscanf(strKey.substr(pos + 1, std::string::npos).c_str(), "%d", &cnt);
			if (cnt > useDimensions.size())
				throw std::exception("� ���� ��� ������� ���������");
			else if(cube==nullptr)
				throw std::exception("��� ���������� �������� ���������, ����� ������� ���");
			else if (useDimensions[cnt - 1] == nullptr)
				useDimensions[cnt - 1] = new Dimension(*cube, cnt);

			useDimensions[cnt - 1]->setElementVal(d);

		}else
			throw std::exception("���������� �� �����������");
	}

	return true;
}
bool Context::RawNumber(const char* str, rapidjson::SizeType length, bool copy) {
	return true;
}

bool Context::StartObject() {
	strKey += '/';
	return true;
}
bool Context::EndObject(rapidjson::SizeType memberCount) {
	strKey.erase(strKey.rfind('/'), std::string::npos);
	return true;
}

bool Context::StartArray() {
	return true;
}
bool Context::EndArray(rapidjson::SizeType elementCount) {
	if (strKey == "/create/dimensions") {
		//[�������� ��������] �������� ��� ����� ��������� ������������ ����
		if (cube != nullptr 
			&& !cube->exist()
			&& cube->makeNew()
			&& cube->registerCube()) 
			;//��� �����������
		else
			throw std::exception("�� ������� ������������ ���");
	}else if (strKey == "/create/elements") {
		//[�������� ��������] �������� ��� �������� ���������
		if (dimension != nullptr			
			&& dimension->registerDimension())
			;//��������� ������������
		else
			throw std::exception("�� ������� ������������ ���������");
	}
	return true;
}