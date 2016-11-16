#ifndef KeyHook_H
#define KeyHook_H
#include "base.h"

/*������������� ������*/
extern DWORD   dwThreadId;
/*���������� hook-�����*/
extern HHOOK   hndlKeyHook;
/*������� �������������� ������� Windows*/
LRESULT CALLBACK KeyHookProcLL(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam);

class KeyHook;
extern KeyHook * pMe;

class KeyHook: public Base {
private:
	/*������� ����������� ���������*/
	bool isOn       = false;	
	bool isOnLL     = false;
public:
	bool onKeyUP    = true;
	bool onKeyDOWN  = false;
	bool onKeyUPLL  = true;
	bool onKeyDOWNLL= false;
	bool KeyLock    = false;

	/*�����������*/
	KeyHook() {
		/*��������� ��� ������ ��������� �� 1�*/ 
		Base::fill_name(L"KeyHook");

		/*��������� �������� ��������� �� 1�*/
		Base::Prop Props[] = {
			{L"�������������������"   , L"isOn"       , true, false},
			{L"�������������������_LL", L"isOnLL"     , true, false},
			{L"���������_KeyUP"       , L"onKeyUP"    , true, true},
			{L"���������_KeyDOWN"     , L"onKeyDOWN"  , true, true},
			{L"���������_KeyUP_LL"    , L"onKeyUPLL"  , true, true},
			{L"���������_KeyDOWN_LL"  , L"onKeyDOWNLL", true, true},
			{L"������������������"    , L"KeyLock"    , true, true}
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*��������� ������ ��������� �� 1�*/
		Base::Method Methods[] = {
			{L"��������"     , L"SetOn"  , 1},			
			{L"���������"    , L"SetOff" , 1},
			{L"�������������", L"Send"   , 1}
		};
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));

		pMe = &(*this);
		dwThreadId = ::GetCurrentThreadId();				
	}
	/*����������*/
	~KeyHook() {
		if(isOn) 
			SetOff();					
	}
	/*����������� ������ ���������� � �������� ��������� ��������� hook-�����*/
	bool SetOn(tVariant* pvarRetValue = nullptr, tVariant* paParams = nullptr, const long len = 0);
	bool SetOff(tVariant* pvarRetValue = nullptr, tVariant* paParams = nullptr, const long len = 0);
	bool Send(tVariant* pvarRetValue, tVariant* paParams, const long len);

	/*��������� ��������*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) const override {
		switch(num) {
			case 0:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = isOn;
				break;			
			case 1:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = isOnLL;
				break;
			case 2:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = onKeyUP;
				break;
			case 3:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = onKeyDOWN;
				break;
			case 4:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = onKeyUPLL;
				break;
			case 5:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = onKeyDOWNLL;
				break;
			case 6:
				TV_VT(var) = VTYPE_BOOL;       //���������� ���
				var->bVal = KeyLock;
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
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				isOn      = var->bVal;
				break;			
			case 1:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				isOn = var->bVal;
				break;
			case 2:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				onKeyUP   = var->bVal;
				if(!(onKeyDOWN || onKeyUP)) SetOff();
				break;
			case 3:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				onKeyDOWN = var->bVal;
				if(!(onKeyDOWN || onKeyUP)) SetOff();
				break;
			case 4:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				onKeyUPLL = var->bVal;
				if(!(onKeyDOWNLL || onKeyUPLL)) SetOff();
				break;
			case 5:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				onKeyDOWNLL = var->bVal;
				if(!(onKeyDOWNLL || onKeyUPLL)) SetOff();
				break;
			case 6:
				if(TV_VT(var) != VTYPE_BOOL)
					return false;
				KeyLock = var->bVal;
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
					res = SetOn(pvarRetValue, paParams, len);
					break;
				case 1:
					res = SetOff(pvarRetValue, paParams, len);
					break;
				case 2:
					res = Send(pvarRetValue, paParams, len);
					break;
				default:
					res = false;
			}
		}
		return res;
	}
};


#endif