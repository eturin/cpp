#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>

#include <deque>

int main() {
	//узнаем кол-во элементов в массиве
	int n;
	std::scanf("%d",&n);

	//аллоцируем и заполн€ем массив
	int * M = (int*)malloc(n*sizeof(int));
	for(int i = 0; i < n; ++i) {
		int k;
		std::scanf("%d", &k);
		M[i] = k;
	}

	//узнаем размер скольз€щего окна
	int m;
	std::scanf("%d", &m);
	
	//вычисл€ем максимумы в скольз€щем окне
	std::deque<int> dq;
	for(int i = 0; i < n; ++i) {
		if(i >= m) {
			//выводим максимум в окне
			int k = dq.front();
			printf("%d ",k);
			//и отврасываем один элемент
			if(k == M[i-m])
				dq.pop_front();
		}

		//вначале чистим хвост от меньших элементов
		while(!dq.empty() && dq.back() < M[i])
			dq.pop_back();
		//потом доавл€ем новый элемент
		dq.push_back(M[i]);		
	}

	if(!dq.empty() && n>=m) {
		int k = dq.front();
		printf("%d ", k);
	}

	//освобождаем ресурсы
	free(M);
	dq.clear();

	std::system("pause");
	return 0;
}