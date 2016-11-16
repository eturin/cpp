#define _CRT_SECURE_NO_WARNINGS
#include "Criterion.h"
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

size_t decomp(byte * data_in, size_t in_size, wchar_t * &wstr, variant_t & vtFileName) {
	z_stream zstream;
	zstream.zalloc   = Z_NULL;
	zstream.zfree    = Z_NULL;
	zstream.opaque   = Z_NULL;
	zstream.avail_in = in_size;  //входящий размер
	zstream.next_in  = data_in;  //входящий массив байт

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
	/*std::wstring file_name(vtFileName.bstrVal, SysStringLen(vtFileName.bstrVal));
	file_name = L"..\\Debug\\text\\" + file_name + L".txt";
	std::ofstream fout(file_name, std::ios::binary | std::ios::out);
	fout.write((char*)mb, total_size);
	fout.close();*/

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
	std::wregex wrg(wpattern);
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
bool Criterion::GetXMLfile(tVariant* pvarRetValue, tVariant* paParams, const long len) {
	bool res = true;	
	std::wstring result;
	
	//setlocale(LC_ALL, "ru");
	try {
		/*инициализация среды COM*/
		int rc = CoInitialize(nullptr);
		/*if(FAILED(rc)) {		
			throw std::runtime_error("Не удается инициализировать COM среду");
		}*/
		
		/*умный указатель на соединение*/
		ADODB::_ConnectionPtr pConn;
		if (FAILED(pConn.CreateInstance(__uuidof(ADODB::Connection))))
			throw std::runtime_error("Не удается инициализировать укзатель на соединение");
		pConn->ConnectionTimeout = 0;
		std::wstring str_con = L"DRIVER={SQL Server};Server=" + Server + L";Database=" + Base + L";APP=EDO;";
		_bstr_t connection_string = str_con.c_str();
		_bstr_t user = User.c_str();
		_bstr_t pwd = Pass.c_str();

		/*установка соединения с базой*/
		HRESULT hr = pConn->Open(connection_string, user, pwd, ADODB::adConnectUnspecified);

		/*создаем command*/
		ADODB::_CommandPtr pCommand;
		try {
			pCommand.CreateInstance(__uuidof(ADODB::Command));
		}
		catch (_com_error & e) {
			e;
			if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
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
	                                Params as t                                    
                               where
	                                t.FileName in ('a07b62f0-1f01-484a-93d9-d42764cedac0.si',
                                                   '1a621f0f-5568-4183-bd9f-f6ef670e7090.si',
                                                   'fe8acd6a-22c9-4b5a-aeae-232a1c8324cb.si')
							 )";	//sp_executesql

		/*отправляем запрос*/
		ADODB::_RecordsetPtr pRst;
		try {
			pRst = pCommand->Execute(nullptr, nullptr, 0);
		}
		catch (_com_error &e) {
			e;
			if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw;
		}
		/*складываем основные тексты в map*/
		std::map<std::wstring, file_ini*> mp;
		while (!pRst->EndOfFile) {
			_variant_t val = pRst->Fields->Item[(_variant_t)"BinaryData"]->Value;
			if (val.vt != VT_NULL) {
				wchar_t *wstr;
				size_t len;
				try {
					len = decomp((Byte*)val.parray->pvData, val.parray->rgsabound->cElements, wstr, pRst->Fields->Item[(_variant_t)"FileName"]->Value);
				}
				catch (std::runtime_error & e) {
					e;
					clear_map(mp);   //очищаем map
					pRst->Close();   //закрываю Recordset
					if (pConn->State != ADODB::adStateClosed) pConn->Close();  //закрываем соединение
					CoUninitialize();//выгрузка COM среды
					throw;
				}
				mp[pRst->Fields->Item[(_variant_t)"FileName"]->Value.bstrVal] = new file_ini(wstr, len);
			}
			pRst->MoveNext();
		}
		/*закрываю Recordset*/
		pRst->Close();

		auto it_descr = mp.find(L"Описание");
		if (mp.end() == it_descr) {
			clear_map(mp); //очищаем map
			if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw std::runtime_error("В таблице Params не найден файл с описаниями (a07b62f0-1f01-484a-93d9-d42764cedac0.si)");
		}
		wchar_t *wstr_descr = it_descr->second->wstr;
		size_t   len_descr = it_descr->second->len;

		auto it_fields = mp.find(L"Поля");
		if (mp.end() == it_fields) {
			clear_map(mp); //очищаем map
			if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
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
		}
		catch (_com_error &e){
			e;
			if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
			throw;
		}
		pPrm->Name = "FileName";
		pPrm->Type = ADODB::adBSTR;
		pPrm->Direction = ADODB::adParamInput;
		pPrm->Value = bstr_t(L"a07b62f0-1f01-484a-93d9-d42764cedac0.si");
		pCommand->Parameters->Append(pPrm);
		pCommand->Prepared = true;

		std::wostringstream wsout;

		/*найдем все критерии отборов (имя и идентификатор)*/
		std::stack<std::wstring> st;
		int k_cnt = find(wstr_descr, len_descr, LR"_("FilterCriterion\.(\w+)","КритерийОтбора\.\w+",(\w{8}-\w{4}-\w{4}-\w{4}-\w{12}),\d+,\d+)_", st) / 3;
		for (int l = 0; l < k_cnt; ++l) {
			std::wstring krit_id = st.top();	st.pop();
			std::wstring krit_name = st.top();	st.pop();
			st.pop();
			if (FilterCriterion != L"" &&  krit_name != FilterCriterion)
				continue;
			wsout << L"<Критерий Имя=\"" << krit_name << "\">" << std::endl;

			/*получение текста описания критерия отбора*/
			file_ini * file_r;
			try {
				file_r = get_file(pCommand, krit_id.c_str(), mp);
			}
			catch (_com_error & e) {
				e;
				if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
				throw;
			}
			catch (std::runtime_error & e) {
				e;
				if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
				CoUninitialize();//выгрузка COM среды
				throw;
			}
			/*извлечение идентификаторов реквизитов из критерия*/
			int r_cnt = find(file_r->wstr, file_r->len, LR"_(\{"#",157fa490-4ce9-11d4-9415-008048da11f9,\r\n\{\d+,(\w{8}-\w{4}-\w{4}-\w{4}-\w{12})+\}\r\n\})_", st) / 2;
			for (int j = 0; j < r_cnt; ++j) {
				std::wstring r_id = st.top();	st.pop();
				st.pop();

				/*извлечение имени реквизита(где искать) и идентификатора файла с группой типов, хранящихся в реквизите*/
				int ri_cnt = find_rn(wstr_fields, len_fields, r_id.c_str(), st);
				if (ri_cnt) {
					std::wstring r_name = st.top();	st.pop();
					std::wstring gt_id = st.top();	st.pop();

					/*извлечение имени типа(таблицы) с этим реквизитом*/
					int ro_cnt = find_t(wstr_descr, len_descr, gt_id.c_str(), st);
					if (ro_cnt == 0)
						continue; //реквизит табличной части					
					std::wstring ro_num = st.top(); st.pop();
					std::wstring ro_id = st.top(); st.pop();
					std::wstring ro_name_pref = st.top(); st.pop();
					std::wstring ro_name = st.top(); st.pop();

					/*получение текста с группой типа*/
					file_ini * file_t;
					try {
						file_t = get_file(pCommand, gt_id.c_str(), mp);
					}
					catch (_com_error & e) {
						e;
						if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
						throw;
					}
					catch (std::runtime_error & e) {
						e;
						if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
						CoUninitialize();//выгрузка COM среды
						throw;
					}
					if (file_t == nullptr)
						continue;

					/*выделение идентификаторов отдельных типов из группы по идентификатору реквизита*/
					int tt_cnt = find_tid(file_t->wstr, file_t->len, r_id.c_str(), st);
					for (int i = 0; i < tt_cnt; ++i) {
						std::wstring t_id = st.top(); st.pop();

						/*определение идентификатора типа(файла) и его имени, по идентификатору*/
						int t_cnt = find_t(wstr_descr, len_descr, t_id.c_str(), st);
						if (t_cnt) //конкретное перечисление типов
							for (int k = 0; k < t_cnt; ++k) {
								std::wstring t_num = st.top(); st.pop();
								std::wstring t_id = st.top(); st.pop();
								std::wstring t_name_pref = st.top(); st.pop();
								std::wstring t_name = st.top(); st.pop();

								wsout << L"<Поле Реквизит=\"" << r_name << L"\" ТаблицаSQL=\"" << ro_name_pref << ro_name << L"ID\" Тип =\"0x08\" Основание=\"" << t_name_pref << t_name << L"ID\" Вид=\"" << t_num << L"\" Составной=\"" << (tt_cnt > 1 ? L"0x01" : L"0x00") << "\" />" << std::endl;
							}
						else if (mp.end() == it_enum_type) {
							clear_map(mp); //очищаем map
							if (pConn->State != ADODB::adStateClosed) pConn->Close();//закрываем соединение
							throw std::runtime_error("В таблице Params не найден файл с перечислениями типов (fe8acd6a-22c9-4b5a-aeae-232a1c8324cb.si)");
						}
						else if (it_enum_type != mp.end()) { //это тип все ссылки
							wchar_t *wstr_enum_type = it_enum_type->second->wstr;
							size_t   len_enum_type = it_enum_type->second->len;
							int tt_cnt = find_tid(wstr_enum_type, len_enum_type, t_id.c_str(), st);
							for (int k = 0; k < tt_cnt; ++k) {
								std::wstring t_id = st.top(); st.pop();

								/*определение идентификатора типа(файла) и его имени, по идентификатору*/
								int t_cnt = find_t(wstr_descr, len_descr, t_id.c_str(), st);
								for (int n = 0; n < t_cnt; ++n) {
									std::wstring t_num = st.top(); st.pop();
									std::wstring t_id = st.top(); st.pop();
									std::wstring t_name_pref = st.top(); st.pop();
									std::wstring t_name = st.top(); st.pop();

									wsout << L"<Поле Реквизит=\"" << r_name << L"\" ТаблицаSQL=\"" << ro_name_pref << ro_name << L"ID\" Тип =\"0x08\" Основание=\"" << t_name_pref << t_name << L"ID\" Вид=\"" << t_num << L"\" Составной=\"0x01\" />" << std::endl;
								}
							}
						}
					}
				}
			}
			wsout << L"</Критерий>" << std::endl;
		}

		/*очищаем map*/
		clear_map(mp);

		/*закрываю command*/
		//pCommand->ActiveConnection = nullptr;	

		/*закрываем соединение*/
		if (pConn->State != ADODB::adStateClosed)
			pConn->Close();

		/*выгрузка среды COM*/
		CoUninitialize();

		result = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<root>\r\n" + wsout.str() + L"</root>";
	}
	catch (std::runtime_error & e) {
		wchar_t * source = L"Error", *descr;
		pMemoryAdapter->AllocMemory((void**)&descr, std::strlen(e.what())*sizeof(WCHAR));
		char_to_wchar(descr, e.what());
		result = descr;
		pAdapter->AddError(1, source, descr, 0);
		res = false;
	}
	catch (_com_error & e) {
		/*передаем ошибку в 1С*/
		wchar_t * source = L"Error", *descr;
		pMemoryAdapter->AllocMemory((void**)&descr, std::strlen(e.Description())*sizeof(WCHAR));
		char_to_wchar(descr, e.Description());
		result = descr;
		pAdapter->AddError(1, source, descr, 0);
		res = false;
		/*выгрузка среды COM*/
		CoUninitialize();
	}
	
	pvarRetValue->vt = VTYPE_PWSTR;
	pvarRetValue->wstrLen = result.length();
	/*нужно аллоцировать место в структуре возвращаемой в 1С*/
	pMemoryAdapter->AllocMemory((void**)&pvarRetValue->pwstrVal, pvarRetValue->wstrLen*sizeof(WCHAR));
	std::memcpy(pvarRetValue->pwstrVal, result.c_str(), pvarRetValue->wstrLen*sizeof(WCHAR));

	return res;
}