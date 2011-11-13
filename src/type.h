#ifndef _type_h_
#define _type_h_

#include "util.h"

typedef enum {
    T_UNIT,
    T_BOOL,
    T_INT,
    T_FLOAT,
    T_CHAR,
    T_STRING,
    T_FUN,
    T_TUPLE,
    T_LIST,
    T_ARRAY,
    T_VAR,
    T_SCHEMA
} BDTypeKind;

typedef struct BDType BDType;

typedef struct {
    Vector *formals;    // Vector<BDType>
    BDType *ret;
} BDTypeFun;

typedef struct {
    Vector *elems;  // Vector<BDType>
} BDTypeTuple;

typedef struct {
    BDType *elem;
} BDTypeList;

typedef struct {
    BDType *elem;
} BDTypeArray;

typedef struct {
    BDType *content;
} BDTypeVar;

typedef struct {
    Vector *vars;   // Vector<BDType>
    BDType *body;
} BDTypeSchema;

struct BDType {
    BDTypeKind kind;
    union {
        BDTypeFun u_fun;
        BDTypeTuple u_tuple;
        BDTypeList u_list;
        BDTypeArray u_array;
        BDTypeVar u_var;
        BDTypeSchema u_schema;
    } u;
};

BDType *bd_type(BDTypeKind kind);
void bd_type_destroy(BDType *t);
BDType *bd_type_clone(BDType *t);
char *bd_type_show(BDType *t);

BDType *bd_type_unit();
BDType *bd_type_bool();
BDType *bd_type_char();
BDType *bd_type_int();
BDType *bd_type_float();
BDType *bd_type_string();
BDType *bd_type_fun(Vector *formals, BDType *ret);
BDType *bd_type_tuple(Vector *elems);
BDType *bd_type_list(BDType *elem);
BDType *bd_type_array(BDType *elem);
BDType *bd_type_var(BDType *content);
BDType *bd_type_schema(Vector *vars, BDType *body);

BDType *bd_return_type(BDType *t);


#endif
