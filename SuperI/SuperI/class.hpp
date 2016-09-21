#include "base.h"
#include <ctime>

#ifndef CLASS_H
#define CLASS_H

class myClass: public Base {
private:
	int     Prop0 = -113; 
	double  Prop1 = 7.65;
	char  * Prop2 = nullptr;
	bool    Prop3 = true;
	tm      Prop4;
public:
	/*�����������*/
	myClass() {
		/*��������� ��� ������ ��������� �� 1�*/
		Base::fill_name(L"myClass");

		/*��������� �������� ��������� �� 1�*/
		Base::Prop Props[] = {
			{L"��������_int"   , L"Prorp0", true, true},
		    {L"��������_double", L"Prorp1", true, true},
			{L"��������_pchar" , L"Prorp2", true, true},
			{L"��������_bool"  , L"Prorp3", true, true},
			{L"��������_tm"    , L"Prorp4", true, true}
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*��������� ������ ��������� �� 1�*/
		Base::Method Methods[] = {
			{L"�������1", L"Func1", 0},
			{L"�������2", L"Func2en", 2},
			{L"���������1", L"Proc1en", 1, true}
		};		
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));
		
		Prop2 = new char[100];
		std::strcpy(Prop2, "abc ��� 123");

		time_t rawtime;
		time(&rawtime);
		Prop4 = *localtime(&rawtime);		
	}
	~myClass() { 
		delete[] Prop2; 		
	}
	
	/*��������� ��������*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) override {
		switch(num) {
			case 0: //��������_int
				TV_VT(var) = VTYPE_I4; //���������� ���
				TV_I4(var) = Prop0;    //���������� ��������
				break;
			case 1: //��������_double
				TV_VT(var) = VTYPE_R8;  //���������� ���
				TV_R8(var) = Prop1;     //���������� ��������
				break;
			case 2: //��������_pchar
				TV_VT(var) = VTYPE_PSTR;  //���������� ���
				var->pstrVal = Prop2;     //����� ��������� �� ������
				var->strLen  = std::strlen(Prop2);
				break;
			case 3: //��������_bool
				TV_VT(var) = VTYPE_BOOL; //���������� ���
				TV_BOOL(var) = Prop3;    //���������� ��������
				break;
			case 4: //��������_tm
				TV_VT(var) = VTYPE_TM; //���������� ���
				var->tmVal = Prop4;    //���������� ��������
				break;
			default:
				return false;
		}
		return true;
	}
	/*��������� ��������*/
	virtual bool ADDIN_API SetPropVal(const long num, tVariant * var) override {
		switch(num) {
			case 0:
				if(TV_VT(var) != VTYPE_I4)
					return false;
				Prop0 = TV_I4(var);
				break;
			case 1:
				if(TV_VT(var) != VTYPE_R8)
					return false;
				Prop1 = TV_R8(var);
				break;
			case 2:
				if(TV_VT(var) == VTYPE_PSTR) { 
					delete[] Prop2;
					size_t len = std::strlen(var->pstrVal);
					Prop2 = new char[len + 1];
					std::strncpy(Prop2, var->pstrVal, len + 1);
					break;
				} else if(TV_VT(var) == VTYPE_PWSTR) {
					delete[] Prop2;
					WCHAR_to_char(Prop2, var->pwstrVal);
					break;
				} else
					return false;
			case 3:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				Prop3 = TV_BOOL(var);
				break;
			case 4:
				if(TV_VT(var) != VTYPE_TM)
					return false;
				Prop4 = var->tmVal;
				break;
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
				case 2:
					res = Proc1(paParams, len);
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
					res = Func1(pvarRetValue, paParams, len);
					break;
				case 1:
					res = Func2(pvarRetValue, paParams, len);
					break;		
				default:
					res = false;
			}
		}
		return res;		
	} 

	bool Proc1(tVariant* paParams, const long lSizeArray) {
		bool res = true;
		if(paParams[0].vt == VTYPE_I4) //��������, ��� ������ ����� �����
			this->Prop0 = paParams[0].lVal; //��������
		else
			res = false;
		return res; 
	}
	bool Func2(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;
		int a,b;
		if(paParams[0].vt == VTYPE_I4) //��������, ��� ������ ����� �����
			a = paParams[0].lVal; //��������
		else
			res = false;
		if(paParams[1].vt == VTYPE_I4) //��������, ��� ������ ����� �����
			b=paParams[1].lVal; //��������
		else
			res = false;
		
		if(res) {
			pvarRetValue->vt = VTYPE_I4; //��������� ������������ ���
			pvarRetValue->lVal = a + b;  //��������� ������������ �������� (��������� pMemoryAdapter->AllocMemory((void**)&pvarRetValue->lVal, size_in_byte) ����� �� ���������, �.�. �������� ��������) 
		}

		return res; 
	}
	bool Func1(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;
		size_t l = std::strlen(Prop2) + 1;
		pvarRetValue->vt = VTYPE_PSTR;
		pvarRetValue->strLen = l;
		/*����� ������������ ����� � ��������� ������������ � 1�*/
		pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pstrVal, l*sizeof(char));
		std::memcpy(pvarRetValue->pstrVal, Prop2, l*sizeof(char));

		return res;
	}
};

#endif