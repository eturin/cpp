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
	char        * strSerPath  = nullptr; //путь к файлу сертификата
	
	AdminServer * adminServer = nullptr; //текущий админ-сервер
	Server      * server      = nullptr; //текущий сервер
	
	Cube		* cube        = nullptr;    //текущий куб
	std::vector<Dimension *> useDimensions; //текущие измерения 

	Dimension   * dimension   = nullptr;    //текущее измерение
	
	
	std::string strKey   ; //текущий ключ	

	std::string strServer; //имя текущего сервера
	std::string strLogin ; //имя текущего пользователя
	std::string strPwd   ; //пароль текущего пользователя
	
	std::string strCube               ; //имя текущего куба
	std::queue<std::string> qDimension; //имена измерений
	

public:
	Context(char * str, bool isFile, char * strSerPath);
	//деструктор
	~Context();

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