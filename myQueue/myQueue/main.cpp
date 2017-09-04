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

	//��������� ������
	std::thread * m_th[LEN];
	for (unsigned i = 0; i<LEN; ++i)
		m_th[i] = new std::thread(
		[&qu](){
			//� ����� ���������� � ������������ �������
			try{
				while (true){
					//����������
					for (unsigned j = 0; j < N; ++j)
						qu->Push(j);

					try{
						//����������
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
	//��������� ������ �������
	qu->Terminate();

	//������������ ������ �����
	for (unsigned i = 0; i<LEN; ++i){
		m_th[i]->join();
		delete m_th[i];
	}

	//������������
	delete qu;

	std::system("pause");
	return 0;
}
