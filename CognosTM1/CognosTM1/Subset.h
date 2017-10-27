#ifndef SUBSET_H
#define SUBSET_H

#include "Common.h"
#include "Object.h"

#include "Dimension.h"

class Subset: public Object {
private:
	//��������
	const Dimension &dimension;
	
public:
	//�������� ���������� �� ������ ��������������� ������������ �� �����
	Subset(const Dimension &dimension, const char * SubsetName, TM1_INDEX SubsetLen = 0);
	//�������� ���������� �� ������ ��������������� ������������ �� �������
	Subset(const Dimension &dimension, TM1_INDEX i);
	//��������� ������������
	Subset(const Subset &)             = delete;
	Subset(Subset &&)                  = delete;
	//��������� ���������
	Subset & operator=(const Subset &) = delete;
	Subset & operator=(Subset &&)      = delete;
		
	//�������� ������������-�� ���������
	virtual bool exist() noexcept override;
	//�������� ������� ������������
	void makeNew() noexcept;
	//�������� ������������ �� ������ MDX ������������
	void makeNewWithMDX(const char * Expression, TM1_INDEX ExpressionLen = 0);
	
	//���������� �������� � ������ ������������
	bool addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0);
	
	//���������� 
	bool registerSubset(const char * SubsetName = nullptr, TM1_INDEX SubsetNameLen = 0);
	

	//���������� ��������� � ������������
	inline TM1_INDEX getCountElements(TM1V hObject = nullptr)const {
		return getListCount(TM1SubsetElements(), hObject);
	}
	//��������� ������ � ������� ���������
	inline std::string showElements()const {
		return std::move(showList(TM1SubsetElements()));
	}

	//���������� ���� ��������� ���������
	bool addAllElements();
	
};

#endif 

