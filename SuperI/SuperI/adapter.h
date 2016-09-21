/* Тута тоже не писать */

#ifndef ADAPTER_H
#define ADAPTER_H

#include "types.h"

/* Базовый интерфейс объекта платформы (для взаимодействия с платформой 1С)*/
class Adapter {
public:
	/*Деструктор*/
	virtual ~Adapter() {}

	/* Добавление сообщения об ошибке
	*  @param wcode  - код ошибки
	*  @param source - источник ошибки
	*  @param descr  - описание ошибки
	*  @param scode  - код ошибки (HRESULT)
	*  @return the result of  */
	virtual bool ADDIN_API AddError(unsigned short wcode, const WCHAR_T* source, const WCHAR_T* descr, long scode) = 0;

	/* Чтение значения свойства
	*  @param wszPropName   - имя свойства
	*  @param pVal          - возвращаемое значение
	*  @param pErrCode      - код ошибки (если была ошибка)
	*  @param errDescriptor - описание ошибки (если была ошибка)
	*  @return the result of read.  */
	virtual bool ADDIN_API Read(WCHAR_T* wszPropName, tVariant* pVal, long *pErrCode, WCHAR_T** errDescriptor) = 0;

	/* Запись значения свойства
	*  @param wszPropName - имя свойства
	*  @param pVar        - новое знаение свойства
	*  @return the result of write. */
	virtual bool ADDIN_API Write(WCHAR_T* wszPropName, tVariant *pVar) = 0;

	/* Регистрация компонеты под именем
	*  @param wszProfileName - имя
	*  @return the result of  */
	virtual bool ADDIN_API RegisterProfileAs(WCHAR_T* wszProfileName) = 0;

	/* Изменение длины буфера событий
	*  @param lDepth - новая длина буфера событий
	*  @return the result of  */
	virtual bool ADDIN_API SetEventBufferDepth(long lDepth) = 0;

	/* Определение длины буфера событий
	*  @return the depth of event buffer  */
	virtual long ADDIN_API GetEventBufferDepth() = 0;

	/* Регистрация внешнего события
	*  @param wszSource  - источник события
	*  @param wszMessage - содержание события
	*  @param wszData    - параметр сообщения
	*  @return the result of */
	virtual bool ADDIN_API ExternalEvent(WCHAR_T* wszSource, WCHAR_T* wszMessage, WCHAR_T* wszData) = 0;

	/* Очистка буфера событий */
	virtual void ADDIN_API CleanEventBuffer() = 0;

	/* Установка значения строки состояния
	*  @param wszStatusLine - новая строка
	*  @return the result of */
	virtual bool ADDIN_API SetStatusLine(WCHAR_T* wszStatusLine) = 0;


	/* Сброс значения строки состояния
	*  @return the result of */
	virtual void ADDIN_API ResetStatusLine() = 0;
};

#endif 
