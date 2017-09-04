#include "MyQueue.hpp"
#include <thread>

static const int N = 10;
static const int LEN = 20;

int main(){
	MyQueue<int> *qu = new MyQueue<int>();
	//MyQueue<int> qu2(*qu);
	//MyQueue<int> qu2;
	//qu2 = *qu;
	//MyQueue<int> qu2=std::move(*qu);

	//запускаем потоки
	std::thread * m_th[LEN];
	for (unsigned i = 0; i<LEN; ++i)
		m_th[i] = new std::thread(
		[&qu](){
			//в цикле наполнение и освобождение очереди
			try{
				while (true){
					//наполнение
					for (unsigned j = 0; j < N; ++j)
						qu->Push(j);

					try{
						//извлечение
						for (unsigned j = 0; j < N + 1; ++j)
							qu->Pop();
					}catch (std::logic_error &e){
						std::cout << e.what() << std::endl;
					}
				}
			}catch (std::logic_error &e){
				std::cout << e.what() << std::endl;
			}
	    });
	

	Sleep(10000);
	//прерываем работу очереди
	qu->Terminate();

	//присоединяем потоки назад
	for (unsigned i = 0; i<LEN; ++i){
		m_th[i]->join();
		delete m_th[i];
	}

	//освобождение
	delete qu;

	std::system("pause");
	return 0;
}
