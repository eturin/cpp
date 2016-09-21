#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stack>
#include <queue>
#include <map>
#include<regex>
#include "zlib.h"

//требует установки свойства: Использовать MFC в статической библиотеке
#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "EndOfFile") 
//#import "msado15.dll" rename("EOF", "EndOfFile") 

struct file_ini {
	wchar_t * wstr;
	size_t len;
	file_ini(wchar_t * wstr, size_t len):wstr(wstr), len(len) {}
	~file_ini() { delete[] wstr; wstr = nullptr; }
};

struct two {
	long long i, j;
	two(long long i, long long j):i(i), j(j) {}
};

size_t wchar_to_char(char * &Dest, const wchar_t* Source, size_t len=0) {
	/*if(len == 0) {//если размер задан, то и место уже зарезервировано
	len = wchar_len(Source) +1;
	Dest = new char[len];
	}
	wcstombs(Dest, Source, len);
	Dest[len - 1] = '\0';*/

	if(len == 0) {
		len = WideCharToMultiByte(
			1251,   // Code page
			0,      // Default replacement of illegal chars
			Source, // Multibyte characters string
			-1,     // Number of unicode chars is not known
			NULL,   // No buffer yet, allocate it later
			0,      // No buffer
			NULL,   // Use system default
			NULL    // We are not interested whether the default char was used
			);
		if(len == 0)
			return 0;
		else
			Dest = new char[len];
	}

	len = WideCharToMultiByte(
		1251,    // Code page
		0,       // Default replacement of illegal chars
		Source,  // Multibyte characters string
		-1,      // Number of unicode chars is not known
		Dest,    // Output buffer
		len,     // buffer size
		NULL,    // Use system default
		NULL     // We are not interested whether the default char was used
		);

	if(len == 0) {
		delete[] Dest;
		return 0;
	}

	return len;
}

size_t decomp(byte * data_in, size_t in_size, wchar_t * &wstr, variant_t & vtFileName) {
	z_stream zstream;
	zstream.zalloc = Z_NULL;
	zstream.zfree = Z_NULL;
	zstream.opaque = Z_NULL;
	zstream.avail_in = in_size;  //входящий размер
	zstream.next_in = data_in;  //входящий массив байт

	/*распаковываем байты и складываем в очередь*/
	std::queue<byte*> qu;
	inflateInit2(&zstream, -MAX_WBITS);
	size_t total_size = 0, out_size = 4 * in_size;
	do {
		byte * mb = new byte[out_size];
		zstream.avail_out = out_size;
		zstream.next_out = mb;
		try {
			int ret = inflate(&zstream, Z_NO_FLUSH);
		} catch(...) {
			if(qu.size() == 0) {
				qu.push(data_in);
				total_size = in_size;
			} else {
				std::wstring ws(vtFileName.bstrVal, SysStringLen(vtFileName.bstrVal));
				char * tmp;
				wchar_to_char(tmp, ws.c_str());
				std::string str_err = "Файл ";
				str_err += tmp;
				str_err += " поврежден.";
				delete[] tmp;
				inflateEnd(&zstream);
				throw std::runtime_error(str_err);
			}
		}
		qu.push(mb);
		total_size += out_size - zstream.avail_out;
	} while(zstream.avail_out == 0);
	inflateEnd(&zstream);

	/*извлекаем из очереди распакованные данные*/
	byte * mb = new byte[total_size], *pmb = mb;
	size_t lost_size = total_size;
	while(!qu.empty()) {
		byte * temp = qu.front();
		qu.pop();
		size_t size = lost_size > out_size ? out_size : lost_size;
		std::memcpy(pmb, temp, size);
		pmb += size;
		delete[] temp;
		lost_size -= out_size;
	}

	/*запись в файл*/
	std::wstring file_name(vtFileName.bstrVal, SysStringLen(vtFileName.bstrVal));
	file_name = L"..\\Debug\\text\\" + file_name + L".txt";
	std::ofstream fout(file_name, std::ios::binary | std::ios::out);
	fout.write((char*)mb, total_size);
	fout.close();

	/*конвертация byte -> wchar_t */
	int len = MultiByteToWideChar(CP_UTF8, 0, (char*)mb, -1, NULL, 0);
	wstr = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, (char*)mb, -1, wstr, len);
	delete[] mb;

	/*конвертация wchar_t -> char */
	/*char * res;
	wchar_to_char(res, wstr,0);*/

	return len;
}

