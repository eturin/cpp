#ifndef HTAB_H
#define HTAB_H

#include <string.h>

struct hTab {
	/*максимальный размер*/
	size_t size;
	/*текущее количество элементов*/
	size_t cnt;
	/*массив хранящий элементы*/
	struct KeyVal {
		const char * key;
		char * val;
	} *key_val;
};

/*Конструктор таблицы (принимает степень двойки, для инициализации начального размера)*/
extern struct hTab * create_hTab(size_t pow/*степень 2*/);
/*Добавление элемента*/
extern int add_hTab(struct hTab * htab, const char *str_key, char *str_val);
/*Удаление элемента*/
extern int del_hTab(struct hTab * htab, const char * str_key);
/*поиск значения в хеш-таблице по ключу*/
extern char * search_hTab(const struct hTab * htab, const char * str_key);
/*поиск индекса в чеш-таблице по ключу*/
extern size_t key_index_htab(const struct hTab * htab, const char * str_key);
/*деструктор таблицы*/
extern struct hTab * releas_hTab(struct hTab * htab);

#endif