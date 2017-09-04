#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include "common.h"
#include <mutex>
#include <condition_variable>

//шаблон класса реализующий очередь
template<typename T>
class MyQueue{
private:
	//служебный тип
	struct Node{
		//свойства
		T val;
		Node *next;
		//конструкторы
		Node(T val) :val(val), next(nullptr){}
		//деструктор
		~Node(){
			delete next;
		}
	};

	//свойства
	struct Node * head;   //голова очереди
	struct Node * tail;   //хвост  очереди
	std::mutex mtx;               //средство синхронизации
	std::condition_variable cond; //средство пробуждения ожидающих
	char status;          // 0 - свободен
	                      // 1 - захвачен
	                      //-1 - прервать работу

	//запрещенные операции
	MyQueue(const MyQueue &);
	MyQueue & operator=(const MyQueue &);
public:
	//конструкторы
	MyQueue() :head(nullptr), tail(nullptr), status(0){};

	//деструктор
	~MyQueue(){
		delete head;
		head = tail = nullptr;
	}

	//методы
	bool Push(T val){
		//захватываем блокировку и ждем освобождения ресурса
		std::unique_lock<std::mutex> ul(mtx); //сразу блокируется		
		while (status == 1)
			cond.wait(ul);

		if (status == -1){
			ul.unlock(); //отпускаем блокировку (нотификация не требуется, т.к. процесс вызвавший прекращение работы нотфицировал всех)
			throw std::logic_error("Queue is terminated.");
		}else{
			//захватываем ресурс
			status = 1;
			ul.unlock();

			try{
				if (isEmpty())
					tail = head = new Node(val);
				else
					tail = tail->next = new Node(val);
			}catch (...){
				//памяти нет?
			}
			//отпускаем ресурс и нотифицируем одного ожидающего
			status = 0;	
			cond.notify_one();
		}
		return true;
	}
	T Pop(){
		//захватываем блокировку и ждем освобождения ресурса
		std::unique_lock<std::mutex> ul(mtx); //сразу блокируется		
		do{
			while (status == 1)
				cond.wait(ul);

			if (status == -1){
				//отпускаем блокировку (нотификация не требуется, т.к. процесс вызвавший прекращение работы нотфицировал всех)
				ul.unlock(); 
				throw std::logic_error("Queue is terminated.");
			}else if (isEmpty()){
				//очередь еще пуста, отпускаем блокировку, нотифицируем следующего ожидающего и снова захватываем				
				ul.unlock();
				cond.notify_one();				
				ul.lock();
			}else{
				//захватываем ресурс
				status = 1;
				ul.unlock();
				break;
			}
		} while (true);		

		//обновляем голову и возвращаем хранимое в ней значение
		T ret = head->val;
		Node * tmp = head;
		head = head->next;
		tmp->next = nullptr;
		delete tmp;		

		//отпускаем ресурс и нотифицируем одного ожидающего
		status = 0;
		cond.notify_one();
			
		return ret;
		
	}
	void Terminate(){
		//захватываем блокировку и ждем освобождения ресурса
		std::unique_lock<std::mutex> ul(mtx); //сразу блокируется		
		while (status == 1)
			cond.wait(ul);

		if (status == -1){
			ul.unlock(); //отпускаем блокировку (нотификация не требуется, т.к. процесс вызвавший прекращение работы нотфицировал всех)
			throw std::logic_error("Queue is terminated.");
		}else{
			//устанавливаем признак прекращения и отпускаем блокировку
			status = -1;
			ul.unlock();

			//нотифицируем всех
			cond.notify_all();
		}
	}
	bool isEmpty() const{
		return head == nullptr;
	}
};


#endif