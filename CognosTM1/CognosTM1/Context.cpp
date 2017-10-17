#define _CRT_SECURE_NO_WARNINGS

#include "Context.h"

char* strError[] = { "No error.",
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

Context::Context(char * str, bool isFile, char * strSerPath):strSerPath(strSerPath){
	rapidjson::Reader reader;
	if (isFile){
		std::ifstream fin(str);
		if (fin.is_open()){
			rapidjson::IStreamWrapper isw(fin);
			reader.Parse(isw, *this);
			fin.close();
			if (reader.HasParseError()) {
				std::stringstream sout;
				sout << "Ошибка парсинга json:\n\t[" << reader.GetParseErrorCode() << "] на смещение - " << reader.GetErrorOffset();
				throw std::exception(sout.str().c_str());
			}
		}
	}else{
		rapidjson::StringStream ss(str);
		reader.Parse(ss, *this);
		if (reader.HasParseError()) {
			std::size_t offset = reader.GetErrorOffset();
			std::stringstream sout;
			sout << "Ошибка парсинга json:\n\t[" << reader.GetParseErrorCode() << "] "<< strError[reader.GetParseErrorCode()] 
				<<":\n\t на смещение - " << offset << " между "
				<< "<.. "+std::string(str, offset>20 ? offset-20 : 0,20)+"> и <"<<std::string(str+offset,20)<<" ..>";
			throw std::exception(sout.str().c_str());
		}
	}
}

Context::~Context(){
	for (std::size_t i = 0, len = useDimensions.size(); i < len; ++i)
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
		//получено имя админ-сервера		
		adminServer = new AdminServer(const_cast<char*>(str), strSerPath);
		delete server;
		server      = new Server(*adminServer);
	}else if (strKey == "/server"){
		//получено имя сервера
		strServer    = str;		
	}else if (strKey == "/login"){
		//получено имя пользователя
		strLogin = str;		
	}else if (strKey == "/pwd") {
		//получен пароль
		strPwd = str;
	}else if(strKey == "/create/dimensionName"){
		//[операция создания] получено имя измерения
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			delete dimension;
			dimension = new Dimension(*server, const_cast<char*>(str), length);
			dimension->makeNew();
		}else
			throw std::exception("Соединение не установлено");
	}else if (strKey == "/create/elements") {
		//[операция создания] получено имя элемента
		if (dimension==nullptr)
			throw std::exception("Отсутствует измерение");
		else if(!dimension->addElement(const_cast<char*>(str), length))
			throw std::exception("Не удается добавить элемент в создаваемое измерение");		
	}else if (strKey == "/delete/dimensionName") {
		//[операция удаления] получено имя измерения
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			try {
				Dimension dimension(*server, const_cast<char*>(str), length);
				if(!dimension.deleteDimension())
					throw std::exception("Не удалось удалить измерение");
			}catch (std::exception &e) {
				//измерения нет на сервере
			}
		}else
			throw std::exception("Соединение не установлено");
	}else if (strKey == "/create/cubeName") {
		//[операция создания] получено имя куба
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			delete cube;
			cube = new Cube(*server, const_cast<char*>(str), length);
		}else
			throw std::exception("Соединение не установлено");
	}else if (strKey == "/create/dimensions") {
		//[операция создания] получено имя измерения (для куба)
		cube->addDimension(const_cast<char*>(str), length);
	}else if (strKey == "/delete/cubeName") {
		//[операция удаления] получено имя измерения
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			try {
				Cube cube(*server, const_cast<char*>(str), length);
				cube.deleteCube();
			}catch (std::exception &e) {
				//куба нет на сервере
			}
		}else
			throw std::exception("Соединение не установлено");
	}else if (strKey == "/set/cubeName"){
		//[операция записи в куб] получено имя куба 		
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			delete cube;
			cube = new Cube(*server, const_cast<char*>(str), length);
			for (std::size_t i = 0, len = useDimensions.size(); i < len;++i)
				delete useDimensions[i];
			useDimensions.resize(cube->getCountDimensions());
		}else
			throw std::exception("Соединение не установлено");
	}else if (strKey =="/set/val/v") {
		//[операция записи в куб] получено значение 		
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			//записываем в куб			
			cube->setCellValue(useDimensions, const_cast<char*>(str));
		}else
			throw std::exception("Соединение не установлено");
	}else if (strKey.find("/set/val")==0) {
		//[операция записи в куб] получено измерение 		
		if (cube!=nullptr) {
			//заполняем значение измерения по номеру в ключе
			auto pos=strKey.rfind('/');
			int cnt=0;
			std::sscanf(strKey.substr(pos + 1, std::string::npos).c_str(), "%d", &cnt);
			if (cnt>useDimensions.size())
				throw std::exception("В кубе нет столько измерений");
			else if(useDimensions[cnt - 1]==nullptr)
				useDimensions[cnt - 1] = new Dimension(*cube, cnt);

			useDimensions[cnt - 1]->setElementVal(const_cast<char*>(str));

		}else
			throw std::exception("Для заполнения значений измерений, нужно указать куб");
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
		//[операция записи в куб] получено значение 		
		if (cube!=nullptr) {
			//записываем в куб
			cube->setCellValue(useDimensions, d);
		}else
			throw std::exception("Куб, для вставляемого значения, не инициализирован");
	}else if (strKey.find("/set/val") == 0) {
		//[операция записи в куб] получено измерение 		
		if (server->isConnected() //проверяем/устаналиваем соединение
			|| server->connect(const_cast<char*>(strServer.c_str()), 0, const_cast<char*>(strLogin.c_str()), 0, const_cast<char*>(strPwd.c_str()), 0)) {
			//заполняем значение измерения по номеру в ключе
			auto pos = strKey.rfind('/');
			int cnt = 0;
			std::sscanf(strKey.substr(pos + 1, std::string::npos).c_str(), "%d", &cnt);
			if (cnt > useDimensions.size())
				throw std::exception("В кубе нет столько измерений");
			else if(cube==nullptr)
				throw std::exception("Для заполнения значений измерений, нужно указать куб");
			else if (useDimensions[cnt - 1] == nullptr)
				useDimensions[cnt - 1] = new Dimension(*cube, cnt);

			useDimensions[cnt - 1]->setElementVal(d);

		}else
			throw std::exception("Соединение не установлено");
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
		//[операция создания] получены все имена измерений создаваемого куба
		if (cube != nullptr 
			&& !cube->exist()
			&& cube->makeNew()
			&& cube->registerCube()) 
			;//куб опубликован
		else
			throw std::exception("Не удалось опубликовать куб");
	}else if (strKey == "/create/elements") {
		//[операция создания] получены все элементы измерения
		if (dimension != nullptr			
			&& dimension->registerDimension())
			;//измерение опубликовано
		else
			throw std::exception("Не удалось опубликовать измерение");
	}
	return true;
}