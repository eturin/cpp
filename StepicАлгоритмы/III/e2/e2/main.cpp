#include <iostream>

#include <string>
#include <cstring>
#include <algorithm> 
#include <functional> 
#include <cctype>

#include <ctime>
#include <cstdlib>

class hTab final {
private:
	/*�������� ���*/
	struct Node{
		struct Node * next;
		std::string   val;
		Node(std::string & val, struct Node * next):val(std::move(val)), next(next) {}
		~Node() {
			delete next;
		}
	};
	/*����� ������*/
	size_t size;
	struct Node ** M;

	/*���������� ������� (� ������ ��������� �����������)*/
	size_t h(const std::string & str) const {
		unsigned long long sum = 0, mul = 1;
		for(std::string::const_iterator it = str.cbegin(); it != str.cend(); ++it) {
			sum += (*it)*mul;
			sum %=  1000000007;
			mul = (mul * 263) % 1000000007;			
		}
		return sum  % size;
	}
public:
	/*�����������*/
	hTab(size_t size):size(size), M(new struct Node*[size]) {
		//�������������
		std::memset(M, 0, sizeof(struct Node*)*size);
	}
	/*����������*/
	~hTab() {
		for(size_t i = 0; i < size; ++i)
			delete M[i];
		delete[] M;
	}
	/*����������*/
	bool add(std::string & str) {
		size_t i = h(str);
		struct Node * head = M[i];
		while(head != nullptr) {
			if(head->val == str)
				return true;
			else
				//��������� ������ �� ������				
				head = head->next;
		}
		//�� ������
		M[i] = new Node(str, M[i]);

		return true;
	}
	/*��������*/
	void del(const std::string & str) {
		size_t i = h(str);
		struct Node * head = M[i], *ph=nullptr;
		while(head!=nullptr) {
			if(head->val == str) {
				//������� �� �������
				if(ph == nullptr)
					M[i] = head->next;
				else
					ph->next = head->next;
				head->next = nullptr;
				delete head;
				head = nullptr;
			} else {
				//��������� ������ �� ������
				ph = head;
				head = head->next;
			}				
		}
	}
	/*�����*/
	std::string * find(const std::string & str) {
		struct Node * head = M[h(str)];
		while(head != nullptr) {
			if(head->val == str) 
				return &head->val;
			else 
				//��������� ������ �� ������				
				head = head->next;			
		}
		//�� ������
		return nullptr;
	}
	/*������*/
	void print(size_t i) {
		struct Node * head = M[i];
		while(head != nullptr) {
			std::cout << head->val << ' ';
			//��������� ������ �� ������				
			head = head->next;
		}
		std::cout << std::endl;
	}
};

std::string get();

int main() {
	//�������� ���������� ��������
	unsigned m,n;
	std::cin >>m >> n;

	//������� ���-�������
	hTab ht(m);

	//��������� ��������
	std::string str;	
	while(n && std::cin >> str) {
		//����������� �������� � ������ �������
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if(str == "add") {
			//����������			
			str = get();
			ht.add(str);
		} else if(str == "find") {
			//����		
			str = get();
			const std::string * pStr = ht.find(str);
			if(pStr == nullptr)
				std::cout << "no\n";
			else
				std::cout << "yes\n";
		} else if(str == "check") {
			//������ ������
			size_t i;
			std::cin >> i;
			ht.print(i);
		} else {
			//�������
			str = get();
			ht.del(str);
		}
		--n;
	}

	std::system("pause");
	return 0;
}

std::string get() {
	std::string str;
	std::getline(std::cin, str);
	//������� ������� �����
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	//������� ������� ������
	str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());

	return std::move(str);
}