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
	TM1V vDimensionName = nullptr;  //��� ��������� � ���� API
	TM1V hElement       = nullptr;  //���������� �������� � ���� API
	TM1V vElement       = nullptr;  //�������� �������� � ���� API
	//������
	TM1_INDEX getLastError(TM1V val, bool isShow = false)const;
public:
	Dimension(const Cube &cube, char * DimensionName, TM1_INDEX CubeNameLen = 0);
	Dimension(const Cube &cube, TM1_INDEX i);
	Dimension(const Server &server, char * DimensionName, TM1_INDEX CubeNameLen = 0);
	Dimension(const Dimension & other) = delete;
	Dimension & operator=(const Dimension & other) = delete;
	~Dimension();

	//������
	char * getDimensionName();
	bool deleteDimension();
	
	bool isRead()const;
	bool isWrite()const;
	bool exist();
	void makeNew();
	bool makeDuplicate();
	bool addElement(char * ElementName, TM1_INDEX ElementNameLen = 0);
	bool check()const;
	bool registerDimension(char * DimensionName = nullptr, TM1_INDEX DimensionNameLen = 0);
	
	bool  setElementVal(char * strVal);
	bool  setElementVal(double val);
	char *getElementVal()const;
	TM1V  getElement()const;

	//������������ �������� ���������
	TM1_INDEX getCountSubsets()const;
	void showSubsets()const;
	TM1V getSubsetByIndex(TM1_INDEX i)const;
	TM1V getSubsetByName(char *NameSubset)const;

	//��� ��������� �������� ���������
	TM1_INDEX getCountElements()const;
	void showElements()const;
	TM1V getElementByIndex(TM1_INDEX i)const;
	TM1V getElementByName(char *NameElement)const;

	TM1V gethDimension()const;
};

#endif