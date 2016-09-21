#include <iostream>

/*простое дерево*/
template<typename T> class Tree {
private:
	/*вложенный тип*/
	struct Node {
		T val;
		Node *left, *right;
		Node(T & val):val(val), left(nullptr), right(nullptr) {}
		~Node() { delete left, right; }
	};
	/*члены данных*/
	Node * root;
	/*методы*/
	int get_high(Node * cur) const { 
		if(cur == nullptr)
			return 0;
		else {
			int hl = get_high(cur->left), hr = get_high(cur->right);
			return 1 + (hl > hr ? hl : hr);
		}			
	}
public:
	Tree():root(nullptr) {}
	~Tree() { delete root; }
	/*методы*/
	void add(T & val) {
		if(root == nullptr)
			root = new Node(val);
		else {
			Node * cur = root;
			bool is_repeat = true;
			while(is_repeat) {
				if(cur->val < val) {
					if(cur->right == nullptr) {
						cur->right = new Node(val);
						is_repeat = false;
					} else
						cur = cur->right;
				} else if(cur->left == nullptr) {
					cur->left = new Node(val);
					is_repeat = false;
				} else
					cur = cur->left;
			}
		}
	}
	int high() const { return get_high(root); }
};

/*декартово дерево*/
template<typename T, typename P> class Treep {
private:
	/*вложенный тип*/
	struct Node {
		T val,p;
		Node *left, *right;
		Node(T & val, P & p):val(val),p(p), left(nullptr), right(nullptr) {}
		~Node() { delete left, right; }
	};
	/*члены данных*/
	Node * root;
	/*методы*/
	int get_high(Node * cur) const {
		if(cur == nullptr)
			return 0;
		else {
			int hl = get_high(cur->left), hr = get_high(cur->right);
			return 1 + (hl > hr ? hl : hr);
		}
	}
	void split(Node * cur,const T & val, Node *& left, Node *& right) { //делим дерево по ключу (val) на два
		if(cur == nullptr)
			left = right = nullptr;                    //нет деревьев и поправим переданную ветвь (здесь не изветсо, что передано уже)
		else if(cur->val <= val) {
			split(cur->right, val, cur->right, right); //вернуть правое дерево и поправить правую ветвь текущего
			left = cur;
		} else {
			split(cur->left, val, left, cur->left);    //вернуть левое дерево и поправить левую ветвь текущего
			right = cur;
		}
	}
	Node * merge(Node * nd1, Node * nd2) { //слияние двух деревьев в одно
		if(nd1 == nullptr || nd2 == nullptr)
			return (nd1 == nullptr ? nd2 : nd1);
		else if(nd1->p > nd2->p) {
			nd1->right = merge(nd1->right, nd2);
			return nd1;
		} else {
			nd2->left = merge(nd1, nd2->left);
			return nd2;
		}
	}
public:
	Treep():root(nullptr) {}
	~Treep() { delete root; }
	/*методы*/
	void add(T & val, P & p) {
		if(root == nullptr)
			root = new Node(val, p);
		else {
			//поиск по ключу val пока приоритет p остается маловат
			Node ** pv = &root;
			Node * cur = root;			
			while(cur->p > p) 
				if(cur->val <= val) {
					if(cur->right == nullptr) { //можно просто добавить, т.к. место свободно
						cur->right = new Node(val,p); 
						return;
					} else {
						pv = &(cur->right);
						cur = cur->right;
					}
				} else if(cur->left == nullptr) { //можно просто добавить, т.к. место свободно
					cur->left = new Node(val, p);
					return;
				} else {
					pv = &(cur->left);
					cur = cur->left;
				}
			//делим дерево по ключу
			Node *left, *new_nd = new Node(val, p), *right;
			split(cur, val, left, right);
			//собираем в одно
			*pv = merge(merge(left, new_nd), right);			
		}
	}
	int high() const { return get_high(root); }
};

int main() {
	/*деревья*/
	Treep<int,int> tp;
	Tree<int>  tr;

	/*заполняем*/
	int n,x,y;
	std::cin >> n;
	while(n-- && std::cin >> x >> y) {
		tp.add(x, y);
		tr.add(x);
	}

	/*результат*/
	std::cout << tr.high() - tp.high();

	std::system("pause");
	return 0;
}