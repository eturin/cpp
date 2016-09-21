#include <iostream>
#include <string>

class HTable{
public:
	HTable():n(0), m(8), M(new std::pair<char, std::string>[m]) {}
	HTable(const HTable & other):n(0), m(other.m), M(new std::pair<char, std::string>[m]) {
		//������� ������ � ��������������� (��������� �� ���������)
		for(int i = 0; n<other.n && i < m; ++i)
			if(1==other.M[i].first) 
				this->add(other.M[i].second);							
	}
	HTable & operator=(const HTable & other) {
		if(this != &other) {
			n = 0;
			delete[] M;
			m = other.m;
			M = new std::pair<char, std::string>[m];
			//������� ������ � ��������������� (��������� �� ���������)
			for(int i = 0; n<other.n && i < m; ++i)
				if(1==other.M[i].first)
					this->add(other.M[i].second);
		}

		return *this;
	}
	bool add(const std::string & str) {
		if(3.0 / 4 < 1.0*n / m)
			resize();

		if(-1 == fnd(str)) {
			int rez = fnd(str, false);
			M[rez].first = 1; //���� ��������
			M[rez].second = str;
			++n;
			return true;
		} else
			return false;

	}
	bool del(const std::string & str) {
		int rez = fnd(str);
		if(-1 == rez)
			return false;
		else
			return (M[rez].first=-1,--n,true);
	}
	int fnd(const std::string & str, bool is_fnd=true) const {
		//��������� ��� ������
		int k = 0, rez = hfunc(str, k);
		//������������
		while(k+1<m && (is_fnd && -1==M[rez].first                 //������ � ��������� �����
			           || 1==M[rez].first && M[rez].second != str) //�� ������, �� ������ ���� ��������
					  )
			rez = hfunc_prob(++k,rez);
		//��������
		if(is_fnd && 1 == M[rez].first && M[rez].second == str)
			return rez;	//�������	
		else if(!is_fnd)
			return rez; //������� �������
		else
			return -1;  //�� �������
	}
	~HTable() { delete[] M;}
private:
	//������� �������� � ���������� ����
	int hfunc_prob(int k, int prev, int m=0) const {
		return (prev + k) % (m ? m : this->m);
	}
	//��� ������� �� ������
	int hfunc(const std::string & str, int k=0, const int a = 3) const {
		//������ ��� ������				
		int rez = str[0] % m;
		for(int i = 1, len = str.length(); i < len; ++i)
			rez = (rez*a + str[i]) % m;
		//���� ������������
		for(int i = 1; i < k; ++i)
			rez = hfunc_prob(i, rez);
		
		return rez;
	}
	//��������� ������� � ���������������
	void resize() {
		//�������� ������
		int old_n = n, old_m=m;
		std::pair<char, std::string> * old_M = M;
		//����� ������
		m *= 2;
		n = 0;
		M = new std::pair<char, std::string>[m];
		//������� ������ � ��������������� (��������� �� ���������)
		for(int i = 0; n<old_n && i < old_m; ++i)
			if(1==old_M[i].first) 
				this->add(old_M[i].second);
				

		//�������� ��������� �������
		delete[] old_M;
	}
	//�������� ������
	int n;
	int m;
	std::pair<char,std::string> * M;
};

int main() {
	HTable ht;
	do {
		char c='\0';
		(std::cin >> c).get();
		if('+' == c) {
			std::string str;
			std::getline(std::cin, str);
			if(ht.add(str))
				std::cout << "OK\n";
			else
				std::cout << "FAIL\n";
		} else if('-' == c) {
			std::string str;
			std::getline(std::cin, str);			
			if(ht.del(str))
				std::cout << "OK\n";
			else
				std::cout << "FAIL\n";
		} else if('?' == c) {
			std::string str;
			std::getline(std::cin, str);
			if(-1==ht.fnd(str))
				std::cout << "FAIL\n";
			else
				std::cout << "OK\n"; 
		} 
	} while(std::cin);

	std::system("pause");
	return 0;
}