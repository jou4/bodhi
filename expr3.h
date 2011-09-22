#ifndef _expr3_h_
#define _expr3_h_

#include "expr2.h"

typedef struct BDExpr3 BDExpr3;

typedef struct {
    BDExprIdent *ident;
    Vector *formals;    // Vector<BDExprIdent>
    Vector *freevars;   // Vector<BDExprIdent>
    BDExpr3 *body;
} BDExpr3Fundef;


typedef struct {
    BDOpKind kind;
    char *val;
} BDExpr3UniOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
} BDExpr3BinOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
    BDExpr3 *t;
    BDExpr3 *f;
} BDExpr3If;

typedef struct {
    BDExprIdent *ident;
    BDExpr3 *val;
    BDExpr3 *body;
} BDExpr3Let;

typedef struct {
    char *name;
} BDExpr3Var;

typedef struct {
    BDExprIdent *ident;
    BDExprClosure *closure;
    BDExpr3 *body;
} BDExpr3MakeCls;

typedef struct {
    char *fun;
    Vector *actuals;    // Vector<String>
} BDExpr3App;

typedef struct {
    Vector *elems;      // Vector<String>
} BDExpr3Tuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    char *val;
    BDExpr3 *body;
} BDExpr3LetTuple;


struct BDExpr3 {
    BDExprKind kind;
    union {
        int u_int;
        double u_double;
        BDExpr3UniOp u_uniop;
        BDExpr3BinOp u_binop;
        BDExpr3If u_if;
        BDExpr3Let u_let;
        BDExpr3Var u_var;
        BDExpr3MakeCls u_makecls;
        BDExpr3App u_app;
        BDExpr3Tuple u_tuple;
        BDExpr3LetTuple u_lettuple;
    } u;
};


BDExpr3 *bd_expr3(BDExprKind kind);
void bd_expr3_destroy(BDExpr3 *e);
void bd_expr3_show(BDExpr3 *e);

BDExpr3 *bd_expr3_unit();
BDExpr3 *bd_expr3_int(int val);
BDExpr3 *bd_expr3_float(double val);
BDExpr3 *bd_expr3_uniop(BDOpKind kind, const char *val);
BDExpr3 *bd_expr3_binop(BDOpKind kind, const char *l, const char *r);
BDExpr3 *bd_expr3_if(BDOpKind kind, const char *l, const char *r, BDExpr3 *t, BDExpr3 *f);
BDExpr3 *bd_expr3_let(BDExprIdent *ident, BDExpr3 *val, BDExpr3 *body);
BDExpr3 *bd_expr3_var(const char *name);
BDExpr3 *bd_expr3_makecls(BDExprIdent *ident, BDExprClosure *closure, BDExpr3 *body);
BDExpr3 *bd_expr3_appcls(const char *fun, Vector *actuals);
BDExpr3 *bd_expr3_appdir(const char *fun, Vector *actuals);
BDExpr3 *bd_expr3_tuple(Vector *elems);
BDExpr3 *bd_expr3_lettuple(Vector *idents, const char *val, BDExpr3 *body);

BDExpr3Fundef *bd_expr3_fundef(BDExprIdent *ident, Vector *formals, BDExpr3 *body);
void bd_expr3_fundef_destroy(BDExpr3Fundef *fundef);

#endif
