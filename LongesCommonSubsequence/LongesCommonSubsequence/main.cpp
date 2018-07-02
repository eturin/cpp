#include <iostream>
#include <iomanip>

#include <vector>
#include <string>
#include <algorithm>

std::string lcs(const std::string &a, const std::string &b){
	std::size_t m = b.size() + 1, n = a.size()+1;
	//начальная инициализация нулями
	std::vector<char> v(m*n);

	//строим динамику
	for(std::size_t j = 1; j < m; ++j)
		for(std::size_t i = 1; i < n; ++i)		
			v[j*n + i] = std::max(char(v[(j - 1)*n + i - 1]+(a[i - 1] == b[j - 1])), std::max(v[j*n + i - 1], v[(j - 1)*n + i]));
			
	
	/*for(std::size_t j = 0; j < m; ++j){
		for(std::size_t i = 0; i < n; ++i)
			std::cout<< std::setw(3) << int(v[j*n + i]);
		
		std::cout << std::endl;
	}*/

	//востанавливаем решение
	std::string str;
	std::size_t l = v[m*n-1],i=n-1,j=m-1;
	str.resize(l);
	
	while(l)
		if(a[i-1] == b[j-1]){
			--j;
			str[--l] = a[i-- -1];
		} else if(v[(j - 1)*n + i - 1] == l)
			(--j, --i);
		else if(v[j*n + i - 1] == l)
			--i;
		else
			--j;

	return str;
}

int main(){
	std::cout << lcs("abcdefghijklmnopq", "apcdefghijklmnobq");

	return 0;
}