#include "req.h"

struct Req * init_Req() {
	struct Req * req = malloc(sizeof(struct Req));
	//���������� ������ ������, ���� �� ������� � NULL ��������� ���������
	memset(req, 0, sizeof(struct Req));

	return req;
}
struct Req * release_Req(struct Req * req) {
	if(req != NULL) {
		/*������������ ������������ ������*/
		free(req->url);
		for(int i = 0; i < req->head_cnt; ++i) {
			free(req->head[i].field);
			free(req->head[i].value);
		}
		free(req->head);
		free(req->status);
		free(req->body);
		free(req);
	}
	return NULL;
}