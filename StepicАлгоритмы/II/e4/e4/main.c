#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

/*��������� ����� ������*/
unsigned get_root(unsigned*, unsigned);

int main() {
	//�������� ���-�� ����������, �������� � �� ��������
	unsigned n, e, d;
	scanf("%u %u %u", &n, &e, &d);

	//��������� � �������������� ������
	unsigned * M = (unsigned *)malloc(n*sizeof(unsigned));
	for(unsigned i = 0; i < n; ++i)
		M[i] = i;

	//������ ���������
	while(e>0) {
		/*��������� ������ ������*/
		unsigned i, j;
		scanf("%u %u", &i, &j);
		/*�������� �������*/
		--i;
		--j;
		i = get_root(M, i);
		j = get_root(M, j);
		/*������������� ��������� ����� ������*/
		M[j] = i;

		--e;
	}

	//��������� �����������
	int is_ok = 1;
	while(is_ok && d>0) {
		/*��������� ������ ������*/
		unsigned i, j;
		scanf("%u %u", &i, &j);
		/*�������� �������*/
		--i;
		--j;
		i = get_root(M, i);
		j = get_root(M, j);
		/*���� ����������� �� �����������, �� �����*/
		is_ok = i != j;

		--d;
	}

	//�������� ��������� ��������
	printf("%d",is_ok);
	
	//������������ ��������
	free(M);

	system("pause");
	return 0;
}

/*��������� ����� ������*/
unsigned get_root(unsigned * M, unsigned i) {
	if(M[i] != i)
		M[i] = get_root(M, M[i]);

	return M[i];
}