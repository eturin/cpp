#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include "common.h"
#include <mutex>
#include <condition_variable>

//������ ������ ����������� �������
template<typename T>
class MyQueue{
private:
	//��������� ���
	struct Node{
		//��������
		T val;
		Node *next;
		//������������
		Node(T val) :val(val), next(nullptr){}
		//����������
		~Node(){
			delete next;
		}
	};

	//��������
	struct Node * head;   //������ �������
	struct Node * tail;   //�����  �������
	std::mutex mtx;               //�������� �������������
	std::condition_variable cond; //�������� ����������� ���������
	char status;          // 0 - ��������
	                      // 1 - ��������
	                      //-1 - �������� ������

	//����������� ��������
	MyQueue(const MyQueue &);
	MyQueue & operator=(const MyQueue &);
public:
	//������������
	MyQueue() :head(nullptr), tail(nullptr), status(0){};

	//����������
	~MyQueue(){
		delete head;
		head = tail = nullptr;
	}

	//������
	bool Push(T val){
		//����������� ���������� � ���� ������������ �������
		std::unique_lock<std::mutex> ul(mtx); //����� �����������		
		while (status == 1)
			cond.wait(ul);

		if (status == -1){
			ul.unlock(); //��������� ���������� (����������� �� ���������, �.�. ������� ��������� ����������� ������ ������������ ����)
			throw std::logic_error("Queue is terminated.");
		}else{
			//����������� ������
			status = 1;
			ul.unlock();

			try{
				if (isEmpty())
					tail = head = new Node(val);
				else
					tail = tail->next = new Node(val);
			}catch (...){
				//������ ���?
			}
			//��������� ������ � ������������ ������ ����������
			status = 0;	
			cond.notify_one();
		}
		return true;
	}
	T Pop(){
		//����������� ���������� � ���� ������������ �������
		std::unique_lock<std::mutex> ul(mtx); //����� �����������		
		do{
			while (status == 1)
				cond.wait(ul);

			if (status == -1){
				//��������� ���������� (����������� �� ���������, �.�. ������� ��������� ����������� ������ ������������ ����)
				ul.unlock(); 
				throw std::logic_error("Queue is terminated.");
			}else if (isEmpty()){
				//������� ��� �����, ��������� ����������, ������������ ���������� ���������� � ����� �����������				
				ul.unlock();
				cond.notify_one();				
				ul.lock();
			}else{
				//����������� ������
				status = 1;
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

		//��������� ������ � ������������ ������ ����������
		status = 0;
		cond.notify_one();
			
		return ret;
		
	}
	void Terminate(){
		//����������� ���������� � ���� ������������ �������
		std::unique_lock<std::mutex> ul(mtx); //����� �����������		
		while (status == 1)
			cond.wait(ul);

		if (status == -1){
			ul.unlock(); //��������� ���������� (����������� �� ���������, �.�. ������� ��������� ����������� ������ ������������ ����)
			throw std::logic_error("Queue is terminated.");
		}else{
			//������������� ������� ����������� � ��������� ����������
			status = -1;
			ul.unlock();

			//������������ ����
			cond.notify_all();
		}
	}
	bool isEmpty() const{
		return head == nullptr;
	}
};


#endif