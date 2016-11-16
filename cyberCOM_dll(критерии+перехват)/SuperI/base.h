/* Тута не писать! */

#ifndef BASE_H
#define BASE_H
#define _CRT_SECURE_NO_WARNINGS


#include "adapter.h"
#include "memory_adapter.h" 
#include "types.h" 
#include <cstring>
#include <functional>
using namespace std::placeholders;

/*интерфейс для инициализации компоненты (абстрактный класс)*/
class IInitDoneBase {
public:
	/*деструктор*/
	virtual ~IInitDoneBase() {}


	/* Инициализация
	*  @param disp - 1C:Enterpise interface
	*  @return the result of */
	virtual bool ADDIN_API Init(void* disp) = 0;


	/* Умтановка менеджера памяти
	*  @param mem - указатель на интерфейс менеджера памяти
	*  @return the result of */
	virtual bool ADDIN_API setMemManager(void* mem) = 0;


	/* Возврат версии компоненты
	*  @return - component version (2000 - это версия 2) */
	virtual long ADDIN_API GetInfo() = 0;


	/* Завершение работы компоненты.
	* Освобождение ресурсов.  */
	virtual void ADDIN_API Done() = 0;
};


/* Предопределенный интерфейс описания методов предназначенный для использования платформой 1С.
*  Интерфейс описывает расширение языка 1С*/
class ILanguageExtenderBase {
public:
	/*Деструктор*/
	virtual ~ILanguageExtenderBase() {}

	/* Имя класса в 1С
	*  @param ex_name - имя
	*  @return the result of  */
	virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** ex_name) = 0;

	/*==== СВОЙСТВА ====*/
	/* Возврат количества свойств класса, доступных из 1С
	*  @return количество свойств */
	virtual long ADDIN_API GetNProps() const = 0;

	/* Определение номера свойства по имени
	*  @param ex_name - имя свойсва
	*  @return индекс свойства или -1, если не найдено */
	virtual long ADDIN_API FindProp(const WCHAR_T* ex_name) const = 0;

	/* Возврат имени свойства по индексу
	*  @param num        - индекс свойства (начиная с 0)
	*  @param cur_locale - 0 - английское имя (обязательно должны быть),
	*                      1 - русское имя.
	*  @return имя свойства или 0, если не найдено */
	virtual const WCHAR_T* ADDIN_API GetPropName(long num, long cur_locale) const = 0;

	/* Возврат значения свойства
	*  @param num - индекс свойства (начиная с 0)
	*  @param val - указатель на variable со значение свойства
	*  @return the result of   */
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* val) const = 0;

	/* Установка значения свойства
	*  @param num - индекс свойства (начиная с 0)
	*  @param val - указатель на variable хранящий новое значение
	*  @return the result of */
	virtual bool ADDIN_API SetPropVal(const long num, tVariant* val) = 0;

	/* Проверка возможности чтения значения свойсва
	*  @param num - индекс свойства (начиная с 0)
	*  @return true, если свойсво можно читать  */
	virtual bool ADDIN_API IsPropReadable(const long num) const = 0;

	/* Проверка возможности изменять значение свойства
	*  @param num - индекс свойства (начиная с 0)
	*  @return true, если свойство можно изменить */
	virtual bool ADDIN_API IsPropWritable(const long num) const = 0;


	/*==== МЕТОДЫ ====*/
	/* Возврат количества методов класса, доступных из 1С
	*  @return количество методов */
	virtual long ADDIN_API GetNMethods() const = 0;

	/* Поиск метода по имени
	*  @param ex_name - имя метода
	*  @return             - индекс метода или -1, если не найдено  */
	virtual long ADDIN_API FindMethod(const WCHAR_T* ex_name) const = 0;

	/* Возврат имени метода по индексу
	*  @param num        - индекс метода (начиная с 0)
	*  @param cur_locale - 0 - английское имя (обязательно должны быть),
	*                      1 - русское имя.
	*  @return имя метода или 0, если не найдено  */
	virtual const WCHAR_T* ADDIN_API GetMethodName(const long num, const long cur_locale) const = 0;

	/* Возврат количества аргументов метода
	*  @param num - индекс метода (начиная с 0)
	*  @return количество аргументов  */
	virtual long ADDIN_API GetNParams(const long num) const = 0;

	/* Возвращает значения поумолчанию аргументов метода
	*  @param lMethodNum - индекс метода (начиная с 0)
	*  @param lParamNum  - индекс параметра (начиная с 0)
	*  @param pvarParamDefValue - указатель на variable со значением поумолчанию
	*  @return the result of  */
	virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) const = 0;

	/* Возвращает ли метод значение
	*  @param num - индекс метода (начиная с 0)
	*  @return true, если метод имеет возвращаемое значение */
	virtual bool ADDIN_API HasRetVal(const long num) const = 0;

	/* Вызов метода без возвращаемого значения
	*  @param lMethodNum - индекс метода (начиная с 0)
	*  @param paParams   - указатель на массив аргументов метода
	*  @param lSizeArray - размер массива
	*  @return the result of   */
	virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) = 0;

	/* Вызов метода с возвращаемым значением
	*  @param lMethodNum   - индекс метода (начиная с 0)
	*  @param pvarRetValue - указатель на возвращаемое значение
	*  @param paParams     - указатель на массив аргументов метода
	*  @param lSizeArray   - размер массива
	*  @return the result of  */
	virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) = 0;
};


