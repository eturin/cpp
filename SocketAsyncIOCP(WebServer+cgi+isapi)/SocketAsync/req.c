#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <ctype.h>
#include "http_parser.h"
#include "req.h"

struct Req * init_Req() {
	struct Req * req = malloc(sizeof(struct Req));
	//заполнение памяти нулями, чтоб не ставить в NULL отдельные указатели
	memset(req, 0, sizeof(struct Req));

	return req;
}
struct Req * release_Req(struct Req * req) {
	if(req != NULL) {
		/*освобождение динамической памяти*/
		free(req->url);
		htab_delete_all(&req->pHeader);
		free(req->status);
		free(req->body);
		free(req);
	}
	return NULL;
}

/* Раскодирование строки "%xx"*/
char DecodeHex(char *str) {
	char ch;

	// Обрабатываем старший разряд
	if(str[0] >= 'A')
		ch = ((str[0] & 0xdf) - 'A') + 10;
	else
		ch = str[0] - '0';

	// Сдвигаем его влево на 4 бита
	ch <<= 4;

	// Обрабатываем младший разряд и складываем
	// его со старшим
	if(str[1] >= 'A')
		ch += ((str[1] & 0xdf) - 'A') + 10;
	else
		ch += str[1] - '0';

	// Возвращаем результат перекодировки
	return ch;
}
/* Раскодирование строки из кодировки URL*/
void DecodeStr(char *szString) {
	int src;
	int dst;
	char ch;

	// Цикл по строке
	for(src = 0, dst = 0; szString[src]; src++, dst++) {
		// Получаем очередной символ перекодируемой строки
		ch = szString[src];

		// Заменяем символ "+" на пробел
		ch = (ch == '+') ? ' ' : ch;

		// Сохраняем результат
		szString[dst] = ch;

		// Обработка шестнадцатеричных кодов вида "%xx"
		if(ch == '%') {
			// Выполняем преобразование строки "%xx"
			// в код символа
			szString[dst] = DecodeHex(&szString[src + 1]);
			src += 2;
		}
	}

	// Закрываем строку двоичным нулем
	szString[dst] = '\0';
}
/*выделение URL*/
int call_request_url_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *preq = (struct Req*)parser->data;
	preq->url = malloc((len + 1)*sizeof(char));
	strncpy(preq->url, buf, len);
	preq->url[len] = '\0';

	//DecodeStr(preq->url);

	return 0;
}

/*выделение имени заголовка*/
int call_header_field_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *preq = (struct Req*)parser->data;	
	htab_add(&preq->pHeader, buf, len, NULL, 0);

	return 0;
}
/*выделение значения заголовка*/
int call_header_value_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *preq = (struct Req*)parser->data;	
	htab_add(&preq->pHeader, NULL, 0, buf, len);
		
	return 0;
}
/*выделение статуса ответа*/
int call_response_status_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *preq = (struct Req*)parser->data;
	preq->status = malloc((len + 1)*sizeof(char));
	strncpy(preq->status, buf, len);
	preq->status[len] = '\0';
	return 0;
}
/*выделение тела*/
int call_body_cb(http_parser *parser, const char *buf, size_t len) {
	struct Req *preq = (struct Req*)parser->data;
	preq->body = malloc(len);
	memset(preq->body, 0, len);
	strncpy(preq->body, buf, len);
	/*размер прочитанной части тела сообщения HTTP*/
	preq->cur = len;

	return 0;
}

int call_message_begin_cb(http_parser *p) { return 0; }
int call_headers_complete_cb(http_parser *p) { return 0; }
int call_message_complete_cb(http_parser *p) { return 0; }
int call_chunk_header_cb(http_parser *p) { return 0; }
int call_chunk_complete_cb(http_parser *p) { return 0; }

struct Req * pars_http(const char* data, size_t *len) {
	/*определяем парсер и его тип*/
	http_parser parser;
	http_parser_init(&parser, HTTP_BOTH);

	/*структура хранящая выделенные парсером поля*/
	struct Req *preq = init_Req();
	parser.data = preq;

