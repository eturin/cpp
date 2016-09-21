#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <stdexcept>
#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "EndOfFile")

int main() {
	setlocale(LC_ALL, "ru");

	/*инициализация среды COM*/
	if(FAILED(CoInitialize(nullptr)))
		throw std::runtime_error("Не удается инициализировать COM среду");

	/*умный указатель на соединение*/
	ADODB::_ConnectionPtr pConn;
	if(FAILED(pConn.CreateInstance(__uuidof(ADODB::Connection))))
		throw std::runtime_error("Не удается инициализировать укзатель на соединение");


	pConn->ConnectionTimeout = 0;
	//_bstr_t connection_string = "Provider=SQLNCLI11;Server=AS-MSK-N0273\CQ;Database=CQ;App=ZUP"; //именованный канал
	_bstr_t connection_string = R"(DRIVER={SQL Server};Server=AS-MSK-N0273\CQ;Database=CQ;App=ZUP)";
	_bstr_t user = "connect_1c";
	_bstr_t pwd = "#&bP8RN7";
	try {
		/*установка соединения с базой*/
		HRESULT hr = pConn->Open(connection_string, user, pwd, ADODB::adConnectUnspecified);

		/*создаем command*/
		ADODB::_CommandPtr pCommand;
		pCommand.CreateInstance(__uuidof(ADODB::Command));
		pCommand->CommandType = ADODB::adCmdText;
		pCommand->ActiveConnection = pConn;
		pCommand->CommandText = "\
								select\
									r.Наименование   as [FileName],\
									r.ХранилищеФайл  as [BinaryData]\
								from\
									DЗаявкаID as d\
									join SПрикрепления1ID as r on(r.Регистратор = d.Ссылка)\
								where d.Номер = 396210\
								";	//sp_executesql
		pCommand->Prepared = true;

		/*отправляем запрос*/
		ADODB::_RecordsetPtr pRst = pCommand->Execute(nullptr, nullptr, 0);

		/*получаем из Recordset данные*/
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

		/*закрываю Recordset*/
		pRst->Close();

		/*закрываю command*/
		pCommand->ActiveConnection = nullptr;

		/*закрываем соединение*/
		if(pConn->State != ADODB::adStateClosed)
			pConn->Close();

	} catch(_com_error & e) {
		std::cout << e.Description() << std::endl;
	} catch(std::exception & e) {
		std::cout << e.what() << std::endl;
	} catch(...) {
		std::cout << "Хрень" << std::endl;
	}

	/*выгрузка среды COM*/
	CoUninitialize();

	//std::system("pause");
	return 0;
}