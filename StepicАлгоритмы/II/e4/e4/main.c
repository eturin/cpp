#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

/*получение корня дерева*/
unsigned get_root(unsigned*, unsigned);

int main() {
	//получаем кол-во переменных, равенстр и не равенств
	unsigned n, e, d;
	scanf("%u %u %u", &n, &e, &d);

	//формируем и инициализируем массив
	unsigned * M = (unsigned *)malloc(n*sizeof(unsigned));
	for(unsigned i = 0; i < n; ++i)
		M[i] = i;

	//читаем равенства
	while(e>0) {
		/*извлекаем номера таблиц*/
		unsigned i, j;
		scanf("%u %u", &i, &j);
		/*получаем индексы*/
		--i;
		--j;
		i = get_root(M, i);
		j = get_root(M, j);
		/*устанавливаем равенство через корень*/
		M[j] = i;

		--e;
	}

	//проверяем неравенства
	int is_ok = 1;
	while(is_ok && d>0) {
		/*извлекаем номера таблиц*/
		unsigned i, j;
		scanf("%u %u", &i, &j);
		/*получаем индексы*/
		--i;
		--j;
		i = get_root(M, i);
		j = get_root(M, j);
		/*если неравенство не выполняется, то выход*/
		is_ok = i != j;

		--d;
	}

	//печатаем результат проверки
	printf("%d",is_ok);
	
	//освобождение ресурсов
	free(M);

	system("pause");
	return 0;
}

/*получение корня дерева*/
unsigned get_root(unsigned * M, unsigned i) {
	if(M[i] != i)
		M[i] = get_root(M, M[i]);

	return M[i];
}