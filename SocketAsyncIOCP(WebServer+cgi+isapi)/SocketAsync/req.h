#ifndef REQ_H
#define REQ_H

#include "common.h"
#include "uthash.h"

/*этот тип хранит структурированный запрос*/
struct Req {
	char cmd[5];
	/*запрошенный URL*/
	char  *url;
	/*это хеш-таблица заголовков*/
	struct hTab {
		char *key;                 /* ключ           */
		char *val;                 /* значение       */
		void* pIsapi;              /* (понадобится только в проекте isapi)*/
		UT_hash_handle hh;         /* служебное поле */
	} *pHeader;
	/*статус ответа*/
	char  *status;
	
	/*размер тела сообщения и объем прочитанных от клиента данных*/	
	size_t body_length;
	size_t cur;
	/*тело*/
	char  *body;	
};

struct Req * init_Req();
struct Req * release_Req(struct Req *);
struct Req * pars_http(const char*, size_t*);

void htab_add(struct hTab**, const char*, size_t, const char*, size_t);
struct hTab * htab_find(struct hTab const *, const char*, size_t);
void htab_delete(struct hTab**, struct hTab*);
struct hTab * htab_delete_all(struct hTab**);
void htab_show(struct hTab const**);
int fsort_by_val(struct hTab*, struct hTab*);
int fsort_by_key(struct hTab*, struct hTab*);
void htab_sort(struct hTab**, int(*)(struct hTab*, struct hTab*));
#endif