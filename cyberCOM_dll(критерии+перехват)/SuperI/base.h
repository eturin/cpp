/* ���� �� ������! */

#ifndef BASE_H
#define BASE_H
#define _CRT_SECURE_NO_WARNINGS


#include "adapter.h"
#include "memory_adapter.h" 
#include "types.h" 
#include <cstring>
#include <functional>
using namespace std::placeholders;

/*��������� ��� ������������� ���������� (����������� �����)*/
class IInitDoneBase {
public:
	/*����������*/
	virtual ~IInitDoneBase() {}


	/* �������������
	*  @param disp - 1C:Enterpise interface
	*  @return the result of */
	virtual bool ADDIN_API Init(void* disp) = 0;


	/* ��������� ��������� ������
	*  @param mem - ��������� �� ��������� ��������� ������
	*  @return the result of */
	virtual bool ADDIN_API setMemManager(void* mem) = 0;


	/* ������� ������ ����������
	*  @return - component version (2000 - ��� ������ 2) */
	virtual long ADDIN_API GetInfo() = 0;


	/* ���������� ������ ����������.
	* ������������ ��������.  */
	virtual void ADDIN_API Done() = 0;
};


/* ���������������� ��������� �������� ������� ��������������� ��� ������������� ���������� 1�.
*  ��������� ��������� ���������� ����� 1�*/
class ILanguageExtenderBase {
public:
	/*����������*/
	virtual ~ILanguageExtenderBase() {}

	/* ��� ������ � 1�
	*  @param ex_name - ���
	*  @return the result of  */
	virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** ex_name) = 0;

	/*==== �������� ====*/
	/* ������� ���������� ������� ������, ��������� �� 1�
	*  @return ���������� ������� */
	virtual long ADDIN_API GetNProps() const = 0;

	/* ����������� ������ �������� �� �����
	*  @param ex_name - ��� �������
	*  @return ������ �������� ��� -1, ���� �� ������� */
	virtual long ADDIN_API FindProp(const WCHAR_T* ex_name) const = 0;

	/* ������� ����� �������� �� �������
	*  @param num        - ������ �������� (������� � 0)
	*  @param cur_locale - 0 - ���������� ��� (����������� ������ ����),
	*                      1 - ������� ���.
	*  @return ��� �������� ��� 0, ���� �� ������� */
	virtual const WCHAR_T* ADDIN_API GetPropName(long num, long cur_locale) const = 0;

	/* ������� �������� ��������
	*  @param num - ������ �������� (������� � 0)
	*  @param val - ��������� �� variable �� �������� ��������
	*  @return the result of   */
	virtual bool ADDIN_API GetPropVal(const long num, tVariant* val) const = 0;

	/* ��������� �������� ��������
	*  @param num - ������ �������� (������� � 0)
	*  @param val - ��������� �� variable �������� ����� ��������
	*  @return the result of */
	virtual bool ADDIN_API SetPropVal(const long num, tVariant* val) = 0;

	/* �������� ����������� ������ �������� �������
	*  @param num - ������ �������� (������� � 0)
	*  @return true, ���� ������� ����� ������  */
	virtual bool ADDIN_API IsPropReadable(const long num) const = 0;

	/* �������� ����������� �������� �������� ��������
	*  @param num - ������ �������� (������� � 0)
	*  @return true, ���� �������� ����� �������� */
	virtual bool ADDIN_API IsPropWritable(const long num) const = 0;


	/*==== ������ ====*/
	/* ������� ���������� ������� ������, ��������� �� 1�
	*  @return ���������� ������� */
	virtual long ADDIN_API GetNMethods() const = 0;

	/* ����� ������ �� �����
	*  @param ex_name - ��� ������
	*  @return             - ������ ������ ��� -1, ���� �� �������  */
	virtual long ADDIN_API FindMethod(const WCHAR_T* ex_name) const = 0;

	/* ������� ����� ������ �� �������
	*  @param num        - ������ ������ (������� � 0)
	*  @param cur_locale - 0 - ���������� ��� (����������� ������ ����),
	*                      1 - ������� ���.
	*  @return ��� ������ ��� 0, ���� �� �������  */
	virtual const WCHAR_T* ADDIN_API GetMethodName(const long num, const long cur_locale) const = 0;

	/* ������� ���������� ���������� ������
	*  @param num - ������ ������ (������� � 0)
	*  @return ���������� ����������  */
	virtual long ADDIN_API GetNParams(const long num) const = 0;

	/* ���������� �������� ����������� ���������� ������
	*  @param lMethodNum - ������ ������ (������� � 0)
	*  @param lParamNum  - ������ ��������� (������� � 0)
	*  @param pvarParamDefValue - ��������� �� variable �� ��������� �����������
	*  @return the result of  */
	virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) const = 0;

	/* ���������� �� ����� ��������
	*  @param num - ������ ������ (������� � 0)
	*  @return true, ���� ����� ����� ������������ �������� */
	virtual bool ADDIN_API HasRetVal(const long num) const = 0;

	/* ����� ������ ��� ������������� ��������
	*  @param lMethodNum - ������ ������ (������� � 0)
	*  @param paParams   - ��������� �� ������ ���������� ������
	*  @param lSizeArray - ������ �������
	*  @return the result of   */
	virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) = 0;

	/* ����� ������ � ������������ ���������
	*  @param lMethodNum   - ������ ������ (������� � 0)
	*  @param pvarRetValue - ��������� �� ������������ ��������
	*  @param paParams     - ��������� �� ������ ���������� ������
	*  @param lSizeArray   - ������ �������
	*  @return the result of  */
	virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) = 0;
};


