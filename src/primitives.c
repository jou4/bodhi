#include <stdarg.h>
#include "util.h"

typedef enum {
    PT_UNIT,
    PT_BOOL,
    PT_INT,
    PT_FLOAT,
    PT_CHAR,
    PT_STRING,
    PT_FUN,
    PT_TUPLE,
    PT_LIST,
    PT_ARRAY,
    PT_VAR,
} PrimTypeKind;

typedef struct {
    PrimTypeKind kind;
    Vector *elems;
    int id;
} PrimType;

typedef struct {
    char *name;
    Vector *ts;
} PrimSig;

#define Sig prim_sig

#define PTUnit pt_type_new(PT_UNIT)
#define PTBool pt_type_new(PT_BOOL)
#define PTInt pt_type_new(PT_INT)
#define PTFloat pt_type_new(PT_FLOAT)
#define PTChar pt_type_new(PT_CHAR)
#define PTString pt_type_new(PT_STRING)
#define PTVar(id) pt_type_new(id)

#define PTFun pt_type_fun
#define PTTuple pt_type_tuple
#define PTList(t) pt_type_list(PT_LIST, t)
#define PTArray(t) pt_type_list(PT_ARRAY, t)

PrimType *pt_type_new(PrimTypeKind kind)
{
    PrimType *t = malloc(sizeof(PrimType));
    t->kind = kind;
    t->elems = vector_new();
    return t;
}

PrimType *pt_type_var(int id)
{
    PrimType *t = pt_type_new(PT_VAR);
    t->id = id;
    return t;
}

PrimType *pt_type_list(PrimTypeKind kind, PrimType *elem)
{
    PrimType *t = pt_type_new(kind);
    vector_add(t->elems, elem);
    return t;
}

PrimType *pt_type_fun(int num, ...)
{
    va_list ap;
    int i;

    va_start(ap, num);

    PrimType *t = pt_type_new(PT_FUN);

    for( i=0; i<num; i++ ){
        vector_add(t->elems, va_arg(ap, PrimType*));
    }

    va_end(ap);

    return t;
}

PrimType *pt_type_tuple(int num, ...)
{
    va_list ap;
    int i;

    va_start(ap, num);

    PrimType *t = pt_type_new(PT_TUPLE);

    for( i=0; i<num; i++ ){
        vector_add(t->elems, va_arg(ap, PrimType*));
    }

    va_end(ap);

    return t;
}

PrimSig *prim_sig(char *name, int num, ...)
{
    va_list ap;
    int i;

    va_start(ap, num);

    PrimSig *sig = malloc(sizeof(PrimSig));
    sig->name = name;
    sig->ts = vector_new();

    for( i=0; i<num; i++ ){
        vector_add(sig->ts, va_arg(ap, PrimType*));
    }

    va_end(ap);

    return sig;
}
