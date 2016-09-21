#ifndef CLASS_H
#define CLASS_H

#include <ctime>
#include <sstream>
#include "base.h"
#include "saprfc.h"

class SapRfc: public Base {
private:
	//вложенные типы
	class ListParams {
	private:
		class Node {
		public:
			RFC_PARAMETER val;
			Node *next;
			Node(RFC_PARAMETER & val):val(val), next(nullptr) {}
			~Node() {
				delete[] val.name, val.addr;
				delete next;
			}
		};
		size_t cnt;
		Node *top=nullptr;	
	public:
		void add(RFC_PARAMETER &val) {
			if(top == nullptr)
				top = new Node(val);
			else {
				Node * last = top;
				while(last->next != nullptr) 
					last = last->next;
				last->next = new Node(val);
			}
			++cnt;
		}
		size_t get_cnt() const { return cnt; }
		void clear_all() {
			delete top;
			top = nullptr;
			cnt = 0;
		}
		void fill(RFC_PARAMETER * &m) const{
			//переносим из списка в массив
			m = new RFC_PARAMETER[cnt + 1];
			memsetR(m, 0, (cnt+1)*sizeofR(RFC_PARAMETER));
			Node * temp=top;
			for(size_t i = 0; i < cnt; ++i) {
				m[i] = temp->val;
				temp = temp->next;
			}
			//последний параметр
			m[cnt].name = nullptr;
			m[cnt].nlen = 0;
			m[cnt].type = 0;
			m[cnt].addr = nullptr;
			m[cnt].leng = 0;
		}
		void load(const RFC_PARAMETER * m) {
			//переносим из массив в список 			
			Node * temp = top;
			for(size_t i = 0; i < cnt; ++i) {
				temp->val = m[i]; 
				temp = temp->next;
			}			
		}
		const RFC_PARAMETER * get_by_name(const char * name) const {
			if(top != nullptr){
				Node * cur = top;
				do {
					if(0 == std::strcmp(cur->val.name, name))
						return &cur->val;
					else if(cur->next != nullptr)
						cur = cur->next;
					else
						break;
				} while(true);									
			}

			return nullptr;
		}
		bool del_by_name(const char * name) {
			bool res = false;
			if(top != nullptr) {
				Node *cur = top, *prev=nullptr;
				do {
					if(0 == std::strcmp(cur->val.name, name)) {
						if(prev == nullptr) {
							top = cur->next;
							cur->next = nullptr;
							delete cur;	
							--cnt;
							res = true;
						} else {
							prev->next = cur->next;
							cur->next = nullptr;
							delete cur;
							--cnt;
							res = true;
						}
						break;
					} else if(cur->next != nullptr) {
						prev = cur;
						cur = cur->next;
					} else
						break;
				} while(true);
			}			

			return res;
		}
		~ListParams() {
			delete top;
		}
	};
	