/* ��������� ������������, ��� ��������� ����������� � ���������� */
class LocaleBase {
public:
	/*����������*/
	virtual ~LocaleBase() {}

	/* ��������� �����������
	*  @param loc - ����� ������ (��� Windows - rus_RUS, ��� Linux - ru_RU, � �.�...)   */
	virtual void ADDIN_API SetLocale(const WCHAR_T* loc) = 0;
};


/***************************************************
*  ������ ����� ������������ ��� ������������      *
****************************************************/
class Base:public IInitDoneBase, public ILanguageExtenderBase, public LocaleBase {
protected:
	/*��������� ���� (��� "��������� ���� �� 1�")*/
	struct Prop {	
		wchar_t *ru, *en;
		bool    r,w;
		void Init(const wchar_t *ru, const wchar_t *en, bool r, bool w) {
			/*�������� ����� ������� ru � en*/
			size_t len = wcslen(ru) + 1;
			this->ru = new wchar_t[len];
			memcpy(this->ru, ru, len*sizeof(wchar_t));
			len = wcslen(en) + 1;
			this->en = new wchar_t[len];
			memcpy(this->en, en, len*sizeof(wchar_t));
			/*�������� ��������� ������ � ������*/
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
			/*�������� ����� ������� ru � en*/
			size_t len = wcslen(ru) + 1;
			this->ru = new wchar_t[len];
			memcpy(this->ru, ru, len*sizeof(wchar_t));
			len = wcslen(en) + 1;
			this->en = new wchar_t[len];
			memcpy(this->en, en, len*sizeof(wchar_t));			
			/*�������� ���������� ����������*/
			this->cnt = cnt;
			/*�������� ������� ���������*/
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
	
	/*��������*/
	Adapter            * pAdapter;
	MemoryAdapter      * pMemoryAdapter;
	
	/*��������� ���*/
	wchar_t * name;
	/*��������, ������ � ������*/
	long cnt_props;
	Prop ** Props;	
	long cnt_methods;
	Method ** Methods;
	/*���������� ������� � �������*/
	void fill_name(const wchar_t * name);
	void fill_props(const Prop * Prors = nullptr, long cnt_props = 0);
	void fill_methods(const Method * Methods = nullptr, long cnt_methods = 0);
		
	virtual void addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code) const;
	virtual void addError(const wchar_t * msg) const;
public:
	/*����������*/
	virtual ~Base();
	/*�����������*/
	Base():cnt_props(0), Props(nullptr), cnt_methods(0), Methods(nullptr), pAdapter(nullptr), pMemoryAdapter(nullptr), name(nullptr) {}

	/*������������� � ����������*/
	virtual bool ADDIN_API Init(void*)                   override;
	virtual bool ADDIN_API setMemManager(void* mem)      override;
	virtual long ADDIN_API GetInfo()                     override;
	virtual void ADDIN_API Done()                        override;
	/*����� �����������*/
	virtual void ADDIN_API SetLocale(const WCHAR_T* loc) override;

	/*���������� �����*/
	/*����������� ���������� ��� ���������� ������*/
	virtual       bool     ADDIN_API RegisterExtensionAs(WCHAR_T**) override;

	virtual       long     ADDIN_API GetNProps() const override { return cnt_props; }
	virtual       long     ADDIN_API FindProp(const WCHAR_T* ex_name) const override;
	virtual const WCHAR_T* ADDIN_API GetPropName(long num, long cur_locale) const override;
	virtual       bool     ADDIN_API IsPropReadable(const long num) const override;
	virtual       bool     ADDIN_API IsPropWritable(const long num) const override;
	virtual       bool     ADDIN_API GetPropVal(const long num, tVariant* var) const override { return false; } //�������� �� ������ ���������� �������
	virtual       bool     ADDIN_API SetPropVal(const long num, tVariant* val) override { return false; } //�������� �� ������ ���������� �������
	
	virtual       long     ADDIN_API GetNMethods() const override { return cnt_methods; }
	virtual       long     ADDIN_API FindMethod(const WCHAR_T* ex_name) const override;
	virtual const WCHAR_T* ADDIN_API GetMethodName(const long num, const long cur_locale) const override;
	virtual       long     ADDIN_API GetNParams(const long num) const override;
	virtual       bool     ADDIN_API GetParamDefValue(const long num, const long lParamNum, tVariant *pvarParamDefValue) const override { return false; }    //�������� �� ������ ���������� �������
	virtual       bool     ADDIN_API HasRetVal(const long num) const override;
	virtual       bool     ADDIN_API CallAsProc(const long num, tVariant* paParams, const long len) { return false; }                                  //�������� �� ������ ���������� �������
	virtual       bool     ADDIN_API CallAsFunc(const long num, tVariant* pvarRetValue, tVariant* paParams, const long len) { return false; }          //�������� �� ������ ���������� �������

	/*�������� ���������*/
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

/*��������� �� �������*/
typedef long(*GetClassObjectPtr)(const WCHAR_T* wsName, Base** pIntf);
typedef long(*DestroyObjectPtr)(Base** pIntf);
typedef const WCHAR_T* (*GetClassNamesPtr)();

#endif 
