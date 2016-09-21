#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <stdexcept>
#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "EndOfFile")

int main() {
	setlocale(LC_ALL, "ru");

	/*������������� ����� COM*/
	if(FAILED(CoInitialize(nullptr)))
		throw std::runtime_error("�� ������� ���������������� COM �����");

	/*����� ��������� �� ����������*/
	ADODB::_ConnectionPtr pConn;
	if(FAILED(pConn.CreateInstance(__uuidof(ADODB::Connection))))
		throw std::runtime_error("�� ������� ���������������� �������� �� ����������");


	pConn->ConnectionTimeout = 0;
	//_bstr_t connection_string = "Provider=SQLNCLI11;Server=AS-MSK-N0273\CQ;Database=CQ;App=ZUP"; //����������� �����
	_bstr_t connection_string = R"(DRIVER={SQL Server};Server=AS-MSK-N0273\CQ;Database=CQ;App=ZUP)";
	_bstr_t user = "connect_1c";
	_bstr_t pwd = "#&bP8RN7";
	try {
		/*��������� ���������� � �����*/
		HRESULT hr = pConn->Open(connection_string, user, pwd, ADODB::adConnectUnspecified);

		/*������� command*/
		ADODB::_CommandPtr pCommand;
		pCommand.CreateInstance(__uuidof(ADODB::Command));
		pCommand->CommandType = ADODB::adCmdText;
		pCommand->ActiveConnection = pConn;
		pCommand->CommandText = "\
								select\
									r.������������   as [FileName],\
									r.�������������  as [BinaryData]\
								from\
									D������ID as d\
									join S������������1ID as r on(r.����������� = d.������)\
								where d.����� = 396210\
								";	//sp_executesql
		pCommand->Prepared = true;

		/*���������� ������*/
		ADODB::_RecordsetPtr pRst = pCommand->Execute(nullptr, nullptr, 0);

		/*�������� �� Recordset ������*/
		while(!pRst->EndOfFile) {
			_variant_t vtFileName = pRst->Fields->Item[(_variant_t)"FileName"]->Value;
			_variant_t val        = pRst->Fields->Item[(_variant_t)"BinaryData"]->Value;
			
			std::wstring file_name(vtFileName.bstrVal, SysStringLen(vtFileName.bstrVal));
			
			file_name = L"..\\Debug\\" + file_name + L".zip";
			size_t size = val.parray->rgsabound->cElements-71;
			std::ofstream fout(file_name, std::ios::binary | std::ios::out);
			fout.write((char*)val.parray->pvData + 83, size);
			fout.close();

			pRst->MoveNext();
		}

		/*�������� Recordset*/
		pRst->Close();

		/*�������� command*/
		pCommand->ActiveConnection = nullptr;

		/*��������� ����������*/
		if(pConn->State != ADODB::adStateClosed)
			pConn->Close();

	} catch(_com_error & e) {
		std::cout << e.Description() << std::endl;
	} catch(std::exception & e) {
		std::cout << e.what() << std::endl;
	} catch(...) {
		std::cout << "�����" << std::endl;
	}

	/*�������� ����� COM*/
	CoUninitialize();

	//std::system("pause");
	return 0;
}