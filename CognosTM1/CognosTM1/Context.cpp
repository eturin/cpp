#define _CRT_SECURE_NO_WARNINGS

#include "Context.h"

//�������� ������ �������
char* strError[] = {"No error.",
				    "The document is empty.",
				    "The document root must not follow by other values.",
				    "Invalid value.",
				    "Missing a name for object member.",
				    "Missing a colon after a name of object member.",
				    "Missing a comma or '}' after an object member.",
				    "Missing a comma or ']' after an array element.",
				    "Incorrect hex digit after \\u escape in string.",
				    "The surrogate pair in string is invalid.",
				    "Invalid escape character in string.",
				    "Missing a closing quotation mark in string.",
				    "Invalid encoding in string.",
				    "Number too big to be stored in double.",
				    "Miss fraction part in number.",
				    "Miss exponent in number.",
				    "Parsing was terminated.",
				    "Unspecific syntax error." };

Context::Context(char * str, bool isFile, const char * strSerPath):strSerPath(strSerPath){
	rapidjson::Reader reader;
	if (isFile){
		//json � �����
		std::ifstream fin(str);
		if (fin.is_open()){
			rapidjson::IStreamWrapper isw(fin);
			reader.Parse(isw, *this);
			fin.close();
			if (reader.HasParseError()) {				
				std::stringstream sout;
				sout << "������ �������� json:\n\t[" << reader.GetParseErrorCode() << "] �� �������� - " << reader.GetErrorOffset();				
				throw std::exception(sout.str().c_str());
			}
		}
	}else{
		//json � �������
		rapidjson::StringStream ss(str);
		reader.Parse(ss, *this);
		if (reader.HasParseError()) {
			std::size_t offset = reader.GetErrorOffset();
			std::stringstream sout;
			sout << "������ �������� json:\n\t[" << reader.GetParseErrorCode() << "] "<< strError[reader.GetParseErrorCode()] 
				<<":\n\t �� �������� - " << offset << " ����� "
				<< "<.. "+std::string(str, offset>20 ? offset-20 : 0,20)+"> � <"<<std::string(str+offset,20)<<" ..>";
			throw std::exception(sout.str().c_str());
		}
	}
}

bool Context::Key(const char* str, rapidjson::SizeType length, bool copy) {
	strKey.erase(strKey.rfind('/') + 1, std::string::npos);
	std::string key = std::string(str, length);
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	strKey += key;
	return true;
}

