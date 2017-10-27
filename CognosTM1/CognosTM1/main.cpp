#include "Common.h"
#include "Context.h"

int main(int argc, char* argv[]){
	std::setlocale(LC_ALL, "russian"); 
	
	/*if (initDLL()) {

		try {
			Context context(R"(C:\Users\etyurin\Documents\Visual Studio 2015\Projects\CognasTM1\Debug\j.json)",
				true,
				R"(C:\Users\etyurin\Documents\Visual Studio 2017\Projects\CognasTM1\Debug\ssl\tm1ca_v2.pem)");
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
		releasDLL();
	}*/
	{
		if(!utilities::initDLL())
			std::cerr << "Инициализация dll\n";
		else {
			AdminServer adminServer("as-msk-a0134", R"(C:\Users\etyurin\Documents\Visual Studio 2013\ProjectsCPP\cpp\CognosTM1\CognosTM1\ssl\tm1ca_v2.pem)");
			Server      server(adminServer);
			if (!server.connect("S2018", 0, "admin", 0, "", 0))
				std::cerr << "Нет соединения\n";
			else {
				Dimension dimension(server, "Период_");
				
				Subset    subset(dimension, "2011-2012");
				subset.deleteObject();
				subset.makeNewWithMDX("{TM1FILTERBYPATTERN(TM1SUBSETALL([Период_]),\"2011.*\"),\
					                    TM1FILTERBYPATTERN(TM1SUBSETALL([Период_]), \"2012.*\")}");
				subset.registerSubset();
				std::cout << subset.getCountElements();

				/*Cube cube(server,"Лимиты BDDS");
				
				View view(cube, "Лимиты_");
				view.deleteView();
				view.addColumn("ЦФО");
				view.addRow("Проект_");
				view.addRow("Подразделение_");
				
				view.addRow("Статья бюджета");
				view.addRow("Регион_");
				view.addRow("Период_");
				view.makeNew();
				view.setSuppressZeroes(true);
				view.setSkipZeroes(true);
				view.setSkipConsolidated(true);
				view.setSkipRule(true);
				view.registerView("Лимиты_");
				
				std::cout << view.show();			*/
			}
		}

	}
	std::system("pause");
	return 0;
}