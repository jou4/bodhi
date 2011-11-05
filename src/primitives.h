#ifndef _primitives_h_
#define _primitives_h_

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
    char *lbl;
    BDType *type;
} PrimSig;

Vector *primitives();

#endif