bool Context::String(const char* str, rapidjson::SizeType length, bool copy) {
	if (strKey == "/adminserver"){
		//�������� ��� �����-�������		
		upAdminServer = std::make_unique<AdminServer>(const_cast<char*>(str), strSerPath);		
	}else if (strKey == "/server/name"){
		//�������� ��� �������
		strServer    = std::string(str, length);
	}else if (strKey == "/server/login"){
		//�������� ��� ������������
		strLogin = std::string(str, length);
	}else if (strKey == "/server/pwd") {
		//������� ������
		strPwd = std::string(str, length);
	}else if(strKey == "/create/dimension/name"){
		//[�������� ��������] �������� ��� ���������
		if (upServer.get() == nullptr) 
			throw std::exception("�� ����������� ���������� � �������� �������.");
		
		upDimension = std::make_unique<Dimension>(*upServer, str, length);
		upDimension->makeNew();		
	}else if (strKey == "/create/dimension/elements") {
		//[�������� ��������] �������� ��� ��������
		if (upDimension.get() == nullptr)
			throw std::exception("��� ��������� ������ ������������� ������������ ���������");
		else
			upDimension->addElement(str, length);			
	}else if (strKey == "/delete/dimension") {
		//[�������� �������� ���������] �������� ��� ���������
		if (upServer.get() == nullptr)
			throw std::exception("�� ����������� ���������� � �������� �������.");

		Dimension dimension(*upServer, str, length);
		if (!dimension.deleteObject())
			throw std::exception("�� ������� ������� ���������");
	}else if (strKey == "/create/cube/name") {
		//[�������� �������� ����] �������� ��� ����
		if (upServer.get() == nullptr)
			throw std::exception("�� ����������� ���������� � �������� �������.");

		upCube = std::make_unique<Cube>(*upServer, str, length);		
	}else if (strKey == "/create/cube/dimensions") {
		//[�������� �������� ����] �������� ��� ��������� )
		if(upCube.get()==nullptr)
			throw std::exception("��� ���� ������ �������������� ������������ ���������.");

		upCube->addDimension(str, length);
	}else if (strKey == "/delete/cube") {
		//[�������� �������� ����] �������� ��� ���������
		if (upServer.get() == nullptr)
			throw std::exception("�� ����������� ���������� � �������� �������.");

		Cube cube(*upServer, str, length);
		cube.deleteObject();		
	}else if (strKey == "/delete/subsets/dimension") {
		//[�������� �������� ������������] �������� ��� ���������
		if (upServer.get() == nullptr)
			throw std::exception("�� ����������� ���������� � �������� �������.");

		upDimension = std::make_unique<Dimension>(*upServer, str, length);		
	}else if (strKey == "/create/subset/dimension") {
		//[�������� �������� ������������] �������� ��� ���������
		if (upServer.get() == nullptr)
			throw std::exception("�� ����������� ���������� � �������� �������.");

		upDimension = std::make_unique<Dimension>(*upServer, str, length);
	}else if (strKey == "/create/subset/name") {
		//[�������� �������� ������������] �������� ��� ���������
		if (upDimension.get() == nullptr)
			throw std::exception("��� ��������� ������ �������������� ����� ������������ ������������.");

		upSubset = std::make_unique<Subset>(*upDimension, str, length);
		upSubset->makeNew();
	}else if (strKey == "/create/subset/elements") {
		//[�������� �������� ������������] �������� ��� ��������
		if (upSubset.get() == nullptr)
			throw std::exception("��� ������������ ������ ������������� ������������ ���������");
		
		upSubset->addElement(str, length);
	}else if (strKey == "/create/subset/mdx") {
		//[�������� �������� ������������] ������� MDX-������
		if (upSubset.get() == nullptr)
			throw std::exception("��� ������������ ������ ������������� ������������ ���������");
		
		upSubset->makeNewWithMDX(str, length);
		upSubset->registerSubset();
		upSubset.release();
		upDimension.release();
	}else if (strKey == "/delete/subsets/names") {
		//[�������� �������� ������������] �������� ��� ������������
		if(upDimension.get()==nullptr)
			throw std::exception("�������� ��������� ������ �������������� ������ ��������� �����������.");

		Subset subset(*upDimension, str, length);
		subset.deleteObject();
	}else if (strKey == "/set/cube/name"){
		//[�������� ������ � ���] �������� ��� ���� 		
		if (upServer.get() == nullptr)
			throw std::exception("�� ����������� ���������� � �������� �������.");

		upCube = std::make_unique<Cube>(*upServer, str, length);
		useDimensions.resize(upCube->getCountDimensions());		
	}else if (strKey =="/set/cube/val/v") {
		//[�������� ������ � ���] �������� �������� 		
		if (upCube.get() == nullptr) 
			throw std::exception("������������ � ��� �������� ����� ��������� ����� ����� ����");			
		else
			upCube->setCellValue(useDimensions, str, length);
	}else if (strKey.find("/set/cube/val")==0) {
		//[�������� ������ � ���] �������� ��������� 		
		if (upCube.get()==nullptr) 
			throw std::exception("������������ ��������� ��� ������ ������ ��������� ����� ����� ����"); 
		else{
			//��������� �������� ��������� �� ������ � �����
			auto pos=strKey.rfind('/');
			int cnt=0;
			std::sscanf(strKey.substr(pos + 1, std::string::npos).c_str(), "%d", &cnt);
			if (cnt>useDimensions.size())
				throw std::exception("� ���� ��� ������� ���������");
			else if(useDimensions[cnt - 1].get() ==nullptr)
				useDimensions[cnt - 1] = std::make_unique<Dimension>(*upCube, cnt);

			useDimensions[cnt - 1]->setElementVal(str, length);
		}
			
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
	if (strKey == "/set/cube/val/v") {
		//[�������� ������ � ���] �������� �������� 		
		if (upCube.get() == nullptr)
			throw std::exception("������������ � ��� �������� ����� ��������� ����� ����� ����");
		else
			upCube->setCellValue(useDimensions, d);			
	}else if (strKey.find("/set/cube/val") == 0) {
		//[�������� ������ � ���] �������� ��������� 		
		if (upCube.get() == nullptr) 
			throw std::exception("������������ ��������� ��� ������ ������ ��������� ����� ����� ����");
		else{
			//��������� �������� ��������� �� ������ � �����
			auto pos = strKey.rfind('/');
			int cnt = 0;
			std::sscanf(strKey.substr(pos + 1, std::string::npos).c_str(), "%d", &cnt);
			if (cnt>useDimensions.size())
				throw std::exception("� ���� ��� ������� ���������");
			else if (useDimensions[cnt - 1].get() == nullptr)
				useDimensions[cnt - 1] = std::make_unique<Dimension>(*upCube, cnt);

			useDimensions[cnt - 1]->setElementVal(d);
		}
		
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
	if (strKey == "/create/dimension") 
		upDimension.release();
	else if (strKey == "/delete/subsets")
		upDimension.release();
	else if (strKey == "/create/subset") {
		upSubset.release();
		upDimension.release();
	}else if (strKey == "/server") { //������������� ���������� � ��������
		upServer = std::make_unique<Server>(*upAdminServer);
		upServer->connect(strServer.c_str(), 0, strLogin.c_str(), 0, strPwd.c_str(), 0);		
	}

	return true;
}

bool Context::StartArray() {
	return true;
}
bool Context::EndArray(rapidjson::SizeType elementCount) {
	if (strKey == "/create/cube/dimensions") {
		//[�������� ��������] �������� ��� ����� ��������� ������������ ����
		if (upCube.get() != nullptr 
			&& !upCube->exist()
			&& upCube->makeNew()
			&& upCube->registerCube())
			;//��� �����������
		else
			throw std::exception("�� ������� ������������ ���");
	}else if (strKey == "/create/dimension/elements") {
		//[�������� �������� ���������] �������� ��� �������� ���������
		if (upDimension.get() != nullptr			
			&& upDimension->registerDimension())
			//��������� ������������
			upDimension.release();
		else
			throw std::exception("�� ������� ������������ ���������");
	}else if (strKey == "/create/subset/elements") {
		//[�������� �������� ������������] �������� ��� �������� ���������
		if (upSubset.get() != nullptr
			&& upSubset->registerSubset()) {
			//������������ ������������
			upSubset.release();
			upDimension.release();
		}else
			throw std::exception("�� ������� ������������ ���������");
	}
	return true;
}