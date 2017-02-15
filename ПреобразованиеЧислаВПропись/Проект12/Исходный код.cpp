#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

char * place[] = {"", " thousand ", " million ", " billion ", " trillion "};
char * _1_9[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
char * _10_19[] = {"ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
char * _20_90[] = {"twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};

void spell_number(int n, int poz) {
	/*знак*/
	if(n < 0) {
		printf("minus ");
		n = -n;
	}

	/*преобразуем старшие разряды числа*/
	if(n / 1000)
		spell_number(n / 1000, poz + 1);
	
	/*преобразуем сотни в слова*/
	n%=1000;//сотни
	if(n / 100) 
		printf("%s hundred ", _1_9[n / 100 - 1]);

	/*преобразуем десятки в слова (или сразу единицы)*/
	n %= 100;
	if(n / 10) {
		if(n < 20)
			printf("%s", _10_19[n - 10]);
		else {
			printf("%s", _20_90[n / 10 - 2]);
			if(n % 10)
				printf(" %s", _1_9[n % 10 - 1]);
		}
	}else
		printf("%s", _1_9[n - 1]);
	
	if(poz<sizeof(place))
		printf("%s",place[poz]);
	else
		printf(" err ");
}


int main() {
	int n;
	if(scanf("%d", &n)) {
		spell_number(n,0);
	}

	return 0;
}

