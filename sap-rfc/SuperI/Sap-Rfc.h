#ifndef CLASS_H
#define CLASS_H

#include <ctime>
#include <sstream>
#include "base.h"
#include "saprfc.h"

class SapRfc: public Base {
private:
	//��������� ����
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
			//��������� �� ������ � ������
			m = new RFC_PARAMETER[cnt + 1];
			memsetR(m, 0, (cnt+1)*sizeofR(RFC_PARAMETER));
			Node * temp=top;
			for(size_t i = 0; i < cnt; ++i) {
				m[i] = temp->val;
				temp = temp->next;
			}
			//��������� ��������
			m[cnt].name = nullptr;
			m[cnt].nlen = 0;
			m[cnt].type = 0;
			m[cnt].addr = nullptr;
			m[cnt].leng = 0;
		}
		void load(const RFC_PARAMETER * m) {
			//��������� �� ������ � ������ 			
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
	
	//����� ������
	char       *connection_string;
	RFC_HANDLE rfc_handle;
	rfc_char_t *function_name;
	char       *last_error;
	ListParams exporting, importing;
public:
	/*�����������*/
	SapRfc():connection_string(nullptr), function_name(nullptr), rfc_handle(RFC_HANDLE_NULL), last_error(nullptr){
		/*��������� ��� ������ ��������� �� 1�*/
		Base::fill_name(L"SapRfc");

		/*��������� �������� ��������� �� 1�*/
		Base::Prop Props[] = {
			{L"����������������", L"ConnectionString", true, true },
			{L"����������"      , L"FunctionName"    , true, true }
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*��������� ������ ��������� �� 1�*/
		Base::Method Methods[] = {
			{L"��������������������", L"myRfcOpenEx"     , 0},
			{L"�����������������"   , L"myRfcClose"      , 0, true},
			{L"����������������"    , L"AddParam"        , 5},
			{L"���������������"     , L"DelParam"        , 1},
			{L"����������������"    , L"GetParam"        , 1},
			{L"��������������������", L"GetCountParam"   , 1},
			{L"��������������������", L"ClearAllParam"   , 0, true},
			{L"�������"             , L"myRfcCallReceive", 0},
			{L"���������������"     , L"myRfcLastErrorEx", 0}
		};		
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));			
	}
	~SapRfc() {
		myRfcClose();
		delete[] connection_string, function_name, last_error;
	}
	
	/*��������� ��������*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) override {
		switch(num) {
			case 0: //������ ����������
				TV_VT(var) = VTYPE_PSTR;           //���������� ���				
				var->strLen = 0;
				if(connection_string!=nullptr)
					var->strLen = std::strlen(connection_string);
				pMemoryAdapter->AllocMemory((void**)&var->pstrVal, var->strLen*sizeof(char));
				if(connection_string != nullptr)
					std::memcpy(var->pstrVal, connection_string, var->strLen*sizeof(char));				
				break;
			case 1: //��� �������
				TV_VT(var) = VTYPE_PSTR;           //���������� ���
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
	/*��������� ��������*/
	virtual bool ADDIN_API SetPropVal(const long num, tVariant * var) override {
		switch(num) {
			case 0: //������ ����������
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
	
	/*������*/
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
			sout << "������:\n������ = " << error_info->group
			<< "\n���� = " << error_info->key
			<< "\n�������� = " << error_info->message << std::endl;

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

		//��������� ��������� 
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
		
		//��������� ������� ����������
		myRfcClose();
		
		//��������� ����� ����������		
		RFC_ERROR_INFO_EX  error_info;
		SetError();
		rfc_handle = RfcOpenEx(connection_string, &error_info);
		if(rfc_handle == RFC_HANDLE_NULL) {		
			SetError(&error_info);
			res = false;			
		}
		
		pvarRetValue->vt = VTYPE_BOOL; //��������� ������������ ���
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
		pvarRetValue->vt = VTYPE_BOOL; //��������� ������������ ���
		pvarRetValue->lVal = res;

		return true;
	}

	bool GetCountParam(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;

		if(paParams[0].vt != VTYPE_PWSTR)
			res = false;			

		pvarRetValue->vt = VTYPE_I4; //��������� ������������ ���
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

		pvarRetValue->vt = VTYPE_BOOL; //��������� ������������ ���
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
				pvarRetValue->vt = VTYPE_PSTR; //��������� ������������ ���
				pvarRetValue->strLen = param->leng;
				pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pstrVal, pvarRetValue->strLen*sizeof(char));
				if(connection_string != nullptr)
					std::memcpy(pvarRetValue->pstrVal, param->addr, pvarRetValue->strLen*sizeof(char));
			}		

		return true;
	}

	bool myRfcCallReceive(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = false;
		
		//��� ������
		RFC_TABLE tables[5];
		tables[0].name = NULL;

		//�������� ������� ����������
		
		RFC_PARAMETER * exporting = nullptr, *importing = nullptr;
		this->exporting.fill(exporting);
		this->importing.fill(importing);	
				
		//������� ��������� �������
		SetError();
		rfc_char_t * exception = nullptr; 
		RFC_RC rfc_rc = RfcCallReceive(rfc_handle    /*handle ����������*/,
								       function_name /*��� ��������� �������*/,
								       exporting     /*������ ������������ ����������*/,
									   importing     /*������ ���������� ����������*/,
									   tables        /*������ ������*/,
									   &exception    /*����������*/);

		//��������� ��������� ������
		char * temp=nullptr;
		switch(rfc_rc) {
			case RFC_OK:
				res = true;
				break;
			case RFC_EXCEPTION:
			case RFC_SYS_EXCEPTION:
				temp = "������:\nRFC Call / Exception: ";				
				break;
			default:
				temp = "RfcCallReceive";
				break;
		}
		//��������� �������� �� ������
		if(rfc_rc != RFC_OK) {
			size_t a = std::strlen(temp), b = exception!=nullptr ? std::strlen(exception) : 0;
			last_error = new char[a + b + 1];
			std::memcpy(last_error, temp, a*sizeof(char));
			std::memcpy(last_error + a, exception, b*sizeof(char));
			last_error[a + b] = '\0';
		}

		pvarRetValue->vt = VTYPE_BOOL; //��������� ������������ ���
		pvarRetValue->lVal = res;

		//��������� ��������� � ������
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
			sout << "������ ����������� ������:\n������ = " << error_info.group
				<< "\n���� = " << error_info.key
				<< "\n�������� = " << error_info.message << std::endl;
		}
		std::string err(sout.str());

		pvarRetValue->vt = VTYPE_PSTR; //��������� ������������ ���
		pvarRetValue->strLen = err.length();
		pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pstrVal, pvarRetValue->strLen*sizeof(char));
		std::memcpy(pvarRetValue->pstrVal, err.c_str(), pvarRetValue->strLen*sizeof(char));

		return true;
	}	
};

#endif