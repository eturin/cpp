#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>
#include <cstring>

template<typename T>
class Stack final {
private:
	//внутренний тип
	struct Node final {
		T val;           //хранимое значение
		struct Node * p; //предыдущий элемент
		Node(T& val, struct Node * p=nullptr):val(val), p(p) {}
		~Node() {
			delete p;
		}
	};
	//вершина стека
	struct Node * top_val, *top_max;
	//запрещаем копирование и присваивание
	Stack(const Stack&);
	Stack & operator=(const Stack&);
public:
	//конструктор
	Stack():top_val(nullptr), top_max(nullptr){}
	//деструкотр
	~Stack() {
		delete top_val, top_max;
		top_val = top_max = nullptr;
	}
	//методы
	const T& max() {
		return top_max->val;
	}
	void push(T& val) {
		top_val = new struct Node(val, top_val);
		T max = top_max!=nullptr ? top_max->val : val;
		max = max < val ? val : max;
		top_max = new struct Node(max, top_max);
	}
	void pop() {
		if(top_val!=nullptr && top_val->p != nullptr) {
			struct Node *tmp = top_val;
			top_val = tmp->p;
			tmp->p = nullptr;
			delete tmp;

			tmp = top_max;
			top_max = tmp->p;
			tmp->p = nullptr;
			delete tmp;
		} else {
			//удаление последнего элемента из стека
			delete top_val, top_max;
			top_val = top_max = nullptr;
		}
	}
};

int main() {
	//узнаем кол-во запросов
	int n;
	std::scanf("%d",&n);

	//разбор запросов
	Stack<int> st;
	char cmd[10];
	for(int i = 0; i < n; ++i) {
		std::scanf("%s",cmd);
		if(!std::strcmp(cmd, "push")) {
			int k;
			std::scanf("%d", &k);
			st.push(k);
		} else if(!std::strcmp(cmd, "pop"))
			st.pop();
		else
			std::printf("%d\n",st.max());
	}
	

	std::system("pause");
	return 0;
}