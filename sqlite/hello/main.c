#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "sqlite3.h"
#include <locale.h>
#include <string.h>



static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
	for (i = 0; i<argc; i++){
	    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	
	}
	printf("\n");
	return 0;	
}

int main(int argc, char **argv){
	setlocale(LC_ALL, "russian");


    //проверяем аргументы командной строки
    if (argc != 2){
      fprintf(stderr, "Использование: %s DATABASE\n", argv[0]);	  
      return 1;
	}

	//устанавливаем соединение
	sqlite3 *db;
	int rc = sqlite3_open(argv[1], &db);
    if (rc){
      fprintf(stderr, "Не удалось установить соединение: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return 2;
	}

	char isRepeat = 1;
	do{
		//прочитаем запрос с консоли
		char query[1000] = { 0 };
		printf("sql>");
		gets(query);
		//условие выхода
		if (!strcmp(query, "exit"))
			break;

		//выполняем запрос
		/*char *zErrMsg = 0;		
		rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
		if (rc != SQLITE_OK){
			fprintf(stderr, "Ошибка SQL: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}*/

		sqlite3_stmt *stmt;
		sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
		//sqlite3_bind_int(stmt, 1, 16);                                                                  
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			int cnt=sqlite3_column_count(stmt);
			for (int i = 0; i < cnt;++i)
				printf("(%s) %s, ", sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
			printf("\n");
		}

		sqlite3_finalize(stmt);
	} while (isRepeat);
	
	//закрываем соединение
	sqlite3_close(db);

	system("pause");
	return 0;	
}