#include <iostream>
#include <queue>

template<typename T>
class Heap {
private:
	//��������� ��������� 
	static bool cmp(const T * pM, size_t i, size_t j) {
		return pM[i - 1] < pM[j - 1]; //����-���� �����
	}
	//������������ ���������
	static void swap(T * pM, size_t i, size_t j, std::queue<std::pair<size_t, size_t>> &Q) {
		Q.push({i - 1, j - 1});
		T t = pM[i - 1];
		pM[i - 1] = pM[j - 1];
		pM[j - 1] = t;
	}
	//����������
	static void shiftDn(T * pM, size_t i, size_t size, std::queue<std::pair<size_t, size_t>> &Q) {
		while(true) {
			//�������� �� �������� ����� (���� ����)
			size_t j = i * 2;
			if(j + 1 <= size) {								
				j = cmp(pM, j, j + 1) ? j : j + 1;
			} else if(j > size)
				j = i; //��� �������� �����
			
			//���������� ��������� � ���������
			if(i==j)
				//�������� ���� �������������
				break;
			else if((j = cmp(pM, i, j) ? i : j)!=i){
				//������ �������
				swap(pM, i, j, Q);				
				//������ �������� ��� ������������� ��������� ����
				i = j;
			} else
				//�������� ���� �������������
				break;
		}
	}
	//������
	//static void shiftUp(T * pM, size_t i, size_t size) {		
	//	while(i>1) {
	//		//��������� ������ ��������
	//		size_t j = i/2;
	//		//��������� � ���������
	//		if(cmp(pM,i, j)) {
	//			//������ �������
	//			swap(pM, i, j);				
	//			//������ �������� ��� ������������� ����
	//			i = j;
	//		} else
	//			break;
	//	}
	//}	
	//��������� ��������� ��������
	/*static T getRoot(const T * pM) {
		return *pM;
	}*/
public:
	//���������� ����
	static void makeHeap(T * pM, size_t size, std::queue<std::pair<size_t, size_t>> &Q) {
		for(size_t i = size / 2; i > 0; --i)
			shiftDn(pM, i, size, Q);
	}
	//static void sort(T * pM, size_t size) {
	//	//������ ����
	//	makeHeap(pM, size);

	//	//������������� ��������
	//	while(size > 1) {
	//		swap(pM, 1, size);      //������ ������������ � �����
	//		shiftDn(pM, 1, --size); //����� ����� ������, �������� ������ �� ������������ �����
	//	}
	//}
};

int main() {
	//�������� ���������� ���������
	size_t n;
	std::cin >> n;
	//������ ��� ��������
	int * M = new int[n];
	for(size_t i = 0; i < n; ++i)
		std::cin >> M[i];
	
	//������� �������
	std::queue<std::pair<size_t, size_t>> Q;
	//������ ���� �� �����
	Heap<int>::makeHeap(M, n, Q);
	//�������� �������
	std::cout << Q.size() << std::endl;
	while(!Q.empty()) {
		std::cout << Q.front().first << ' ' << Q.front().second << std::endl;
		Q.pop();
	}

	//����������� �������
	delete[] M;


	std::system("pause");
	return 0;
}
