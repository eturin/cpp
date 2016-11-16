/* ���� ���� �� ������ */

#ifndef ADAPTER_H
#define ADAPTER_H

#include "types.h"

/* ������� ��������� ������� ��������� (��� �������������� � ���������� 1�)*/
class Adapter {
public:
	/*����������*/
	virtual ~Adapter() {}

	/* ���������� ��������� �� ������
	*  @param wcode  - ��� ������
	*  @param source - �������� ������
	*  @param descr  - �������� ������
	*  @param scode  - ��� ������ (HRESULT)
	*  @return the result of  */
	virtual bool ADDIN_API AddError(unsigned short wcode, const WCHAR_T* source, const WCHAR_T* descr, long scode) = 0;

	/* ������ �������� ��������
	*  @param wszPropName   - ��� ��������
	*  @param pVal          - ������������ ��������
	*  @param pErrCode      - ��� ������ (���� ���� ������)
	*  @param errDescriptor - �������� ������ (���� ���� ������)
	*  @return the result of read.  */
	virtual bool ADDIN_API Read(WCHAR_T* wszPropName, tVariant* pVal, long *pErrCode, WCHAR_T** errDescriptor) = 0;

	/* ������ �������� ��������
	*  @param wszPropName - ��� ��������
	*  @param pVar        - ����� ������� ��������
	*  @return the result of write. */
	virtual bool ADDIN_API Write(WCHAR_T* wszPropName, tVariant *pVar) = 0;

	/* ����������� ��������� ��� ������
	*  @param wszProfileName - ���
	*  @return the result of  */
	virtual bool ADDIN_API RegisterProfileAs(WCHAR_T* wszProfileName) = 0;

	/* ��������� ����� ������ �������
	*  @param lDepth - ����� ����� ������ �������
	*  @return the result of  */
	virtual bool ADDIN_API SetEventBufferDepth(long lDepth) = 0;

	/* ����������� ����� ������ �������
	*  @return the depth of event buffer  */
	virtual long ADDIN_API GetEventBufferDepth() = 0;

	/* ����������� �������� �������
	*  @param wszSource  - �������� �������
	*  @param wszMessage - ���������� �������
	*  @param wszData    - �������� ���������
	*  @return the result of */
	virtual bool ADDIN_API ExternalEvent(WCHAR_T* wszSource, WCHAR_T* wszMessage, WCHAR_T* wszData) = 0;

	/* ������� ������ ������� */
	virtual void ADDIN_API CleanEventBuffer() = 0;

	/* ��������� �������� ������ ���������
	*  @param wszStatusLine - ����� ������
	*  @return the result of */
	virtual bool ADDIN_API SetStatusLine(WCHAR_T* wszStatusLine) = 0;


	/* ����� �������� ������ ���������
	*  @return the result of */
	virtual void ADDIN_API ResetStatusLine() = 0;
};

#endif 
