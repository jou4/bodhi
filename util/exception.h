#ifndef _exception_h_
#define _exception_h_

#include <setjmp.h>
#include "list.h"

typedef struct {
    List *envs;
    List *errors;
} Exception;

typedef struct {
    const char *msg;
} Error;

Exception _exception;

#define try \
    int _ret_value;  \
    jmp_buf *_ex_env = (jmp_buf*)malloc(sizeof(jmp_buf));    \
    _exception.envs = list_cons(_ex_env, _exception.envs); \
    if((_ret_value = setjmp(*_ex_env)) == 0)

#define catch else
#define error_type _ret_value

#define throw(type, e)    \
    _exception.errors = list_cons(e, _exception.errors);    \
    jmp_buf *_ex_env = list_head(_exception.envs);   \
    _exception.envs = list_tail(_exception.envs);     \
    longjmp(*_ex_env, type)

void *catch_error();
Error *error_new(const char *msg);

#endif
