#include <iostream>

template<typename T>
class Heap {
private:
	//��������� ��������� 
	static bool cmp(const T * pM, size_t i, size_t j) {
		return pM[i - 1] < pM[j - 1]; //����-���� �����
	}
	//������������ ���������
	static void swap(T * pM, size_t i, size_t j) {		
		T t = pM[i - 1];
		pM[i - 1] = pM[j - 1];
		pM[j - 1] = t;
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
public:
	//����������
	static void shiftDn(T * pM, size_t i, size_t size) {
		while(true) {
			//�������� �� �������� ����� (���� ����)
			size_t j = i * 2;
			if(j + 1 <= size) {
				j = cmp(pM, j, j + 1) ? j : j + 1;
			} else if(j > size)
				j = i; //��� �������� �����

			//���������� ��������� � ���������
			if(i == j)
				//�������� ���� �������������
				break;
			else if((j = cmp(pM, i, j) ? i : j) != i) {
				//������ �������
				swap(pM, i, j);
				//������ �������� ��� ������������� ��������� ����
				i = j;
			} else
				//�������� ���� �������������
				break;
		}
	}
	//���������� ����
	static void makeHeap(T * pM, size_t size) {
		for(size_t i = size / 2; i > 0; --i)
			shiftDn(pM, i, size);
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
	//�������� ���-�� ������������ � �����
	unsigned n, m = 0;
	std::cin>> n >> m;

	//������ ������ ������������� ����������� (� ������� ����� ����� ����������, � � ������� ����� ����� ������������ �� ��������� ������)
	struct AB * M = new struct AB[n];
	for(unsigned i = 0; i < n; ++i) {
		//��� ����� ������������� ����-���� �������� ���� �����
		M[i].p = i;		
	}
	//makeHeap(M, n);

	//�������� ������ � ������������ �� �� ����������� �����
	unsigned t, k = 0;
	while(k<m && std::cin>>t) {		
		//�������� ����� ����������(������� �����) � ����� ������ ���������(�������)
		std::cout << M[0].p << ' ' << M[0].t << std::endl;
		//�������� ����� ������������ ���������� �� ������� ����
		M[0].t += t;
		++k;
		//��������������� �������� ����
		Heap<struct AB>::shiftDn(M, 1, n);
	}

	//������������ ��������
	delete[] M;

	std::system("pause");
	return 0;
}