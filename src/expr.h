#ifndef _expr_h_
#define _expr_h_

#include "util.h"
#include "env.h"
#include "type.h"
#include "show.h"

typedef enum {
    E_UNIT,
    E_BOOL,
    E_INT,
    E_FLOAT,
    E_CHAR,
    E_STR,
    E_NIL,
    E_FUN,
    E_UNIOP,
    E_BINOP,
    E_IF,
    E_LET,
    E_VAR,
    E_LETREC,
    E_APP,
    E_CCALL,
    E_TUPLE,
    E_LETTUPLE,
    E_MATCH,
    E_PATTERNVAR,
    E_MAKECLS,
    E_APPCLS,
    E_APPDIR,
    E_CONT,
} BDExprKind;

typedef enum {
    OP_NOT,
    OP_NEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_FNEG,
    OP_FADD,
    OP_FSUB,
    OP_FMUL,
    OP_FDIV,
    OP_SADD,
    OP_EQ,
    OP_LE,
    OP_GE,
    OP_CONS,
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
BDExprIdent *bd_expr_ident_clone(BDExprIdent *ident);
Vector *bd_expr_idents_clone(Vector *idents);
void bd_set_env_expr_idents(Env *env, Vector *idents);
BDExprIdent *bd_expr_ident_typevar(const char *name);
void bd_expr_ident_destroy(BDExprIdent *ident);
char *bd_expr_ident_show(BDExprIdent *ident);

BDExprClosure *bd_expr_closure(const char *entry, Vector *freevars);
void bd_expr_closure_destroy(BDExprClosure *cls);

#endif
