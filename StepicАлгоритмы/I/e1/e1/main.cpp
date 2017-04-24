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
			//открывающуюся скобку укладываем в стек
			st.push({c, n});		
		else {
			//определим парную скобку
			if(c == ')')
				c = '(';
			else if(c == ']')
				c = '[';
			else if(c=='}')
				c = '{';
			else {
				//мусор в строке
				continue;
			}
			if(st.empty())
				//стек пуст, значит нет парной скобки, для закрывающейся
				is_ok = false;
			else if(st.top().first != c)
				//на вершине стека другая скобка
				is_ok = false;
			else
				st.pop();			
		} 			
	}

	if(!is_ok)
		//ошибка с закрывающейся скобкой
		std::cout << n;
	else if(st.empty())
		//ошибок не найдено и стек пуст
		std::cout << "Success";
	else {
		//для открывающейся скобки нет парной
		do {
			n = st.top().second;
			st.pop();
		} while(!st.empty());
		std::cout << n;
	}


	system("pause");

	return 0;
}