int find(const wchar_t * wstr, size_t len, const wchar_t * wpattern, std::stack<std::wstring> &res) {
	int cnt = 0;
	std::tr1::wregex wrg(wpattern);
	for(std::wcregex_iterator it(wstr, wstr + len, wrg), it_end; it != it_end; ++it) {
		for(size_t i = 0; i < (*it).size(); ++i) {
			res.push((*it)[i].str());
			++cnt;
		}
	}

	return cnt;
}

two find_str(const wchar_t * wstr, size_t len, const wchar_t * wpattern) {
	bool is_ok;
	size_t i = 0, j = 0;
	while(i < len) {
		j = 0;
		is_ok = true;
		while(is_ok && i + j < len && wpattern[j])
			if(wstr[i + j] != wpattern[j])
				is_ok = false;
			else
				++j;
		if(is_ok)
			break;
		else
			++i;
	}
	return is_ok ? two(i, j) : two(-1, j);
}

int find_rn(const wchar_t * wstr, size_t len, const wchar_t * wpattern, std::stack<std::wstring> &res) {
	two tw = find_str(wstr, len, wpattern);
	if(tw.i >= 0) {
		size_t i = size_t(tw.i), j = size_t(tw.j);
		wchar_t * id = new wchar_t[37];
		std::memcpy(id, wstr + i + j + 1, 36 * sizeof(wchar_t));
		id[36] = '\0';
		res.push(id);
		delete[] id;

		while(i<len && wstr[i] != L'"') ++i;
		j = i + 1;
		while(j<len && wstr[j] != L'"') ++j;
		find(wstr + i + 1, j - i - 1, LR"(\w+)", res);
		return 1;
	} else
		return 0;
}

int find_t(const wchar_t * wstr, size_t len, const wchar_t * wpattern, std::stack<std::wstring> &res) {
	two tw = find_str(wstr, len, wpattern);
	if(tw.i >= 0) {
		/*получаем имя*/
		size_t i = size_t(tw.i);
		while(i>0 && wstr[i] != L'.') --i;
		++i;
		size_t j = i;
		while(j<len && wstr[j] != L'"') ++j;
		wchar_t *  id = new wchar_t[j - i + 1];
		std::memcpy(id, wstr + i, (j - i) * sizeof(wchar_t));
		id[j - i] = '\0';
		res.push(id);
		delete[] id;

		/*получаем первую букву*/
		i = j + 2;
		while(i<len && wstr[i] != L'"') ++i;
		++i;
		id = new wchar_t[2];
		id[0] = wstr[i];
		id[1] = '\0';
		res.push(id);
		delete[] id;

		/*получаем файл хранения*/
		id = new wchar_t[37];
		std::memcpy(id, wstr + j + 2, 36 * sizeof(wchar_t));
		id[36] = '\0';
		res.push(id);
		delete[] id;

		/*получение номера таблицы хранения*/
		if(wstr[tw.i - 2] != '\"') {
			i = size_t(tw.i) - 2;
			while(i >= 0 && wstr[i] != L',') --i;
			++i;
			id = new wchar_t[size_t(tw.i - i)];
			std::memcpy(id, wstr + i, size_t(tw.i - 1 - i) * sizeof(wchar_t));
			id[tw.i - i - 1] = '\0';
			int num = _wtoi(id);
			delete[] id;
			std::wostringstream wsout;
			wsout.width(8);
			wsout << std::setfill(L'0');
			wsout << std::hex << num;
			res.push(L"0x" + wsout.str());
		} else {
			i = size_t(tw.i) + 36;
			while(i<len && wstr[i] != L'"') ++i;
			++i;
			while(i<len && wstr[i] != L'"') ++i;
			i += 2;
			j = i;
			while(i<len && wstr[i] != L',') ++i;
			id = new wchar_t[i - j + 1];
			std::memcpy(id, wstr + j, (i - j) * sizeof(wchar_t));
			id[i - j] = '\0';
			int num = _wtoi(id);
			delete[] id;
			std::wostringstream wsout;
			wsout.width(8);
			wsout << std::setfill(L'0');
			wsout << std::hex << num;
			res.push(L"0x" + wsout.str());
		}

		return 1;
	} else
		return 0;
}

