#include <iostream>

//����������� ������� ���������� ������� ��������
int deep(int, std::pair<int, int>*);

int main() {
	//�������� ���������� �����
	int n;
	std::cin >> n;
	
	//������������� ���������, ����������� ������
	std::pair<int, int> * m = new std::pair<int, int>[n];
	for(int i = 0; i < n; ++i) {
		int k;
		std::cin >> k;
		m[i] = {k,-1};
	}
	
	//������� ������ ������
	int h = 0;

	//���������� ������� ���� ���������
	for(int i = 0; i < n; ++i) 		
		h = h < deep(i, m) ? m[i].second : h;
	
	//�����
	std::cout << h;

	//������������ ������
	delete[] m;

	std::system("pause");
	return 0;
}

//����������� ������� ���������� ������� ��������
int deep(int i, std::pair<int, int> * m) {
	if(m[i].first == -1)
		m[i].second = 1;
	else if(m[i].second==-1)
		m[i].second = 1 + deep(m[i].first, m);

	return m[i].second;
}