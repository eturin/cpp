#ifndef ERROR_H
#define ERROR_H

#include "common.h"

HANDLE open_log();
int show_err_wsa(const char *);
int show_err(const char *,BOOL);
int show_err_HRESULT(const char * msg, HRESULT hr);

#endif