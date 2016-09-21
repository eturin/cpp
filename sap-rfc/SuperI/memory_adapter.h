/* � ���� �� ������ */

#ifndef MEMORY_ADAPTER_H
#define MEMORY_ADAPTER_H

#include "types.h"

/* ���������������� �����, ��� ��������� � ������������ ������ � ���������*/
class MemoryAdapter {
public:
	/*����������*/
	virtual ~MemoryAdapter() {}


	/* ��������� ������ ������������� �������
	*  @param pMemory - ��������� �� ��������� �� ����������, ������� ����� ������� ����� ������ ����� ������ NULL, ���� �������� �� ������� .
	*  @param ulCountByte - ������ ������
	*  @return the result of  */
	virtual bool ADDIN_API AllocMemory(void** pMemory, unsigned long ulCountByte) = 0;


	/* ������������ ������
	*  @param pMemory - ��������� �� ��������� �� ���� ������, ������� ���� ����������  */
	virtual void ADDIN_API FreeMemory(void** pMemory) = 0;
};

#endif 
