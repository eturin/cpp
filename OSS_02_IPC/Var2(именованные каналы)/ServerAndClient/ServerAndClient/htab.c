#include "htab.h"
#include <stdlib.h>


void resize_htab(struct hTab *htab);

/*������������ ��������� ������*/
void free_KeyVal(struct KeyVal * p) {
	free(p->key);
	p->key = NULL;

	free(p->val);
	p->val = NULL;
}

/*����������� ������� (��������� ������� ������, ��� ������������� ���������� �������)*/
struct hTab * create_hTab(size_t pow/*������� 2*/) {
	/*�������� ����� ��� �������*/
	struct hTab * htab = malloc(sizeof(struct hTab));	

	/*�������� ����� ��� ������, �������� ��������*/
	htab->size = 1 << (pow ? pow : 3);
	htab->key_val = malloc(htab->size*sizeof(struct KeyVal));
	memset(htab->key_val, 0, htab->size*sizeof(struct KeyVal));

	/*��������� ���� ���*/
	htab->cnt = 0;

	return htab;
}

/*���������� �������*/
struct hTab * releas_hTab(struct hTab * htab) {
	for(rsize_t i = 0; i < htab->size; ++i)
		free_KeyVal(&htab->key_val[i]);
	htab->cnt = 0;
	free(htab->key_val);
	htab->size = 0;
	free(htab);

	return NULL;
}

/*������� �������� � ���������� ���� (����� ����� � ������� ������ � ����� ��������� ��������� ���)*/
size_t hfunc_prob(size_t k, size_t prev, size_t size) {
	return (prev + k) % size;
}

/*��� ������� �� ������ (���������� ������ � �������)*/
size_t hfunc(const char * str, /*���������� ������*/
			 size_t k,         /*���������� ����*/
			 size_t a,         /*������������ �����������*/
			 size_t size       /*������ ���-�������*/) {
	//������ ��� �� ������
	size_t rez = *str % size;
	while(*++str!=0)
		rez = (rez*a + *str) % size;

	//���� ������������
	for(size_t i = 1; i < k; ++i)
		rez = hfunc_prob(i, rez, size);

	return rez;
}

/*����� �������� � ���-������� �� �����*/
char * search_hTab(const struct hTab * htab, const char * str_key) {
	int k = 0;
	//��������� ��� ������
	int rez = hfunc(str_key, 0, 3, htab->size);
		
	//������������
	while(k+1 < htab->size && (NULL == htab->key_val[rez].key             //��� �������� (���� ������, �.�. �� ��� ���� ������)
		                      || strcmp(htab->key_val[rez].key, str_key)) //������� ����� ������ ������ (���� ������)
		  )
		rez = hfunc_prob(++k, rez,htab->size);

	//��������
	if(NULL == htab->key_val[rez].key)
		return NULL;                   //�� �������	
	else if(strcmp(htab->key_val[rez].key, str_key))
		return NULL;                   //������ ������ ���������
	else 
		return htab->key_val[rez].val; //���������� �������� �� �����
}

/*����� ������� � ���-������� �� �����*/
size_t key_index_htab(const struct hTab * htab, const char * str_key) {
	int k = 0;
	//��������� ��� ������
	int rez = hfunc(str_key, 0, 3, htab->size);

	//������������
	while(k+1 < htab->size 
		  && NULL != htab->key_val[rez].key && strcmp(htab->key_val[rez].key, str_key) //������� ����� ������ ������ (���� ������ ����� ������)
		)
		rez = hfunc_prob(++k, rez, htab->size);

	//��������
	if(NULL == htab->key_val[rez].key
	   || !strcmp(htab->key_val[rez].key, str_key))
		return rez;	//������� ���������� �����	
	else 
		return htab->size; //�� �������
}

/*�������� ��������*/
int del_hTab(struct hTab * htab, const char * str_key) {
	size_t rez = key_index_htab(htab,str_key);
	if(rez == htab->size)
		return 0;
	else if(htab->key_val[rez].key==NULL)
		return 0;
	else {
		free_KeyVal(&htab->key_val[rez]);
		htab->cnt -= 1;
		return 1;
	}
}

/*���������� ��������*/
int add_hTab(struct hTab * htab, const char *str_key, char *str_val) {
	if(3.0 / 4 < 1.0 * htab->cnt / htab->size)
		resize_htab(htab);

	size_t rez = key_index_htab(htab, str_key);
	if(rez!=htab->size) {//���� ��������
		if(NULL != htab->key_val[rez].key) {
			free(htab->key_val[rez].key);
			free(htab->key_val[rez].val);
		}
		htab->key_val[rez].key = str_key;
		htab->key_val[rez].val = str_val;
		htab->cnt+=1;

		return 1;
	} else
		return 0;

}

/*��������� ������� � ���������������*/
void resize_htab(struct hTab *htab) {
	//�������� ������
	int old_cnt = htab->cnt, old_size = htab->size;
	struct KeyVal * old_key_val = htab->key_val;

	//��������� ������
	htab->size<<=1;
	htab->cnt = 0;
	/*�������� ����� �����*/
	htab->key_val = malloc(htab->size*sizeof(struct KeyVal));
	memset(htab->key_val, 0, htab->size*sizeof(struct KeyVal));

	//������� ������ � ���������������� (��������� �� ���������)
	for(size_t i = 0; htab->cnt<old_cnt && i < old_size; ++i)
		if(NULL != old_key_val[i].key) //���� ��������� �� ���� ����� NULL, ������ ������� ������
			add_hTab(htab,old_key_val[i].key, old_key_val[i].val);
	
	//�������� ��������� ������� (������ ����� ���������� �� ����� �����)
	free(old_key_val);
}