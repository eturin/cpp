#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tlhelp32.h>

#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>

#include "expat.h"
#include "sqlite3.h"

#include <sql.h>
#include <sqlext.h>

#define MAX_DATA 255

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

//����� id
#define LENID 37
#define MAXPATHNODE 1024

//��������� ��������� � ������� xml 
struct DataContent{
	sqlite3 *db;                //���� sqLite	
	char    *Job;               //��������
	char     task[LENID];       //id ������� 
	int      depth;             //������� ����������� ��������
	char	 node[MAXPATHNODE]; //������� ����
	void    *data;              //����������� ������ ����
};

//��������� �������
struct Task{
	char   *Id;
	char   *Job;        //��������
	char   *Priority;   //���������
	char    Date[28];   //����
	char   *Name;       //�������� 
	size_t  lenName;    //����� ������������
	char   *User;       //������������
	size_t  lenUser;   
	char   *Property;   //������ �������� �������
	size_t  lenProperty;
};

int convert1251toUtf8(char * str, char ** putf8){
	int len = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
	if (len){
		wchar_t * unicode_str = (wchar_t*)malloc(sizeof(wchar_t)*len);
		if ((len = MultiByteToWideChar(1251, 0, str, len, unicode_str, len))
			&& (len = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, 0, 0, 0, 0))){
			*putf8 = (char*)malloc(len);
			if (!WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, *putf8, len, 0, 0)){
				free(*putf8);
				*putf8 = NULL;
			}						
		}
		free(unicode_str);
	}

	return len;
}

int convertUnicodeToUtf8(wchar_t * unicode_str, char ** putf8){	
	int len = 0;
	if (len = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, 0, 0, 0, 0)){
		*putf8 = (char*)malloc(len+1);
		memset(*putf8, 0, len + 1);
		if (!WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, *putf8, len, 0, 0)){
			free(*putf8);
			*putf8 = NULL;
		}
	}		

	return len;
}

