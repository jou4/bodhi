#ifndef _expr2_h_
#define _expr2_h_

#include "util.h"
#include "expr1.h"

typedef struct BDExpr2 BDExpr2;

typedef struct {
    BDExprIdent *ident;
    Vector *formals;    // Vector<BDExprIdent>
    BDExpr2 *body;
} BDExpr2Fundef;


typedef struct {
    BDOpKind kind;
    char *val;
} BDExpr2UniOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
} BDExpr2BinOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
    BDExpr2 *t;
    BDExpr2 *f;
} BDExpr2If;

typedef struct {
    BDExprIdent *ident;
    BDExpr2 *val;
    BDExpr2 *body;
} BDExpr2Let;

typedef struct {
    char *name;
} BDExpr2Var;

typedef struct {
    BDExpr2Fundef *fundef;
    BDExpr2 *body;
} BDExpr2LetRec;

typedef struct {
    char *fun;
    Vector *actuals;    // Vector<String>
} BDExpr2App;

typedef struct {
    Vector *elems;      // Vector<String>
} BDExpr2Tuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    char *val;
    BDExpr2 *body;
} BDExpr2LetTuple;


struct BDExpr2 {
    BDExprKind kind;
    union {
        int u_int;
        double u_double;
        BDExpr2UniOp u_uniop;
        BDExpr2BinOp u_binop;
        BDExpr2If u_if;
        BDExpr2Let u_let;
        BDExpr2Var u_var;
        BDExpr2LetRec u_letrec;
        BDExpr2App u_app;
        BDExpr2Tuple u_tuple;
        BDExpr2LetTuple u_lettuple;
    } u;
};

BDExpr2 *bd_expr2(BDExprKind kind);
void bd_expr2_destroy(BDExpr2 *e);
void bd_expr2_show(BDExpr2 *e);
Set *bd_expr2_freevars(BDExpr2 *e);

BDExpr2 *bd_expr2_unit();
BDExpr2 *bd_expr2_int(int val);
BDExpr2 *bd_expr2_float(double val);
BDExpr2 *bd_expr2_uniop(BDOpKind kind, const char *val);
BDExpr2 *bd_expr2_binop(BDOpKind kind, const char *l, const char *r);
BDExpr2 *bd_expr2_if(BDOpKind kind, const char *l, const char *r, BDExpr2 *t, BDExpr2 *f);
BDExpr2 *bd_expr2_let(BDExprIdent *ident, BDExpr2 *val, BDExpr2 *body);
BDExpr2 *bd_expr2_var(const char *name);
BDExpr2 *bd_expr2_letrec(BDExpr2Fundef *fundef, BDExpr2 *body);
BDExpr2 *bd_expr2_app(const char *fun, Vector *actuals);
BDExpr2 *bd_expr2_tuple(Vector *elems);
BDExpr2 *bd_expr2_lettuple(Vector *idents, const char *val, BDExpr2 *body);

BDExpr2Fundef *bd_expr2_fundef(BDExprIdent *ident, Vector *formals, BDExpr2 *body);
void bd_expr2_fundef_destroy(BDExpr2Fundef *fundef);

#endif
