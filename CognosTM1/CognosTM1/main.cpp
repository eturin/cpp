#include "Common.h"
#include "Context.h"

int main(int argc, char* argv[]){
	std::setlocale(LC_ALL, "russian"); 
	
	if (initDLL()) {

		try {
			Context context(R"(C:\Users\etyurin\Documents\Visual Studio 2015\Projects\CognasTM1\Debug\j.json)",
				true,
				R"(C:\Users\etyurin\Documents\Visual Studio 2015\Projects\CognasTM1\Debug\ssl\tm1ca_v2.pem)");
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
		releasDLL();
	}
	
	std::system("pause");
	return 0;
}