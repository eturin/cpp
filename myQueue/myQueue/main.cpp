#include "MyQueue.hpp"
#include <thread>

static const int N = 100;
static const int CNT_WRITER = 1;
static const int CNT_READER = 10;

int main(){
	MyQueue<int> *qu = new MyQueue<int>();
	//MyQueue<int> qu2(*qu);
	//MyQueue<int> qu2;
	//qu2 = *qu;
	//MyQueue<int> qu2=std::move(*qu);

	//��������� ������
	std::thread * m_th[CNT_WRITER + CNT_READER] = {0};
	
	for (unsigned i = 0; i<CNT_WRITER; ++i)
		m_th[i] = new std::thread(
		[&qu](){
			//��������
			try{
				//while (true){
					//����������
					for (int j = 0; j < N; ++j)
						qu->Push(j);					
				//}
			}catch (std::logic_error &e){
				std::cout << e.what() << std::endl;
			}
	    });

	for (unsigned i = CNT_WRITER; i<CNT_WRITER+CNT_READER; ++i)
		m_th[i] = new std::thread(
		[&qu](){
		//��������
		try{
			while (true){
				//����������
				for (unsigned j = 0; j < N; ++j)
					std::cout<<qu->Pop()<<std::endl;				
			}
		}catch (std::logic_error &e){
			std::cout << e.what() << std::endl;
		}
	});
	

	Sleep(10000);
	//��������� ������ �������
	qu->Terminate();

	//������������ ������ �����
	for (unsigned i = 0; i<CNT_WRITER + CNT_READER; ++i){
		m_th[i]->join();
		delete m_th[i];
	}

	//������������

	delete qu;

	std::system("pause");
	return 0;
}
