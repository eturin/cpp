#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

//http://e-maxx.ru/algo/z_function
std::vector<std::size_t> zFunc(std::string &str){
	std::vector<std::size_t> z(str.size());
	z[0] = str.size();
	std::size_t iL = 0, iR = 0, len = str.size();
	for(std::size_t i = 0; i < len; ++i){ //обходим каждую позицию
		if(iR >= i)
			z[i] = std::min<std::size_t>(z[i - iL], iR - i + 1);
		else
			z[i] = 0;

		while(i + z[i] < len && str[i + z[i]] == str[z[i]]) ++z[i];

		if(iR < i + z[i] - 1){
			iL = i;
			iR = i + z[i] - 1;
		}
	}

	return z;
}

int main(){
	//забираем обе строки
	std::string a, b;
	std::getline(std::cin, a);
	std::getline(std::cin, b);

	//посчитаем вхождения
	std::size_t len = b.size(), cnt = 0;
	b += '\1';
	b += a;
	std::vector<std::size_t> z = zFunc(b);

	//печатаем
	for(std::size_t i = len; i<b.size(); ++i)
		if(z[i] == len)
			(++cnt, std::cout << (i - len - 1) << ' ');
	if(!cnt)
		std::cout << -1;

	return 0;
}