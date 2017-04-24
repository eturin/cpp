#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>

#include <deque>

int main() {
	//������ ���-�� ��������� � �������
	int n;
	std::scanf("%d",&n);

	//���������� � ��������� ������
	int * M = (int*)malloc(n*sizeof(int));
	for(int i = 0; i < n; ++i) {
		int k;
		std::scanf("%d", &k);
		M[i] = k;
	}

	//������ ������ ����������� ����
	int m;
	std::scanf("%d", &m);
	
	//��������� ��������� � ���������� ����
	std::deque<int> dq;
	for(int i = 0; i < n; ++i) {
		if(i >= m) {
			//������� �������� � ����
			int k = dq.front();
			printf("%d ",k);
			//� ����������� ���� �������
			if(k == M[i-m])
				dq.pop_front();
		}

		//������� ������ ����� �� ������� ���������
		while(!dq.empty() && dq.back() < M[i])
			dq.pop_back();
		//����� �������� ����� �������
		dq.push_back(M[i]);		
	}

	if(!dq.empty() && n>=m) {
		int k = dq.front();
		printf("%d ", k);
	}

	//����������� �������
	free(M);
	dq.clear();

	std::system("pause");
	return 0;
}