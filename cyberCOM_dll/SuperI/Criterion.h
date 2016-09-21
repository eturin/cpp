
#ifndef Criterion_H
#define Criterion_H

#include "base.h"
#include <ctime>


class Criterion: public Base {
private:
	std::wstring Server; 
	std::wstring Base;
	std::wstring FilterCriterion;
	std::wstring User;
	std::wstring Pass;

	bool GetXMLfile(tVariant* pvarRetValue, tVariant* paParams, const long len);
public:
	/*�����������*/
	Criterion() {
		/*��������� ��� ������ ��������� �� 1�*/
		Base::fill_name(L"Criterion");

		/*��������� �������� ��������� �� 1�*/
		Base::Prop Props[] = {
			{L"������"         , L"Server"         , true , true},
		    {L"����"           , L"Base"           , true , true},
			{L"��������������" , L"FilterCriterion", true , true},
			{L"������������SQL", L"User"           , true , true},
			{L"������SQL"      , L"Pass"           , false, true}			
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*��������� ������ ��������� �� 1�*/
		Base::Method Methods[] = {
			{L"��������XML����", L"GetXMLfile", 0},			
		};		
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));		
	}
	~Criterion() {}
	
	/*��������� ��������*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) override {
		
		TV_VT(var) = VTYPE_PWSTR;       //���������� ���
		const wchar_t * p_wc;
		size_t len = 0;

		switch (num) {
		case 0:
			p_wc = Server.c_str();
			len = Server.length();
			break;
		case 1:
			p_wc = Base.c_str();
			len = Base.length();
			break;
		case 2:
			p_wc = FilterCriterion.c_str();
			len = FilterCriterion.length();
			break;
		case 3:
			p_wc = User.c_str();
			len = User.length();
			break;				
		default:
			return false;
		}

		/*���������� ����� � ��������� ������������ � 1�*/
		pMemoryAdapter->AllocMemory((void**)&var->pwstrVal, len*sizeof(WCHAR));
		/*�������� �������� ��������*/
		std::memcpy(var->pwstrVal, p_wc, len*sizeof(WCHAR));
		var->wstrLen = len;

		return true;
	}
	/*��������� ��������*/
	virtual bool ADDIN_API SetPropVal(const long num, tVariant * var) override {
		if(TV_VT(var) != VTYPE_PWSTR)
			return false;

		switch(num) {
			case 0:
				Server=var->pwstrVal;
				break;				
			case 1:
				Base = var->pwstrVal;
				break;
			case 2:
				FilterCriterion = var->pwstrVal;
				break;
			case 3:
				User = var->pwstrVal;
				break;
			case 4:
				Pass = var->pwstrVal;
				break;
			default:
				return false;
		}

		return true;
	}
	
	/*������*/	
	virtual       bool     ADDIN_API CallAsFunc(const long num, tVariant* pvarRetValue, tVariant* paParams, const long len) override {
		bool res = false;
		if(num < cnt_methods && !Methods[num]->is_proc) {
			switch(num) {
				case 0:
					res = GetXMLfile(pvarRetValue, paParams, len);
					break;				
				default:
					res = false;
			}
		}
		return res;		
	} 
	
};

#endif