int find_tid(const wchar_t * wstr, size_t len, const wchar_t * wpattern, std::stack<std::wstring> &res) {
	two tw = find_str(wstr, len, wpattern);
	if(tw.i >= 0) {
		wstr += size_t(tw.i) + size_t(tw.j) + 1;
		len -= size_t(tw.i + tw.j) + 1;
		tw = find_str(wstr, len, L"{\"Pattern\",\r\n");
		wstr += tw.i + tw.j;
		len -= size_t(tw.i + tw.j);
		if(tw.i >= 0) {
			int cnt = 0, t = 0;
			do {
				tw = find_str(wstr, len, L"\r\n");
				t = find(wstr, 45, LR"(\w{8}-\w{4}-\w{4}-\w{4}-\w{12})", res);
				cnt += t;
				wstr += tw.i + tw.j;
				len -= size_t(tw.i + tw.j);
			} while(t);
			return cnt;
		} else
			return 0;
	} else
		return 0;
}


file_ini * get_file(ADODB::_CommandPtr & pCommand, const wchar_t * wstr_param, std::map<std::wstring, file_ini*> &mp) {
	if(mp.find(wstr_param) == mp.end()) {
		pCommand->Parameters->Item[(_variant_t)"FileName"]->Value = bstr_t(wstr_param);
		ADODB::_RecordsetPtr pRst = pCommand->Execute(nullptr, nullptr, 0);
		if(!pRst->EndOfFile) {
			_variant_t val = pRst->Fields->Item[(_variant_t)"BinaryData"]->Value;
			if(val.vt != VT_NULL) {
				wchar_t * wstr;
				size_t len = decomp((Byte*)val.parray->pvData, val.parray->rgsabound->cElements, wstr, pRst->Fields->Item[(_variant_t)"FileName"]->Value);
				mp[pRst->Fields->Item[(_variant_t)"FileName"]->Value.bstrVal] = new file_ini(wstr, len);
			}
		}
		/*закрываю Recordset*/
		pRst->Close();
	}
	return mp[wstr_param];
}

void clear_map(std::map<std::wstring, file_ini*> &mp) {
	/*очищаем map*/
	for(auto it = mp.begin(); it != mp.end(); ++it)
		delete it->second;
	mp.clear();
}

