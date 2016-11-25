#ifndef HTAB_H
#define HTAB_H

#include <string.h>

struct hTab {
	/*������������ ������*/
	size_t size;
	/*������� ���������� ���������*/
	size_t cnt;
	/*������ �������� ��������*/
	struct KeyVal {
		const char * key;
		char * val;
	} *key_val;
};

/*����������� ������� (��������� ������� ������, ��� ������������� ���������� �������)*/
extern struct hTab * create_hTab(size_t pow/*������� 2*/);
/*���������� ��������*/
extern int add_hTab(struct hTab * htab, const char *str_key, char *str_val);
/*�������� ��������*/
extern int del_hTab(struct hTab * htab, const char * str_key);
/*����� �������� � ���-������� �� �����*/
extern char * search_hTab(const struct hTab * htab, const char * str_key);
/*����� ������� � ���-������� �� �����*/
extern size_t key_index_htab(const struct hTab * htab, const char * str_key);
/*���������� �������*/
extern struct hTab * releas_hTab(struct hTab * htab);

#endif