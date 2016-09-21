#include "base.h"
#include <wchar.h>
#include <iostream>
#include <string>



/*Реализация базового абстрактного класса*/
void Base::fill_name(const wchar_t * name) {
	/*копируем имя*/
	size_t len = wcslen(name) + 1;
	this->name = new wchar_t[len];
	memcpy(this->name, name, len*sizeof(wchar_t));
}
void Base::fill_props(const Prop * Props, long cnt_props) {
	/*заполним возможные свойства, если они есть*/
	if(cnt_props) {
		this->cnt_props = cnt_props;
		this->Props     = new Prop*[cnt_props];
		for(long i = 0; i < cnt_props; ++i)
			this->Props[i] = new Prop(Props[i]);
	}
}
void Base::fill_methods(const Method * Methods, long cnt_methods) {
	/*заполним возможные методы, если они есть*/
	if(cnt_methods) {
		this->cnt_methods = cnt_methods;
		this->Methods = new Method*[cnt_methods];
		for(long i = 0; i < cnt_methods; ++i)
			this->Methods[i] = new Method(Methods[i]);
	}
}

Base::~Base() {
	/*освобождаем массивы имен свойств*/
	for(long i = 0; i < cnt_props; ++i)
		delete Props[i];
	delete[] Props;
	Props = nullptr;
	cnt_props = 0;

	/*освобождаем массивы имен методов*/
	for(long i = 0; i < cnt_methods; ++i)
		delete Methods[i];
	delete[] Methods;
	Methods = nullptr;
	cnt_methods = 0;

	delete[] name;
}


bool Base::Init(void* pConnection) {
	pAdapter = (Adapter*)pConnection;
	return pAdapter != nullptr;
}
bool Base::setMemManager(void* mem) {
	pMemoryAdapter = (MemoryAdapter*)mem;
	return pMemoryAdapter != 0;
}
long Base::GetInfo() {
	/*Компонента должна сообщить свою версию*/
	return 2000;
}
void Base::Done() {}

void Base::SetLocale(const WCHAR_T* loc) {
	#ifndef __linux__
		_wsetlocale(LC_ALL, loc);
	#else
		//We convert in char* char_locale
		//also we establish locale
		//setlocale(LC_ALL, char_locale);
	#endif
}

bool Base::RegisterExtensionAs(WCHAR_T** ext_name) {
	if(pMemoryAdapter!=nullptr) {
		int len = wcslen(name) + 1;
		if(pMemoryAdapter->AllocMemory((void**)ext_name, len * sizeof(WCHAR_T)))
			wchar_to_WCHAR(*ext_name, name, len);
		return true;
	}else
		return false;
}

/*"Рефлексия" из 1С*/
long Base::FindProp(const WCHAR_T* ex_name) {
	long res = -1;
	wchar_t * temp = nullptr;
	WCHAR_to_wchar(temp, ex_name);
		
	for(long i = 0; res == -1 && i < cnt_props; ++i)
		if(!wcscmp(Props[i]->en, temp) || !wcscmp(Props[i]->ru, temp))
			res = i;
	
	delete[] temp;

	return res;
}
bool Base::IsPropReadable(const long num) {
	if(num < cnt_props)
		return Props[num]->r;
	else
		return false;
}
bool Base::IsPropWritable(const long num) {
	if(num < cnt_props)
		return Props[num]->w;
	else
		return false;
}
const WCHAR_T* Base::GetPropName(long num, long cur_locale) {
	if(num < cnt_props) {
		wchar_t * temp;
		if(cur_locale == 0)
			temp=Props[num]->en;
		else 
			temp = Props[num]->ru;		
		
		WCHAR_T * res_name = NULL;
		int len = wcslen(temp) + 1;
		
		if(pMemoryAdapter 
		   && temp
		   && pMemoryAdapter->AllocMemory((void**)&res_name, len*sizeof(WCHAR_T)))
				wchar_to_WCHAR(res_name, temp, len);
		
		return res_name;
	} else
		return nullptr;
}
long Base::FindMethod(const WCHAR_T* ex_name) {
	long res = -1;
	wchar_t * temp = nullptr;
	WCHAR_to_wchar(temp, ex_name);

	for(long i = 0; res == -1 && i < cnt_methods; ++i)
		if(!wcscmp(Methods[i]->en, temp) || !wcscmp(Methods[i]->ru, temp))
			res = i;

	delete[] temp;

	return res;
}
bool Base::HasRetVal(const long num) {
	if(num < cnt_methods)
		return Methods[num]->is_proc == false;
	else
		return false;
}
const WCHAR_T* Base::GetMethodName(const long num, const long cur_locale) {
	if(num < cnt_methods) {
		wchar_t * temp;
		if(cur_locale == 0)
			temp = Methods[num]->en;
		else
			temp = Methods[num]->ru;

		WCHAR_T * res_name = NULL;
		int len = wcslen(temp) + 1;

		if(pMemoryAdapter
		   && temp
		   && pMemoryAdapter->AllocMemory((void**)&res_name, len*sizeof(WCHAR_T)))
		   wchar_to_WCHAR(res_name, temp, len);

		return res_name;
	} else
		return nullptr;
}
long Base::GetNParams(const long num) {
	if(num < cnt_methods)
		return Methods[num]->cnt;
	else
		return 0;
}

void Base::addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code) {
	if(pAdapter) {
		WCHAR_T *err   = nullptr;
		WCHAR_T *descr = nullptr;

		wchar_to_WCHAR(err, source);
		wchar_to_WCHAR(descr, descriptor);

		pAdapter->AddError(wcode, err, descr, code);
		delete[] err, descr;
	}
}