int main() {
	bool res = true;
	std::wstring result;
	setlocale(LC_ALL, "rus");
	try {
		/*инициализация среды COM*/
		int rc = CoInitialize(nullptr);
		/*if(FAILED(rc)) {
		throw std::runtime_error("Не удается инициализировать COM среду");
		}*/

		/*умный указатель на соединение*/
		ADODB::_ConnectionPtr pConn;
		if(FAILED(pConn.CreateInstance(__uuidof(ADODB::Connection))))
			throw std::runtime_error("Не удается инициализировать укзатель на соединение");
		pConn->ConnectionTimeout = 0;
		std::wstring str_con = LR"(DRIVER={SQL Server};Server=AS-MSK-N0298\EDO;Database=EDO;APP=EDO;)";
		_bstr_t connection_string = str_con.c_str();
		_bstr_t user = "connect_1c";
		_bstr_t pwd = "#&bP8RN7";

		/*установка соединения с базой*/
		HRESULT hr = pConn->Open(connection_string, user, pwd, ADODB::adConnectUnspecified);

		/*создаем command*/
		ADODB::_CommandPtr pCommand;
		try {
			pCommand.CreateInstance(__uuidof(ADODB::Command));
		} catch(_com_error & e) {
			e;
			if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw;
		}
		pCommand->CommandType = ADODB::adCmdText;
		pCommand->ActiveConnection = pConn;
		pCommand->CommandText = R"(select 							
									case 
                                            when t.FileName='a07b62f0-1f01-484a-93d9-d42764cedac0.si' then 'Описание'
                                            when t.FileName='1a621f0f-5568-4183-bd9f-f6ef670e7090.si' then 'Поля'
                                            when t.FileName='fe8acd6a-22c9-4b5a-aeae-232a1c8324cb.si' then 'ПеречислениеТипов'
                                            else t.FileName
                                    end                     as [FileName],
                                    t.BinaryData		    as [BinaryData]
                               from 
	                                --Params as t                                    
									Config as t                                    
                               /*where
	                                t.FileName in ('a07b62f0-1f01-484a-93d9-d42764cedac0.si',
                                                   '1a621f0f-5568-4183-bd9f-f6ef670e7090.si',
                                                   'fe8acd6a-22c9-4b5a-aeae-232a1c8324cb.si',
                                                   'DBNames')*/
							 )";	//sp_executesql

		/*отправляем запрос*/
		ADODB::_RecordsetPtr pRst;
		try {
			pRst = pCommand->Execute(nullptr, nullptr, 0);
		} catch(_com_error &e) {
			e;
			if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw;
		}
		/*складываем основные тексты в map*/
		std::map<std::wstring, file_ini*> mp;
		while(!pRst->EndOfFile) {
			_variant_t val = pRst->Fields->Item[(_variant_t)"BinaryData"]->Value;
			if(val.vt != VT_NULL) {
				wchar_t *wstr;
				size_t len;
				try {
					len = decomp((Byte*)val.parray->pvData, val.parray->rgsabound->cElements, wstr, pRst->Fields->Item[(_variant_t)"FileName"]->Value);
				} catch(std::runtime_error & e) {
					e;
					clear_map(mp);   //очищаем map
					pRst->Close();   //закрываю Recordset
					if(pConn->State != ADODB::adStateClosed) pConn->Close();  //закрываем соединение
					CoUninitialize();//выгрузка COM среды
					throw;
				}
				mp[pRst->Fields->Item[(_variant_t)"FileName"]->Value.bstrVal] = new file_ini(wstr, len);
				std::wcout << pRst->Fields->Item[(_variant_t)"FileName"]->Value.bstrVal << std::endl;
				if(!std::wcout)
					std::wcout.clear();
			}
			pRst->MoveNext();
		}
		/*закрываю Recordset*/
		pRst->Close();
		
		auto it_descr = mp.find(L"Описание");
		if(mp.end() == it_descr) {
			clear_map(mp); //очищаем map
			if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw std::runtime_error("В таблице Params не найден файл с описаниями (a07b62f0-1f01-484a-93d9-d42764cedac0.si)");
		}
		wchar_t *wstr_descr = it_descr->second->wstr;
		size_t   len_descr  = it_descr->second->len;

		auto it_DBNames = mp.find(L"DBNames");
		if(mp.end() == it_DBNames) {
			clear_map(mp); //очищаем map
			if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw std::runtime_error("В таблице Params не найден файл со структурой полей SQL (DBNames)");
		}
		wchar_t *wstr_DBNames = it_DBNames->second->wstr;
		size_t   len_DBNames  = it_DBNames->second->len;

		auto it_fields = mp.find(L"Поля");
		if(mp.end() == it_fields) {
			clear_map(mp); //очищаем map
			if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw std::runtime_error("В таблице Params не найден файл с полями (1a621f0f-5568-4183-bd9f-f6ef670e7090.si)");
		}
		wchar_t *wstr_fields = it_fields->second->wstr;
		size_t   len_fields = it_fields->second->len;

		auto it_enum_type = mp.find(L"ПеречислениеТипов");

		/*готовим многократно выполняемый запрос*/
		pCommand->CommandText = R"(select top 1								
									t.BinaryData		    as [BinaryData],
									t.FileName              as [FileName]
							  from
									config as t
							  where
									t.FileName =? 
							 )";

		/*создаем Parameter*/
		ADODB::_ParameterPtr pPrm;
		try {
			pPrm.CreateInstance(__uuidof(ADODB::Parameter));
		} catch(_com_error &e) {
			e;
			if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw;
		}
		pPrm->Name = "FileName";
		pPrm->Type = ADODB::adBSTR;
		pPrm->Direction = ADODB::adParamInput;
		pPrm->Value = bstr_t(L"a07b62f0-1f01-484a-93d9-d42764cedac0.si");
		pCommand->Parameters->Append(pPrm);
		pCommand->Prepared = true;

		std::wstring obj_name = L"КомплектДокументовДоходы";
		
		std::wstring xml = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<root>\r\n";
		std::map<std::wstring, std::wstring> mp_sql;
		mp_sql[obj_name] = L"select top 1";
		std::stack<std::wstring> st;
		std::wstring pattern = obj_name + LR"_(",(\w{8}-\w{4}-\w{4}-\w{4}-\w{12}),\d+,\d+,"((\w)\w+)")_";
		int k_cnt = find(wstr_descr, len_descr, pattern.c_str(), st)/4 ;
		for(int l = 0; l < k_cnt; ++l) {
			std::wstring type_pref  = st.top();	     st.pop();
			std::wstring type_table = L"_"+st.top(); st.pop();
			std::wstring type_id    = st.top();	     st.pop();
			st.pop();			
			
			/*обязательные реквизиты*/
			xml += L"<Реквизит name='Ссылка' descr='' help='' owner='" + obj_name + L"' />\r\n";
			mp_sql[obj_name] += LR"( t._IDRRef     as [Ссылка])";
			if(type_pref == L"D") {//Документ
				xml += L"<Реквизит name='Номер'            descr='' help='' owner='" + obj_name + L"' />\r\n"				
				+ L"<Реквизит name='Дата'             descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Проведен'         descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='ПометкаУдаления'  descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Версия'           descr='' help='' owner='" + obj_name + L"' />\r\n";
				mp_sql[obj_name] += LR"(,t._Number     as [Номер]
                                        ,t._Date_Time  as [Дата]
                                        ,t._Posted     as [Проведен]
                                        ,t._Marked     as [ПометкаУдаления]
                                        ,t._Version    as [Версия])";

			} else if(type_pref == L"R") {//справочник
				xml += L"<Реквизит name='ПометкаУдаления'  descr='' help='' owner='" + obj_name + L"' />\r\n";
				xml += L"<Реквизит name='Версия'           descr='' help='' owner='" + obj_name + L"' />\r\n";
			} else if(type_pref == L"B") {//бизнес процесс
				xml += L"<Реквизит name='Номер'            descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Дата'             descr='' help='' owner='" + obj_name + L"' />\r\n"				
				+ L"<Реквизит name='ПометкаУдаления'  descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Версия'           descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Стартован'        descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Завершен'         descr='' help='' owner='" + obj_name + L"' />\r\n";
				mp_sql[obj_name] += LR"(,t._Number     as [Номер]
                                        ,t._Date_Time  as [Дата]
                                        ,t._Started    as [Стартован]
									    ,t._Completed  as [Завершен]
                                        ,t._Marked     as [ПометкаУдаления]
                                        ,t._Version    as [Версия])";
			} else if(type_pref == L"T") {//задача
				xml += L"<Реквизит name='Номер'            descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Дата'             descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='ПометкаУдаления'  descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Версия'           descr='' help='' owner='" + obj_name + L"' />\r\n"
				+ L"<Реквизит name='Выполнена'        descr='' help='' owner='" + obj_name + L"' />\r\n";
				mp_sql[obj_name] += LR"(,t._Number     as [Номер]
                                        ,t._Date_Time  as [Дата]
                                        ,t._Executed   as [Выполнена]
                                        ,t._Marked     as [ПометкаУдаления]
                                        ,t._Version    as [Версия])";
			}

			/*получение текста типа*/
			file_ini * file_t;
			try {
				file_t = get_file(pCommand, type_id.c_str(), mp);
			} catch(_com_error & e) {
				e;
				if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
				throw;
			} catch(std::runtime_error & e) {
				e;
				if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
				CoUninitialize();//выгрузка COM среды
				throw;
			}
			if(file_t == nullptr)
				continue;

			/*Поиск реквизитов*/		                                                                                                                  
			pattern = LR"_(\{\d+,\d+,(\w{8}-\w{4}-\w{4}-\w{4}-\w{12})\},"(\w+)",\r\n\{1,"ru","([\w ]*)"\},"([\w ]*)"\},\r\n\{"Pattern",\r\n((?:\{"[SBND#]"(?:,(?:(?:\w{8}-\w{4}-\w{4}-\w{4}-\w{12})|\d+|(?:"D")))*\},?\r\n)*))_";
			int r_cnt = find(file_t->wstr, file_t->len, pattern.c_str(), st)/6;
			for(int l = 0; l < r_cnt; ++l) {				
				std::wstring rec_typeALL = st.top();	st.pop();
				std::wstring rec_help    = st.top();	st.pop();
				std::wstring rec_descr   = st.top();	st.pop();
				std::wstring rec_name    = st.top();	st.pop();
				std::wstring rec_id      = st.top();	st.pop();
				std::wstring rec_all     = st.top();	st.pop();
				
				/*Определение принадлежности реквизита*/
				pattern = rec_id + LR"_(,(\w{8}-\w{4}-\w{4}-\w{4}-\w{12}))_";
				int o_cnt = find(wstr_fields, len_fields, pattern.c_str(), st)/2;
				for(int i = 0; i < o_cnt; ++i) {
					std::wstring owner_id = st.top();	st.pop();
					st.pop();
					std::wstring table_sql   = type_table;
					std::wstring table_descr = obj_name;

					xml += L"<Реквизит name='" + rec_name + L"' descr='" + rec_descr + L"' help='" + rec_help + L"'";

					if(owner_id != type_id) {//это реквизит табличной части
						/*найдем имя таблицы*/
						pattern = LR"_(\{\d+,\d+,)_" + owner_id+ LR"_(\},"(\w+)",\r\n\{1,"ru","([\w ]*)"\},"([\w ]*)"\}\r\n)_"; 
						int d_cnt = find(file_t->wstr, file_t->len, pattern.c_str(), st)/4;
						for(int j = 0; j < d_cnt; ++j) {
							std::wstring table_help  = st.top();	st.pop();
							std::wstring table_descr = st.top();	st.pop();
							std::wstring table_name  = st.top();	st.pop();
							st.pop();
							xml += L" owner='" + table_name + L"' />\r\n";
						}

						/*найдем sql таблицу*/
						pattern = owner_id;
						pattern += LR"_(,"VT",(\d+)\})_";
						int f_cnt = find(wstr_DBNames, len_DBNames, pattern.c_str(), st) / 2;
						for(int j = 0; j < f_cnt; ++j) {
							table_sql += L"_VT"+st.top();	st.pop();							
							st.pop();
						}
					} else
						xml += L" owner='" + obj_name + L"' />\r\n";
					
					/*Поиск имени поля в SQL*/
					pattern = rec_id;
					pattern += LR"_(,"(\w+)",(\d+)\})_";
					int f_cnt = find(wstr_DBNames, len_DBNames, pattern.c_str(), st) / 3;
					for(int j = 0; j < f_cnt; ++j) {
						std::wstring rec_field_num = st.top();	st.pop();
						std::wstring rec_field_pref= L"_" + st.top();	st.pop();
						st.pop();
						
						/*определение состава типа*/
						std::wstring fragment_sql;
						pattern = LR"_(\{"([SBND#])"(?:,(?:(\w{8}-\w{4}-\w{4}-\w{4}-\w{12})|\d+|(?:"D")))*\})_";
						int type_cnt = find(rec_typeALL.c_str(), rec_typeALL.length(), pattern.c_str(), st)/3;						
						for(int i = 0; i < type_cnt; ++i) {
							std::wstring type_i = st.top(); st.pop();
							std::wstring type_p = st.top(); st.pop();
							st.pop();
							if(type_cnt == 1 && type_i != L"#") {
								fragment_sql = L",t." + rec_field_pref + rec_field_num;
							} else if(type_i == L"#") {
								/*определение идентификатора типа(файла) и его имени, по идентификатору*/
								int t_cnt = find_t(wstr_descr, len_descr, type_i.c_str(), st);
								if(t_cnt) //конкретное перечисление типов
									for(int k = 0; k < t_cnt; ++k) {
										std::wstring t_num       = st.top(); st.pop();
										std::wstring t_id        = st.top(); st.pop();
										std::wstring t_name_pref = st.top(); st.pop();
										std::wstring t_name      = st.top(); st.pop();

										//wsout << L"<Поле Реквизит=\"" << r_name << L"\" ТаблицаSQL=\"" << ro_name_pref << ro_name << L"ID\" Тип =\"0x08\" Основание=\"" << t_name_pref << t_name << L"ID\" Вид=\"" << t_num << L"\" Составной=\"" << (tt_cnt > 1 ? L"0x01" : L"0x00") << "\" />" << std::endl;
									} else if(mp.end() == it_enum_type) {
										clear_map(mp); //очищаем map
										if(pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
										throw std::runtime_error("В таблице Params не найден файл с перечислениями типов (fe8acd6a-22c9-4b5a-aeae-232a1c8324cb.si)");
									} else if(it_enum_type != mp.end()) { //это тип все ссылки
										wchar_t *wstr_enum_type = it_enum_type->second->wstr;
										size_t   len_enum_type = it_enum_type->second->len;
										int tt_cnt = find_tid(wstr_enum_type, len_enum_type, type_i.c_str(), st);
										for(int k = 0; k < tt_cnt; ++k) {
											std::wstring t_id = st.top(); st.pop();

											/*определение идентификатора типа(файла) и его имени, по идентификатору*/
											int t_cnt = find_t(wstr_descr, len_descr, type_i.c_str(), st);
											for(int n = 0; n < t_cnt; ++n) {
												std::wstring t_num = st.top(); st.pop();
												std::wstring t_id = st.top(); st.pop();
												std::wstring t_name_pref = st.top(); st.pop();
												std::wstring t_name = st.top(); st.pop();

												//wsout << L"<Поле Реквизит=\"" << r_name << L"\" ТаблицаSQL=\"" << ro_name_pref << ro_name << L"ID\" Тип =\"0x08\" Основание=\"" << t_name_pref << t_name << L"ID\" Вид=\"" << t_num << L"\" Составной=\"0x01\" />" << std::endl;
											}
										}
									}
							}
						}
						mp_sql[table_sql] += fragment_sql + L"as [" + rec_name + L"]";
						//std::wcout << L"Реквизит " << l << L":\n" << L"Имя: " << rec_name << L"\nСиноним: " << rec_descr << L"\nID: " << rec_id << L"\nтипы: " << rec_typeALL << L"Имя в таблице sql: " << table_sql<<L"."<<rec_field_pref << rec_field_num << std::endl << std::endl;
					}
				}
			}

		}

		/*закрываю command*/
		//pCommand->ActiveConnection = nullptr;	

		/*закрываем соединение*/
		if(pConn->State != ADODB::adStateClosed)
			pConn->Close();

		/*выгрузка среды COM*/
		CoUninitialize();

		xml += L"</root>\r\n";
		std::wcout << xml;
	} catch(std::runtime_error & e) {
		std::cout<< e.what()<<std::endl;		
	} catch(_com_error & e) {
		std::cout << e.Description() << std::endl;		
		/*выгрузка среды COM*/
		CoUninitialize();
	}

	std::system("pause");
	return 0;
}