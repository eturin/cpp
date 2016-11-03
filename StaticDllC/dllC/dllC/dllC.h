#ifndef dllC
#define dllC

extern "C" _declspec(dllexport) int SymbolCounter(const char *, char, int);
extern "C" _declspec(dllexport) int Hello();

#endif