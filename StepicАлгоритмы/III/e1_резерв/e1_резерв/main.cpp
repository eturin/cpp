#include <iostream>
#include <map>

#include <string>
#include <cstring>
#include <algorithm> 
#include <functional> 
#include <cctype>

#include <ctime>
#include <cstdlib>



int main() {
	//�������� ���������� ��������
	unsigned n;
	std::cin >> n;

	//������� ���-�������
	std::map<size_t, std::string> ht;

	//��������� ��������
	std::string str;
	size_t num;
	while(/*n &&*/ std::cin >> str >> num) {
		//����������� �������� � ������ �������
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if(str == "add") {
			//����������			
			std::getline(std::cin, str);
			//������� ������� �����
			str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			//������� ������� ������
			str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
			//����� ������ ������ 15 ��������
			std::string tmp = str.substr(0, 15);
			
			auto it = ht.find(num);
			if(it != ht.end())
				ht.erase(num);

			ht.insert({num, tmp});
		} else if(str == "find") {
			//����			
			auto it = ht.find(num);			
			if(it == ht.end())
				std::cout << "not found\n";
			else
				std::cout << it->second << std::endl;
		} else {
			//�������						
			ht.erase(num);
		}
		--n;
	}

	std::system("pause");
	return 0;
}