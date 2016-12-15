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
	struct Header {
		char *key;                 /* ключ           */
		char *val;                 /* значение       */
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

void htab_add(struct Header**, const char*, size_t, const char*, size_t);
struct Header * htab_find(struct Header const *, const char*, size_t);
void htab_delete(struct Header**, struct Header*);
void htab_delete_all(struct Header**);
void htab_show(struct Header const**);
int fsort_by_val(struct Header*, struct Header*);
int fsort_by_key(struct Header*, struct Header*);
void htab_sort(struct Header**, int(*)(struct Header*, struct Header*));
#endif