//������ ������ � ���-����
void saveErrorTo_stderr(const char * title, const char * msg) {
	time_t t = time(0);
	struct tm * now = localtime(&t);
	fprintf(stderr, "[%04d.%02d.%02d %02d:%02d:%02d] %s: %s\n", now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, title, msg);
	fflush(stderr);
}
//������ ������ � ����
void saveError(sqlite3 *db, char * msg, char * comm, char * strId){
	//���������� � sqLite
	char * strDML = "insert into err values(datetime('now','localtime'),?,?,?)";
	sqlite3_stmt *stmt;
	sqlite3_prepare(db, strDML, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, strId, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, msg  , -1, SQLITE_TRANSIENT);
		
	wchar_t *utf8 = NULL;
	int len = convert1251toUtf8(comm, &utf8);
	if (utf8){
		sqlite3_bind_text(stmt, 3, utf8, len, SQLITE_TRANSIENT);
		free(utf8);
	}else
		sqlite3_bind_text(stmt, 3, comm, -1, SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		saveErrorTo_stderr("�� ������� ��������� ������ ������ � ����", sqlite3_errmsg(db));
	
	sqlite3_finalize(stmt);
}

//����������� ����: DD.MM.YYYY HH:MM:SS.SSSSSS->YYYY-MM-DD HH:MM:SS.SSS
char * strDateToDate(char  (*str)[28]){
	char str_new[24] = { 0 };
	unsigned YYYY, MM, DD, h, m, s, ss;
	YYYY = MM = DD = h = m = s = ss = 0;
	sscanf(*str, "%u.%u.%u %u:%u:%u.%u", &DD, &MM, &YYYY, &h, &m, &s, &ss);
	ss /= 1000;
	sprintf(str_new, "%04u-%02u-%02u %02u:%02u:%02u.%03u", YYYY, MM, DD, h, m, s, ss);
	strncpy(*str,str_new,24);
	return *str;
}

//������������ ��������� �������
void freeTask(void ** data, sqlite3 *db){
	if (*data != NULL){
		struct Task *tsk = (struct Task*)*data;
		//���������� � sqLite
		char * strDML = "insert or replace into tasks (id,job,begin,stop,name,user,user_prop,priority) values(?,?,?,datetime('now','localtime'),?,?,?,?)";
		sqlite3_stmt *stmt;
		sqlite3_prepare(db, strDML, -1, &stmt, NULL);
		sqlite3_bind_text(stmt, 1/*������ ���������*/, tsk->Id                  , -1              , SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 2                    , tsk->Job                 , -1              , SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 3                    , strDateToDate(&tsk->Date), -1              , SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 4                    , tsk->Name                , tsk->lenName    , SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 5                    , tsk->User                , tsk->lenUser    , SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 6                    , tsk->Property            , tsk->lenProperty, SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 7                    , tsk->Priority            , -1              , SQLITE_TRANSIENT);

		int rc = 0;
		do{
			rc = sqlite3_step(stmt);
		} while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

		if (rc != SQLITE_DONE)			
			saveError(db, sqlite3_errmsg(db),"�� ������� ��������� ������ �������� � ������", tsk->Id);
		
		sqlite3_finalize(stmt);

		//���������� �������		
		free(tsk->Priority);
		free(tsk->Name);
		free(tsk->User);
		free(tsk->Property);
		free(tsk);
		*data = NULL;
	}
}

//��������� ��������� �����
struct FileIn{
	char  Id[LENID];  //id-�����
	char *Name;       //��� 
	char  Date[28];   //���� �����������
	unsigned long long totalChar; //����� ��������
	unsigned long long UncerChar; //�� ���������� ��������
	unsigned long long Pages;     //����� �������
};

//������������ ��������� �������
void freeFileIn(void ** data){
	if (*data != NULL){
		struct FileIn *fin = (struct FileIn*)*data;
		free(fin->Name);
		free(fin);
		*data = NULL;
	}
}

//��������� ���������� �����
struct FileOut{
	char   Id[LENID]; //id-�����
	char  *Name;      //��� 
	size_t lenName;   //����� �����
	unsigned long long totalChar; //����� ��������
	unsigned long long UncerChar; //�� ���������� ��������
	unsigned long long Pages;     //����� �������
	char   *BarCode;   //��-����������� ����� �����
	size_t  lenBarCode;
	char   *path;      //������������ �����
	size_t  lenPath;
};

//������������ ��������� �������
void freeFileOut(void ** data){
	if (*data != NULL){
		struct FileOut *fout = (struct FileOut*)*data;
		printf("%s code=%s char=%llu fail=%llu pages=%llu\n", fout->Name, fout->BarCode, fout->totalChar, fout->UncerChar, fout->Pages);
		free(fout->Name);
		free(fout->BarCode);
		free(fout->path);
		free(fout);
		*data = NULL;
	}
}

void setValLen(char **target, char *source, int length, size_t * plen){
	if (*target != NULL){
		int len = plen != NULL ? *plen : strlen(*target);
		char *tmp = (char*)malloc(len+length + 1);
		strncpy(tmp, *target, len);
		free(*target);
		strncpy(tmp+len, source, length);
		tmp[len + length] = '\0';
		*target = tmp;		
	}else{
		*target = (char*)malloc(length + 1);
		strncpy(*target, source, length);
		(*target)[length] = '\0';
	}

	if (plen)
		*plen += length;
	
}

//������� ���������� ��� ����������� ������ ��������(����/node) � �������
void XMLCALL startElement(void *userData, const char *name, const char **atts){
	struct DataContent *dc = (struct DataContent *)userData;
	//������ ���� � �������� ����
	strcat(dc->node, "\\");
	strcat(dc->node, name);
	//����������� ������� ����������� �������� ��������
	dc->depth += 1;

	if (dc->depth == 1 && !strcmp(dc->node, "\\XmlResult")){
		free(dc->data);
		dc->data = malloc(sizeof(struct Task));
		memset(dc->data, 0, sizeof(struct Task));
		struct Task *tsk = (struct Task*)dc->data;		
		tsk->Job = dc->Job;

		for (int i = 0; atts[i]; i += 2)
			if (!strcmp(atts[i], "Id")){            //id-�������
				strncpy(dc->task, atts[i + 1] + 1, LENID-1);
				dc->task[LENID-1] = '\0';
				tsk->Id = dc->task;
			}else if (!strcmp(atts[i], "Priority"))	//��������� �������
				setValLen(&tsk->Priority, atts[i + 1], strlen(atts[i + 1]),NULL);
			else if (!strcmp(atts[i], "Date"))		//���� �������
				strcpy(tsk->Date, atts[i + 1]);
			
	}else if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\InputFile")){			
		freeTask(&dc->data, dc->db);
		dc->data = malloc(sizeof(struct FileIn));
		memset(dc->data, 0, sizeof(struct FileIn));
		struct FileIn *fin = (struct FileIn*)dc->data;

		for (int i = 0; atts[i]; i += 2)
			if (!strcmp(atts[i], "Name")){      	            //��� ����� 
				char * begin = strstr(atts[i + 1], "}_") + 2;
				setValLen(&fin->Name, begin, strlen(begin), NULL);
			}else if (!strcmp(atts[i], "FileModificationTime")) //���� �������
				strcpy(fin->Date, atts[i + 1]);
			else if (!strcmp(atts[i], "Id")){                   //id-�����
				strncpy(fin->Id, atts[i + 1] + 1, LENID - 1);
				fin->Id[LENID - 1] = '\0';
			}
			
	}else if (dc->depth == 3 && !strcmp(dc->node, "\\XmlResult\\InputFile\\Statistics")){
		struct FileIn *fin = (struct FileIn*)dc->data;

		for (int i = 0; atts[i]; i += 2){
			char * pEnd=NULL;
			if (!strcmp(atts[i], "TotalCharacters")) 			//����� �������� 
				fin->totalChar = strtoull(atts[i + 1], &pEnd, 10);
			else if (!strcmp(atts[i], "UncertainCharacters"))	//�� ���������� ��������
				fin->UncerChar = strtoull(atts[i + 1], &pEnd, 10);
			else if (!strcmp(atts[i], "PagesArea"))  			//����� �������
				fin->Pages = strtoull(atts[i + 1], &pEnd, 10);
		}

	}else if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\JobDocument")){
		freeTask(&dc->data, dc->db);
		dc->data = malloc(sizeof(struct FileOut));
		memset(dc->data, 0, sizeof(struct FileOut));
		struct FileOut *fout = (struct FileOut*)dc->data;
		for (int i = 0; atts[i]; i += 2)
			if (!strcmp(atts[i], "Id")){            //id-�����
				strncpy(fout->Id, atts[i + 1] + 1, LENID - 1);
				fout->Id[LENID - 1] = '\0';				
			}
	}else if (dc->depth == 3 && !strcmp(dc->node, "\\XmlResult\\JobDocument\\Statistics")){
		struct FileOut *fout = (struct FileOut*)dc->data;

		for (int i = 0; atts[i]; i += 2){
			char * pEnd = NULL;
			if (!strcmp(atts[i], "TotalCharacters"))			//����� �������� 
				fout->totalChar = strtoull(atts[i + 1], &pEnd, 10);
			else if (!strcmp(atts[i], "UncertainCharacters"))	//�� ���������� ��������
				fout->UncerChar = strtoull(atts[i + 1], &pEnd, 10);
			else if (!strcmp(atts[i], "PagesArea"))				//����� �������
				fout->Pages = strtoull(atts[i + 1], &pEnd, 10);
		}
	}
}

