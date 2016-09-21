#include <iostream>

/*������� ������*/
template<typename T> class Tree {
private:
	/*��������� ���*/
	struct Node {
		T val;
		Node *left, *right;
		Node(T & val):val(val), left(nullptr), right(nullptr) {}
		~Node() { delete left, right; }
	};
	/*����� ������*/
	Node * root;
	/*������*/
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
	/*������*/
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

/*��������� ������*/
template<typename T, typename P> class Treep {
private:
	/*��������� ���*/
	struct Node {
		T val,p;
		Node *left, *right;
		Node(T & val, P & p):val(val),p(p), left(nullptr), right(nullptr) {}
		~Node() { delete left, right; }
	};
	/*����� ������*/
	Node * root;
	/*������*/
	int get_high(Node * cur) const {
		if(cur == nullptr)
			return 0;
		else {
			int hl = get_high(cur->left), hr = get_high(cur->right);
			return 1 + (hl > hr ? hl : hr);
		}
	}
	void split(Node * cur,const T & val, Node *& left, Node *& right) { //����� ������ �� ����� (val) �� ���
		if(cur == nullptr)
			left = right = nullptr;                    //��� �������� � �������� ���������� ����� (����� �� �������, ��� �������� ���)
		else if(cur->val <= val) {
			split(cur->right, val, cur->right, right); //������� ������ ������ � ��������� ������ ����� ��������
			left = cur;
		} else {
			split(cur->left, val, left, cur->left);    //������� ����� ������ � ��������� ����� ����� ��������
			right = cur;
		}
	}
	Node * merge(Node * nd1, Node * nd2) { //������� ���� �������� � ����
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
	/*������*/
	void add(T & val, P & p) {
		if(root == nullptr)
			root = new Node(val, p);
		else {
			//����� �� ����� val ���� ��������� p �������� �������
			Node ** pv = &root;
			Node * cur = root;			
			while(cur->p > p) 
				if(cur->val <= val) {
					if(cur->right == nullptr) { //����� ������ ��������, �.�. ����� ��������
						cur->right = new Node(val,p); 
						return;
					} else {
						pv = &(cur->right);
						cur = cur->right;
					}
				} else if(cur->left == nullptr) { //����� ������ ��������, �.�. ����� ��������
					cur->left = new Node(val, p);
					return;
				} else {
					pv = &(cur->left);
					cur = cur->left;
				}
			//����� ������ �� �����
			Node *left, *new_nd = new Node(val, p), *right;
			split(cur, val, left, right);
			//�������� � ����
			*pv = merge(merge(left, new_nd), right);			
		}
	}
	int high() const { return get_high(root); }
};

int main() {
	/*�������*/
	Treep<int,int> tp;
	Tree<int>  tr;

	/*���������*/
	int n,x,y;
	std::cin >> n;
	while(n-- && std::cin >> x >> y) {
		tp.add(x, y);
		tr.add(x);
	}

	/*���������*/
	std::cout << tr.high() - tp.high();

	std::system("pause");
	return 0;
}