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

#include "rapidjson/reader.h"
#include "rapidjson/istreamwrapper.h"


class Context{
private:
	char        * strSerPath  = nullptr; //���� � ����� �����������
	
	AdminServer * adminServer = nullptr; //������� �����-������
	Server      * server      = nullptr; //������� ������
	
	Cube		* cube        = nullptr;    //������� ���
	std::vector<Dimension *> useDimensions; //������� ��������� 

	Dimension   * dimension   = nullptr;    //������� ���������
	
	
	std::string strKey   ; //������� ����	

	std::string strServer; //��� �������� �������
	std::string strLogin ; //��� �������� ������������
	std::string strPwd   ; //������ �������� ������������
	
	std::string strCube               ; //��� �������� ����
	std::queue<std::string> qDimension; //����� ���������
	

public:
	Context(char * str, bool isFile, char * strSerPath);
	//����������
	~Context();

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