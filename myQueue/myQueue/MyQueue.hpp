#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include "common.h"
#include <mutex>
#include <condition_variable>

//������ ������ ����������� �������
template<typename T>
class MyQueue{
private:
	//��������� ����
	enum Status{ Terminated=-1, Free, Lock };
	struct Node{
		//��������
		T val;
		Node *next;
		//������������
		Node(T & val) :val(val), next(nullptr){}
		//����������
		~Node(){
			delete next;
		}
	};

	//��������
	struct Node * head;   //������ �������
	struct Node * tail;   //�����  �������
	std::size_t   cnt;    //���-�� ��������� � �������
	std::mutex mtx;                      //�������� �������������
	std::condition_variable cond_writer; //�������� ����������� ��������� ���������
	std::condition_variable cond_reader; //�������� ����������� ��������� ���������
	Status status;          

	//����������� ��������
	MyQueue(const MyQueue &);
	MyQueue & operator=(const MyQueue &);
	//������
	bool isEmpty() const{
		return head == nullptr;
	}
public:
	//������������
	MyQueue() :head(nullptr), tail(nullptr), status(Free),cnt(0){};

	//����������
	~MyQueue(){	
		//���� ������� ������� ������ ������ ����� delete head, �� ������������� ���� �������, �.�. �� ������� ����������� (Node) ���������� ��������� ���������� (Node)
		while (head!=nullptr){
			Node * tmp = head;
			head = head->next;
			tmp->next = nullptr;
			delete tmp;
			--cnt;
		}
		
		head = tail = nullptr;
	}

	//������
	bool Push(T & val){
		//����������� ���������� � ���� ������������ �������
		std::unique_lock<std::mutex> ul(mtx); //����� �����������		
		while (status == Lock)
			cond_writer.wait(ul);

		if (status == Terminated){
			ul.unlock(); //��������� ���������� (����������� �� ���������, �.�. ������� ��������� ����������� ������ ������������ ����)
			throw std::logic_error("Queue is terminated.");
		}else{
			//����������� ������
			status = Lock;
			ul.unlock();

			try{
				if (isEmpty())
					tail = head = new Node(val);
				else
					tail = tail->next = new Node(val);
				++cnt;
			}catch (...){
				//������ ���?
			}
			//��������� ������ � ������������ ������ ����������
			status = Free;	
			
			cond_reader.notify_one(); //�������� ������ ��������, ��� �� ����� ������
			cond_writer.notify_one(); //�������� ������ ��������, ��� �� ����� ������
		}
		return true;
	}
	T Pop(){
		//����������� ���������� � ���� ������������ �������
		std::unique_lock<std::mutex> ul(mtx); //����� �����������		
		do{
			while (status == Lock)
				cond_reader.wait(ul);

			if (status == Terminated){
				//��������� ���������� (����������� �� ���������, �.�. ������� ��������� ����������� ������ ������������ ����)
				ul.unlock(); 
				throw std::logic_error("Queue is terminated.");
			}else if (isEmpty()){
				//������� ��� �����, ��������� ����������, ������������ ���������� ���������� � ����� �����������								
				cond_writer.notify_one();								
				cond_reader.wait(ul);
			}else{
				//����������� ������
				status = Lock;
				ul.unlock();
				break;
			}
		} while (true);		

		//��������� ������ � ���������� �������� � ��� ��������
		T ret = head->val;
		Node * tmp = head;
		head = head->next;
		tmp->next = nullptr;
		delete tmp;		
		--cnt;		
		bool okReader = !isEmpty();
		
		//��������� ������ 
		status = Free;
		
		//������������ �����������
		if (okReader) 
			cond_reader.notify_one(); //�������� ������ ��������, ��� �� ����� ������, ���� � ������� ���� �������� (���� �������� �� ����� ���������)
		
		cond_writer.notify_one(); //�������� ������ ��������, ��� �� ����� ������ 
			
		return ret;
		
	}
	void Terminate(){
		//����������� ���������� � ���� ������������ �������
		std::unique_lock<std::mutex> ul(mtx); //����� �����������		
		while (status == Lock)	; //���� � ����������� �����������

		if (status == Terminated){
			ul.unlock(); //��������� ���������� (����������� �� ���������, �.�. ������� ��������� ����������� ������ ������������ ����)
			throw std::logic_error("Queue is terminated.");
		}else{
			//������������� ������� ����������� � ��������� ����������
			status = Terminated;
			ul.unlock();

			//������������ ����
			cond_writer.notify_all();
			cond_reader.notify_all();
		}
	}
	
};


#endif