//������� ���������� ��� ����������� ���� ��������(����/node) � ������� (�� ����������, ���� ��� ����). 
//���� ���� �������� ������, �� ���������� ��������������� ��� ������� ���������.
void startData(void *userData, const char *content, int length){
	struct DataContent *dc = (struct DataContent *)userData;
	if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\Name")){
		struct Task *tsk = (struct Task*)dc->data;
		//��� �������
		setValLen(&tsk->Name, content, length, &tsk->lenName);
	}else if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\UserName")){
		struct Task *tsk = (struct Task*)dc->data;
		//��� ������������
		setValLen(&tsk->User, content, length, &tsk->lenUser);
	}else if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\UserProperty")){
		struct Task *tsk = (struct Task*)dc->data;
		//������ ��������
		setValLen(&tsk->Property, content, length, &tsk->lenProperty);
	}else if (dc->depth == 4 && !strcmp(dc->node, "\\XmlResult\\JobDocument\\OutputDocuments\\FileName")){
		struct FileOut *fout = (struct FileOut*)dc->data;
		//��� �����		
		char * strName;
		if (fout->Name != NULL){
			strName = (char*)malloc(fout->lenName + length + 2);
			strncpy(strName, fout->Name, fout->lenName);
			strcat(strName, ";");
			strncpy(strName + fout->lenName, content, length);
			strName[fout->lenName + length + 1] = '\0';
			fout->lenName += length;
		}else{
			strName = (char*)malloc(length + 1);
			strncpy(strName, content, length);
			strName[length] = '\0';
			fout->lenName = length;
		}
		
		free(fout->Name);
		fout->Name = strName;
	}else if (dc->depth == 3 && !strcmp(dc->node, "\\XmlResult\\JobDocument\\BarcodeText")){
		struct FileOut *fout = (struct FileOut*)dc->data;
		//�����-��� �����������
		setValLen(&fout->BarCode, content, length, &fout->lenBarCode);
	}else if (dc->depth == 5 && !strcmp(dc->node, "\\XmlResult\\JobDocument\\OutputDocuments\\FormatSettings\\OutputLocation")){
		struct FileOut *fout = (struct FileOut*)dc->data;
		//������������ �����
		setValLen(&fout->path, content, length, &fout->lenPath);
	}
}

