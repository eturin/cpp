#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>

int start_server();
int start_client();

int main() {
	/*локализация*/
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	/*выбор варианта работы*/
	printf("Меню:\n\t1 - сервер\n\t2 - клиент\n: ");
	int res=0;
	scanf("%d",&res);

	if(res == 1)
		res = start_server();
	else
		res = start_client();

	system("pause");
	return res;
}