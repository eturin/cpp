#define _CRT_SECURE_NO_WARNINGS

#include <locale>
#include <iostream>
#include <cstring>
#include <cctype>
#import "OCRServerClient3.tlb" no_namespace

class ClientRecognition{
private:
	char       *pstrServer;
	IClientPtr  ClientObject;	

	//������
	void setStrServer(const char * pstrServer){
		delete[] this->pstrServer;
		this->pstrServer = nullptr;

		if (pstrServer){						 
			this->pstrServer = new char[std::strlen(pstrServer)+1];
			std::strcpy(this->pstrServer, pstrServer);
		}
	}
public:
	//�����������
	ClientRecognition():pstrServer(nullptr){		
		std::memset(&ClientObject, 0, sizeof(ClientObject));
	}
	//����������
	~ClientRecognition(){
		disconnect();		
	}

	//������ ����������
	void disconnect(){
		delete[] this->pstrServer;
		this->pstrServer = nullptr;

		if (ClientObject != nullptr)
			ClientObject = nullptr;					
		std::memset(&ClientObject, 0, sizeof(ClientObject));		
	}
	//��������� ����������
	bool connect(const char * pstrServer){
		//�����������
		disconnect();

		//������������		
		ClientObject.CreateInstance(__uuidof(Client));	
		HRESULT hr;
		setStrServer(pstrServer);		
		if (ClientObject == nullptr) {			
			perror("�� ������� ������� ��������� ������, ��� ���������� � ABBYY Recognition Server.");			
			setStrServer(nullptr);
			return false;
		}else if (S_OK != (hr = ClientObject->Connect(this->pstrServer))){
			perror("���������� �� �����������");			
			setStrServer(nullptr);
			return false;
		}		

		return true;
	}
	//��������� ���������
	void getWorkflows()const{		
		IStringsCollectionPtr workflows = ClientObject->Workflows;
		for (int i = 0; i < workflows->Count; ++i) {
			wchar_t* workflow = workflows->Item(i);
			std::wcout << workflow << std::endl;
		}
	}
	//��������� ��������� �������
	void GetJobState(const char * strJobId)const{
		JobStateEnum State;
		long		 Progress;
		char tmp[39] = {'{',0};
		std::strcat(tmp, strJobId);
		std::strcat(tmp, "}");
		char *c = tmp;
		while (*c != '\0')
			*c++ = (char)std::toupper(*c);
		_bstr_t jobId = tmp;
		try{
			HRESULT hr = ClientObject->GetJobState(jobId, &State, &Progress);
		}catch (_com_error error) {
			std::cerr<< error.Description()<<std::endl<<error.ErrorMessage()<<std::endl;
		}
		switch (State){
		case JS_NoSuchJob:
			std::cout << "�� ������� ������� �� ��������������\n";
			break;
		case JS_Waiting:
			std::cout << "������� ������� ���������\n";
			break;
		case JS_Paused:
			std::cout << "������� ��������������, �.�. �������� ����������\n";
			break;
		case JS_Processing:
			std::cout << "������� ��������������\n";
			break;
		case JS_Verification:
			std::cout << "������� �� ������� �����������\n";
			break;
		case JS_VerificationWait:
			std::cout << "������� ������� �����������\n";
			break;
		case JS_Indexing:
			std::cout << "������� �������������\n";
			break;
		case JS_IndexingWait:
			std::cout << "������� ������� ����������\n";
			break;
		case JS_Processed:
			std::cout << "������� ����������, �� ��� �� ������������\n";
			break;
		case JS_ProcessedPaused:
			std::cout << "������� �� ����� ���� ������������\n";
			break;
		case JS_Publishing:
			std::cout << "������� ������������\n";
			break;
		default:
			std::cout << "���\n";
			break;
		}
	}
};

int main(int argc, char** argv){
	//�����������
	setlocale(LC_ALL, "russian");

	//������������� COM
	CoInitialize(NULL);

	{
		ClientRecognition cli;
		cli.connect("AS-MSK-N7060");
		cli.getWorkflows();	
		cli.GetJobState("2de0fd4c-7ad9-4ed7-8cca-b70c88919111");
		cli.GetJobState("12A1D4FC-10A8-4F08-9418-5C3A7E65E62C");
		                 
		cli.disconnect();
	}
	
	//������������ COM
	CoUninitialize();

	std::system("pause");
	return 0;
}