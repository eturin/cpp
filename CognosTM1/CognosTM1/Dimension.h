#ifndef DIMENSION_H
#define DIMENSION_H

#include "Common.h"
#include "Object.h"

#include "Server.h"
#include "Cube.h"


class Dimension: public Object{
private:
	//��������
	const Server &server;	
	TM1V hElement       = nullptr;  //���������� �������� � ���� API
	TM1V vElement       = nullptr;  //�������� �������� � ���� API
	
public:
	//�������� ���������� �� ������ ��������������� ��������� �� �����
	Dimension(const Cube &cube, const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//�������� ���������� �� ������ ��������������� ��������� �� �������
	Dimension(const Cube &cube, TM1_INDEX i);
	//�������� ���������� ��� ������ ��������� ��� ��������� ��������������� �� �������
	Dimension(const Server &server, const char * DimensionName, TM1_INDEX DimensionNameLen = 0) noexcept;
	//��������� ������������
	Dimension(const Dimension &)             = delete;
	Dimension(Dimension &&)                  = delete;
	//��������� ���������
	Dimension & operator=(const Dimension &) = delete;
	Dimension & operator=(Dimension &&)      = delete;
					
	//�������� ������������-�� ���������
	virtual bool exist() noexcept override;
	//�������� ������� ���������
	inline void makeNew() noexcept {
		hNewObject = TM1DimensionCreateEmpty(hPool, server.gethObject());
	}
	
	//���������� �������� � ������ ���������
	bool addElement(const char * ElementName, TM1_INDEX ElementNameLen = 0);	
	//�������� ������������ ���������
	bool check()const;
	//���������� ���������
	bool registerDimension(const char * DimensionName = nullptr, TM1_INDEX DimensionNameLen = 0);
	
	bool  setElementVal(const char * strVal, TM1_INDEX strValLen = 0);
	bool  setElementVal(double val);	
	inline TM1V  gethElement()const noexcept { return hElement; }

	//��������� ���-�� �����������
	inline TM1_INDEX getCountSubsets()const {
		return getListCount(TM1DimensionSubsets());
	}
	//��������� ������ � ������� �����������
	inline std::string showSubsets()const {
		return std::move(showList(TM1DimensionSubsets()));
	}
	//��������� ������������ �� �������
	inline TM1V getSubsetByIndex(TM1_INDEX i)const {
		return getListItemByIndex(i, TM1DimensionSubsets());
	}
	//��������� ������������ �� �����
	inline TM1V getSubsetByName(const char *NameSubset, TM1_INDEX NameSubsetLen=0)const {
		TM1V vName;
		return getListItemByName(TM1DimensionSubsets(), NameSubset, vName, NameSubsetLen);
	}

	//��� ��������� �������� ���������
	inline TM1_INDEX getCountElements()const {
		return getListCount(TM1DimensionElements());
	}
	//��������� ������ � ������� ���������
	inline std::string showElements()const {
		return std::move(showList(TM1DimensionElements()));
	}
	//��������� �������� �� �������
	inline TM1V getElementByIndex(TM1_INDEX i)const {
		return getListItemByIndex(i, TM1DimensionElements());
	}
	//��������� �������� �� �����
	inline TM1V getElementByName(const char *NameElement, TM1_INDEX NameElementLen=0)const {
		TM1V vName;
		return getListItemByName(TM1DimensionElements(), NameElement, vName, NameElementLen);
	}
		
	inline const Server & getServer() const noexcept { return server; }
};

#endif