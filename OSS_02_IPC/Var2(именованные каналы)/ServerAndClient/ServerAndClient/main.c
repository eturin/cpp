#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

extern int StartServer();
extern int StartClient();

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	printf("Меню:\n\t1 - сервер\n\t2 - клиент\n: ");
	int type = 0;
	scanf("%d",&type);

	int isOk=0;
	if(type == 1)
		isOk = StartServer();
	else
		isOk = StartClient();

	system("pause");
	return isOk;
}