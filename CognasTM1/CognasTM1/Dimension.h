#ifndef DIMENSION_H
#define DIMENSION_H

#include "Common.h"
#include "Server.h"
#include "Cube.h"


class Dimension{
private:
	//��������
	const Server &server;
	TM1U hUser          = nullptr;  //TM1 API session handle
	TM1P hPool          = nullptr;  //���������� ���� ��������
	TM1V hDimension     = nullptr;  //���������� ��������� � ���� API
	TM1V hNewDimension  = nullptr;  //���������� ������������ ��������� � ���� API
	mutable TM1V vDimensionName = nullptr;  //��� ��������� � ���� API
	TM1V hElement       = nullptr;  //���������� �������� � ���� API
	TM1V vElement       = nullptr;  //�������� �������� � ���� API
	//������
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept;
public:
	//�������� ���������� �� ������ ��������������� ��������� �� �����
	Dimension(const Cube &cube, const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//�������� ���������� �� ������ ��������������� ��������� �� �������
	Dimension(const Cube &cube, TM1_INDEX i);
	//�������� ���������� ��� ������ ��������� ��� ��������� ��������������� �� �������
	Dimension(const Server &server, const char * DimensionName, TM1_INDEX DimensionNameLen = 0) noexcept;
	//��������� ������������
	Dimension(const Dimension &) = delete;
	Dimension(Dimension &&) = delete;
	//��������� ���������
	Dimension & operator=(const Dimension &) = delete;
	Dimension & operator=(Dimension &&) = delete;
	//����������
	~Dimension() noexcept;

	//��������� ����� ���������
	const char * getDimensionName()const;
	//�������� ��������������� ���������
	bool deleteDimension() noexcept;
	
	//��������� ����������� ��� ������
	bool isRead()const noexcept;
	//��������� ����������� ��� ������
	bool isWrite()const noexcept;
	//�������� ������������-�� ���������
	bool exist() noexcept;
	//�������� ������� ���������
	void makeNew() noexcept;
	//����������� �� ������ ��������������� ���������
	bool makeDuplicate();
	//���������� �������� � ������ ���������
	bool addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0);
	//�������� ������������ ���������
	bool check()const;
	//���������� ���������
	bool registerDimension(const char * DimensionName = nullptr, TM1_INDEX DimensionNameLen = 0);
	
	bool  setElementVal(const char * strVal);
	bool  setElementVal(double val);
	char *getElementVal()const noexcept;
	TM1V  gethElement()const noexcept;

	//��������� ���-�� �����������
	TM1_INDEX getCountSubsets()const;
	//��������� ������ � ������� �����������
	std::string showSubsets()const;
	//��������� ������������ �� �������
	TM1V getSubsetByIndex(TM1_INDEX i)const noexcept;
	//��������� ������������ �� �����
	TM1V getSubsetByName(const char *NameSubset)const noexcept;

	//��� ��������� �������� ���������
	TM1_INDEX getCountElements()const;
	std::string showElements()const;
	TM1V getElementByIndex(TM1_INDEX i)const noexcept;
	TM1V getElementByName(const char *NameElement)const noexcept;

	TM1V gethDimension()const noexcept;
};

#endif