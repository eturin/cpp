#ifndef KeyHook_H
#define KeyHook_H
#include "base.h"

/*идентификатор потока*/
extern DWORD   dwThreadId;
/*дескриптор hook-точки*/
extern HHOOK   hndlKeyHook;
/*Функция обрабатывающая событие Windows*/
LRESULT CALLBACK KeyHookProcLL(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam);

class KeyHook;
extern KeyHook * pMe;

class KeyHook: public Base {
private:
	/*признак включенного перехвата*/
	bool isOn       = false;	
	bool isOnLL     = false;
public:
	bool onKeyUP    = true;
	bool onKeyDOWN  = false;
	bool onKeyUPLL  = true;
	bool onKeyDOWNLL= false;
	bool KeyLock    = false;

	/*Конструктор*/
	KeyHook() {
		/*объявляем имя класса доступное из 1С*/ 
		Base::fill_name(L"KeyHook");

		/*объявляем свойства доступные из 1С*/
		Base::Prop Props[] = {
			{L"РегистрацияВключена"   , L"isOn"       , true, false},
			{L"РегистрацияВключена_LL", L"isOnLL"     , true, false},
			{L"СообщатьО_KeyUP"       , L"onKeyUP"    , true, true},
			{L"СообщатьО_KeyDOWN"     , L"onKeyDOWN"  , true, true},
			{L"СообщатьО_KeyUP_LL"    , L"onKeyUPLL"  , true, true},
			{L"СообщатьО_KeyDOWN_LL"  , L"onKeyDOWNLL", true, true},
			{L"БлокироватьКлавиши"    , L"KeyLock"    , true, true}
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*объявляем методы доступные из 1С*/
		Base::Method Methods[] = {
			{L"Включить"     , L"SetOn"  , 1},			
			{L"Выключить"    , L"SetOff" , 1},
			{L"НажатьКлавишу", L"Send"   , 1}
		};
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));

		pMe = &(*this);
		dwThreadId = ::GetCurrentThreadId();				
	}
	/*Деструктор*/
	~KeyHook() {
		if(isOn) 
			SetOff();					
	}
	/*Специфичные методы добавления и удаления процедуры обработки hook-точки*/
	bool SetOn(tVariant* pvarRetValue = nullptr, tVariant* paParams = nullptr, const long len = 0);
	bool SetOff(tVariant* pvarRetValue = nullptr, tVariant* paParams = nullptr, const long len = 0);
	bool Send(tVariant* pvarRetValue, tVariant* paParams, const long len);

	/*Получение свойства*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) const override {
		switch(num) {
			case 0:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = isOn;
				break;			
			case 1:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = isOnLL;
				break;
			case 2:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = onKeyUP;
				break;
			case 3:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = onKeyDOWN;
				break;
			case 4:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = onKeyUPLL;
				break;
			case 5:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = onKeyDOWNLL;
				break;
			case 6:
				TV_VT(var) = VTYPE_BOOL;       //выставляем тип
				var->bVal = KeyLock;
				break;
			default:
				return false;
		}		

		return true;
	}
	/*Установка свойства*/
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
	/*Методы*/
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