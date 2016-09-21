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
	/*конструктор*/
	myClass() {
		/*объявляем имя класса доступное из 1С*/
		Base::fill_name(L"myClass");

		/*объявляем свойства доступные из 1С*/
		Base::Prop Props[] = {
			{L"Свойство_int"   , L"Prorp0", true, true},
		    {L"Свойство_double", L"Prorp1", true, true},
			{L"Свойство_pchar" , L"Prorp2", true, true},
			{L"Свойство_bool"  , L"Prorp3", true, true},
			{L"Свойство_tm"    , L"Prorp4", true, true}
		};
		Base::fill_props(Props, sizeof(Props) / sizeof(Base::Prop));

		/*объявляем методы доступные из 1С*/
		Base::Method Methods[] = {
			{L"Функция1", L"Func1", 0},
			{L"Функция2", L"Func2en", 2},
			{L"Процедура1", L"Proc1en", 1, true}
		};		
		Base::fill_methods(Methods, sizeof(Methods) / sizeof(Base::Method));
		
		Prop2 = new char[100];
		std::strcpy(Prop2, "abc абС 123");

		time_t rawtime;
		time(&rawtime);
		Prop4 = *localtime(&rawtime);		
	}
	~myClass() { 
		delete[] Prop2; 		
	}
	
	/*Получение свойства*/
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* var) override {
		switch(num) {
			case 0: //Свойство_int
				TV_VT(var) = VTYPE_I4; //выставляем тип
				TV_I4(var) = Prop0;    //выставляем значение
				break;
			case 1: //Свойство_double
				TV_VT(var) = VTYPE_R8;  //выставляем тип
				TV_R8(var) = Prop1;     //выставляем значение
				break;
			case 2: //Свойство_pchar
				TV_VT(var) = VTYPE_PSTR;  //выставляем тип
				var->pstrVal = Prop2;     //сразу указатель на строку
				var->strLen  = std::strlen(Prop2);
				break;
			case 3: //Свойство_bool
				TV_VT(var) = VTYPE_BOOL; //выставляем тип
				TV_BOOL(var) = Prop3;    //выставляем значение
				break;
			case 4: //Свойство_tm
				TV_VT(var) = VTYPE_TM; //выставляем тип
				var->tmVal = Prop4;    //выставляем значение
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
	
	/*Методы*/
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
		if(paParams[0].vt == VTYPE_I4) //Проверка, что пришло целое число
			this->Prop0 = paParams[0].lVal; //значение
		else
			res = false;
		return res; 
	}
	bool Func2(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;
		int a,b;
		if(paParams[0].vt == VTYPE_I4) //Проверка, что пришло целое число
			a = paParams[0].lVal; //значение
		else
			res = false;
		if(paParams[1].vt == VTYPE_I4) //Проверка, что пришло целое число
			b=paParams[1].lVal; //значение
		else
			res = false;
		
		if(res) {
			pvarRetValue->vt = VTYPE_I4; //указываем возвращаемый тип
			pvarRetValue->lVal = a + b;  //указываев возвращаемое значение (выполнять pMemoryAdapter->AllocMemory((void**)&pvarRetValue->lVal, size_in_byte) здесь не требуется, т.к. передаем значение) 
		}

		return res; 
	}
	bool Func1(tVariant* pvarRetValue, tVariant* paParams, const long len) {
		bool res = true;
		size_t l = std::strlen(Prop2) + 1;
		pvarRetValue->vt = VTYPE_PSTR;
		pvarRetValue->strLen = l;
		/*нужно аллоцировать место в структуре возвращаемой в 1С*/
		pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pstrVal, l*sizeof(char));
		std::memcpy(pvarRetValue->pstrVal, Prop2, l*sizeof(char));

		return res;
	}
};

#endif