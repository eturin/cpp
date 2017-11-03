#include "View.h"
#include "Cube.h"

View::View(const Cube &cube, const char * ViewName, TM1_INDEX ViewNameLen, bool isMDX, bool isPublic):Object(cube),cube(cube), vType(isMDX ? TM1ValIndex(hPool, 357) : TM1CubeViews()){
	//�������� ���������� ���������
	if (ViewName != nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, cube.gethObject(), vType, ViewName, vName, ViewNameLen, isPublic);
}

View::View(const Cube &cube, TM1_INDEX i, bool isMDX, bool isPublic):Object(cube),cube(cube), vType(isMDX ? TM1ValIndex(hPool, 357) : TM1CubeViews()) {
	//�������� ���������� �������������
	if (0 < i && i <= cube.getCountViews(isPublic,isMDX))
		hObject = utilities::getObjectByIndex(hUser, hPool, cube.gethObject(), vType, i, isPublic);
	else {
		this->~View();
		std::ostringstream sout;
		sout << "������������� � �������� " << i << " �� ������� � ���� " << cube.getName() << std::endl;
		throw std::exception(sout.str().c_str());
	}
}

bool View::exist(bool isPublic) noexcept {
	if (vName != nullptr)
		hObject = utilities::getObjectByName(hUser, hPool, cube.gethObject(), vType, vName, isPublic);
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

void View::makeNewWithMDX(const char * Expression, TM1_INDEX ExpressionLen) {
	if(Expression==nullptr)
		throw std::exception("�� ������ MDX-������.");

	TM1V vExpression = TM1ValString(hPool, const_cast<char*>(Expression), ExpressionLen);
	TM1V hNewObject = TM1ViewCreateByExpression(hPool, cube.getServer().gethObject(), vExpression);
	if (TM1ValType(hUser, hNewObject) == TM1ValTypeString()) {
		std::ostringstream sout;
		sout << "������ �������� ������������� �� ������ ��������� MDX (" << Expression << "):\n" << TM1ValStringGet(hUser, hNewObject) << std::endl;
		throw std::exception(sout.str().c_str());
	}else
		this->hNewObject = hNewObject;
}

bool View::registerView(bool isPublic, const char * ViewName, TM1_INDEX ViewNameLen) {
	if(hNewObject==nullptr)
		throw std::exception("�� ��������������� ����� ������");
	else if (ViewName != nullptr)
		hObject = utilities::registerObject(hUser, hPool, cube.gethObject(), hNewObject, isPublic, ViewName, vName);
	else if (vName != nullptr)
		hObject = utilities::registerObject(hUser, hPool, cube.gethObject(), hNewObject, isPublic, vName);
	else
		throw std::exception("�� ������� ��� �������������");
	return true;
}

std::string View::show(bool isPublic,TM1V hObject) const {

	TM1_INDEX maxN = cube.getCountDimensions(isPublic, getParrent(hObject ? hObject : this->hObject));
	//�������� ������ ������� �������������
	TM1V hExtractList = TM1ViewExtractCreate(hPool, hObject? hObject: this->hObject);

	//������� �������������� Pool
	TM1P hPool = TM1ValPoolCreate(hUser);

	//������� ��������� ������
	TM1_INDEX cnt = 0;
	TM1_INDEX n;
	bool is = false;
	TM1V hExtract = nullptr;
	std::ostringstream sout;

	try {
		while (nullptr != (hExtract = TM1ViewExtractGetNext(hPool, hExtractList))) {
			if (TM1ValType(hUser, hExtract) == TM1ValTypeError()) {
				std::ostringstream sout;
				sout << "�� ������ ������������� �� �������������: " << getName() << "\n\t";
				getLastError(sout, hExtract, true);
				//���������� ������ �������
				TM1ViewExtractDestroy(hPool, hExtractList);
				TM1ValPoolDestroy(hPool);
				throw std::exception(sout.str().c_str());
			}
			TM1_INDEX ind = TM1ValType(hUser, hExtract);
			if (ind == TM1ValTypeIndex()) {
				n = TM1ValIndexGet(hUser, hExtract);
				if (n > 0) {
					sout << (cnt ? ",\n" : "") << "{";
					++cnt;
					is = false;
				}else
					break;
			}else if (ind == TM1ValTypeString()) {
				sout << (is ? ", " : "");
				if (maxN < n)
					sout << "\"v\":" << TM1ValStringGet(hUser, hExtract) << "\"}";
				else
					sout << "\"" << n << "\":\"" << TM1ValStringGet(hUser, hExtract) << "\"";
				is = true;
				++n;
			}else if (ind == TM1ValTypeReal()) {
				sout << (is ? ", " : "") << "\"v\":" << TM1ValRealGet(hUser, hExtract) << '}';
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
