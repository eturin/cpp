#ifndef CUBE_H
#define CUBE_H

#include "Common.h"
#include "Object.h"

#include "Server.h"
#include <queue>
#include <string>
#include <vector>

class Dimension;

class Cube: public Object{
private:
	//��������
	const Server &server; 	
	std::vector<std::string> Dimensions;	
public:
	//�����������
	Cube(const Server &server, const char * CubeName=nullptr, TM1_INDEX CubeNameLen=0, bool isPublic = true);
	//�������� ���������� �� ������ ��������������� ���� �� �������
	Cube(const Server &server, TM1_INDEX i, bool isPublic = true);
	//��������� ������������
	Cube(const Cube &)             = delete;
	Cube(Cube &&)                  = delete;
	//��������� ���������
	Cube & operator=(const Cube &) = delete;
	Cube & operator=(Cube &&)      = delete;
		
	//�������� �����������-�� ���
	virtual bool exist(bool isPublic = true) noexcept override;
	//�������� ������� ����
	bool   makeNew();
	
	//���������� ��������������� ��������� � ���
	void   addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//���������� ����
	bool   registerCube(bool isPublic, const char * CubeName=nullptr, TM1_INDEX CubeNameLen = 0);
		
	//�������� ���-�� ���������
	inline TM1_INDEX getCountDimensions(bool isPublic=true,TM1V hObject=nullptr)const {
		return getListCount(TM1CubeDimensions(), isPublic, (hObject ? hObject : this->hObject));
	}
	//��������� ������ � �����������
	inline std::string showDimensions()const {
		return std::move(showList(TM1CubeDimensions()));
	}
	//��������� ��������� �� �������
	inline TM1V getDimensionByIndex(TM1_INDEX i)const  {
		return getListItemByIndex(i, TM1DimensionSubsets());
	}
	//��������� ��������� �� �����
	inline TM1V getDimensionByName(const char *NameDimension, TM1_INDEX NameDimensionLen = 0)const {
		TM1V vName;
		return getListItemByName(TM1DimensionSubsets(), NameDimension, vName, NameDimensionLen);
	}

	//��������� ���-�� �������������
	inline TM1_INDEX getCountViews(bool isPublic=true, bool isMDX=false)const {
		return getListCount(isMDX ? TM1ValIndex(hPool, 357) : TM1CubeViews(), isPublic);
	}
	
	//��������� ������ � ������� �������������
	inline std::string showViews()const {
		return std::move(showList(TM1CubeViews()));
	}
	//��������� ������������� �� �������
	inline TM1V getViewByIndex(TM1_INDEX i)const {
		return getListItemByIndex(i, TM1CubeViews());
	}
	//��������� ������������� �� �����
	inline TM1V getViewByName(const char *NameView, TM1_INDEX NameViewLen = 0)const {
		TM1V vName;
		return getListItemByName(TM1CubeViews(), NameView, vName, NameViewLen);
	}
		
	//��������� �������� ������ ����
	TM1V getCellValue(const std::vector<Dimension*> & Dimensions)const;
	//��������� �������� ������ ����
	TM1V setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, const char * val, TM1_INDEX valLen = 0)const;
	TM1V setCellValue(const std::vector<std::unique_ptr<Dimension>> & Dimensions, double val)const;

	//��������� �������
	inline const Server& getServer()const noexcept {
		return server;
	}
};

#endif