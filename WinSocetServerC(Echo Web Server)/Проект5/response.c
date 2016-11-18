#define _CRT_SECURE_NO_WARNINGS

#include "http_parser.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*этот тип хранит имена заголовков и их значения*/
struct Head {
	char * field;
	char * value;
};
/*этот тип хранит структурированный запрос*/
struct Rec {
	char * url;
	struct Head * head;
	int    head_cnt;
	char * status;
	char * body;
};

/*выделение имени заголовка*/
int call_header_field_cb(http_parser *parser, const char *buf, size_t len) {
	struct Rec *req = (struct Rec*)parser->data;
	++(req->head_cnt);
	if(req->head_cnt>1) 		
		req->head = realloc(req->head, sizeof(struct Head)*req->head_cnt);
	else
		req->head = malloc(sizeof(struct Head)*req->head_cnt);
	char * field = req->head[req->head_cnt - 1].field = malloc((len + 1)*sizeof(char));
	req->head[req->head_cnt - 1].value = NULL;

	strncpy(field, buf, len);
	field[len] = '\0';
	return 0;
}
/*выделение значения заголовка*/
int call_header_value_cb(http_parser *parser, const char *buf, size_t len) {
	struct Rec *req = (struct Rec*)parser->data;
	char * value=req->head[req->head_cnt-1].value = malloc((len + 1)*sizeof(char));

	strncpy(value, buf, len);
	value[len] = '\0';
	return 0;
}
/*выделение статуса ответа*/
int call_response_status_cb(http_parser *parser, const char *buf, size_t len) {
	struct Rec *req = (struct Rec*)parser->data;
	req->status = malloc((len + 1)*sizeof(char));
	strncpy(req->status, buf, len);
	req->status[len] = '\0';
	return 0;
}
/*выделение тела*/
int call_body_cb(http_parser *parser, const char *buf, size_t len) {
	struct Rec *req = (struct Rec*)parser->data;
	req->body = malloc((len + 1)*sizeof(char));
	strncpy(req->body, buf, len);
	req->body[len] = '\0';
	return 0;
}
/*выделение URL*/
int call_request_url_cb(http_parser *parser, const char *buf, size_t len) {
	struct Rec *req = (struct Rec*)parser->data;
	req->url = malloc((len + 1)*sizeof(char));
	strncpy(req->url, buf, len);
	req->url[len] = '\0';
	return 0;
}

int call_message_begin_cb   (http_parser *p) { return 0; }
int call_headers_complete_cb(http_parser *p) { return 0; }
int call_message_complete_cb(http_parser *p) { return 0; }
int call_chunk_header_cb    (http_parser *p) { return 0; }
int call_chunk_complete_cb  (http_parser *p) { return 0; }

size_t make_response(char** data, size_t *len) {
	http_parser parser;
	
	struct Rec req;
	memset(&req, 0, sizeof(req)); //заполнение памяти нулями, чтоб не ставить в NULL отдельные указатели
	
	parser.data = (void*)&req;
	http_parser_init(&parser, HTTP_REQUEST);
	
	/*это функции обраьного вызова, для выделение фрагментов http-сообщения*/
	static http_parser_settings settings_null = { .on_message_begin    = call_message_begin_cb
		                                        , .on_header_field     = call_header_field_cb
												, .on_header_value     = call_header_value_cb
												, .on_url              = call_request_url_cb
												, .on_status           = call_response_status_cb
												, .on_body             = call_body_cb
												, .on_headers_complete = call_headers_complete_cb
												, .on_message_complete = call_message_complete_cb
												, .on_chunk_header     = call_chunk_header_cb
												, .on_chunk_complete   = call_chunk_complete_cb
												};


	size_t parsed = http_parser_execute(&parser, &settings_null, *data, *len);
	char * format="HTTP/1.1 404 ERROR\r\n"
			      "Version: HTTP/1.1\r\n"
			      "Content-Type: text/html; charset=utf-8\r\n"
			      "Content-Length: 0 \r\n\r\n";
	if(parsed == *len) {
		format = "HTTP / 1.1 200 OK\r\nVersion: HTTP / 1.1\r\n"
		         "Content-Type: text/html; charset=utf-8\r\n"
		         "Content-Length: %d\r\n\r\n%s";

		char * response = malloc(sizeof(char)*(*len + strlen(format)));
		sprintf(response, format, *len, *data);
		free(*data);
		*data = response;		    
	} else {
		/*распарсить не удалось, ответим 404*/
		char * response = malloc(sizeof(char)*strlen(format));
		sprintf(response, format);
		free(*data);
		*data = response;
	}
	*len = strlen(*data);

	/*освобождение динамической памяти*/
	free(req.url);
	for(int i = 0; i < req.head_cnt; ++i) {
		free(req.head[i].field);
		free(req.head[i].value);
	}
	free(req.head);
	free(req.status);
	free(req.body);

	return parsed;
}