#include <iostream>

#include <string>
#include <cstring>
#include <algorithm> 
#include <functional> 
#include <cctype>

#include <ctime>
#include <cstdlib>


template<typename K, typename T, size_t N=10000000>
class hTab final {
private:
	/*�����-������*/
	size_t size;
	size_t cnt;
	std::pair<K, T> ** M;
	size_t p, a, b; //��� ��������� �������������� ��������� ���������� �������

	/*���-������� �� �������������� ���������*/
	size_t h(const K & num) const {
		return ((a*num + b) % p) % size;
	}
	/*����� �������� �������� �����*/
	size_t get_prime() {
		p = N;
		
		bool isPrime = false;
		do{
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
		} while(!isPrime && p >= N/*�� ������ ������������*/);

		return p;
	}

	/*��������� ����������� � ������������*/
	hTab(const hTab &);
	hTab & operator=(const hTab &);
public:
	/*�����������*/
	hTab(size_t size):size(size), cnt(0), M(new std::pair<K, T>*[size]) {
		//�������������
		std::memset(M, 0, sizeof(std::pair<K, T>*)*size);
		//���������� ��� ��������� ��������� ���-������� �� �������������� ��������� ���������� �������
		get_prime();
		std::srand(std::time(0));
		/*1<=a<=p-1*/
		a = std::rand()*(p - 1) / RAND_MAX;
		a += a == 0 ? 1 : 0;
		/*1<=b<=p-1*/
		b = std::rand()*(p - 1) / RAND_MAX;
	}
	/*����������*/
	~hTab() {
		for(size_t i = 0; i < size; ++i)
			if(M[i] != (std::pair<K, T>*)this)
				delete M[i];
		delete[] M;
	}
	/*���������� ������ ����� � ��������*/
	bool add(K & num, T & val) {
		if(size > cnt) {
			size_t k = 0, i = h(num), j;			
			do {				
				j = (i + k) % size;
				if(M[j] == nullptr || M[j] == (std::pair<K, T>*)this) {
					//������� ������ �������� � �����					
					break;
				} else if(M[j]->first == num) {
					//������ �������� �� �����					
					break;
				}
				++k;
			} while(true); 
			
			if(M[j] == nullptr || M[j] == (std::pair<K, T>*)this)
				M[j] = new std::pair<K, T>({std::move(num), std::move(val)});
			else
				M[j]->second = std::move(val);

			++cnt;
			return true;
		}else
			//������� ��������� ���������
			return false;
	}
	/*����� �������� �� �����*/
	const T * find(const K & num) const {
		size_t k = 0, i = h(num), j;
		do {				
			j = (i + k) % size;
			if(M[j] == nullptr)
				//������� ����� � �� �������
				return nullptr;
			else if(M[j] != (std::pair<K, T>*)this && M[j]->first == num)
				//����������
				return &M[j]->second;
			
			//���� ������� ������ ��� �������, �� ���������� ����� �� �������������
			++k;
		} while((i + k) % size!=i);

		return nullptr;
	}
	/*�������� �������� �� �����*/
	void del(const K & num) {
		size_t k = 0, i=h(num), j;
		do {
			j = (i + k) % size;
			if(M[j] == nullptr)
				//������� ����� � �� ������� (�������� �� ���������)
				return;
			else if(M[j] != (std::pair<K, T>*)this && M[j]->first == num) {
				//���������� (����� �������)
				delete M[j];
				M[j] = (std::pair<K, T>*)this;
				--cnt;
				return;
			}

			//���� ������� ������ ��� �������, �� ���������� ����� �� �������������
			++k;
		} while((i + k) % size != i);
	}
};

int main() {
	//�������� ���������� ��������
	unsigned n;
	std::cin >> n;

	//������� ���-�������
	hTab<size_t, std::string> ht(n);

	//��������� ��������
	std::string str; 
	size_t num;
	while(n && std::cin >> str >> num) {
		//����������� �������� � ������ �������
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if(str == "add") {
			//����������			
			std::getline(std::cin, str);
			//������� ������� �����
			str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			//������� ������� ������
			str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),str.end());
			//����� ������ ������ 15 ��������
			std::string tmp = str.substr(0, 15);
			ht.add(num, tmp);
		} else if(str == "find") {
			//����			
			const std::string * pStr=ht.find(num);
			if(pStr == nullptr)
				std::cout << "not found\n";
			else
				std::cout << *pStr << std::endl;
		} else {
			//�������						
			ht.del(num);
		}
		--n;
	}

	std::system("pause");
	return 0;
}