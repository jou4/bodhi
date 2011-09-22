#include <stdio.h>
#include <stdlib.h>
#include "exception.h"

void *catch_error()
{
    void *err = list_head(_exception.errors);
    _exception.errors = list_tail(_exception.errors);
    return err;
}

Error *error_new(const char *msg)
{
    Error *e = (Error *)malloc(sizeof(Error));
    e->msg = msg;
    return e;
}


/*
void test_func()
{
    try{
        throw(1, error_new("first error!"));
    }catch{
        throw(2, error_new("inner error!"));
    }
}

int main()
{
    try
    {
        try{
            test_func();
            printf("inner ok\n");
            printf("outer ok\n");
        }catch{
            Error *err = catch_error();

            printf("inner ng %d\n", error_type);
            printf("%s\n", err->msg);

            throw(3, error_new("outer error!"));
        }
    }
    catch
    {
        printf("outer ng %d\n", error_type);
        Error *err = catch_error();
        printf("%s\n", err->msg);
    }

    return 0;
}
*/
