#ifndef _expr_h_
#define _expr_h_

#include "util/vector.h"
#include "type.h"

typedef enum {
    E_UNIT,
    E_BOOL,
    E_INT,
    E_FLOAT,
    E_UNIOP,
    E_BINOP,
    E_IF,
    E_LET,
    E_VAR,
    E_LETREC,
    E_APP,
    E_TUPLE,
    E_LETTUPLE,
    E_ARRAY,
    E_MAKECLS,
    E_APPCLS,
    E_APPDIR
} BDExprKind;

typedef enum {
    OP_NOT,
    OP_NEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_EQ,
    OP_LE,
    OP_GE,
} BDOpKind;

typedef struct {
    char *name;
    BDType *type;
} BDExprIdent;

typedef struct {
    char *entry;
    Vector *freevars;   // Vector<String>
} BDExprClosure;


BDExprIdent *bd_expr_ident(const char *name, BDType *type);
BDExprIdent *bd_expr_ident_typevar(const char *name);
void bd_expr_ident_destroy(BDExprIdent *ident);
void bd_expr_ident_show(BDExprIdent *ident);

BDExprClosure *bd_expr_closure(const char *entry, Vector *freevars);
void bd_expr_closure_destroy(BDExprClosure *cls);

#endif
