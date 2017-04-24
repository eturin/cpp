#include <iostream>

//рекурсивная функция вычисления глубины элемента
int deep(int, std::pair<int, int>*);

int main() {
	//получаем количество узлов
	int n;
	std::cin >> n;
	
	//инициализация структуры, описывающей дерево
	std::pair<int, int> * m = new std::pair<int, int>[n];
	for(int i = 0; i < n; ++i) {
		int k;
		std::cin >> k;
		m[i] = {k,-1};
	}
	
	//текущая высота дерева
	int h = 0;

	//вычисление глубины всех элементов
	for(int i = 0; i < n; ++i) 		
		h = h < deep(i, m) ? m[i].second : h;
	
	//ответ
	std::cout << h;

	//освобождение памяти
	delete[] m;

	std::system("pause");
	return 0;
}

//рекурсивная функция вычисления глубины элемента
int deep(int i, std::pair<int, int> * m) {
	if(m[i].first == -1)
		m[i].second = 1;
	else if(m[i].second==-1)
		m[i].second = 1 + deep(m[i].first, m);

	return m[i].second;
}