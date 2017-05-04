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
	/*члены-данные*/
	size_t size;
	size_t cnt;
	std::pair<K, T> ** M;
	size_t p, a, b; //дл€ получени€ универсального семейства хеширующих функций

	/*хеш-функци€ из универсального семейства*/
	size_t h(const K & num) const {
		return ((a*num + b) % p) % size;
	}
	/*поиск большого простого числа*/
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
						//проверим следующее нечетное число
						p += 2;
					}
					d += 2;
				}
			} else
				++p;//корректировка дл€ четного числа			
		} while(!isPrime && p >= N/*на случай переполнени€*/);

		return p;
	}

	/*запрещаем копирование и присваивание*/
	hTab(const hTab &);
	hTab & operator=(const hTab &);
public:
	/*конструктор*/
	hTab(size_t size):size(size), cnt(0), M(new std::pair<K, T>*[size]) {
		//инициализаци€
		std::memset(M, 0, sizeof(std::pair<K, T>*)*size);
		//посто€нные дл€ получени€ случайной хеш-функции из универсального семейства хеширующих функций
		get_prime();
		std::srand(std::time(0));
		/*1<=a<=p-1*/
		a = std::rand()*(p - 1) / RAND_MAX;
		a += a == 0 ? 1 : 0;
		/*1<=b<=p-1*/
		b = std::rand()*(p - 1) / RAND_MAX;
	}
	/*деструктор*/
	~hTab() {
		for(size_t i = 0; i < size; ++i)
			if(M[i] != (std::pair<K, T>*)this)
				delete M[i];
		delete[] M;
	}
	/*добавление нового ключа и значени€*/
	bool add(K & num, T & val) {
		if(size > cnt) {
			size_t k = 0, i = h(num), j;			
			do {				
				j = (i + k) % size;
				if(M[j] == nullptr || M[j] == (std::pair<K, T>*)this) {
					//вставка нового значени€ и ключа					
					break;
				} else if(M[j]->first == num) {
					//замена значени€ по ключу					
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
			//таблица полностью заполнена
			return false;
	}
	/*поиск значени€ по ключу*/
	const T * find(const K & num) const {
		size_t k = 0, i = h(num), j;
		do {				
			j = (i + k) % size;
			if(M[j] == nullptr)
				//позици€ пуста и не удалена
				return nullptr;
			else if(M[j] != (std::pair<K, T>*)this && M[j]->first == num)
				//совпадение
				return &M[j]->second;
			
			//если позици€ зан€та или удалена, то продолжаем поиск по перестановкам
			++k;
		} while((i + k) % size!=i);

		return nullptr;
	}
	/*удаление значени€ по ключу*/
	void del(const K & num) {
		size_t k = 0, i=h(num), j;
		do {
			j = (i + k) % size;
			if(M[j] == nullptr)
				//позици€ пуста и не удалена (действий не требуетс€)
				return;
			else if(M[j] != (std::pair<K, T>*)this && M[j]->first == num) {
				//совпадение (будем удал€ть)
				delete M[j];
				M[j] = (std::pair<K, T>*)this;
				--cnt;
				return;
			}

			//если позици€ зан€та или удалена, то продолжаем поиск по перестановкам
			++k;
		} while((i + k) % size != i);
	}
};

int main() {
	//забираем количество запросов
	unsigned n;
	std::cin >> n;

	//создаем хеш-таблицу
	hTab<size_t, std::string> ht(n);

	//обработка запросов
	std::string str; 
	size_t num;
	while(n && std::cin >> str >> num) {
		//конвертаци€ символов в нижний регистр
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if(str == "add") {
			//добавление			
			std::getline(std::cin, str);
			//удал€ем пробелы слева
			str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			//удал€ем пробелы справа
			str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),str.end());
			//берем только первые 15 символов
			std::string tmp = str.substr(0, 15);
			ht.add(num, tmp);
		} else if(str == "find") {
			//ищем			
			const std::string * pStr=ht.find(num);
			if(pStr == nullptr)
				std::cout << "not found\n";
			else
				std::cout << *pStr << std::endl;
		} else {
			//удал€ем						
			ht.del(num);
		}
		--n;
	}

	std::system("pause");
	return 0;
}