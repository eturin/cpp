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
	TM1_INDEX getLastError(std::ostringstream &sout, TM1V val, bool isShow = false)const noexcept;
public:
	//�����������
	Cube(const Server &server, const char * CubeName=nullptr, TM1_INDEX CubeNameLen=0);
	//��������� ������������
	Cube(const Cube &) = delete;
	Cube(Cube &&) = delete;
	//��������� ���������
	Cube & operator=(const Cube &) = delete;
	Cube & operator=(Cube &&) = delete;
	//����������
	~Cube() noexcept;

	//��������� ����� ����
	const char * getCubeName()const noexcept;
	//�������� �����������-�� ���
	bool   exist() noexcept;
	//�������� ������� ����
	bool   makeNew();
	//����������� �� ������ ��������������� ����
	bool   makeDuplicate();
	//���������� ��������������� ��������� � ���
	void   addDimension(const char * DimensionName, TM1_INDEX DimensionNameLen = 0);
	//���������� ����
	bool   registerCube(const char * CubeName=nullptr, TM1_INDEX CubeNameLen = 0);
	//�������� ����
	bool   deleteCube() noexcept;
	
	//�������� ���-�� ���������
	TM1_INDEX getCountDimensions()const;
	//��������� ������ � �����������
	std::string showDimensions()const;
	//��������� ��������� �� �������
	TM1V getDimensionByIndex(TM1_INDEX i)const noexcept;
	//��������� ��������� �� �����
	TM1V getDimensionByName(const char *NameDimension)const noexcept;
	
	//��������� ���-�� �������������
	TM1_INDEX getCountViews()const;
	//��������� ������ � ������� �������������
	std::string showViews()const;
	//��������� ������������� �� �������
	TM1V getViewByIndex(TM1_INDEX i)const noexcept;
	//��������� ������������� �� �����
	TM1V getViewByName(const char *NameView)const noexcept;
		
	TM1V getRule();	
	
	//��������� �������� ������ ����
	TM1V getCellValue(const std::vector<Dimension*> & Dimensions)const;
	//��������� �������� ������ ����
	TM1V setCellValue(const std::vector<Dimension*> & Dimensions, const char * val)const;
	TM1V setCellValue(const std::vector<Dimension*> & Dimensions, double val)const;

	//��������� �������
	const Server& getServer()const noexcept;
	//��������� ����������� ����
	TM1P gethPool()const noexcept;
	//��������� ����������� ������
	TM1U gethUser()const noexcept;
	//��������� ����������� ����
	TM1V gethCube()const noexcept;

};

#endif