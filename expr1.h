#ifndef _expr1_h_
#define _expr1_h_

#include "expr.h"

typedef struct BDExpr1 BDExpr1;

typedef struct {
    BDExprIdent *ident;
    Vector *formals;    // Vector<BDExprIdent>
    BDExpr1 *body;
} BDExpr1Fundef;


typedef struct {
    BDOpKind kind;
    BDExpr1 *val;
} BDExpr1UniOp;

typedef struct {
    BDOpKind kind;
    BDExpr1 *l;
    BDExpr1 *r;
} BDExpr1BinOp;

typedef struct {
    BDExpr1 *pred;
    BDExpr1 *t;
    BDExpr1 *f;
} BDExpr1If;

typedef struct {
    BDExprIdent *ident;
    BDExpr1 *val;
    BDExpr1 *body;
} BDExpr1Let;

typedef struct {
    char *name;
} BDExpr1Var;

typedef struct {
    BDExpr1Fundef *fundef;
    BDExpr1 *body;
} BDExpr1LetRec;

typedef struct {
    BDExpr1 *fun;
    Vector *actuals;    // Vector<BDExpr1>
} BDExpr1App;

typedef struct {
    Vector *elems;      // Vector<BDExpr1>
} BDExpr1Tuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    BDExpr1 *val;
    BDExpr1 *body;
} BDExpr1LetTuple;


struct BDExpr1 {
    BDExprKind kind;
    union {
        int u_int;
        double u_double;
        BDExpr1UniOp u_uniop;
        BDExpr1BinOp u_binop;
        BDExpr1If u_if;
        BDExpr1Let u_let;
        BDExpr1Var u_var;
        BDExpr1LetRec u_letrec;
        BDExpr1App u_app;
        BDExpr1Tuple u_tuple;
        BDExpr1LetTuple u_lettuple;
    } u;
};

BDExpr1 *bd_expr1(BDExprKind kind);
void bd_expr1_destroy(BDExpr1 *e);
void bd_expr1_show(BDExpr1 *e);

BDExpr1 *bd_expr1_unit();
BDExpr1 *bd_expr1_bool(int val);
BDExpr1 *bd_expr1_int(int val);
BDExpr1 *bd_expr1_float(double val);
BDExpr1 *bd_expr1_uniop(BDOpKind kind, BDExpr1 *val);
BDExpr1 *bd_expr1_binop(BDOpKind kind, BDExpr1 *l, BDExpr1 *r);
BDExpr1 *bd_expr1_if(BDExpr1 *pred, BDExpr1 *t, BDExpr1 *f);
BDExpr1 *bd_expr1_let(BDExprIdent *ident, BDExpr1 *val, BDExpr1 *body);
BDExpr1 *bd_expr1_var(const char *name);
BDExpr1 *bd_expr1_letrec(BDExpr1Fundef *fundef, BDExpr1 *body);
BDExpr1 *bd_expr1_app(BDExpr1 *fun, Vector *actuals);
BDExpr1 *bd_expr1_tuple(Vector *elems);
BDExpr1 *bd_expr1_lettuple(Vector *idents, BDExpr1 *val, BDExpr1 *body);

BDExpr1Fundef *bd_expr1_fundef(BDExprIdent *ident, Vector *formals, BDExpr1 *body);
void bd_expr1_fundef_destroy(BDExpr1Fundef *fundef);

#endif
