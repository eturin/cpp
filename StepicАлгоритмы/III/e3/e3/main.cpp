#include <iostream>
#include <string>
#include <cstring>

#include <ctime>
#include <cstdlib>

/*получение простого числа не менее чем 2^k раз большего длинны образца n
(чтобы вероятность совпадений хеш-значений у разных строк была не выше 1/2^k )*/
unsigned long long get_prime(size_t, char k);

int main() {
	//получаем образец
	std::string pattern;
	std::getline(std::cin, pattern);

	//получаем текст
	std::string text;
	std::getline(std::cin, text);

	//постоянные хеш-функции зависят от длины образца (чтоб сократить кол-во совпадений хеш-значений)
	std::srand(time(0));
	unsigned long long p = get_prime(pattern.size(), 8), x = 1.0*std::rand() / RAND_MAX*p;
	x += x == 0 ? 1 : 0;

	//вычисляем хеш-значение образца
	long long k_1 = 0, mul_1=1;	
	for(std::string::const_reverse_iterator it = pattern.crbegin(); it != pattern.crend(); ++it) {
		k_1 = (k_1 + (*it)*mul_1 % p) % p;
		if(it+1 != pattern.crend())
			mul_1 = mul_1*x % p;
	}
		
	//поиск образца
	if(text.size() >= pattern.size()) {
		size_t j = 0;
		//вычисление хеш-значения первого фрагмента
		long long k_2 = 0, mul_2 = 1;
		for(size_t i = pattern.size(); i > 0; --i) {
			k_2 = (k_2 + text[i-1]*mul_2 % p) % p;
			mul_2 = mul_2*x % p;
		}

		do {
			if(k_1 == k_2 && !std::strncmp(&text[j], &pattern[0], pattern.size()))
				//совпали хеш-значения и символы 
				std::cout << j << ' ';

			//корректируем хеш-значение
			k_2 = (text[j + pattern.size()] + (p + k_2 - text[j] * mul_1 % p)*x) % p;
			//сдвигаем позицию
			++j;

		} while(j + pattern.size()<=text.size());			
	}
		
	std::system("pause");
	return 0;
}

/*получение простого числа не менее чем 2^k раз большего длинны образца n 
 (чтобы вероятность совпадений хеш-значений у разных строк была не выше 1/2^k )*/
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
					//проверим следующее нечетное число
					p += 2;
				}
				d += 2;
			}
		} else
			++p;//корректировка для четного числа			
	} while(!isPrime && p >= n/*на случай переполнения*/);

	return p;
}

