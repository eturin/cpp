#include <iostream>
#include <string>
#include <cstring>

#include <ctime>
#include <cstdlib>

/*��������� �������� ����� �� ����� ��� 2^k ��� �������� ������ ������� n
(����� ����������� ���������� ���-�������� � ������ ����� ���� �� ���� 1/2^k )*/
unsigned long long get_prime(size_t, char k);

int main() {
	//�������� �������
	std::string pattern;
	std::getline(std::cin, pattern);

	//�������� �����
	std::string text;
	std::getline(std::cin, text);

	//���������� ���-������� ������� �� ����� ������� (���� ��������� ���-�� ���������� ���-��������)
	std::srand(time(0));
	unsigned long long p = get_prime(pattern.size(), 8), x = 1.0*std::rand() / RAND_MAX*p;
	x += x == 0 ? 1 : 0;

	//��������� ���-�������� �������
	long long k_1 = 0, mul_1=1;	
	for(std::string::const_reverse_iterator it = pattern.crbegin(); it != pattern.crend(); ++it) {
		k_1 = (k_1 + (*it)*mul_1 % p) % p;
		if(it+1 != pattern.crend())
			mul_1 = mul_1*x % p;
	}
		
	//����� �������
	if(text.size() >= pattern.size()) {
		size_t j = 0;
		//���������� ���-�������� ������� ���������
		long long k_2 = 0, mul_2 = 1;
		for(size_t i = pattern.size(); i > 0; --i) {
			k_2 = (k_2 + text[i-1]*mul_2 % p) % p;
			mul_2 = mul_2*x % p;
		}

		do {
			if(k_1 == k_2 && !std::strncmp(&text[j], &pattern[0], pattern.size()))
				//������� ���-�������� � ������� 
				std::cout << j << ' ';

			//������������ ���-��������
			k_2 = (text[j + pattern.size()] + (p + k_2 - text[j] * mul_1 % p)*x) % p;
			//�������� �������
			++j;

		} while(j + pattern.size()<=text.size());			
	}
		
	std::system("pause");
	return 0;
}

/*��������� �������� ����� �� ����� ��� 2^k ��� �������� ������ ������� n 
 (����� ����������� ���������� ���-�������� � ������ ����� ���� �� ���� 1/2^k )*/
unsigned long long get_prime(size_t n, char k) {
	unsigned long long p = n << k;
	++p;

	bool isPrime = false;
	do {
		if(p == 2)
			isPrime = true;
		else if(p % 2) {
			isPrime = true;
			long long d = 3;
			while(isPrime && d <= p / d) {
				if(p % d == 0) {
					isPrime = false;
					//�������� ��������� �������� �����
					p += 2;
				}
				d += 2;
			}
		} else
			++p;//������������� ��� ������� �����			
	} while(!isPrime && p >= n/*�� ������ ������������*/);

	return p;
}

