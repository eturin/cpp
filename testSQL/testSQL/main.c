#include <stdio.h>
#include <windows.h> 
#include <sql.h>
#include <sqlext.h>
#include <locale.h>

#define MAX_DATA 255

void printErr(HSTMT hstmt){
	unsigned      char szSQLSTATE[10];
	SDWORD        nErr;
	unsigned char msg[SQL_MAX_MESSAGE_LENGTH + 1];
	SWORD         cbmsg;
	unsigned char szData[MAX_DATA];

	while(SQLError(0, 0, hstmt, szSQLSTATE, &nErr, msg, sizeof(msg), &cbmsg) == SQL_SUCCESS){
		sprintf_s((char *)szData, sizeof(szData), "Error:\nSQLSTATE=%s, Native error=%ld, msg='%s'", szSQLSTATE, nErr, msg);
		printf("%s\n",(const char *)szData);
	}
}

void printErrD(HDBC hdbc){
	SQLRETURN  rc;
	SQLINTEGER NativeError;
	SQLCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLLEN     numRecs = 0;
	SQLGetDiagField(SQL_HANDLE_DBC, hdbc, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
	SQLSMALLINT i=1, MsgLen;
	while(i <= numRecs && (rc = SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA){
		printf("%s %d %s %d",SqlState, NativeError, Msg, MsgLen);
		i++;
	}
}

int main(){
	setlocale(LC_ALL, "russian");

	SQLHENV henv;
	SQLAllocEnv(&henv);
	
	SQLHDBC    hdbc; 
	SQLHSTMT   hstmt=NULL;
	SQLAllocConnect(henv, &hdbc);
	while(1){
		//подключение
		SQLCHAR connStr[] = "DRIVER={SQL Server}; Server=edo.maxus.lan\\EDO; Database=EDO; App=EDO; UID=connect_1c; PWD=#&bP8RN7;";
		SQLCHAR OutConnStr[MAX_DATA] = {0};
		SQLSMALLINT OutConnStrLen=0;
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
		
		//запрос
		rc = SQLAllocStmt(hdbc, &hstmt);
		rc = SQLExecDirect(hstmt, "select * from EДаНетID as t", SQL_NTS);
		if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
			printErr(hstmt);
		else{
			//определение количества колонок
			SQLSMALLINT cnt = 0;
			rc = SQLNumResultCols(hstmt, &cnt);
			if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
				printErr(hstmt);
				break;
			}
			//имена колонок
			for(SQLSMALLINT i = 1, len=0; i <= cnt; ++i){
				char name[MAX_DATA] = {0};
				rc = SQLColAttribute(hstmt, (SQLUSMALLINT)i, SQL_DESC_LABEL, name, MAX_DATA, &len, NULL);
				if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
					printErr(hstmt);
					break;
				}
				printf("%s ",name);
			}
			printf("\n");
			//извлечение
			SDWORD cbData;
			unsigned char szData[MAX_DATA];
			for(rc = SQLFetch(hstmt); rc == SQL_SUCCESS; rc = SQLFetch(hstmt)){				
				for(SQLSMALLINT i = 1; i <= cnt; ++i){
					SQLGetData(hstmt, i, SQL_C_CHAR, szData, sizeof(szData), &cbData);
					printf("%s ", (const char *)szData);
				}
				printf("\n");
			}
		}

		break;
	}

	//отключение
	SQLFreeStmt(hstmt, SQL_DROP);
	SQLDisconnect(hdbc);
	SQLFreeConnect(hdbc);
	SQLFreeEnv(henv);

	return 0;
}