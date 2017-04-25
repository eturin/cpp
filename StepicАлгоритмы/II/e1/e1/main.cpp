#include <iostream>
#include <queue>

template<typename T>
class Heap {
private:
	//сравнение элементов 
	static bool cmp(const T * pM, size_t i, size_t j) {
		return pM[i - 1] < pM[j - 1]; //мини-куча будет
	}
	//перестановка элементов
	static void swap(T * pM, size_t i, size_t j, std::queue<std::pair<size_t, size_t>> &Q) {
		Q.push({i - 1, j - 1});
		T t = pM[i - 1];
		pM[i - 1] = pM[j - 1];
		pM[j - 1] = t;
	}
	//погружение
	static void shiftDn(T * pM, size_t i, size_t size, std::queue<std::pair<size_t, size_t>> &Q) {
		while(true) {
			//максимум на дочерних узлах (если есть)
			size_t j = i * 2;
			if(j + 1 <= size) {								
				j = cmp(pM, j, j + 1) ? j : j + 1;
			} else if(j > size)
				j = i; //нет дочерних узлов
			
			//финанльное сравнение с родителем
			if(i==j)
				//свойство кучи восстановлено
				break;
			else if((j = cmp(pM, i, j) ? i : j)!=i){
				//меняем местами
				swap(pM, i, j, Q);				
				//дальше повторим для изменившегося дочернего узла
				i = j;
			} else
				//свойство кучи восстановлено
				break;
		}
	}
	//подъем
	//static void shiftUp(T * pM, size_t i, size_t size) {		
	//	while(i>1) {
	//		//вычисляем индекс родителя
	//		size_t j = i/2;
	//		//сравнение с родителем
	//		if(cmp(pM,i, j)) {
	//			//меняем местами
	//			swap(pM, i, j);				
	//			//дальше повторим для родительского узла
	//			i = j;
	//		} else
	//			break;
	//	}
	//}	
	//получение корневого элемента
	/*static T getRoot(const T * pM) {
		return *pM;
	}*/
public:
	//построение кучи
	static void makeHeap(T * pM, size_t size, std::queue<std::pair<size_t, size_t>> &Q) {
		for(size_t i = size / 2; i > 0; --i)
			shiftDn(pM, i, size, Q);
	}
	//static void sort(T * pM, size_t size) {
	//	//строим кучу
	//	makeHeap(pM, size);

	//	//упорядочиваем элементы
	//	while(size > 1) {
	//		swap(pM, 1, size);      //голову переставляем в конец
	//		shiftDn(pM, 1, --size); //топим новую голову, уменьшив размер не обработанной части
	//	}
	//}
};

int main() {
	//выясняем количество элементов
	size_t n;
	std::cin >> n;
	//читаем все элементы
	int * M = new int[n];
	for(size_t i = 0; i < n; ++i)
		std::cin >> M[i];
	
	//очередь обменов
	std::queue<std::pair<size_t, size_t>> Q;
	//строим кучу на месте
	Heap<int>::makeHeap(M, n, Q);
	//печатаем очередь
	std::cout << Q.size() << std::endl;
	while(!Q.empty()) {
		std::cout << Q.front().first << ' ' << Q.front().second << std::endl;
		Q.pop();
	}

	//освобождаем ресурсы
	delete[] M;


	std::system("pause");
	return 0;
}