//������� ���������� ��� ����������� ����� ��������(����/node) � ������� (�� ����������, ���� ��� ������������ ���� ��������)
void XMLCALL endElement(void *userData, const char *name){
	struct DataContent *dc = (struct DataContent *)userData;
	if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\InputFile")){
		//���������� � sqLite
		char * strDML = "insert or replace into fin (id,task,name,data,total_char,uncertain_char,pages) values(?,?,?,?,?,?,?)";
		sqlite3_stmt *stmt;
		sqlite3_prepare(dc->db, strDML, -1, &stmt, NULL);
		struct FileIn *fin = (struct FileIn*)dc->data;
		sqlite3_bind_text (stmt, 1, fin->Id                  , -1, SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 2, dc->task                 , -1, SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 3, fin->Name                , -1, SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 4, strDateToDate(&fin->Date), -1, SQLITE_TRANSIENT);		
		sqlite3_bind_int64(stmt, 5, fin->totalChar           , -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(stmt, 6, fin->UncerChar           , -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(stmt, 7, fin->Pages               , -1, SQLITE_TRANSIENT);

		int rc = 0;
		do{
			rc = sqlite3_step(stmt);
		} while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

		if (rc != SQLITE_DONE)			
			saveError(dc->db, sqlite3_errmsg(dc->db),"�� ������� ��������� ������ �������� � �������� �����", dc->task);
		

		sqlite3_finalize(stmt);

		//����������� �������
		freeFileIn(&dc->data);
	}else if (dc->depth == 2 && !strcmp(dc->node, "\\XmlResult\\JobDocument")){
		//���������� � sqLite		
		char * strDML = "insert or replace into fout (id,task,name,data,total_char,uncertain_char,pages,barcode,path) values(?,?,?,datetime('now','localtime'),?,?,?,?,?)";
		sqlite3_stmt *stmt;
		sqlite3_prepare(dc->db, strDML, -1, &stmt, NULL);
		struct FileOut *fout = (struct FileOut*)dc->data;
		sqlite3_bind_text (stmt, 1, fout->Id       , -1              , SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 2, dc->task       , -1              , SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 3, fout->Name     , fout->lenName   , SQLITE_TRANSIENT);
		sqlite3_bind_int64(stmt, 4, fout->totalChar, -1              , SQLITE_TRANSIENT);
		sqlite3_bind_int64(stmt, 5, fout->UncerChar, -1              , SQLITE_TRANSIENT);
		sqlite3_bind_int64(stmt, 6, fout->Pages    , -1              , SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 7, fout->BarCode  , fout->lenBarCode, SQLITE_TRANSIENT);
		sqlite3_bind_text (stmt, 8, fout->path     , fout->lenPath   , SQLITE_TRANSIENT);

		int rc = 0;
		do{
			rc = sqlite3_step(stmt);
		} while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

		if (rc != SQLITE_DONE)			
			saveError(dc->db, sqlite3_errmsg(dc->db), "�� ������� ��������� ������ �������� �� ��������� �����", dc->task);
		

		sqlite3_finalize(stmt);

		//����������� �������
		freeFileOut(&dc->data);
	}

	//������������ ��� �������� ����
	int len = 0;
	char *tmp = dc->node;
	while (*tmp!='\0'){
		if (*tmp == '\\') len = tmp - dc->node;
		++tmp;
	}
	dc->node[len] = '\0';
	//��������� ������� ����������� �������� ��������
	dc->depth  -= 1;
}

sqlite3 * initSql(char * path){
	//������������� ����������
	sqlite3 *db = NULL;
	int rc = sqlite3_open(path, &db);
	if(rc){
		saveErrorTo_stderr("�� ������� ���������� ����������", sqlite3_errmsg(db));		
		sqlite3_close(db);
		db = NULL;
	}else{
		//���������� ������������� ����
		char * strDDL = "                                                             \n"
			"--�������                                                                \n"
			"create table if not exists tasks(                                        \n"
			"	id         varchar(38) PRIMARY KEY, --�������� ����                   \n"
			"	job        varchar,                 --��� ��������                    \n"
			"	begin      datetime,                --���� �������                    \n"
			"	stop       datetime,                --���� ���������� �������         \n"
			"	name       varchar,                 --��� �������                     \n"
			"	user       varchar,                 --��� ������������                \n"
			"	user_prop  varchar,                 --�������� ������������           \n"
			"   priority   varchar                  --���������                       \n"
			");                                                                       \n"
			"                                                                         \n"
			"--��������� �����                                                        \n"
			"create table if not exists fout(                                         \n"
			"	id         varchar(38) PRIMARY KEY,          --�������� ����          \n"
			"	task       varchar(38) REFERENCES tasks(id), --id �������             \n"
			"	name       varchar,                          --��� �����              \n"
			"	data       datetime,                         --���� �����������       \n"
			"	                                                                      \n"
			"	total_char     integer, --����� �������� � �����                      \n"
			"	uncertain_char integer, --�� ���������� ��������                      \n"
			"	pages          integer, --����� ������� � �����                       \n"
			"	barcode        varchar, --���������������� ���� ��                    \n"
			"	path           varchar  --������������ �����                          \n"
			");                                                                       \n"
			"                                                                         \n"
			"--�������� �����                                                         \n"
			"create table if not exists fin(                                          \n"
			"	id         varchar(38) PRIMARY KEY,          --�������� ����          \n"
			"	task       varchar(38) REFERENCES tasks(id), --id �������             \n"
			"	name       varchar,                          --��� �����              \n"
			"	data       datetime,                         --���� �����������       \n"
			"	                                                                      \n"
			"	total_char     integer, --����� �������� � �����                      \n"
			"	uncertain_char integer, --�� ���������� ��������                      \n"
			"	pages          integer  --����� ������� � �����                       \n"			
			");                                                                       \n"
			"                                                                         \n"
			"--xml - ����                                                             \n"
			"create table if not exists xml(                                          \n"
			"	task       varchar(38) REFERENCES tasks(id) PRIMARY KEY, --id ������� \n"
			"	xml        varchar                                       --xml - ���� \n"
			");                                                                       \n"
			"                                                                         \n"
			"--error - ����                                                           \n"
			"create table if not exists err(                                          \n"
			"	data       datetime,                         --���� ������            \n"
			"	task       varchar(38),                      --id �������             \n"
			"	msg        varchar,                          --���������              \n"
			"	comm       varchar                           --����������             \n"			
			");                                                                       \n";
		

		char *error=NULL;
		do{
			rc = sqlite3_exec(db, strDDL, NULL, NULL, &error);
		} while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

		if(rc)	{
			saveErrorTo_stderr("�� ������� ��������� DDL", sqlite3_errmsg(db));
			sqlite3_free(error);
			free(error);
			sqlite3_close(db);
			db = NULL;
		}
	}	

	return db;	
}

//� ���� ������� �������� ��������� xml-����
void parseXML(sqlite3 *db, char * job, FILE * file) {
	//��������� ��������� � ������� xml 
	struct DataContent deepCont = { db, job, 0 };

	//������� ������
	XML_Parser parser = XML_ParserCreate("UTF-8");
	//��������� ��� � ����� ����������
	XML_SetUserData(parser, &deepCont);
	//������������� ����������� �������
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, startData);

	int done;
	char buf[BUFSIZ];
	do {
		//��������� ���� ����
		size_t len = fread(buf, sizeof(char), sizeof(buf), file);
		done = len < sizeof(buf); //���� ��������� ������, ��� ������ ������, ������ ��������� ���� �� �����

	    //������ ���� ����
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			char msg[20] = { 0 };
			sprintf(msg, "%" XML_FMT_INT_MOD "u line", XML_GetCurrentLineNumber(parser));
			saveError(db, XML_ErrorString(XML_GetErrorCode(parser)), msg, "");
			done = 1;
		}
	} while (!done);

	//���������� � sqLite
	if (deepCont.task != NULL) {
		char * strDML = "insert or replace into xml values(?,?)";
		sqlite3_stmt *stmt;
		sqlite3_prepare(db, strDML, -1, &stmt, NULL);
		sqlite3_bind_text(stmt, 1, deepCont.task, -1, SQLITE_TRANSIENT);

		fseek(file, 0, SEEK_END);             //��������� � ����� 
		size_t size = ftell(file);            //�������� �������� � ������
		rewind(file);                         //��������� � ������ ����� 
											  //fseek(file, 0, SEEK_SET);

											  //��������� ���� ���� � ����� 
		char *buf = (char *)malloc(size);
		memset(buf, 0, size);
		if (fread(buf, sizeof(char), size, file) < size) {
			perror("�� ������� ��������� ������ ������ xml-����� � ����");
			saveError(db, "I/O error", "�� ������� ��������� ������ ������ xml-�����", deepCont.task);
		}else {
			char * utf8 = NULL;
			int len = convertUnicodeToUtf8(buf, &utf8);
			if (utf8 != NULL)
				sqlite3_bind_blob(stmt, 2, utf8, len, SQLITE_TRANSIENT);
			else
				sqlite3_bind_blob(stmt, 2, buf, size, SQLITE_TRANSIENT);

			int rc = 0;
			do {
				rc = sqlite3_step(stmt);
			} while (rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

			if (rc != SQLITE_DONE)
				saveError(db, sqlite3_errmsg(db), "�� ������� ��������� ������ ������ xml-�����", deepCont.task);

			free(utf8);
		}
		free(buf);
		sqlite3_finalize(stmt);
	}

	//������������ ��������
	XML_ParserFree(parser);	
}

