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
	/*хранимый тип*/
	struct Node{
		struct Node * next;
		std::string   val;
		Node(std::string & val, struct Node * next):val(std::move(val)), next(next) {}
		~Node() {
			delete next;
		}
	};
	/*члены данных*/
	size_t size;
	struct Node ** M;

	/*хеширующая функция (с жестко заданными параметрами)*/
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
	/*конструктор*/
	hTab(size_t size):size(size), M(new struct Node*[size]) {
		//инициализация
		std::memset(M, 0, sizeof(struct Node*)*size);
	}
	/*деструктор*/
	~hTab() {
		for(size_t i = 0; i < size; ++i)
			delete M[i];
		delete[] M;
	}
	/*добавление*/
	bool add(std::string & str) {
		size_t i = h(str);
		struct Node * head = M[i];
		while(head != nullptr) {
			if(head->val == str)
				return true;
			else
				//двигаемся дальше по списку				
				head = head->next;
		}
		//не найден
		M[i] = new Node(str, M[i]);

		return true;
	}
	/*удаление*/
	void del(const std::string & str) {
		size_t i = h(str);
		struct Node * head = M[i], *ph=nullptr;
		while(head!=nullptr) {
			if(head->val == str) {
				//удаляем из цепочки
				if(ph == nullptr)
					M[i] = head->next;
				else
					ph->next = head->next;
				head->next = nullptr;
				delete head;
				head = nullptr;
			} else {
				//двигаемся дальше по списку
				ph = head;
				head = head->next;
			}				
		}
	}
	/*поиск*/
	std::string * find(const std::string & str) {
		struct Node * head = M[h(str)];
		while(head != nullptr) {
			if(head->val == str) 
				return &head->val;
			else 
				//двигаемся дальше по списку				
				head = head->next;			
		}
		//не найден
		return nullptr;
	}
	/*печать*/
	void print(size_t i) {
		struct Node * head = M[i];
		while(head != nullptr) {
			std::cout << head->val << ' ';
			//двигаемся дальше по списку				
			head = head->next;
		}
		std::cout << std::endl;
	}
};

std::string get();

int main() {
	//забираем количество запросов
	unsigned m,n;
	std::cin >>m >> n;

	//создаем хеш-таблицу
	hTab ht(m);

	//обработка запросов
	std::string str;	
	while(n && std::cin >> str) {
		//конвертация символов в нижний регистр
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if(str == "add") {
			//добавление			
			str = get();
			ht.add(str);
		} else if(str == "find") {
			//ищем		
			str = get();
			const std::string * pStr = ht.find(str);
			if(pStr == nullptr)
				std::cout << "no\n";
			else
				std::cout << "yes\n";
		} else if(str == "check") {
			//печать списка
			size_t i;
			std::cin >> i;
			ht.print(i);
		} else {
			//удаляем
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
	//удаляем пробелы слева
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	//удаляем пробелы справа
	str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());

	return std::move(str);
}