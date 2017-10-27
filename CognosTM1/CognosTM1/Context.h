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
	const char * strSerPath;                              //путь к файлу сертификата
	
	std::unique_ptr<AdminServer> upAdminServer;           //текущий админ-сервер
	std::unique_ptr<Server>      upServer;                //текущий сервер
	
	std::unique_ptr<Cube>        upCube;                  //текущий куб
	std::vector<std::unique_ptr<Dimension>> useDimensions;//текущие измерения 

	std::unique_ptr<Dimension>   upDimension;             //текущее измерение
	std::unique_ptr<Subset>      upSubset;                //текущее подмножество
		
	std::string strKey   ; //текущий ключ	
	std::string strServer; //имя текущего сервера
	std::string strLogin ; //имя текущего пользователя
	std::string strPwd   ; //пароль текущего пользователя
	
	std::string strCube               ; //имя текущего куба
	std::queue<std::string> qDimension; //имена измерений
	

public:
	Context(char * str, bool isFile = false, const char * strSerPath = nullptr);
	//деструктор
	~Context() = default;

	//найден ключ
	bool Key(const char* str, rapidjson::SizeType length, bool copy);
	//найдены значения
	bool String(const char* str, rapidjson::SizeType length, bool copy);
	bool Null();
	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned u);
	bool Int64(int64_t i);
	bool Uint64(uint64_t u);
	bool Double(double d);
	bool RawNumber(const char* str, rapidjson::SizeType length, bool copy);

	//найден сложный объект в фигурных скобках
	bool StartObject();
	//закончился сложный объект
	bool EndObject(rapidjson::SizeType memberCount);

	//найден список
	bool StartArray();
	//закончился список
	bool EndArray(rapidjson::SizeType elementCount);
};

#endif