#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <fstream>
#include <queue>
#include <vector>

#include "AdminServer.h"
#include "Server.h"
#include "Cube.h"
#include "Dimension.h"
#include "Subset.h"
#include "View.h"

#include "rapidjson/reader.h"
#include "rapidjson/istreamwrapper.h"


class Context{
private:
	const char * strSerPath;                              //���� � ����� �����������
	
	std::unique_ptr<AdminServer> upAdminServer;           //������� �����-������
	std::unique_ptr<Server>      upServer;                //������� ������
	
	std::unique_ptr<Cube>        upCube;                  //������� ���
	std::vector<std::unique_ptr<Dimension>> useDimensions;//������� ��������� 

	std::unique_ptr<Dimension>   upDimension;             //������� ���������
	std::unique_ptr<Subset>      upSubset;                //������� ������������
		
	std::string strKey   ; //������� ����	
	std::string strServer; //��� �������� �������
	std::string strLogin ; //��� �������� ������������
	std::string strPwd   ; //������ �������� ������������
	
	std::string strCube               ; //��� �������� ����
	std::queue<std::string> qDimension; //����� ���������
	

public:
	Context(char * str, bool isFile = false, const char * strSerPath = nullptr);
	//����������
	~Context() = default;

	//������ ����
	bool Key(const char* str, rapidjson::SizeType length, bool copy);
	//������� ��������
	bool String(const char* str, rapidjson::SizeType length, bool copy);
	bool Null();
	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned u);
	bool Int64(int64_t i);
	bool Uint64(uint64_t u);
	bool Double(double d);
	bool RawNumber(const char* str, rapidjson::SizeType length, bool copy);

	//������ ������� ������ � �������� �������
	bool StartObject();
	//���������� ������� ������
	bool EndObject(rapidjson::SizeType memberCount);

	//������ ������
	bool StartArray();
	//���������� ������
	bool EndArray(rapidjson::SizeType elementCount);
};

#endif