	//члены данных
	char       *connection_string;
	RFC_HANDLE rfc_handle;
	rfc_char_t *function_name;
	char       *last_error;
	ListParams exporting, importing;
public:
	/*конструктор*/
	SapRfc():connection_string(nullptr), function_name(nullptr), rfc_handle(RFC_HANDLE_NULL), last_error(nullptr){
		/*объявляем имя класса доступное из 1С*/
		Base::fill_name(L"SapRfc");

		/*объявляем свойства доступные из 1С*/
		Base::Prop Props[] = {
			{L"СтрокаСоединения", L"ConnectionString", true, true },
			{L"ИмяФункции"      , L"FunctionName"    , true, true }
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*объявляем методы доступные из 1С*/
		Base::Method Methods[] = {
			{L"УстановитьСоединение", L"myRfcOpenEx"     , 0},
			{L"ЗакрытьСоединение"   , L"myRfcClose"      , 0, true},
			{L"ДобавитьПараметр"    , L"AddParam"        , 5},
			{L"УдалитьПараметр"     , L"DelParam"        , 1},
			{L"ПолучитьПараметр"    , L"GetParam"        , 1},
			{L"КоличествоПараметров", L"GetCountParam"   , 1},
			{L"ОчиститьВсеПараметры", L"ClearAllParam"   , 0, true},
			{L"Вызвать"             , L"myRfcCallReceive", 0},
			{L"ПоследняяОшибка"     , L"myRfcLastErrorEx", 0}
		};		
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));			
	}
	~SapRfc() {
		myRfcClose();
		delete[] connection_string, function_name, last_error;
	}
	
	/*Получение свойства*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) override {
		switch(num) {
			case 0: //строка соединения
				TV_VT(var) = VTYPE_PSTR;           //выставляем тип				
				var->strLen = 0;
				if(connection_string!=nullptr)
					var->strLen = std::strlen(connection_string);
				pMemoryAdapter->AllocMemory((void**)&var->pstrVal, var->strLen*sizeof(char));
				if(connection_string != nullptr)
					std::memcpy(var->pstrVal, connection_string, var->strLen*sizeof(char));				
				break;
			case 1: //имя функции
				TV_VT(var) = VTYPE_PSTR;           //выставляем тип
				var->strLen = 0;
				if(function_name != nullptr)
					var->strLen = std::strlen(function_name);
				pMemoryAdapter->AllocMemory((void**)&var->pstrVal, var->strLen*sizeof(wchar_t));
				if(function_name != nullptr)
					std::memcpy(var->pstrVal, function_name, var->strLen*sizeof(wchar_t));
				break;						
			default:
				return false;
		}
		return true;
	}
	/*Установка свойства*/
	virtual bool ADDIN_API SetPropVal(const long num, tVariant * var) override {
		switch(num) {
			case 0: //строка соединения
				if(TV_VT(var) == VTYPE_PSTR) { 
					delete[] connection_string;
					size_t len = std::strlen(var->pstrVal);
					connection_string = new char[len + 1];
					std::strncpy(connection_string, var->pstrVal, len + 1);
					break;
				} else if(TV_VT(var) == VTYPE_PWSTR) {
					delete[] connection_string;
					WCHAR_to_char(connection_string, var->pwstrVal);
					break;
				} else
					return false;
			case 1:
				if(TV_VT(var) == VTYPE_PWSTR) {
					delete[] function_name;
					WCHAR_to_char(function_name, var->pwstrVal);
					break;
				} else
					return false;
			default:
				return false;
		}
		return true;
	}
	
	/*Методы*/
	virtual       bool     ADDIN_API CallAsProc(const long num, tVariant* paParams, const long len) override {
		bool res = false;
		if(num < cnt_methods && Methods[num]->is_proc) {
			switch(num) {
				case 1:
					res = myRfcClose(paParams, len);
					break;
				case 6:
					res = ClearAllParam(paParams, len);
					break;
				default:
					res = false;
			}
		}
		return res; 
	} 
	virtual       bool     ADDIN_API CallAsFunc(const long num, tVariant* pvarRetValue, tVariant* paParams, const long len) override {
		bool res = false;
		if(num < cnt_methods && !Methods[num]->is_proc) {
			switch(num) {
				case 0:
					res = myRfcOpenEx(pvarRetValue, paParams, len);
					break;
				case 2:
					res = AddParam(pvarRetValue, paParams, len);
					break;		
				case 3:
					res = DelParam(pvarRetValue, paParams, len);
					break;
				case 4:
					res = GetParam(pvarRetValue, paParams, len);
					break;
				case 5:
					res = GetCountParam(pvarRetValue, paParams, len);
					break;
				case 7:
					res = myRfcCallReceive(pvarRetValue, paParams, len);
					break;
				case 8:
					res = myRfcLastErrorEx(pvarRetValue, paParams, len);
					break;
				default:
					res = false;
			}
		}
		return res;		
	} 

	void SetError(RFC_ERROR_INFO_EX  * error_info=nullptr) {
		delete  last_error;
		last_error = nullptr;

		std::ostringstream sout;
		if(error_info!=nullptr)
			sout << "Ошибка:\nГруппа = " << error_info->group
			<< "\nКлюч = " << error_info->key
			<< "\nОписание = " << error_info->message << std::endl;

		std::string err(sout.str());

		size_t l = err.length();
		if(l) {
			last_error = new char[l + 1];
			std::memcpy(last_error, err.c_str(), l*sizeof(char));
			last_error[l] = '\0';
		}
	}

	bool myRfcClose(tVariant* paParams=nullptr, const long lSizeArray=0) {
		bool res = true;

		//закрываем соединени 
		if(rfc_handle != RFC_HANDLE_NULL)
			RfcClose(rfc_handle);
		rfc_handle = RFC_HANDLE_NULL;
		
		return res; 
	}

	bool ClearAllParam(tVariant* paParams = nullptr, const long lSizeArray = 0) {
		bool res = true;

		exporting.clear_all();
		importing.clear_all();

		return res;
	}
	
	bool myRfcOpenEx(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;
		
		//закрываем прежнее соединение
		myRfcClose();
		
		//открываем новое соединение		
		RFC_ERROR_INFO_EX  error_info;
		SetError();
		rfc_handle = RfcOpenEx(connection_string, &error_info);
		if(rfc_handle == RFC_HANDLE_NULL) {		
			SetError(&error_info);
			res = false;			
		}
		
		pvarRetValue->vt = VTYPE_BOOL; //указываем возвращаемый тип
		pvarRetValue->lVal = res;  
		
		return true; 
	}

	bool AddParam(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;
		
		if(paParams[0].vt != VTYPE_PWSTR || 0 != std::wcscmp(paParams[0].pwstrVal, L"exporting") && 0 != std::wcscmp(paParams[0].pwstrVal, L"importing"))
			res = false;
		else if(paParams[1].vt != VTYPE_PWSTR)
			res = false;
		else if(paParams[2].vt != VTYPE_PWSTR)
			res = false;
		else if(paParams[4].vt != VTYPE_I4)
			res = false;
		
		if(res) {
			RFC_PARAMETER param;
			param.leng = paParams[4].lVal;
			param.addr = nullptr;
			if(0 == std::wcscmp(paParams[2].pwstrVal, L"TYPC")) {
				param.type = TYPC;
				if(paParams[3].vt != VTYPE_PWSTR)
					res = false;
				else {
					rfc_char_t *val = new rfc_char_t[param.leng+1];
					memsetU(val, cU(' '), param.leng);
					val[param.leng] = '\0';
					WCHAR_to_char(val, paParams[3].pwstrVal, param.leng);										
					param.addr = val;
				}
			} else if(0 == std::wcscmp(paParams[2].pwstrVal, L"TYPNUM")) {
				param.type = TYPNUM;
				if(paParams[3].vt != VTYPE_PWSTR)
					res = false;
				else {
					rfc_char_t *val = new rfc_char_t[param.leng+1];
					memsetU(val, cU(' '), param.leng);
					val[param.leng] = '\0';
					WCHAR_to_char(val, paParams[3].pwstrVal, param.leng);
					param.addr = val;
				}
			} else
				res = false;

			if(res) {
				param.nlen = 0;
				char *val;
				param.nlen = WCHAR_to_char(val, paParams[1].pwstrVal)-1;
				param.name = val;

				if(0 == std::wcscmp(paParams[0].pwstrVal, L"exporting"))
					exporting.add(param);
				else if(0 == std::wcscmp(paParams[0].pwstrVal, L"importing"))
					importing.add(param);
			}
		}
		pvarRetValue->vt = VTYPE_BOOL; //указываем возвращаемый тип
		pvarRetValue->lVal = res;

		return true;
	}

	bool GetCountParam(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;

		if(paParams[0].vt != VTYPE_PWSTR)
			res = false;			

		pvarRetValue->vt = VTYPE_I4; //указываем возвращаемый тип
		if(0 == std::wcscmp(paParams[0].pwstrVal, L"exporting"))
			pvarRetValue->lVal = exporting.get_cnt();
		else if(0 == std::wcscmp(paParams[0].pwstrVal, L"importing"))
			pvarRetValue->lVal = importing.get_cnt();
		else
			pvarRetValue->lVal = 0;

		return true;
	}

	bool DelParam(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;

		if(paParams[0].vt != VTYPE_PWSTR)
			res = false;

		char *val=nullptr;
		WCHAR_to_char(val, paParams[0].pwstrVal);
		if(!exporting.del_by_name(val) && !importing.del_by_name(val))
			res = false;
		delete[] val;

		pvarRetValue->vt = VTYPE_BOOL; //указываем возвращаемый тип
		pvarRetValue->lVal = res;
		
		return true;
	}

	bool GetParam(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;

		if(paParams[0].vt != VTYPE_PWSTR)
			res = false;

		char *val = nullptr;
		WCHAR_to_char(val, paParams[0].pwstrVal);
		const RFC_PARAMETER * param = exporting.get_by_name(val);
		if(param==nullptr)
			param = importing.get_by_name(val);
		delete[] val;

		if(param != nullptr) 
			if(param->type == TYPC || param->type == TYPNUM) {
				pvarRetValue->vt = VTYPE_PSTR; //указываем возвращаемый тип
				pvarRetValue->strLen = param->leng;
				pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pstrVal, pvarRetValue->strLen*sizeof(char));
				if(connection_string != nullptr)
					std::memcpy(pvarRetValue->pstrVal, param->addr, pvarRetValue->strLen*sizeof(char));
			}		

		return true;
	}

	bool myRfcCallReceive(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = false;
		
		//нет таблиц
		RFC_TABLE tables[5];
		tables[0].name = NULL;

		//формирую массивы параметров
		
		RFC_PARAMETER * exporting = nullptr, *importing = nullptr;
		this->exporting.fill(exporting);
		this->importing.fill(importing);	
				
		//вызываю удаленную функцию
		SetError();
		rfc_char_t * exception = nullptr; 
		RFC_RC rfc_rc = RfcCallReceive(rfc_handle    /*handle соединения*/,
								       function_name /*имя удаленной функции*/,
								       exporting     /*массив передаваемых параметров*/,
									   importing     /*массив получаемых параметров*/,
									   tables        /*массив таблиц*/,
									   &exception    /*исключение*/);

		//проверяем результат вызова
		char * temp=nullptr;
		switch(rfc_rc) {
			case RFC_OK:
				res = true;
				break;
			case RFC_EXCEPTION:
			case RFC_SYS_EXCEPTION:
				temp = "Ошибка:\nRFC Call / Exception: ";				
				break;
			default:
				temp = "RfcCallReceive";
				break;
		}
		//переносим сведения об ошибке
		if(rfc_rc != RFC_OK) {
			size_t a = std::strlen(temp), b = exception!=nullptr ? std::strlen(exception) : 0;
			last_error = new char[a + b + 1];
			std::memcpy(last_error, temp, a*sizeof(char));
			std::memcpy(last_error + a, exception, b*sizeof(char));
			last_error[a + b] = '\0';
		}

		pvarRetValue->vt = VTYPE_BOOL; //указываем возвращаемый тип
		pvarRetValue->lVal = res;

		//переносим параметры в список
		this->exporting.load(exporting);
		this->importing.load(importing);
		delete[] exporting, importing;

		return true;
	}

	bool myRfcLastErrorEx(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;	

		std::ostringstream sout;
		RFC_ERROR_INFO_EX  error_info;
		if(0 == RfcLastErrorEx(&error_info)) {
			if(last_error != nullptr)
				sout << last_error << std::endl;
			sout << "Ошибка внутреннего вызова:\nГруппа = " << error_info.group
				<< "\nКлюч = " << error_info.key
				<< "\nОписание = " << error_info.message << std::endl;
		}
		std::string err(sout.str());

		pvarRetValue->vt = VTYPE_PSTR; //указываем возвращаемый тип
		pvarRetValue->strLen = err.length();
		pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pstrVal, pvarRetValue->strLen*sizeof(char));
		std::memcpy(pvarRetValue->pstrVal, err.c_str(), pvarRetValue->strLen*sizeof(char));

		return true;
	}	
};

#endif