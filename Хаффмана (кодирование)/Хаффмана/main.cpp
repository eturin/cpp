/*�� ������ �������� ������ ss ����� �� ����� 10^4, ��������� �� �������� ���� ���������� ��������, ��������� ����������� ������������� ���. � ������ ������ �������� ���������� ��������� ���� kk, ������������� � ������, � ������ ������������ �������������� ������. � ��������� kk ������� �������� ���� ���� � ������� "letter: code". � ��������� ������ �������� �������������� ������.*/
#include <iostream>
#include <string>
#include <map>
#include <vector>

class Node {
public:
	const char c;
	unsigned long long cnt;
	Node * zero, *one;

	Node(unsigned long long cnt, const char c, Node * zero = NULL, Node * one = NULL):cnt(cnt), c(c), zero(zero), one(one) {};
	~Node() {
		delete zero, one;
	}
};

unsigned long long fill(std::map<const char, std::vector<bool>> & map_char, Node * tree, std::vector<bool> path);

int main() {
	//���������� ������
	std::string str;
	std::getline(std::cin, str);

	//������ ����� ������
	std::map<const char, unsigned long long> mp;
	for(size_t i = 0; i<str.length(); ++i)
		mp[str[i]] += 1;
	//���������� � map
	std::multimap<unsigned long long, Node *> mp_tree;
	for(auto x = mp.begin(); x != mp.end(); ++x)
		mp_tree.insert(std::pair<unsigned int, Node *>(x->second, new Node(x->second, x->first)));
	mp.clear();
	//������ ������
	if(mp_tree.size() == 1) {
		Node * fst = mp_tree.begin()->second;
		Node * nd = new Node(fst->cnt, 0, fst);
		//������� ������ � ��������� �����, �� � ������ �����, ���� ����� ���� ���������
		mp_tree.erase(mp_tree.begin());
		mp_tree.insert(std::pair<unsigned int, Node *>(nd->cnt, nd));
	} else
		while(mp_tree.size()>1) {
			//����� ������ ��� � ���������� � ����
			Node * fst = mp_tree.begin()->second;
			Node * snd = (++mp_tree.begin())->second;
			Node * nd = new Node(fst->cnt + snd->cnt, 0, fst, snd);
			//������� ������ ��� � ��������� ������� ����
			mp_tree.erase(mp_tree.begin());
			mp_tree.erase(mp_tree.begin());
			mp_tree.insert(std::pair<unsigned int, Node *>(nd->cnt, nd));
		}
	Node * tree = mp_tree.begin()->second;
	mp_tree.clear();
	//�������� ����� �� ������
	std::map<const char, std::vector<bool>> map_char;
	std::vector<bool> path;
	unsigned long long len = fill(map_char, tree, path);

	//����� ���������
	std::cout << map_char.size() << " " << len << std::endl;
	//�����
	for(auto x = map_char.begin(); x != map_char.end(); ++x) {
		std::cout << x->first << ": ";
		for(size_t j = 0; j<x->second.size(); ++j)
			std::cout << x->second[j];
		std::cout << std::endl;
	}
	//�������������� ������
	for(size_t i = 0; i<str.length(); ++i)
		for(size_t j = 0; j<map_char[str[i]].size(); ++j)
			std::cout << map_char[str[i]][j];

	system("pause");
	return 0;
}

unsigned long long fill(std::map<const char, std::vector<bool>> & map_char, Node * tree, std::vector<bool> path) {
	unsigned long long cnt = 0;
	if(tree->zero == NULL && tree->one == NULL) {
		map_char[tree->c] = path;
		cnt = tree->cnt*path.size();
	} else {
		if(tree->zero != NULL) {
			path.push_back(false);
			cnt += fill(map_char, tree->zero, path);
			path.pop_back();
		}

		if(tree->one != NULL) {
			path.push_back(true);
			cnt += fill(map_char, tree->one, path);
			path.pop_back();
		}
	}

	return cnt;
}