/* Интерфейс используется, для изменения локализации в компоненте */
class LocaleBase {
public:
	/*Деструктор*/
	virtual ~LocaleBase() {}

	/* Изменение локализации
	*  @param loc - новая локаль (для Windows - rus_RUS, для Linux - ru_RU, и т.п...)   */
	virtual void ADDIN_API SetLocale(const WCHAR_T* loc) = 0;
};


/***************************************************
*  Данный класс предназначен для наследования      *
****************************************************/
class Base:public IInitDoneBase, public ILanguageExtenderBase, public LocaleBase {
protected:
	/*вложенные типы (для "рефлексии типа из 1С")*/
	struct Prop {	
		wchar_t *ru, *en;
		bool    r,w;
		void Init(const wchar_t *ru, const wchar_t *en, bool r, bool w) {
			/*копируем имена свойств ru и en*/
			size_t len = wcslen(ru) + 1;
			this->ru = new wchar_t[len];
			memcpy(this->ru, ru, len*sizeof(wchar_t));
			len = wcslen(en) + 1;
			this->en = new wchar_t[len];
			memcpy(this->en, en, len*sizeof(wchar_t));
			/*копируем призначки чтения и записи*/
			this->r = r;
			this->w = w;
		}
		Prop(const wchar_t *ru, const wchar_t *en, bool r, bool w) {
			Init(ru, en, r, w);
		}
		Prop(const Prop & other) {
			Init(other.ru, other.en, other.r, other.w);
		}
		~Prop() {
			delete[] ru, en;
			ru = en = nullptr;
		}
	};
	struct Method {
		typedef bool(ADDIN_API *func_t)(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
		typedef bool(ADDIN_API *proc_t)(tVariant* paParams, const long lSizeArray);
		wchar_t *ru, *en;
		bool    is_proc;
		short   cnt;
		void Init(const wchar_t *ru, const wchar_t *en, short cnt, bool is_proc) {
			/*копируем имена методов ru и en*/
			size_t len = wcslen(ru) + 1;
			this->ru = new wchar_t[len];
			memcpy(this->ru, ru, len*sizeof(wchar_t));
			len = wcslen(en) + 1;
			this->en = new wchar_t[len];
			memcpy(this->en, en, len*sizeof(wchar_t));			
			/*копируем количество параметров*/
			this->cnt = cnt;
			/*копируем признак процедуры*/
			this->is_proc = is_proc;			
		}
		Method(const wchar_t *ru, const wchar_t *en, short cnt = 0, bool is_proc = false) {
			Init(ru, en, cnt, is_proc);
		}
		Method(const Method & other) {
			Init(other.ru, other.en, other.cnt, other.is_proc);
		}
		~Method() {
			delete[] ru, en;
			ru = en = nullptr;
		}
	};
	
	/*адаптеры*/
	Adapter            * pAdapter;
	MemoryAdapter      * pMemoryAdapter;
	
