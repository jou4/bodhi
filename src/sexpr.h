#ifndef _sexpr_h_
#define _sexpr_h_

#include "expr.h"

typedef struct BDSExpr BDSExpr;

typedef struct {
    BDExprIdent *ident;
    BDSExpr *body;
} BDSExprDef;

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
    BDExprIdent *ident;
    BDSExpr *fun;
    BDSExpr *body;
} BDSExprLetRec;

typedef struct {
    BDSExpr *fun;
    Vector *actuals;    // Vector<BDSExpr>
} BDSExprApp;

typedef struct {
    char *fun;
    Vector *actuals;    // Vector<BDSExpr>
} BDSExprCCall;

typedef struct {
    Vector *elems;      // Vector<BDSExpr>
} BDSExprTuple;

typedef struct {
    Vector *idents;     // Vector<BDExprIdent>
    BDSExpr *val;
    BDSExpr *body;
} BDSExprLetTuple;

typedef struct {
    BDType *type;
    Vector *formals;    // Vector<BDExprIdent>
    BDSExpr *body;
} BDSExprFun;

typedef struct {
    BDSExpr *pattern;
    BDSExpr *body;
} BDSExprMatchBranch;

typedef struct {
    BDSExpr *target;
    Vector *branches;   // Vector<BDSExprMatchBranch>
} BDSExprMatch;

typedef struct {
    BDExprIdent *ident;
} BDSExprPatternVar;

typedef struct {
    BDSExpr *body;
} BDSExprCont;


struct BDSExpr {
    BDExprKind kind;
    union {
        long u_int;
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
        BDSExprCCall u_ccall;
        BDSExprTuple u_tuple;
        BDSExprLetTuple u_lettuple;
        BDSExprFun u_fun;
        BDSExprMatch u_match;
        BDSExprPatternVar u_patvar;
        BDSExprCont u_cont;
    } u;
};

typedef struct {
    Vector *defs;       // Vector<BDSExprdef>
    BDSExprDef *maindef;
} BDSProgram;


void bd_sprogram_init(BDSProgram *prog);
void bd_sprogram_destroy(BDSProgram *prog);
void bd_sprogram_show(BDSProgram *prog);

BDSExpr *bd_sexpr(BDExprKind kind);
void bd_sexpr_destroy(BDSExpr *e);
void bd_sexpr_show(BDSExpr *e);

BDSExpr *bd_sexpr_unit();
BDSExpr *bd_sexpr_bool(long val);
BDSExpr *bd_sexpr_int(long val);
BDSExpr *bd_sexpr_float(double val);
BDSExpr *bd_sexpr_char(char val);
BDSExpr *bd_sexpr_str(char *val);
BDSExpr *bd_sexpr_fun(BDType *type, Vector *formals, BDSExpr *body);
BDSExpr *bd_sexpr_nil();
BDSExpr *bd_sexpr_uniop(BDOpKind kind, BDSExpr *val);
BDSExpr *bd_sexpr_binop(BDOpKind kind, BDSExpr *l, BDSExpr *r);
BDSExpr *bd_sexpr_if(BDSExpr *pred, BDSExpr *t, BDSExpr *f);
BDSExpr *bd_sexpr_let(BDExprIdent *ident, BDSExpr *val, BDSExpr *body);
BDSExpr *bd_sexpr_var(const char *name);
BDSExpr *bd_sexpr_letrec(BDExprIdent *ident, BDSExpr *fun, BDSExpr *body);
BDSExpr *bd_sexpr_app(BDSExpr *fun, Vector *actuals);
BDSExpr *bd_sexpr_ccall(const char *fun, Vector *actuals);
BDSExpr *bd_sexpr_tuple(Vector *elems);
BDSExpr *bd_sexpr_lettuple(Vector *idents, BDSExpr *val, BDSExpr *body);
BDSExpr *bd_sexpr_match(BDSExpr *target, Vector *branches);
BDSExpr *bd_sexpr_pattern_var(BDExprIdent *ident);
BDSExpr *bd_sexpr_error(const char *message);
BDSExpr *bd_sexpr_cont(BDSExpr *body);
BDSExprMatchBranch *bd_sexpr_match_branch(BDSExpr *pattern, BDSExpr *body);
void bd_sexpr_match_branch_destroy(BDSExprMatchBranch *branch);

BDSExprDef *bd_sexpr_def(BDExprIdent *ident, BDSExpr *body);
void bd_sexpr_def_destroy(BDSExprDef *def);

#endif
