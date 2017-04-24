#include <stack>
#include <iostream>

int main() {
	std::stack<std::pair<char,unsigned>> st;
	char c;
	unsigned n = 0;	
	bool is_ok = true;
	while(is_ok && std::cin >> c) {
		++n;
		if(c == '(' || c == '[' || c == '{') 
			//������������� ������ ���������� � ����
			st.push({c, n});		
		else {
			//��������� ������ ������
			if(c == ')')
				c = '(';
			else if(c == ']')
				c = '[';
			else if(c=='}')
				c = '{';
			else {
				//����� � ������
				continue;
			}
			if(st.empty())
				//���� ����, ������ ��� ������ ������, ��� �������������
				is_ok = false;
			else if(st.top().first != c)
				//�� ������� ����� ������ ������
				is_ok = false;
			else
				st.pop();			
		} 			
	}

	if(!is_ok)
		//������ � ������������� �������
		std::cout << n;
	else if(st.empty())
		//������ �� ������� � ���� ����
		std::cout << "Success";
	else {
		//��� ������������� ������ ��� ������
		do {
			n = st.top().second;
			st.pop();
		} while(!st.empty());
		std::cout << n;
	}


	system("pause");

	return 0;
}