#include "View.h"
#include "Cube.h"

View::View(const Cube &cube, const char * ViewName, TM1_INDEX ViewNameLen):Object(cube),cube(cube) {
	//�������� ���������� ���������
	if(ViewName!=nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, cube.gethObject(), TM1CubeViews(), ViewName, vName, ViewNameLen);
}

View::View(const Cube &cube, TM1_INDEX i):Object(cube),cube(cube) {
	//�������� ���������� �������������
	if (0 < i && i <= cube.getCountViews())
		hObject = utilities::getObjectByIndex(hUser, hPool, cube.gethObject(), TM1CubeViews(), i);
	else {
		this->~View();
		std::ostringstream sout;
		sout << "������������� � �������� " << i << " �� ������� � ���� " << cube.getName() << std::endl;
		throw std::exception(sout.str().c_str());
	}
}

bool View::exist() noexcept {
	if (vName != nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, cube.gethObject(), TM1CubeViews(), vName);
	return hObject != nullptr;
}

void View::makeNew() {
	//������� ����� �������������
	TM1V tmp[256] = { 0 };
	//��� ���� ������
	TM1_INDEX cnt = 1;
	TM1V hTitleSubsetArray = utilities::makeArray(hUser, hPool, TM1_INDEX(Titles.size()), tmp);
	for (auto & strName : Titles) {
		TM1V vName, hDimension = utilities::getObjectByName(hUser, hPool, cube.getServer().gethObject(), TM1ServerDimensions(), strName.c_str(), vName);
		if (hDimension)
			TM1ValArraySet(hTitleSubsetArray, hDimension, cnt++);
		else {
			std::stringstream sout;
			sout << "�� ������� ����� ���������: " << strName << std::endl;
			throw std::exception(sout.str().c_str());
		}
	}
	//��������� � ��������
	cnt = 1;
	TM1V hColumnSubsetArray = utilities::makeArray(hUser, hPool, TM1_INDEX(Columns.size()), tmp);
	for (auto & strName : Columns) {
		TM1V vName, hDimension = utilities::getObjectByName(hUser, hPool, cube.getServer().gethObject(), TM1ServerDimensions(), strName.c_str(), vName);
		if (hDimension)
			TM1ValArraySet(hColumnSubsetArray, hDimension, cnt++);
		else {
			std::stringstream sout;
			sout << "�� ������� ����� ���������: " << strName << std::endl;
			throw std::exception(sout.str().c_str());
		}
	}
	//��������� � �������
	cnt = 1;
	TM1V hRowSubsetArray = utilities::makeArray(hUser, hPool, TM1_INDEX(Rows.size()), tmp);
	for (auto & strName : Rows) {
		TM1V vName, hDimension = utilities::getObjectByName(hUser, hPool, cube.getServer().gethObject(), TM1ServerDimensions(), strName.c_str(), vName);
		if (hDimension)
			TM1ValArraySet(hRowSubsetArray, hDimension, cnt++);
		else {
			std::stringstream sout;
			sout << "�� ������� ����� ���������: " << strName << std::endl;
			throw std::exception(sout.str().c_str());
		}
	}
	
	hNewObject = TM1ViewCreate(hPool, cube.gethObject(), hTitleSubsetArray, hColumnSubsetArray, hRowSubsetArray);	
}

bool View::registerView(const char * ViewName, TM1_INDEX ViewNameLen) {
	if(hNewObject==nullptr)
		throw std::exception("�� ��������������� ����� ������");
	else if (ViewName != nullptr)
		hObject = utilities::registerObject(hUser, hPool, cube.gethObject(), hNewObject, ViewName, vName);
	else if (vName != nullptr)
		hObject = utilities::registerObject(hUser, hPool, cube.gethObject(), hNewObject, vName);
	else
		throw std::exception("�� ������� ��� �������������");
	return true;
}

std::string View::show() const {
	TM1_INDEX maxN = cube.getCountDimensions();
	//�������� ������ ������� �������������
	TM1V hExtractList = TM1ViewExtractCreate(hPool, hObject);

	//������� �������������� Pool
	TM1P hPool = TM1ValPoolCreate(hUser);

	//������� ��������� ������
	TM1_INDEX cnt = 0;
	TM1_INDEX n;
	bool is = false;
	TM1V hOvject = nullptr;
	std::ostringstream sout;

	try {
		while (nullptr != (hOvject = TM1ViewExtractGetNext(hPool, hExtractList))) {
			if (TM1ValType(hUser, hOvject) == TM1ValTypeError()) {
				std::ostringstream sout;
				sout << "�� ������ ������������� �� �������������: " << getName() << "\n\t";
				getLastError(sout, hOvject, true);
				//���������� ������ �������
				TM1ViewExtractDestroy(hPool, hExtractList);
				TM1ValPoolDestroy(hPool);
				throw std::exception(sout.str().c_str());
			}
			TM1_INDEX ind = TM1ValType(hUser, hOvject);
			if (ind == TM1ValTypeIndex()) {
				n = TM1ValIndexGet(hUser, hOvject);
				if (n > 0) {
					std::cout << (cnt ? ",\n" : "") << "{";
					++cnt;
					is = false;
				}
				else
					break;
			}else if (ind == TM1ValTypeString()) {
				std::cout << (is ? ", " : "");
				if (maxN < n)
					sout << "\"v\":" << TM1ValStringGet(hUser, hOvject) << "\"}";
				else
					sout << "\"" << n << "\":\"" << TM1ValStringGet(hUser, hOvject) << "\"";
				is = true;
				++n;
			}else if (ind == TM1ValTypeReal()) {
				sout << (is ? ", " : "") << "\"v\":" << TM1ValRealGet(hUser, hOvject) << '}';
				is = true;
				++n;
			}else
				sout << ind << std::endl;
		}
	}catch(...){
		//����������� �������
		TM1ViewExtractDestroy(hPool, hExtractList);
		TM1ValPoolDestroy(hPool);
		//��������� ����������
		throw;
	}	

	return std::move(sout.str());
}
