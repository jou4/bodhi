#ifndef _asm_h_
#define _asm_h_


#include "expr.h"


typedef enum {
    AI_ANS,
    AI_LET
} BDAsmInsKind;

typedef enum {
    AE_NOP,
    AE_SET,
    AE_MOV,
    AE_UNIOP,
    AE_BINOP,
    AE_IF,
    AE_CALLCLS,
    AE_CALLDIR
} BDAsmExprKind;

typedef struct BDAsmProg BDAsmProg;
typedef struct BDAsmIns BDAsmIns;
typedef struct BDAsmInsAns BDAsmInsAns;
typedef struct BDAsmInsLet BDAsmInsLet;
typedef struct BDAsmExpr BDAsmExpr;
typedef struct BDAsmExprFundef BDAsmExprFundef;

struct BDAsmProg {
    Vector *fundefs;    // Vector<BDAsmExprFundef>
    BDAsmIns *main;
};

struct BDAsmInsAns {
    BDAsmExpr *expr;
};

struct BDAsmInsLet {
    BDExprIdent *ident;
    BDAsmExpr *val;
    BDAsmIns *body;
};

struct BDAsmIns {
    BDAsmInsKind kind;
    union {
        BDAsmInsAns u_ans;
        BDAsmInsLet u_let;
    } u;
};

typedef struct {
    BDOpKind kind;
    char *val;
} BDAsmExprUniOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
} BDAsmExprBinOp;

typedef struct {
    BDOpKind kind;
    char *l;
    char *r;
    BDAsmIns *t;
    BDAsmIns *f;
} BDAsmExprIf;

typedef struct {
    char *fun;
    Vector *actuals;    // Vector<String>
} BDAsmExprCall;

struct BDAsmExpr {
    BDAsmExprKind kind;
    union {
        int u_int;
        char *u_lbl;
        BDAsmExprUniOp u_uniop;
        BDAsmExprBinOp u_binop;
        BDAsmExprIf u_if;
        BDAsmExprCall u_call;
    } u;
};

struct BDAsmExprFundef {
    char *name;
    BDType *type;
    Vector *formals;    // Vector<String>
    BDAsmIns *body;
};


BDAsmProg *bd_asmprog(Vector *fundefs, BDAsmIns *main);
void bd_asmprog_destroy(BDAsmProg *prog);
void bd_asmprog_show(BDAsmProg *prog);

BDAsmIns *bd_asmins_ans(BDAsmExpr *e);
BDAsmIns *bd_asmins_let(BDExprIdent *ident, BDAsmExpr *val, BDAsmIns *body);
void bd_asmins_destroy(BDAsmIns *ins);

BDAsmExpr *bd_asmexpr_nop();
BDAsmExpr *bd_asmexpr_set(int val);
BDAsmExpr *bd_asmexpr_mov(const char *lbl);
BDAsmExpr *bd_asmexpr_uniop(BDOpKind kind, const char *val);
BDAsmExpr *bd_asmexpr_binop(BDOpKind kind, const char *l, const char *r);
BDAsmExpr *bd_asmexpr_if(BDOpKind kind, const char *l, const char *r, BDAsmIns *t, BDAsmIns *f);
BDAsmExpr *bd_asmexpr_call(BDAsmExprKind kind, const char *fun, Vector *actuals);
void bd_asmexpr_destroy(BDAsmExpr *e);

BDAsmExprFundef *bd_asmexpr_fundef(const char *name, BDType *type, Vector *formals, BDAsmIns *body);
void bd_asmexpr_fundef_destory(BDAsmExprFundef *fundef);


#endif
