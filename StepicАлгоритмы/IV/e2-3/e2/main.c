#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
	int left, right;
	int key;
};

char check_Get_min_max(int, struct Node*, struct Node ** ppmin, struct Node ** pmax);

int main() {
	//выясняем количество вершин
	int n;
	scanf("%d", &n);

	//строим массив поиска вершины дерева и проверки наличия только одного родителя у вершины
	char * roots = (char*)malloc(n*sizeof(char));
	memset(roots, 0, n*sizeof(char));

	char is_ok = 1;

	//строим массив с описаниями 
	struct Node * m = (struct Node*)malloc(n*sizeof(struct Node));
	for(int i = 0; is_ok && i < n; ++i) {
		is_ok = 3 == scanf("%d %d %d", &m[i].key, &m[i].left, &m[i].right);

		if(m[i].left >= n)
			is_ok = 0;
		else if(m[i].left >= 0)
			roots[m[i].left] += 1;

		if(m[i].right >= n)
			is_ok = 0;
		else if(m[i].right >= 0)
			roots[m[i].right] += 1;

		//у вершины не должно быть двух родителей
		is_ok = is_ok && roots[m[i].left] <= 1 && roots[m[i].right] <= 1;
	}

	//найдем вершину (этот код не найдет вершину, потому что даже корень оказывается чей-то дочкой в 14-м тесте)
	int r = -1;
	for(int i = 0; is_ok && i < n; ++i)
		if(roots[i] == 0) {
			if(r != -1)
				is_ok = 0;
			else
				r = i;
		}
	free(roots);

	struct Node *pmin, *pmax;
	if(!is_ok /*|| r == -1*/) //
		printf("INCORRECT\n");
	else if(check_Get_min_max(0, m, &pmin, &pmax))
		printf("CORRECT\n");
	else
		printf("INCORRECT\n");

	//освобождение ресурсов	
	free(m);
	system("pause");

	return 0;
}

char check_Get_min_max(int i, struct Node * m, struct Node ** ppmin, struct Node ** ppmax) {
	char is_ok = 1;
	struct Node * pmin, *pmax;

	if(m[i].left > 0) {//отвергаем корень, как дочерний узел
		//максимум в левом дереве должен быть меньше значения родителя
		is_ok = check_Get_min_max(m[i].left, m, &pmin, &pmax)
			&& pmax->key < m[i].key;
		*ppmin = pmin;
	} else
		*ppmin = &m[i];


	if(is_ok && m[i].right > 0) {//отвергаем корень, как дочерний узел
		//минимум в правом дереве должен быть не меньше значения родителя
		is_ok = check_Get_min_max(m[i].right, m, &pmin, &pmax)
			&& m[i].key <= pmin->key;
		*ppmax = pmax;
	} else
		*ppmax = &m[i];

	return is_ok;
}