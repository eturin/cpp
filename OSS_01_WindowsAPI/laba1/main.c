#define WINVER 0x0601 /*API Windows 7 и выше*/
#include <Windows.h>
#include <locale.h>
#include <stdio.h>

/*подавляем ошибки из-за declared deprecated*/
#pragma warning(disable : 4996)

int show_err(const char * msg);
int main() {
	setlocale(LC_ALL, "russian");
	
	/*определяем параметры OS*/
	OSVERSIONINFO osver;
	if(GetVersionEx(&osver))
		show_err("Ошибка получения версии");
	else {

	}

	LPSTR buf[100]; //char*
	/*системный каталог*/
	if(GetSystemDirectory(buf, sizeof(buf))) {
		printf("System directory: %s\n", buf);
	} else
		show_err("Ошибка получения системного каталога");

	/*имя компьютера*/
	int len = sizeof(buf);
	if(GetComputerName(buf, &len)) {
		printf("Computer name: %s\n", buf);
	} else
		show_err("Ошибка получения имени компьютера");

	/*имя польхователя*/
	len = sizeof(buf);
	if(GetUserName(buf, &len)) {
		printf("User name: %s\n", buf);
	} else
		show_err("Ошибка получения имени пользователя");

	/*сведения о дисках*/
	printf("\nVolume inf:\n");
	HANDLE vol = FindFirstVolume(buf, sizeof(buf));
	do {
		LPSTR pathDrive[100];
		GetVolumePathNamesForVolumeName(buf, pathDrive, sizeof(pathDrive), &len);
		DWORD dwSectPerClust,
			dwBytesPerSect,
			dwFreeClusters,
			dwTotalClusters;
		GetDiskFreeSpace(pathDrive,
						 &dwSectPerClust,
						 &dwBytesPerSect,
						 &dwFreeClusters,
						 &dwTotalClusters);
		__int64 i64TotalBytes = (__int64)dwTotalClusters * dwSectPerClust * dwBytesPerSect;
		__int64 i64FreeBytes = (__int64)dwFreeClusters  * dwSectPerClust *	dwBytesPerSect;
		printf("Vlume name: %s\n\tpath: %s\n\ttotal bytes: %I64u\n\tfree bytes: %I64u\n", buf, pathDrive, i64TotalBytes, i64FreeBytes);

	} while(FindNextVolume(vol, buf, sizeof(buf)));
	FindVolumeClose(vol);

	/*список программ запускаемых при старте Windows*/
	printf("Run on the start:\n");
	HKEY hkey;
	LPSTR subKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	if(RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS){
		len = sizeof(buf);
		LPBYTE data[500];
		int data_len = sizeof(data), type=0;
		unsigned i = 0, rc;
		while(ERROR_NO_MORE_ITEMS != (rc = RegEnumValue(hkey,	  /* дескриптор запрашиваемого ключа*/
											i,		  /* индекс запрашиваемого значения*/
											buf,	  /* адрес буфера для имени значения */
											&len,	  /* адрес переменной с  размером буфера для имени значения*/
											0,	      /* зарезервировано*/
											&type,	  /* адрес переменной с типом данных */
											data,	  /* адрес буфера для данных значения*/
											&data_len /* адрес переменной с  размером буфера для данных*/))) {
			if(ERROR_SUCCESS != rc)
				show_err("err");
			else if(type==REG_SZ) //(в наддом случае, строка заканчивающаяси 0)
				printf("%s = %s\n",buf,(char*)data);
			
			//увеличим индекс
			++i;
			//Восстановим размеры
			len = sizeof(buf);
			data_len = sizeof(data);
		}		
	} else
		show_err("Ошибка получения ключа реестра");

	/*замер производительности*/
	unsigned long long tb, te;
	QueryPerformanceCounter(&tb);

	unsigned long long freq = 0;
	/*частота процессора*/
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&te);
	printf("Кол-во микросекунд: %f\n", 1000000.0*(te - tb) / freq);

	return 0;
}

int show_err(const char * msg) {
	int      no = GetLastError();
	char  str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s:\n%s\n", msg, str_err);
	else
		printf("%s:\nномер ошибки %d\n", msg, no);

	return no;
}
