/*������ N ����� �� ���������. ������� (N-1)-������� �������������������� ����������� �������, ���������� ����� ��� ��� �����.

������� ������� � ������� ����������� x-����������. ���� ������� ��� ����� � ���������� x-�����������, �� ����������� ������ �� �����, � ������� y-���������� ������.

��� ���������� ����� ���������� ������������� ����������.*/
#include <iostream>
#include <utility>

typedef std::pair<double, double> xy;

int change(xy *, int, int);
void ShiftDown(xy *, int, int);

int main() {
	//��������� ������
	int n;
	std::cin >> n;
	xy * m = new xy[n];
	for(int i = 0; i < n; ++i)
		std::cin >> m[i].first >> m[i].second;

	//������ ���� �� �������, ������ ������ ������� � ��������
	for(int i = (n - 1) / 2, j; i >= 0; --i) {
		j = change(m, i, n);
		//��������������� �������� ����
		ShiftDown(m, j, n);
	}

	//��������������� ��������� ������� ���� � �����
	int heap_size = n;
	while(heap_size) {
		//������ ������� ������� � �����
		xy temp = m[0];
		m[0] = m[heap_size - 1];
		m[heap_size - 1] = temp;
		//��������� ������ ����
		--heap_size;
		//��������������� �������� ����
		ShiftDown(m, 0, heap_size);
	}

	//��������
	xy temp;
	for(int i = 0; i < n; ++i) {
		if(i == 0)
			std::cout << m[i].first << " " << m[i].second << std::endl;
		else if(temp != m[i])
			std::cout << m[i].first << " " << m[i].second << std::endl;
		temp = m[i];
	}

	delete[] m;
	std::system("pause");
	return 0;
}

int change(xy * m, int i, int n) {
	int j = 2 * i + 1;
	if(2 * i + 2 < n)
		//"���������� �� ���� ������"
		if(m[2 * i + 1].first > m[2 * i + 2].first)
			j = 2 * i + 1;
		else if(m[2 * i + 1].first == m[2 * i + 2].first && m[2 * i + 1].second > m[2 * i + 2].second)
			j = 2 * i + 1;
		else
			j = 2 * i + 2;

		//���������� � �����
		if(m[j].first > m[i].first);
		else if(m[j].first == m[i].first && m[j].second > m[i].second);
		else j = i;
		//������ �������
		if(i != j) {
			xy temp = m[i];
			m[i] = m[j];
			m[j] = temp;
		}
		return j;
}
void ShiftDown(xy * m, int i, int n) {
	while(2 * i + 1 < n) { //���� � ���� ���� ������, ���������
		int j = change(m, i, n);
		if(i == j)
			break;
		else
			i = j;
	}
}