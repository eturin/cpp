#include <iostream>

template<typename T>
class Heap {
private:
	//сравнение элементов 
	static bool cmp(const T * pM, size_t i, size_t j) {
		return pM[i - 1] < pM[j - 1]; //мини-куча будет
	}
	//перестановка элементов
	static void swap(T * pM, size_t i, size_t j) {		
		T t = pM[i - 1];
		pM[i - 1] = pM[j - 1];
		pM[j - 1] = t;
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
public:
	//погружение
	static void shiftDn(T * pM, size_t i, size_t size) {
		while(true) {
			//максимум на дочерних узлах (если есть)
			size_t j = i * 2;
			if(j + 1 <= size) {
				j = cmp(pM, j, j + 1) ? j : j + 1;
			} else if(j > size)
				j = i; //нет дочерних узлов

			//финанльное сравнение с родителем
			if(i == j)
				//свойство кучи восстановлено
				break;
			else if((j = cmp(pM, i, j) ? i : j) != i) {
				//меняем местами
				swap(pM, i, j);
				//дальше повторим для изменившегося дочернего узла
				i = j;
			} else
				//свойство кучи восстановлено
				break;
		}
	}
	//построение кучи
	static void makeHeap(T * pM, size_t size) {
		for(size_t i = size / 2; i > 0; --i)
			shiftDn(pM, i, size);
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


struct AB {
	unsigned           p;
	unsigned long long t = 0;
	bool operator<(const struct AB & other) const {
		if(t == other.t)
			return p < other.p;
		else
			return t < other.t;
	}
};

int main() {
	//забираем кол-во апроцессоров и задач
	unsigned n, m = 0;
	std::cin>> n >> m;

	//строим массив загруженности процессоров (в младшей части номер процессора, а в старшей части время освобождения от последней задачи)
	struct AB * M = new struct AB[n];
	for(unsigned i = 0; i < n; ++i) {
		//при такой инициализации мини-куча строится сама собой
		M[i].p = i;		
	}
	//makeHeap(M, n);

	//забираем задачи и распределяем их по процессорам сразу
	unsigned t, k = 0;
	while(k<m && std::cin>>t) {		
		//сообщаем номер процессора(младшая часть) и время начала обработки(старшая)
		std::cout << M[0].p << ' ' << M[0].t << std::endl;
		//сдвигаем время освобождения процессора на поздний срок
		M[0].t += t;
		++k;
		//восстанавливаем свойство кучи
		Heap<struct AB>::shiftDn(M, 1, n);
	}

	//освобождение ресурсов
	delete[] M;

	std::system("pause");
	return 0;
}