	/*Локальное имя*/
	wchar_t * name;
	/*свойства, методы и классы*/
	long cnt_props;
	Prop ** Props;	
	long cnt_methods;
	Method ** Methods;
	/*заполнение свойств и методов*/
	void fill_name(const wchar_t * name);
	void fill_props(const Prop * Prors = nullptr, long cnt_props = 0);
	void fill_methods(const Method * Methods = nullptr, long cnt_methods = 0);
		
	virtual void addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code) const;
	virtual void addError(const wchar_t * msg) const;
public:
	/*Деструктор*/
	virtual ~Base();
	/*Конструктор*/
	Base():cnt_props(0), Props(nullptr), cnt_methods(0), Methods(nullptr), pAdapter(nullptr), pMemoryAdapter(nullptr), name(nullptr) {}

	/*Инициализация и завершение*/
	virtual bool ADDIN_API Init(void*)                   override;
	virtual bool ADDIN_API setMemManager(void* mem)      override;
	virtual long ADDIN_API GetInfo()                     override;
	virtual void ADDIN_API Done()                        override;
	/*смена локализации*/
	virtual void ADDIN_API SetLocale(const WCHAR_T* loc) override;

	/*Расширение языка*/
	/*Регистрация расширения под конкретным именем*/
	virtual       bool     ADDIN_API RegisterExtensionAs(WCHAR_T**) override;

	virtual       long     ADDIN_API GetNProps() const override { return cnt_props; }
	virtual       long     ADDIN_API FindProp(const WCHAR_T* ex_name) const override;
	virtual const WCHAR_T* ADDIN_API GetPropName(long num, long cur_locale) const override;
	virtual       bool     ADDIN_API IsPropReadable(const long num) const override;
	virtual       bool     ADDIN_API IsPropWritable(const long num) const override;
	virtual       bool     ADDIN_API GetPropVal(const long num, tVariant* var) const override { return false; } //заглушка на случай отсутствия свойств
	virtual       bool     ADDIN_API SetPropVal(const long num, tVariant* val) override { return false; } //заглушка на случай отсутствия свойств
	
	virtual       long     ADDIN_API GetNMethods() const override { return cnt_methods; }
	virtual       long     ADDIN_API FindMethod(const WCHAR_T* ex_name) const override;
	virtual const WCHAR_T* ADDIN_API GetMethodName(const long num, const long cur_locale) const override;
	virtual       long     ADDIN_API GetNParams(const long num) const override;
	virtual       bool     ADDIN_API GetParamDefValue(const long num, const long lParamNum, tVariant *pvarParamDefValue) const override { return false; }    //заглушка на случай отсутствия методов
	virtual       bool     ADDIN_API HasRetVal(const long num) const override;
	virtual       bool     ADDIN_API CallAsProc(const long num, tVariant* paParams, const long len) { return false; }                                  //заглушка на случай отсутствия методов
	virtual       bool     ADDIN_API CallAsFunc(const long num, tVariant* pvarRetValue, tVariant* paParams, const long len) { return false; }          //заглушка на случай отсутствия методов

	/*Отправка сообщений*/
	void addMsg(wchar_t * str_type, wchar_t * msg) const;
};

size_t char_to_wchar(wchar_t* &Dest, const char* Source, size_t len = 0);
size_t wchar_to_WCHAR(WCHAR_T* &Dest, const wchar_t* Source, size_t len = 0);
size_t wchar_to_char(char * &Dest, const wchar_t* Source, size_t len = 0);
size_t WCHAR_to_wchar(wchar_t* &Dest, const WCHAR_T* Source, size_t len = 0);
size_t WCHAR_to_char(char * &Dest, const WCHAR_T* Source, size_t len = 0);
size_t WCHAR_len(const WCHAR_T* Source);
size_t wchar_len(const wchar_t* Source);

/* These functions should be implemented that component can be loaded and created. */
extern "C" long GetClassObject(const WCHAR_T*, Base** pIntf);
extern "C" long DestroyObject(Base** pIntf);
extern "C" const WCHAR_T* GetClassNames();

/*указатели на функции*/
typedef long(*GetClassObjectPtr)(const WCHAR_T* wsName, Base** pIntf);
typedef long(*DestroyObjectPtr)(Base** pIntf);
typedef const WCHAR_T* (*GetClassNamesPtr)();

#endif 
