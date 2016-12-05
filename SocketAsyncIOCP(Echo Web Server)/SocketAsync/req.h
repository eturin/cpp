#ifndef REQ_H
#define REQ_H

#include "common.h"

/*этот тип хранит структурированный запрос*/
struct Req {
	/*запрошенный URL*/
	char  *url;
	/*количество заголовков*/
	int    head_cnt;
	/*массив имен и значений заголовка*/
	struct Head {
		char * field;
		char * value;
	}     *head;
	/*статус ответа*/
	char  *status;
	/*тело ответа*/
	char  *body;
	/*размеры сообщения*/
	size_t head_length;
	size_t body_length;
};
struct Req * init_Req();
struct Req * release_Req(struct Req *);

#endif