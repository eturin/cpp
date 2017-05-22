#include <iostream>

template<typename T> class SplayTree{
public:
	/*�������� ���*/
	struct Node{
		/*�������� ��������*/
		T	val;
		/*������ ��������� � �������� � ���� ����*/
		std::size_t hight;
		/*���-�� ��������� � ��������� � �������� � ���� ����*/
		std::size_t cnt;
		/*��������� �� ��������, � ����� ����� � ������ ����������*/
		struct Node *parrent, *left, *right;
		/*������������*/
		Node(T & val, struct Node *parrent = nullptr) :val(val), parrent(parrent), left(nullptr), right(nullptr),hight(1),cnt(1){}
		Node(const struct Node & other){
			//������ �� �������� �� ����������
			val = other.val;
			cnt = other.cnt;
			hight = other.hight;
			left = new struct Node(*other.left);
			right = new struct Node(*other.right);
		}
		/*��������*/
		struct Node & operator=(const struct Node & other){
			if (this != &other){
				//������ �� �������� �� ����������
				val = other.val;
				cnt = other.cnt;
				hight = other.hight;
				left = new struct Node(*other.left);
				right = new struct Node(*other.right);
			}

			return *this;
		}
		/*����������*/
		~Node(){
			delete left, right;
			left = right = nullptr;
		}
		/*������*/
		std::size_t cnt_getLeft() const{
			return (left==nullptr ? 0 :left->cnt);
		}
		std::size_t cnt_getRight() const{
			return (right == nullptr ? 0 : right->cnt);
		}
		std::size_t hight_getLeft() const{
			return (left == nullptr ? 0 : left->hight);
		}
		std::size_t hight_getRight() const{
			return (right == nullptr ? 0 : right->hight);
		}
		void update(){
			/*���������� ��������� �����*/
			cnt = 1 + cnt_getLeft() + cnt_getRight();
			hight = hight_getLeft();
			std::size_t tmp = hight_getRight();
			hight = 1 + (hight >= tmp ? hight : tmp);
		}
		/*������*/
		friend std::ostream & operator<<(std::ostream & out, const Node & other){
			//in-order �������
			if (other.left != nullptr)
				out << *other.left;
			out << other.val;
			if(other.right!=nullptr)
				out << *other.right;
			return out;
		}
	};
private:
	struct Node *head;
	/*�������� ������*/
	void splay(struct Node * b){
		if (b != nullptr){
			while (b->parrent != nullptr){
				struct Node * p = b->parrent->parrent, *a = b->parrent;
				if (a->right == b){
					//����� ������ �������				
					a->right = b->left;
					if (a->right != nullptr)
						a->right->parrent = a;
					a->update();

					b->left = a;
					a->parrent = b;
					b->update();
				}
				else{
					//����� ����� �������
					a->left = b->right;
					if (a->left != nullptr)
						a->left->parrent = a;
					a->update();

					b->right = a;
					a->parrent = b;
					b->update();
				}

				if (p != nullptr){
					if (p->right == a)
						p->right = b;
					else
						p->left = b;
				}
				b->parrent = p;
			}

			//������������� �����
			head = b;
		}
	}
public:
	/*������������*/
	SplayTree():head(nullptr){}
	/*����������*/
	~SplayTree(){
		delete head;
		head = nullptr;
	}
	/*������*/
	void add(T & val){
		struct Node  *cur= new struct Node(val);
		if (head == nullptr)
			head = cur;
		else{
			head->parrent = cur;
			cur->left   = head;
			cur->cnt   += head->cnt;
			cur->hight += head->hight;
			head = cur;			
		}
	}
	void replace(std::size_t i, std::size_t j, std::size_t k){
		
		//������ ����� ����
		struct Node * l = (*this)[i];
		//������� ���, ��� ����� ������
		struct Node * ll = l->left;
		if (ll != nullptr)
			ll->parrent = nullptr;		
		l->left = nullptr;
		l->update();

		//������ ������ ����
		head = l;
		struct Node * r = (*this)[j-i];
		//������� ���, ��� ������ �������
		struct Node * rr = r->right;
		if (rr != nullptr)
			rr->parrent = nullptr;
		r->right = nullptr;
		r->update();
		
		//��������� ���������� ����� � ������ ���������		
		if (ll != nullptr){
			struct Node * c = ll;
			while (c->right != nullptr)
				c = c->right;
			c->right = rr;
			if (rr != nullptr)
				rr->parrent = c;
			do{
				c->update();
				c = c->parrent;
			} while (c != nullptr);
		}else
			ll = rr;
		
		//������ ����, ����� �������� ��������� �������
		if (k > 0){
			head = ll;
			struct Node *c = (*this)[k - 1];
			//��������� ����������� ����� ����� k-���
			r->right = c->right;
			if (r->right != nullptr)
				r->right->parrent = r;
			r->update();
			c->right = r;
			r->parrent = c;
			c->update();
		}else{
			head = r;
			r->right = ll;
			if (ll != nullptr)
				ll->parrent = r;
			r->update();
		}
	}
	/*���������*/
	struct Node * operator[](std::size_t i){
		struct Node * cur=head;
		while(cur != nullptr){
			if (cur->cnt_getLeft() == i)
				//������� ���� ��� ��� � ����� i-��
				break;
			else if (cur->cnt_getLeft() < i){
				//����� �� ���������� �����
				i -= cur->cnt_getLeft() + 1;
				cur = cur->right;
			}else
				//i-�� ���� ���-�� �����
				cur = cur->left;			
		}
		//��������� ����������� ������� � ������
		splay(cur);
		return cur;
	}
	/*������*/
	friend std::ostream & operator<<(std::ostream & out, const SplayTree & other){
		if (other.head != nullptr)
			out << *other.head << std::endl;
		
		return out;
	}
};


int main(){
	SplayTree<char> splTree;

	//�������� ������ �� ��������
	char c;
	while ((c = std::cin.get()) != '\n')
		splTree.add(c);
		
	//�������� ���-�� ������
	unsigned q=0;
	std::cin >> q;
	//�������� �������
	while (q){
		std::size_t i, j, k;
		std::cin >> i >> j >> k;
		//������������
		splTree.replace(i, j, k);
		//��������
		//std::cout << splTree;
		--q;
	}
	//��������
	std::cout << splTree;

	std::system("pause");
	return 0;
}