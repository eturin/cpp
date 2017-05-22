#include <iostream>

template<typename T> class SplayTree{
public:
	/*хранимый тип*/
	struct Node{
		/*хранимое значение*/
		T	val;
		/*высота поддерева с вершиной в этом узле*/
		std::size_t hight;
		/*кол-во элементов в поддерева с вершиной в этом узле*/
		std::size_t cnt;
		/*указатели на родителя, а также левое и правое поддеревья*/
		struct Node *parrent, *left, *right;
		/*конструкторы*/
		Node(T & val, struct Node *parrent = nullptr) :val(val), parrent(parrent), left(nullptr), right(nullptr),hight(1),cnt(1){}
		Node(const struct Node & other){
			//ссылка на родителя не копируется
			val = other.val;
			cnt = other.cnt;
			hight = other.hight;
			left = new struct Node(*other.left);
			right = new struct Node(*other.right);
		}
		/*операции*/
		struct Node & operator=(const struct Node & other){
			if (this != &other){
				//ссылка на родителя не копируется
				val = other.val;
				cnt = other.cnt;
				hight = other.hight;
				left = new struct Node(*other.left);
				right = new struct Node(*other.right);
			}

			return *this;
		}
		/*деструктор*/
		~Node(){
			delete left, right;
			left = right = nullptr;
		}
		/*методы*/
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
			/*обновление служебных полей*/
			cnt = 1 + cnt_getLeft() + cnt_getRight();
			hight = hight_getLeft();
			std::size_t tmp = hight_getRight();
			hight = 1 + (hight >= tmp ? hight : tmp);
		}
		/*друзья*/
		friend std::ostream & operator<<(std::ostream & out, const Node & other){
			//in-order порядок
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
	/*закрытые методы*/
	void splay(struct Node * b){
		if (b != nullptr){
			while (b->parrent != nullptr){
				struct Node * p = b->parrent->parrent, *a = b->parrent;
				if (a->right == b){
					//малый правый поворот				
					a->right = b->left;
					if (a->right != nullptr)
						a->right->parrent = a;
					a->update();

					b->left = a;
					a->parrent = b;
					b->update();
				}
				else{
					//малый левый поворот
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

			//корректировка корня
			head = b;
		}
	}
public:
	/*конструкторы*/
	SplayTree():head(nullptr){}
	/*деструктор*/
	~SplayTree(){
		delete head;
		head = nullptr;
	}
	/*методы*/
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
		
		//найдем левый узел
		struct Node * l = (*this)[i];
		//отрежем все, что левее левого
		struct Node * ll = l->left;
		if (ll != nullptr)
			ll->parrent = nullptr;		
		l->left = nullptr;
		l->update();

		//найдем правый узел
		head = l;
		struct Node * r = (*this)[j-i];
		//отрежем все, что правее правого
		struct Node * rr = r->right;
		if (rr != nullptr)
			rr->parrent = nullptr;
		r->right = nullptr;
		r->update();
		
		//объединим отрезанные левый и правый фрагменты		
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
		
		//найдем узел, после которого требуется встарка
		if (k > 0){
			head = ll;
			struct Node *c = (*this)[k - 1];
			//вставляем центральную часть после k-ого
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
	/*операторы*/
	struct Node * operator[](std::size_t i){
		struct Node * cur=head;
		while(cur != nullptr){
			if (cur->cnt_getLeft() == i)
				//текущий узел как раз и будет i-ым
				break;
			else if (cur->cnt_getLeft() < i){
				//слева не достаточно узлов
				i -= cur->cnt_getLeft() + 1;
				cur = cur->right;
			}else
				//i-ый узел где-то слева
				cur = cur->left;			
		}
		//поднимаем запрошенную вершину в корень
		splay(cur);
		return cur;
	}
	/*друзья*/
	friend std::ostream & operator<<(std::ostream & out, const SplayTree & other){
		if (other.head != nullptr)
			out << *other.head << std::endl;
		
		return out;
	}
};


int main(){
	SplayTree<char> splTree;

	//забираем строку по символам
	char c;
	while ((c = std::cin.get()) != '\n')
		splTree.add(c);
		
	//забираем кол-во команд
	unsigned q=0;
	std::cin >> q;
	//забираем команды
	while (q){
		std::size_t i, j, k;
		std::cin >> i >> j >> k;
		//переставляем
		splTree.replace(i, j, k);
		//печатаем
		//std::cout << splTree;
		--q;
	}
	//печатаем
	std::cout << splTree;

	std::system("pause");
	return 0;
}