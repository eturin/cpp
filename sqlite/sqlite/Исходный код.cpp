#include <iostream>
using std::cout;
using std::endl;
using std::cin;
#include "sqlite3.h"


int main(){
	setlocale(LC_ALL, "rus");

	sqlite3* cn;
	
	char *err;

	//��������� ����������
	if(sqlite3_open("myBD.sq", &cn))  
		cout << "������ ��������/�������� ��:\n" << sqlite3_errmsg(cn) << endl << endl;
	
	else{
		char *cmd{"CREATE TABLE IF NOT EXISTS foo(a, b, c); \
				   INSERT INTO FOO VALUES(7, 8, 9);         \
				   INSERT INTO FOO VALUES(10, 11, 12);"};
		/*��������� SQL*/
		if(sqlite3_exec(cn, cmd, 0, 0, &err)) 	{
			cout << "������ SQL:\n" << err << endl << endl;
			sqlite3_free(err);
		}

		//Select
		sqlite3_stmt *statement;
		cmd="select * from foo;";
		if(sqlite3_prepare(cn, cmd, -1, &statement, 0) == SQLITE_OK){
			//���������� �������
			int iCol = sqlite3_column_count(statement);
			
			int res = 0;
			while(1){
				//��������� ���������
				res = sqlite3_step(statement);
				if(res == SQLITE_ROW){
					//������� �� ��������
					for(int i = 0; i < iCol; i++){
						char *s = (char*)sqlite3_column_text(statement, i);
						cout << s << " ";
					}
					cout << endl;
				}else if(res == SQLITE_DONE){
					cout << "done " << endl;
					break;
				}else if(res == SQLITE_ERROR){
					cout << "������ ����������:\n" << sqlite3_errmsg(cn) << endl << endl;
					break;
				}
			}
		}
	}

	//��������� ����������
	sqlite3_close(cn);

	system("pause");
	return 0;
}