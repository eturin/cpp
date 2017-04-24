#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cstdio>
#include <cstring>

template<typename T>
class Stack final {
private:
	//���������� ���
	struct Node final {
		T val;           //�������� ��������
		struct Node * p; //���������� �������
		Node(T& val, struct Node * p=nullptr):val(val), p(p) {}
		~Node() {
			delete p;
		}
	};
	//������� �����
	struct Node * top_val, *top_max;
	//��������� ����������� � ������������
	Stack(const Stack&);
	Stack & operator=(const Stack&);
public:
	//�����������
	Stack():top_val(nullptr), top_max(nullptr){}
	//����������
	~Stack() {
		delete top_val, top_max;
		top_val = top_max = nullptr;
	}
	//������
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
			//�������� ���������� �������� �� �����
			delete top_val, top_max;
			top_val = top_max = nullptr;
		}
	}
};

int main() {
	//������ ���-�� ��������
	int n;
	std::scanf("%d",&n);

	//������ ��������
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