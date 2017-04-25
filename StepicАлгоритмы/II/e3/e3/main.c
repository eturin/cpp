#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>

struct PC {
	unsigned p; /*ссылка на ...*/
	unsigned c; /*кол-во строк*/
};

/*получение реальной таблицы*/
unsigned get_real_tab(struct PC*, unsigned);

int main() {
	//получаем кол-во таблиц и запросов
	unsigned n, m;
	scanf("%u %u", &n, &m);
	
	//формируем и заполняем массив
	struct PC * M = (struct PC *)malloc(n*sizeof(struct PC));
	unsigned max = 0;
	for(unsigned i = 0; i < n; ++i) {
		M[i].p = i;
		scanf("%u",&M[i].c);
		max = max < M[i].c ? M[i].c : max; //обновляем максимальное кол-во строк
	}

	//выполняем запросы и выводим максимум строк
	while(m>0) {
		/*извлекаем номера таблиц*/
		unsigned d, s;
		scanf("%u %u", &d, &s);
		/*получаем индексы*/
		--d;
		--s;
		d = get_real_tab(M, d);
		s = get_real_tab(M, s);
		if(d != s) {
			M[d].c += M[s].c;
			M[s].c = 0;
			M[s].p = d;
			max = max < M[d].c ? M[d].c : max; //обновляем максимальное кол-во строк
		}
		printf("%u\n",max);
		--m;
	}

	//освобождение ресурсов
	free(M);

	system("pause");
	return 0;
}

/*получение реальной таблицы*/
unsigned get_real_tab(struct PC * M, unsigned i) {
	if(M[i].p != i)
		M[i].p = get_real_tab(M, M[i].p);

	return M[i].p;
}