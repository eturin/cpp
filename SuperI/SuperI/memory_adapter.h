/* » тута не писать */

#ifndef MEMORY_ADAPTER_H
#define MEMORY_ADAPTER_H

#include "types.h"

/* ѕредопределенный класс, дл€ выделени€ и освобождени€ пам€ти в платформе*/
class MemoryAdapter {
public:
	/*ƒеструктор*/
	virtual ~MemoryAdapter() {}


	/* ¬ыделение пам€ти определенного размера
	*  @param pMemory - указатель на указатель на переменную, который будет хранить адрес нового блока пам€ти NULL, если выделить не удалось .
	*  @param ulCountByte - размер пам€ти
	*  @return the result of  */
	virtual bool ADDIN_API AllocMemory(void** pMemory, unsigned long ulCountByte) = 0;


	/* ќсвобождение пам€ти
	*  @param pMemory - указатель на указатель на блок пам€ти, который надо освободить  */
	virtual void ADDIN_API FreeMemory(void** pMemory) = 0;
};

#endif 
