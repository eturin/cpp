#include <stdio.h>
#include <windows.h>

_declspec(dllexport) int SymbolCounter(const char *lpSource, char lpMask, int size) {
	int number = 0;
	if(lpSource == NULL)
		return 0;
	else {
		for(int i = 0; i < size; i++) {
			if(lpSource[i] == lpMask)
				number++;
		}
	}
	return number;
}

_declspec(dllexport) int Hello() {
	 printf("Hello!\n");
	 return 0;
 }
