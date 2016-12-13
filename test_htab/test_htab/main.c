#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#include <locale.h>

#include "uthash.h"

/*��� ���-�������, ������ �������*/
struct Environment {
	char *key;                 /* ����           */
	char *val;                 /* ��������       */
	UT_hash_handle hh;         /* ��������� ���� */
};

void htab_add(struct Environment **ppEnv, const char * _key, const char * _val) {
	struct Environment *s = NULL;
	HASH_FIND_STR(*ppEnv, _key, s);
	if(s == NULL) {
		s = (struct Environment*)malloc(sizeof(struct Environment));		
		s->key = (char *)malloc(strlen(_key) + 1); 	strcpy(s->key, _key);
		s->val = (char *)malloc(strlen(_val) + 1);	strcpy(s->val, _val);
		HASH_ADD_STR(*ppEnv, key, s); /*key - ��� ���� ��������� � ������*/
	} else {
		free(s->val);
		s->val = (char *)malloc(strlen(_val) + 1); strcpy(s->val, _val);
	}
}


struct Environment * htab_find(struct Environment const **ppEnv, const char * _key) {
	struct Environment *s;
	HASH_FIND_STR(*ppEnv, _key, s);

	return s;
}

void htab_delete(struct Environment const **ppEnv, struct Environment *s) {
	HASH_DEL(*ppEnv, s);  
	free(s->key);
	free(s->val);
	free(s);
}

void htab_delete_all(struct Environment **ppEnv) {
	struct Environment *s, *tmp;

	HASH_ITER(hh, *ppEnv, s, tmp) {
		HASH_DEL(*ppEnv, s);  /* delete it (users advances to next) */
		free(s->key);
		free(s->val);
		free(s);              /* free it */
	}
}

void htab_show(struct Environment const **ppEnv) {
	for(struct Environment const * s = *ppEnv; s != NULL; s = (struct Environment const*)(s->hh.next))
		printf("key=%s val=%s\n", s->key, s->val);	
}

int fsort_by_val(struct Environment *a, struct Environment *b) {
	return strcmp(a->val, b->val);
}
int fsort_by_key(struct Environment *a, struct Environment *b) {
	return strcmp(a->key,b->key);
}
void htab_sort(struct Environment **ppEnv, int (*func)(struct Environment*,struct Environment*)) {
	HASH_SORT(*ppEnv, func);
}

int main() {
	setlocale(LC_ALL,"russian");

	/*������ ������ ���-�������*/
	struct Environment *pEnv = NULL;

	char key[100], val[100], in[10];
	int id = 1, is_repeat = 1;	

	struct Environment *s = NULL;
	
	while(is_repeat) {
		printf( " 1. ��������\n"				
				" 3. �����\n"
				" 4. �������\n"
				" 5. ������� ���\n"
				" 6. ����������� �� ��������\n"
				" 7. ����������� �� �����\n"
				" 8. ������\n"
				" 9. ����������\n"
				"10. �����\n");
		gets(in);

		switch(atoi(in)) {
			case 1:
				printf("����? ");
				gets(key);
				printf("��������? ");
				htab_add(&pEnv, key, gets(val));
				break;			
			case 3:
				printf("����? ");
				s = htab_find(&pEnv, gets(in));
				printf("val=%s\n", s ? s->val : "unknown");
				break;
			case 4:
				printf("����? ");
				s = htab_find(&pEnv, gets(in));
				if(s) 
					htab_delete(&pEnv,s);
				else 
					printf("�� ������ ����\n");
				break;
			case 5:
				htab_delete_all(&pEnv);
				break;
			case 6:
				htab_sort(&pEnv,fsort_by_val);
				break;
			case 7:
				htab_sort(&pEnv, fsort_by_key);
				break;
			case 8:
				htab_show(&pEnv);
				break;
			case 9:
				printf("����� %u ���������\n", HASH_COUNT(pEnv));
				break;
			case 10:
				is_repeat = 0;
				break;
		}
	}

	htab_delete_all(&pEnv);  /* free any structures */

	system("pause");
	return 0;
}
