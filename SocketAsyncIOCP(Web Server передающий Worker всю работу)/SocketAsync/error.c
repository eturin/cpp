#include "error.h" 

int show_err_wsa(const char * msg) {
	int      no = WSAGetLastError();
	char     str_err[10000] = {0};
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
		printf("%s: %d\n%s\n", msg, no, str_err);
	else
		printf("%s:\nномер ошибки %d\n", msg, no);

	return no;
}

int show_err(const char * msg, BOOL check_err) {
	int      no = 0;
	if(check_err) {
		no = GetLastError();
		char     str_err[10000] = {0};
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, no, MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), str_err, sizeof(str_err), NULL))
			printf("%s: %d\n%s\n", msg, no, str_err);
		else
			printf("%s:\nномер ошибки %d\n", msg, no);
	}else
		printf("%s\n", msg);

	return no;
}
