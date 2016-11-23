#define _CRT_SECURE_NO_WARNINGS


#include "htab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void show(const struct  hTab * ht) {
	printf("\nSEE:\ncnt = %d\nsize = %d\n", ht->cnt, ht->size);
	for(int i = 0; i < ht->size; ++i)
		if(ht->key_val[i].key == NULL)
			printf("%d NULL\n", i);
		else
			printf("%d key=%s val=%s\n", i, ht->key_val[i].key, ht->key_val[i].val);
}

int main() {
	struct  hTab * ht=create_hTab(0);
	
	while(1) {
		char buf[2];
		printf("\nmenu\n\t1) add\n\t2) del\n\t3)get val\n\t4)get index\n:");
		gets(buf);

		
		printf("key = ");
		char * str_key = malloc(100);
		gets(str_key);


		if(!strcmp(buf, "1")) {
			printf("val = ");
			char * str_val = malloc(100);
			gets(str_val);
			add_hTab(ht, str_key, str_val);
		} else if(!strcmp(buf, "2"))
			del_hTab(ht, str_key);
		else if(!strcmp(buf, "3"))
			printf("val = %s\n", search_hTab(ht,str_key));
		else if(!strcmp(buf, "4"))
			printf("index = %d\n", key_index_htab(ht, str_key));

		show(ht);
	}

	
	ht=releas_hTab(ht);


	system("pause");
	return 0;
}