#ifndef _cexpr_h_
#define _cexpr_h_

#include "util.h"
#include "nexpr.h"

typedef struct BDCExpr BDCExpr;

typedef struct {
    BDExprIdent *ident;
    BDCExpr *body;
} BDCExprDef;


typedef struct {
    BDOpKind kind;
    char *val;
} BDCExprUniOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
} BDCExprBinOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
    BDCExpr *t;
    BDCExpr *f;
} BDCExprIf;

typedef struct {
    BDExprIdent *ident;
    BDCExpr *val;
    BDCExpr *body;
} BDCExprLet;

typedef struct {
    char *name;
} BDCExprVar;

typedef struct {
    BDType *type;
    Vector *formals;    // Vector<BDExprIdent>
    Vector *freevars;   // Vector<BDExprIdent>
    BDCExpr *body;
} BDCExprFun;

typedef struct {
    BDExprIdent *ident;
    BDExprClosure *closure;
    BDCExpr *body;
} BDCExprMakeCls;

typedef struct {
    char *fun;
    Vector *actuals;    // Vector<String>
} BDCExprApp;

typedef struct {
    Vector *elems;      // Vector<String>
} BDCExprTuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    char *val;
    BDCExpr *body;
} BDCExprLetTuple;


struct BDCExpr {
    BDExprKind kind;
    union {
        int u_int;
        double u_double;
        char u_char;
        char *u_str;
        BDCExprUniOp u_uniop;
        BDCExprBinOp u_binop;
        BDCExprIf u_if;
        BDCExprLet u_let;
        BDCExprVar u_var;
        BDCExprFun u_fun;
        BDCExprMakeCls u_makecls;
        BDCExprApp u_app;
        BDCExprTuple u_tuple;
        BDCExprLetTuple u_lettuple;
    } u;
};

typedef struct {
    Vector *datadefs;       // Vector<BDCExprdef>
    Vector *fundefs;        // Vector<BDCExprdef>
    BDCExprDef *maindef;
} BDCProgram;

void bd_cprogram_init(BDCProgram *prog);
void bd_cprogram_show(BDCProgram *prog);
void bd_cprogram_destroy(BDCProgram *prog);


BDCExpr *bd_cexpr(BDExprKind kind);
void bd_cexpr_destroy(BDCExpr *e);
void bd_cexpr_show(BDCExpr *e);
Set *bd_cexpr_freevars(BDCExpr *e);

BDCExpr *bd_cexpr_unit();
BDCExpr *bd_cexpr_int(int val);
BDCExpr *bd_cexpr_float(double val);
BDCExpr *bd_cexpr_char(char val);
BDCExpr *bd_cexpr_str(char *val);
BDCExpr *bd_cexpr_nil();
BDCExpr *bd_cexpr_uniop(BDOpKind kind, const char *val);
BDCExpr *bd_cexpr_binop(BDOpKind kind, const char *l, const char *r);
BDCExpr *bd_cexpr_if(BDOpKind kind, const char *l, const char *r, BDCExpr *t, BDCExpr *f);
BDCExpr *bd_cexpr_let(BDExprIdent *ident, BDCExpr *val, BDCExpr *body);
BDCExpr *bd_cexpr_var(const char *name);
BDCExpr *bd_cexpr_fun(BDType *type, Vector *formals, Vector *freevars, BDCExpr *body);
BDCExpr *bd_cexpr_makecls(BDExprIdent *ident, BDExprClosure *closure, BDCExpr *body);
BDCExpr *bd_cexpr_appcls(const char *fun, Vector *actuals);
BDCExpr *bd_cexpr_appdir(const char *fun, Vector *actuals);
BDCExpr *bd_cexpr_tuple(Vector *elems);
BDCExpr *bd_cexpr_lettuple(Vector *idents, const char *val, BDCExpr *body);

BDCExprDef *bd_cexpr_def(BDExprIdent *ident, BDCExpr *body);
void bd_cexpr_def_destroy(BDCExprDef *fundef);


#endif