void printErr(HSTMT hstmt){
	unsigned      char szSQLSTATE[10];
	SDWORD        nErr;
	unsigned char msg[SQL_MAX_MESSAGE_LENGTH + 1];
	SWORD         cbmsg;
	unsigned char szData[MAX_DATA];

	while(SQLError(0, 0, hstmt, szSQLSTATE, &nErr, msg, sizeof(msg), &cbmsg) == SQL_SUCCESS){
		sprintf_s((char *)szData, sizeof(szData), "Error:\nSQLSTATE=%s, Native error=%ld, msg='%s'", szSQLSTATE, nErr, msg);
		saveErrorTo_stderr("������ ������� � SQL ������� (�������� ������)", (const char *)szData);
	}
}

void printErrD(HDBC hdbc){
	SQLRETURN  rc;
	SQLINTEGER NativeError;
	SQLCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLLEN     numRecs = 0;
	SQLGetDiagField(SQL_HANDLE_DBC, hdbc, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
	SQLSMALLINT i = 1, MsgLen;
	while(i <= numRecs && (rc = SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA){
		saveErrorTo_stderr("������ ����������� � SQL ������� (�������� ������)", (const char *)Msg);
		i++;
	}
}

int main(int argc, char *argv[]){
	//����������� �������
	setlocale(LC_ALL, "russian");
	
	if (argc < 4)
		printf("�� ���������� ���������� ��������� ������. ����������:\n\t%s <pathToSqliteBase> <�����������> <pathToXml> [<pathToNextXml>]", argv[0]);
	else{
		//��������������� stderr
		char strPath[256] = {0};
		strcat(strPath, argv[0]);
		strcat(strPath, ".log");
		freopen(strPath, "a", stderr);
		//���������� �������������
		HANDLE ghSemaphore = CreateSemaphore(NULL,           //default security attributes
			                                 1,              //��������� ����������
			                                 1,              //������������ ����������
			                                 "xmlToSqLite"); //��� ��������
		if (ghSemaphore == NULL) 
			saveErrorTo_stderr("�� ������� ������� �������", "xmlToSqLite"); 			
		else {
			//��������� ��� �������������� �����
			FILE ** mFiles = (FILE *)malloc(sizeof(FILE *)*(argc - 3));
			memset(mFiles, 0, sizeof(FILE *)*(argc - 3));
			for (int i = 3; i < argc; ++i) {
				FILE *file = mFiles[i - 3] = fopen(argv[i], "rb");
				if (file == NULL) {
					saveErrorTo_stderr("�� ������� ������� ����", argv[i]);
					perror("�� ������� ������� xml-����");
					fprintf(stderr, "\n");
					fflush(stderr);
				}
			}

			for(int i = 0; i < 3; ++i){ //�� 3-� ���� �� ���� ����
				//������� ����� � �������
				DWORD  dwWaitResult = WaitForSingleObject(ghSemaphore, 3600000); //INFINITE(����������� ��������)
				if(WAIT_OBJECT_0 == dwWaitResult){

					//������������� ���� sqLite
					sqlite3 *db = initSql(argv[1]);
					if(db != NULL){
						//������ xml-������
						for(int i = 3; i < argc; ++i)
							parseXML(db, argv[2], mFiles[i - 3]);

						//��������� ����������
						sqlite3_close(db);
					}

					//������� ����� �� ��������
					if(!ReleaseSemaphore(ghSemaphore, 1, NULL)){
						saveErrorTo_stderr("�� ������� ����� �� ��������", "xmlToSqLite");
						fprintf(stderr, "%d", GetLastError());
					}

					break;
				} else{
					saveErrorTo_stderr("������� ����� �������� ��������", argv[i]);
					//�������� ������
					SQLHENV henv;
					SQLAllocEnv(&henv);

					SQLHDBC    hdbc;
					SQLHSTMT   hstmt = NULL;
					SQLAllocConnect(henv, &hdbc);
					while(1){
						//�����������
						SQLCHAR connStr[] = "DRIVER={SQL Server}; Server=edo.maxus.lan\\EDO; Database=EDO; App=EDO; UID=connect_1c; PWD=#&bP8RN7;";
						SQLCHAR OutConnStr[MAX_DATA] = {0};
						SQLSMALLINT OutConnStrLen = 0;
						RETCODE rc = SQLDriverConnect(hdbc,
													  NULL,
													  connStr,
													  sizeof(connStr),
													  OutConnStr,
													  MAX_DATA,
													  &OutConnStrLen,
													  SQL_DRIVER_NOPROMPT);
						if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
							printf("%s %d\n", connStr, sizeof(connStr));
							printErrD(hdbc);
							break;
						}

						//������
						rc = SQLAllocStmt(hdbc, &hstmt);
						SQLCHAR strSql[] = "\
						EXECUTE AS LOGIN = 'maxus\\sql.mail.services';                      \
						exec msdb.dbo.sp_send_dbmail @profile_name = 'edo.mail.service',    \
						@recipients            = 'etyurin@maxus.ru',                        \
						@from_address          = '',                                        \
						@reply_to              = '',                                        \
						@copy_recipients       = 'dharitonova@svyaznoy.ru',                 \
						@blind_copy_recipients = '',                                        \
						@body_format           = 'html',                                    \
						@body                  = '<html><body><b>������ � ����� ��������.</b><br><i>�� ���� �������� ���������� � ������� ������� ��� ������.</i><br><br><br>������ ����.<br>��. ����.<br>� ���������� �����������, ��� Recognition Server 3.0.<br><br><i><b>PS:</b>��������! �������������. ����� ����-������.</i></body></html>',\
						@subject               = 'Double trouble, coldren bubble',          \
						@importance            = 'High'                                     \
						REVERT;";
						rc = SQLExecDirect(hstmt, strSql, SQL_NTS);
						if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
							printErr(hstmt);

						break;
					}

					//����������
					SQLFreeStmt(hstmt, SQL_DROP);
					SQLDisconnect(hdbc);
					SQLFreeConnect(hdbc);
					SQLFreeEnv(henv);
				}
			}
			
			//��������� �������
			CloseHandle(ghSemaphore);
			
			//������������ ��������
			for (int i = 3; i < argc; ++i) 
				fclose(mFiles[i - 3]);			
		}
	}

	//������ �������� � ������ OCRProcessor3.exe 
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
		while (Process32Next(snapshot, &entry) == TRUE)
			if (strcmp(entry.szExeFile, "OCRProcessor3.exe") == 0) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

				//��������� ���������
				if (GetPriorityClass(hProcess) != BELOW_NORMAL_PRIORITY_CLASS && !SetPriorityClass(hProcess, BELOW_NORMAL_PRIORITY_CLASS)) {
					DWORD dwError = GetLastError();
					if (ERROR_PROCESS_MODE_ALREADY_BACKGROUND == dwError)
						; //������ ��������� ��� ����������
					else {
						saveErrorTo_stderr("������ ��������� ����������", "OCRProcessor3.exe");
						fprintf(stderr,"��� ������: %d\n", dwError);
					}
				}

				CloseHandle(hProcess);
			}

	CloseHandle(snapshot);

	//system("pause");
	return 0;
}
