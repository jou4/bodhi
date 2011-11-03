#ifndef _nexpr_h_
#define _nexpr_h_

#include "util.h"
#include "sexpr.h"

typedef struct BDNExpr BDNExpr;

typedef struct {
    BDExprIdent *ident;
    Vector *formals;    // Vector<BDExprIdent>
    BDNExpr *body;
} BDNExprFundef;


typedef struct {
    BDOpKind kind;
    char *val;
} BDNExprUniOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
} BDNExprBinOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
    BDNExpr *t;
    BDNExpr *f;
} BDNExprIf;

typedef struct {
    BDExprIdent *ident;
    BDNExpr *val;
    BDNExpr *body;
} BDNExprLet;

typedef struct {
    char *name;
} BDNExprVar;

typedef struct {
    BDNExprFundef *fundef;
    BDNExpr *body;
} BDNExprLetRec;

typedef struct {
    char *fun;
    Vector *actuals;    // Vector<String>
} BDNExprApp;

typedef struct {
    Vector *elems;      // Vector<String>
} BDNExprTuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    char *val;
    BDNExpr *body;
} BDNExprLetTuple;


struct BDNExpr {
    BDExprKind kind;
    union {
        int u_int;
        double u_double;
        char u_char;
        char *u_str;
        BDNExprUniOp u_uniop;
        BDNExprBinOp u_binop;
        BDNExprIf u_if;
        BDNExprLet u_let;
        BDNExprVar u_var;
        BDNExprLetRec u_letrec;
        BDNExprApp u_app;
        BDNExprTuple u_tuple;
        BDNExprLetTuple u_lettuple;
    } u;
};

typedef struct {
    Vector *fundefs;
    Vector *datadefs;
    BDNExprFundef *maindef;
} BDNProgram;

void bd_nprogram_init(BDNProgram *prog);
void bd_nprogram_show(BDNProgram *prog);
void bd_nprogram_toplevels(Env *env, BDNProgram *prog);


BDNExpr *bd_nexpr(BDExprKind kind);
void bd_nexpr_destroy(BDNExpr *e);
void bd_nexpr_show(BDNExpr *e);
Set *bd_nexpr_freevars(BDNExpr *e);

BDNExpr *bd_nexpr_unit();
BDNExpr *bd_nexpr_int(int val);
BDNExpr *bd_nexpr_float(double val);
BDNExpr *bd_nexpr_char(char val);
BDNExpr *bd_nexpr_str(char *val);
BDNExpr *bd_nexpr_nil();
BDNExpr *bd_nexpr_uniop(BDOpKind kind, const char *val);
BDNExpr *bd_nexpr_binop(BDOpKind kind, const char *l, const char *r);
BDNExpr *bd_nexpr_if(BDOpKind kind, const char *l, const char *r, BDNExpr *t, BDNExpr *f);
BDNExpr *bd_nexpr_let(BDExprIdent *ident, BDNExpr *val, BDNExpr *body);
BDNExpr *bd_nexpr_var(const char *name);
BDNExpr *bd_nexpr_letrec(BDNExprFundef *fundef, BDNExpr *body);
BDNExpr *bd_nexpr_app(const char *fun, Vector *actuals);
BDNExpr *bd_nexpr_tuple(Vector *elems);
BDNExpr *bd_nexpr_lettuple(Vector *idents, const char *val, BDNExpr *body);

BDNExprFundef *bd_nexpr_fundef(BDExprIdent *ident, Vector *formals, BDNExpr *body);
void bd_nexpr_fundef_destroy(BDNExprFundef *fundef);

#endif
