#include "htab.h"
#include <stdlib.h>


void resize_htab(struct hTab *htab);

/*Освобождение отдельной записи*/
void free_KeyVal(struct KeyVal * p) {
	free(p->key);
	p->key = NULL;

	free(p->val);
	p->val = NULL;
}

/*конструктор таблицы (принимает степень двойки, для инициализации начального размера)*/
struct hTab * create_hTab(size_t pow/*степень 2*/) {
	/*выделяем место под таблицу*/
	struct hTab * htab = malloc(sizeof(struct hTab));	

	/*выделяем место под массив, хранящий элементы*/
	htab->size = 1 << (pow ? pow : 3);
	htab->key_val = malloc(htab->size*sizeof(struct KeyVal));
	memset(htab->key_val, 0, htab->size*sizeof(struct KeyVal));

	/*элементов пока нет*/
	htab->cnt = 0;

	return htab;
}

/*деструктор таблицы*/
struct hTab * releas_hTab(struct hTab * htab) {
	for(rsize_t i = 0; i < htab->size; ++i)
		free_KeyVal(&htab->key_val[i]);
	htab->cnt = 0;
	free(htab->key_val);
	htab->size = 0;
	free(htab);

	return NULL;
}

/*Функция перехода к следующему хешу (когда место в массиве занято и нужно вычислить следующий хеш)*/
size_t hfunc_prob(size_t k, size_t prev, size_t size) {
	return (prev + k) % size;
}

/*Хеш функция от строки (возвращает индекс в массиве)*/
size_t hfunc(const char * str, /*хешируемая строка*/
			 size_t k,         /*количество проб*/
			 size_t a,         /*произвольный коэффициент*/
			 size_t size       /*размер хеш-таблицы*/) {
	//строим хеш по строке
	size_t rez = *str % size;
	while(*++str!=0)
		rez = (rez*a + *str) % size;

	//учет пробирования
	for(size_t i = 1; i < k; ++i)
		rez = hfunc_prob(i, rez, size);

	return rez;
}

/*поиск значения в хеш-таблице по ключу*/
char * search_hTab(const struct hTab * htab, const char * str_key) {
	int k = 0;
	//вычисляем хеш строки
	int rez = hfunc(str_key, 0, 3, htab->size);
		
	//пробирование
	while(k+1 < htab->size && (NULL == htab->key_val[rez].key             //нет элемента (ищем дальше, т.к. он мог быть удален)
		                      || strcmp(htab->key_val[rez].key, str_key)) //элемент занят другим ключом (ищем дальше)
		  )
		rez = hfunc_prob(++k, rez,htab->size);

	//проверка
	if(NULL == htab->key_val[rez].key)
		return NULL;                   //не найдено	
	else if(strcmp(htab->key_val[rez].key, str_key))
		return NULL;                   //занято другим значением
	else 
		return htab->key_val[rez].val; //возвращаем значение по ключу
}

/*поиск индекса в чеш-таблице по ключу*/
size_t key_index_htab(const struct hTab * htab, const char * str_key) {
	int k = 0;
	//вычисляем хеш строки
	int rez = hfunc(str_key, 0, 3, htab->size);

	//пробирование
	while(k+1 < htab->size 
		  && NULL != htab->key_val[rez].key && strcmp(htab->key_val[rez].key, str_key) //элемент занят другим ключом (ищем пустое место дальше)
		)
		rez = hfunc_prob(++k, rez, htab->size);

	//проверка
	if(NULL == htab->key_val[rez].key
	   || !strcmp(htab->key_val[rez].key, str_key))
		return rez;	//найдено подходящее место	
	else 
		return htab->size; //не найдено
}

/*Удаление элемента*/
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

/*Добавление элемента*/
int add_hTab(struct hTab * htab, const char *str_key, char *str_val) {
	if(3.0 / 4 < 1.0 * htab->cnt / htab->size)
		resize_htab(htab);

	size_t rez = key_index_htab(htab, str_key);
	if(rez!=htab->size) {//есть местечко
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

/*Изменение размера и перехеширование*/
void resize_htab(struct hTab *htab) {
	//исходные данные
	int old_cnt = htab->cnt, old_size = htab->size;
	struct KeyVal * old_key_val = htab->key_val;

	//удваиваем размер
	htab->size<<=1;
	htab->cnt = 0;
	/*выделяем новое место*/
	htab->key_val = malloc(htab->size*sizeof(struct KeyVal));
	memset(htab->key_val, 0, htab->size*sizeof(struct KeyVal));

	//перенос данных с перехешированием (удаленные не переносим)
	for(size_t i = 0; htab->cnt<old_cnt && i < old_size; ++i)
		if(NULL != old_key_val[i].key) //если указатель на ключ равен NULL, значит элемент удален
			add_hTab(htab,old_key_val[i].key, old_key_val[i].val);
	
	//удаление исходного массива (адреса строк копируются на новое место)
	free(old_key_val);
}