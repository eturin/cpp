#ifndef CUBE_H
#define CUBE_H

#include "Common.h"
#include "Server.h"
#include <queue>
#include <string>
#include <vector>

class Dimension;

class Cube{
private:
	//��������
	const Server &server; 
	TM1U hUser          = nullptr;  //TM1 API session handle
	TM1P hPool          = nullptr;  //���������� ���� ��������
	TM1V hCube          = nullptr;  //���������� ���� � ���� API
	TM1V hNewCube       = nullptr;  //���������� ������������ ���� � ���� API
	TM1V vCubeName      = nullptr;  //��� ������� � ���� API
	TM1V vRule          = nullptr;  //���������� � ���� API	

	std::vector<std::string> Dimensions;
	//������
	TM1_INDEX getLastError(TM1V val, bool isShow = false)const;	
public:
	Cube(const Server &server, char * CubeName=nullptr, TM1_INDEX CubeNameLen=0);
	//Cube(const Server &server, char * CubeName, TM1_INDEX CubeNameLen, std::queue<std::string> & qDimensions);
	Cube(const Cube &other) = delete;
	Cube & operator=(const Cube &other) = delete;
	~Cube();

	//������
	char * getCubeName()const;
	bool   exist();
	bool   makeNew();
	bool   makeDuplicate();
	void   addDimension(char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	bool   registerCube(char * CubeName=nullptr, TM1_INDEX CubeNameLen = 0);
	bool   deleteCube();
	const Server& getServer()const;

	TM1_INDEX getCountDimensions()const;
	void showDimensions()const;
	TM1V getDimensionByIndex(TM1_INDEX i)const;
	TM1V getDimensionByName(char *NameDimension)const;
	
	TM1_INDEX getCountViews()const;
	void showViews()const;
	TM1V getViewByIndex(TM1_INDEX i)const;
	TM1V getViewByName(char *NameView)const;
		
	TM1V getRule();	
	

	TM1V getCellValue(const std::vector<Dimension*> & Dimensions)const;
	TM1V setCellValue(const std::vector<Dimension*> & Dimensions, char * val)const;
	TM1V setCellValue(const std::vector<Dimension*> & Dimensions, double val)const;

	
	TM1P gethPool()const;
	TM1U gethUser()const;
	TM1V gethCube()const;

};

#endif