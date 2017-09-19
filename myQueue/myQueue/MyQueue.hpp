#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include "common.h"
#include <mutex>
#include <condition_variable>

//шаблон класса реализующий очередь
template<typename T>
class MyQueue{
private:
	//служебные типы
	enum Status{ Terminated=-1, Free, Lock };
	struct Node{
		//свойства
		T val;
		Node *next;
		//конструкторы
		Node(T & val) :val(val), next(nullptr){}
		//деструктор
		~Node(){
			delete next;
		}
	};

	//свойства
	struct Node * head;   //голова очереди
	struct Node * tail;   //хвост  очереди
	std::size_t   cnt;    //кол-во элементов в очереди
	std::mutex mtx;                      //средство синхронизации
	std::condition_variable cond_writer; //средство пробуждения ожидающих писателей
	std::condition_variable cond_reader; //средство пробуждения ожидающих писателей
	Status status;          

	//запрещенные операции
	MyQueue(const MyQueue &);
	MyQueue & operator=(const MyQueue &);
	//методы
	bool isEmpty() const{
		return head == nullptr;
	}
public:
	//конструкторы
	MyQueue() :head(nullptr), tail(nullptr), status(Free),cnt(0){};

	//деструктор
	~MyQueue(){	
		//если удалять большой список просто через delete head, то переполняется стек вызовов, т.к. из каждого деструктора (Node) вызывается следующий деструктор (Node)
		while (head!=nullptr){
			Node * tmp = head;
			head = head->next;
			tmp->next = nullptr;
			delete tmp;
			--cnt;
		}
		
		head = tail = nullptr;
	}

	//методы
	bool Push(T & val){
		//захватываем блокировку и ждем освобождения ресурса
		std::unique_lock<std::mutex> ul(mtx); //сразу блокируется		
		while (status == Lock)
			cond_writer.wait(ul);

		if (status == Terminated){
			ul.unlock(); //отпускаем блокировку (нотификация не требуется, т.к. процесс вызвавший прекращение работы нотфицировал всех)
			throw std::logic_error("Queue is terminated.");
		}else{
			//захватываем ресурс
			status = Lock;
			ul.unlock();

			try{
				if (isEmpty())
					tail = head = new Node(val);
				else
					tail = tail->next = new Node(val);
				++cnt;
			}catch (...){
				//памяти нет?
			}
			//отпускаем ресурс и нотифицируем одного ожидающего
			status = Free;	
			
			cond_reader.notify_one(); //сообщаем одному читателю, что он может читать
			cond_writer.notify_one(); //сообщаем одному писателю, что он может писать
		}
		return true;
	}
	T Pop(){
		//захватываем блокировку и ждем освобождения ресурса
		std::unique_lock<std::mutex> ul(mtx); //сразу блокируется		
		do{
			while (status == Lock)
				cond_reader.wait(ul);

			if (status == Terminated){
				//отпускаем блокировку (нотификация не требуется, т.к. процесс вызвавший прекращение работы нотфицировал всех)
				ul.unlock(); 
				throw std::logic_error("Queue is terminated.");
			}else if (isEmpty()){
				//очередь еще пуста, отпускаем блокировку, нотифицируем следующего ожидающего и снова захватываем								
				cond_writer.notify_one();								
				cond_reader.wait(ul);
			}else{
				//захватываем ресурс
				status = Lock;
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
		--cnt;		
		bool okReader = !isEmpty();
		
		//отпускаем ресурс 
		status = Free;
		
		//нотифицируем ожидающющих
		if (okReader) 
			cond_reader.notify_one(); //сообщаем одному читателю, что он может читать, если в очереди есть элементы (чтоб читатели не грели процессор)
		
		cond_writer.notify_one(); //сообщаем одному писателю, что он может писать 
			
		return ret;
		
	}
	void Terminate(){
		//захватываем блокировку и ждем освобождения ресурса
		std::unique_lock<std::mutex> ul(mtx); //сразу блокируется		
		while (status == Lock)	; //ждем с захваченной блокировкой

		if (status == Terminated){
			ul.unlock(); //отпускаем блокировку (нотификация не требуется, т.к. процесс вызвавший прекращение работы нотфицировал всех)
			throw std::logic_error("Queue is terminated.");
		}else{
			//устанавливаем признак прекращения и отпускаем блокировку
			status = Terminated;
			ul.unlock();

			//нотифицируем всех
			cond_writer.notify_all();
			cond_reader.notify_all();
		}
	}
	
};


#endif