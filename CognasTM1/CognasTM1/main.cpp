#include "Common.h"
#include "Context.h"

int main(int argc, char* argv[]){
	std::setlocale(LC_ALL, "russian"); 
	
	try{
		Context context(R"(C:\Users\etyurin\Documents\Visual Studio 2015\Projects\CognasTM1\Debug\j.json)",
						true,
					    R"(C:\Users\etyurin\Documents\Visual Studio 2015\Projects\CognasTM1\Debug\ssl\tm1ca_v2.pem)");
	}catch (std::exception &e){
		std::cerr << e.what()<<std::endl;
	}


		//DBService dbService("as-msk-a0136", R"(C:\Users\etyurin\Documents\Visual Studio 2013\ProjectsCPP\CognasTM1\Debug\ssl\tm1ca_v2.pem)");	
		//DBService dbService("as-msk-a0135", R"(C:\Users\etyurin\Documents\Visual Studio 2013\ProjectsCPP\CognasTM1\Debug\ssl\tm1ca_v2.pem)");
		
		//AdminServer adminServer("as-msk-a0134", R"(C:\Users\etyurin\Documents\Visual Studio 2013\ProjectsCPP\CognasTM1\Debug\ssl\tm1ca_v2.pem)");
		
		//adminServer.showServers();
		
		//Server server(adminServer);
		//server.connect("S2018","admin", "");
		
	
	std::system("pause");
	return 0;
}