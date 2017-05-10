#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct Node {
	int left, right;
	unsigned key;
};

void In_Order(size_t, struct Node *);
void Pre_Order(size_t, struct Node *);
void Post_Order(size_t, struct Node *);

int main() {
	//выясняем количество вершин
	size_t n;
	scanf("%u",&n);

	//строим массив поиска вершины дерева
	char * roots = (char*)malloc(n*sizeof(char));
	memset(roots, 1, n*sizeof(char));

	//строим массив с описаниями (корректность не проверяем)
	struct Node * m = (struct Node*)malloc(n*sizeof(struct Node));
	for(size_t i = 0; i < n; ++i) {
		scanf("%u %d %d", &m[i].key, &m[i].left, &m[i].right);
		if(m[i].left!=-1)
			roots[m[i].left] = 0;
		if(m[i].right != -1)
			roots[m[i].right] = 0;
	}

	//найдем вершину
	size_t r = SIZE_MAX;
	for(size_t i = 0; r == SIZE_MAX && i < n; ++i)
		if(roots[i])
			r = i;
	do {
		if(r == SIZE_MAX) {
			perror("Нет вершины у дерева");
			break;
		}

		//печатаем обходы
		In_Order(r, m);
		printf("\n");
		Pre_Order(r, m);
		printf("\n");
		Post_Order(r, m);
		
	} while(0);

	//освобождение ресурсов
	free(m);
	system("pause");

	return 0;
}


void In_Order(size_t i, struct Node * m) {
	if(m[i].left != -1)
		In_Order(m[i].left, m);

	printf("%u ",m[i].key);

	if(m[i].right != -1)
		In_Order(m[i].right, m);
}

void Pre_Order(size_t i, struct Node * m) {
	printf("%u ", m[i].key);

	if(m[i].left != -1)
		Pre_Order(m[i].left, m);
	
	if(m[i].right != -1)
		Pre_Order(m[i].right, m);
}

void Post_Order(size_t i, struct Node * m) {
	if(m[i].left != -1)
		Post_Order(m[i].left, m);

	if(m[i].right != -1)
		Post_Order(m[i].right, m);

	printf("%u ", m[i].key);
}