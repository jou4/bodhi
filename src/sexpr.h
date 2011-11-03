#ifndef _sexpr_h_
#define _sexpr_h_

#include "expr.h"

typedef struct BDSExpr BDSExpr;

typedef struct {
    BDExprIdent *ident;
    Vector *formals;    // Vector<BDExprIdent>
    BDSExpr *body;
} BDSExprFundef;

typedef struct {
    BDExprIdent *ident;
    BDSExpr *body;
} BDSExprDatadef;

typedef struct {
    BDOpKind kind;
    BDSExpr *val;
} BDSExprUniOp;

typedef struct {
    BDOpKind kind;
    BDSExpr *l;
    BDSExpr *r;
} BDSExprBinOp;

typedef struct {
    BDSExpr *pred;
    BDSExpr *t;
    BDSExpr *f;
} BDSExprIf;

typedef struct {
    BDExprIdent *ident;
    BDSExpr *val;
    BDSExpr *body;
} BDSExprLet;

typedef struct {
    char *name;
} BDSExprVar;

typedef struct {
    BDSExprFundef *fundef;
    BDSExpr *body;
} BDSExprLetRec;

typedef struct {
    BDSExpr *fun;
    Vector *actuals;    // Vector<BDSExpr>
} BDSExprApp;

typedef struct {
    Vector *elems;      // Vector<BDSExpr>
} BDSExprTuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    BDSExpr *val;
    BDSExpr *body;
} BDSExprLetTuple;


struct BDSExpr {
    BDExprKind kind;
    union {
        int u_int;
        double u_double;
        char u_char;
        char *u_str;
        BDSExprUniOp u_uniop;
        BDSExprBinOp u_binop;
        BDSExprIf u_if;
        BDSExprLet u_let;
        BDSExprVar u_var;
        BDSExprLetRec u_letrec;
        BDSExprApp u_app;
        BDSExprTuple u_tuple;
        BDSExprLetTuple u_lettuple;
    } u;
};

typedef struct {
    Vector *fundefs;
    Vector *datadefs;
    BDSExprFundef *maindef;
} BDSProgram;


void bd_sprogram_init(BDSProgram *prog);
void bd_sprogram_show(BDSProgram *prog);
void bd_sprogram_toplevels(Env *env, BDSProgram *prog);

BDSExpr *bd_sexpr(BDExprKind kind);
void bd_sexpr_destroy(BDSExpr *e);
void bd_sexpr_show(BDSExpr *e);

BDSExpr *bd_sexpr_unit();
BDSExpr *bd_sexpr_bool(int val);
BDSExpr *bd_sexpr_int(int val);
BDSExpr *bd_sexpr_float(double val);
BDSExpr *bd_sexpr_char(char val);
BDSExpr *bd_sexpr_str(char *val);
BDSExpr *bd_sexpr_nil();
BDSExpr *bd_sexpr_uniop(BDOpKind kind, BDSExpr *val);
BDSExpr *bd_sexpr_binop(BDOpKind kind, BDSExpr *l, BDSExpr *r);
BDSExpr *bd_sexpr_if(BDSExpr *pred, BDSExpr *t, BDSExpr *f);
BDSExpr *bd_sexpr_let(BDExprIdent *ident, BDSExpr *val, BDSExpr *body);
BDSExpr *bd_sexpr_var(const char *name);
BDSExpr *bd_sexpr_letrec(BDSExprFundef *fundef, BDSExpr *body);
BDSExpr *bd_sexpr_app(BDSExpr *fun, Vector *actuals);
BDSExpr *bd_sexpr_tuple(Vector *elems);
BDSExpr *bd_sexpr_lettuple(Vector *idents, BDSExpr *val, BDSExpr *body);

BDSExprFundef *bd_sexpr_fundef(BDExprIdent *ident, Vector *formals, BDSExpr *body);
void bd_sexpr_fundef_destroy(BDSExprFundef *fundef);

#endif
