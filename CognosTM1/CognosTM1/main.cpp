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
				Cube cube(server, "}ClientGroups");
				std::cout << cube.getCountDimensions();
				View view(cube, "mdx_View_last9", 0, true, true);
				view.deleteObject();
				view.makeNewWithMDX(R"(SELECT                                          
                                            { [}Clients].[Admin] }  ON COLUMNS, 
                                            { [}Groups].[ADMIN] }  ON ROWS
                                       FROM [}ClientGroups]                                       
									)");
				view.setSkipConsolidated(true,view.gethNewObject());
				view.setSkipRule(true, view.gethNewObject());
				view.setSkipZeroes(true, view.gethNewObject());
				view.setSuppressZeroes(true, view.gethNewObject());

				view.registerView(true);
				std::cout << view.show(true);

				{   //поиск MDX-view на сервере
					for (TM1_INDEX j = 1, len = server.getCountCubes(); j <= len; ++j) {
						Cube cube(server, j);
						cube.uploadName();
						for (TM1_INDEX i = 1, cnt = cube.getCountViews(true,true); i <= cnt; ++i) {
							View view(cube, i,true);
							view.uploadName();
							std::cout << cube.getName() << " --> " << view.getName() << std::endl;
						}
					}
				}
				
			}
		}

	}
	std::system("pause");
	return 0;
}