	/*это функции обратного вызова, для выделение фрагментов http-сообщения*/
	static http_parser_settings settings_null = {.on_message_begin    = call_message_begin_cb,
		                                         .on_header_field     = call_header_field_cb,
												 .on_header_value     = call_header_value_cb,
												 .on_url              = call_request_url_cb,
												 .on_status           = call_response_status_cb,
												 .on_body             = call_body_cb,
												 .on_headers_complete = call_headers_complete_cb,
												 .on_message_complete = call_message_complete_cb,
												 .on_chunk_header     = call_chunk_header_cb,
												 .on_chunk_complete   = call_chunk_complete_cb};

	/*запускаем парсер*/
	size_t parsed = http_parser_execute(&parser, &settings_null, data, *len);

	/*распарсить не удалось*/
	if(parsed != *len)
		preq = release_Req(preq);
	else {		
		/*определяем команду http*/
		strncpy(preq->cmd, data, 4);		
		if(!strncmp(preq->cmd,"GET",3))
			preq->cmd[3] = '\0';
		else
			preq->cmd[4] = '\0';
		
		struct Header *s = htab_find(preq->pHeader, "CONTENT-LENGTH",0);
		preq->body_length = 0;
		if(s != NULL) 
			sscanf(s->val,"%d", &preq->body_length);			
	}

	return preq;
}

void htab_add(struct Header **ppEnv, const char * _key, size_t key_len, const char * _val, size_t val_len) {
	struct Header *s;

	if(_key != NULL) {//добавляется ключ и возможно значение
		s = htab_find(*ppEnv, _key, key_len);
		if(s == NULL) {
			s = (struct Header*)malloc(sizeof(struct Header));
			memset(s, 0, sizeof(struct Header));
			/*переносим значение ключа*/
			s->key = (char *)malloc(key_len + 1);
			strncpy(s->key, _key, key_len);
			s->key[key_len] = '\0';
			/*переводим в верхний регистр значение ключа*/
			for(size_t i = 0; i < key_len; ++i)
				s->key[i] = toupper(s->key[i]);
			/*добавляем ключ в таблицу*/
			HASH_ADD_STR(*ppEnv, key, s); /*key - имя поля структуры с ключом*/
		}
	} else {//добавляется значение в последний добавленный элемент (только сортировка сломает этот порядок)
		s = *ppEnv;
		/*ключи хранятся в хеш-таблице с указателями на элементы списка*/
		while(s->hh.next != NULL)
			s = (struct Header *)s->hh.next;
	}

	/*переносим значение*/
	free(s->val);
	if(_val != NULL) {		
		s->val = (char *)malloc(val_len + 1);
		strncpy(s->val, _val, val_len);
		s->val[val_len] = '\0';
	} else		
		s->val = NULL;	
}
struct Header * htab_find(struct Header const *ppEnv, const char * _key, size_t len) {
	if(len == 0)
		len = strlen(_key);

	struct Header *s=NULL;
	HASH_FIND_STR_LEN(ppEnv, _key, len, s);

	return s;
}
void htab_delete_all(struct Header **ppEnv) {
	struct Header *s, *tmp;

	HASH_ITER(hh, *ppEnv, s, tmp) {
		htab_delete(ppEnv, s);
	}
}
void htab_delete(struct Header **ppEnv, struct Header *s) {
	HASH_DEL(*ppEnv, s);
	free(s->key);
	free(s->val);
	free(s);
}

/*не задействованы (оставил доя примера)*/
void htab_show(struct Header const **ppEnv) {
	for(struct Header const * s = *ppEnv; s != NULL; s = (struct Header const*)(s->hh.next))
		printf("key=%s val=%s\n", s->key, s->val);
}
int fsort_by_val(struct Header *a, struct Header *b) {
	return strcmp(a->val, b->val);
}
int fsort_by_key(struct Header *a, struct Header *b) {
	return strcmp(a->key, b->key);
}
void htab_sort(struct Header **ppEnv, int(*func)(struct Header*, struct Header*)) {
	HASH_SORT(*ppEnv, func);
}