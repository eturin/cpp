#define _CRT_SECURE_NO_WARNINGS

#include "Subset.h"

Subset::Subset(const Dimension &dimension, const char * SubsetName, TM1_INDEX SubsetLen):Object(dimension),dimension(dimension) {
	//�������� ���������� ������������
	hObject = utilities::getObjectByName(hUser, hPool, dimension.gethObject(), TM1DimensionSubsets(), SubsetName, vName, SubsetLen);
}
Subset::Subset(const Dimension &dimension, TM1_INDEX i) : Object(dimension), dimension(dimension) {
	//�������� ���������� ���������
	if (0 < i && i <= dimension.getCountSubsets())
		hObject = utilities::getObjectByIndex(hUser, hPool, dimension.gethObject(), TM1DimensionSubsets(), i);
	else {
		this->~Subset();
		std::ostringstream sout;
		sout << "������������ � �������� " << i << " �� ������� � ���������� " << dimension.getName() << std::endl;
		throw std::exception(sout.str().c_str());
	}
}

bool Subset::exist() noexcept {
	if (vName != nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, dimension.gethObject(), TM1DimensionSubsets(), vName);
	return hObject != nullptr;
}

void Subset::makeNew() noexcept {
	//������� ������ ������������ � ������� ���������
	hNewObject = TM1SubsetCreateEmpty(hPool, dimension.gethObject());
}

void Subset::makeNewWithMDX(const char * Expression, TM1_INDEX ExpressionLen) {	
	TM1V vExpression = TM1ValString(hPool, const_cast<char*>(Expression), ExpressionLen);
	hNewObject = TM1SubsetCreateByExpression(hPool, dimension.getServer().gethObject(), vExpression);
}

bool Subset::addElement(const char * ElementName, TM1_INDEX ElementNameLen) {
	if (hNewObject == nullptr)
		throw std::exception("����� ������ ������������ �� ���������������.");

	//�������� ����� � ���� ��� ��������
	TM1V vName, hElement = utilities::getObjectByName(hUser, hPool, dimension.gethObject(), TM1DimensionElements(), ElementName, vName, ElementNameLen);
	if (hElement == nullptr) {
		std::ostringstream sout;
		sout << "������� � ������ " << ElementName << " �� ������ � ���������� " << dimension.getName() << std::endl;
		throw std::exception(sout.str().c_str());
	}

	TM1V vOk = TM1SubsetInsertElement(hPool, hNewObject, hElement, TM1ValIndex(hPool, 0));
	//��������� ���������� ���������� ��������
	if (TM1ValType(hUser, vOk) == TM1ValTypeBool() && !TM1ValBoolGet(hUser,vOk)) {
		std::ostringstream sout;
		sout << "�� ������� �������� ������� " << ElementName << " � ������ ������������:\n\t";		
		throw std::exception(sout.str().c_str());
	}

	return true;
}

bool Subset::registerSubset(const char * SubsetName , TM1_INDEX SubsetNameLen) {
	if (hNewObject == nullptr)
		throw std::exception("������������ �� ����������������.");
	else if (hObject != nullptr
		&& (SubsetName == nullptr
			|| SubsetName != nullptr && std::strncmp(TM1ValStringGet(hUser, vName), SubsetName, SubsetNameLen? SubsetNameLen :std::strlen(SubsetName)) == 0)) {
		//������ ����������, � �� ������� ������
		TM1V hObject = TM1SubsetUpdate(hPool, this->hObject, hNewObject);
		if (TM1ValType(hUser, hObject) == TM1ValTypeError()) {
			std::ostringstream sout;
			sout << "�� ������� �������� �������������� ������ ������������ " << getName() << "\n\t";
			getLastError(sout, hObject, true);
			throw std::exception(sout.str().c_str());
		}
		this->hObject = hObject;
		return true;
	}else if (SubsetName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, dimension.gethObject(), hNewObject, SubsetName, vName, SubsetNameLen);
		this->hObject = hObject;
	}else if (vName != nullptr) {
		TM1V hObject = utilities::registerObject(hUser, hPool, dimension.gethObject(), hNewObject, vName);
		this->hObject = hObject;
	}

	hNewObject = nullptr;
	return true;
}

bool Subset::addAllElements() {
	if(hNewObject==nullptr)
		throw std::exception("����� ������ ������������ �� ���������������");

	TM1V vOk=TM1SubsetAll(hPool, hNewObject);
	if (TM1ValType(hUser, vOk) == TM1ValTypeBool() && !TM1ValBoolGet(hUser, vOk)) {
		std::ostringstream sout;
		sout << "�� ������� �������� ��� �������� � ������������ �� ���������\n";
		throw std::exception(sout.str().c_str());
	}

	return true;
}

