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
    AE_CALLDIR,
    AE_STORE,
    AE_LOAD
} BDAsmExprKind;

typedef enum {
    AV_LBL,
    AV_REG,
    AV_MEM,
    AV_IMMINT
} BDAsmValKind;

typedef struct BDAsmProg BDAsmProg;
typedef struct BDAsmIns BDAsmIns;
typedef struct BDAsmInsAns BDAsmInsAns;
typedef struct BDAsmInsLet BDAsmInsLet;
typedef struct BDAsmExpr BDAsmExpr;
typedef struct BDAsmExprFundef BDAsmExprFundef;

typedef struct {
    BDAsmValKind kind;
    union {
        char *u_lbl;
        struct {
            int index;
        } u_reg;
        struct {
            int offset;
        } u_mem;
        int u_int;
    } u;
} BDAsmVal;

struct BDAsmProg {
    Vector *fundefs;    // Vector<BDAsmExprFundef>
    BDAsmIns *main;
};

struct BDAsmInsAns {
    BDAsmExpr *expr;
};

struct BDAsmInsLet {
    BDAsmVal *lbl;
    BDType *type;
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

struct BDAsmExpr {
    BDAsmExprKind kind;
    union {
        struct {
            BDOpKind kind;
            BDAsmVal *val;
        } u_uniop;
        struct {
            BDOpKind kind;
            BDAsmVal *l;
            BDAsmVal *r;
        } u_binop;
        struct {
            BDOpKind kind;
            BDAsmVal *l;
            BDAsmVal *r;
            BDAsmIns *t;
            BDAsmIns *f;
        } u_if;
        struct {
            BDAsmVal *fun;
            Vector *actuals;    // Vector<String>
        } u_call;
        struct {
            BDAsmVal *lbl;
            char *name;
        } u_store;
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
BDAsmIns *bd_asmins_let(BDAsmVal *lbl, BDType *type, BDAsmExpr *val, BDAsmIns *body);
void bd_asmins_destroy(BDAsmIns *ins);

BDAsmExpr *bd_asmexpr_nop();
BDAsmExpr *bd_asmexpr_set(BDAsmVal *val);
BDAsmExpr *bd_asmexpr_mov(BDAsmVal *lbl);
BDAsmExpr *bd_asmexpr_store(BDAsmVal *lbl, const char *name);
BDAsmExpr *bd_asmexpr_load(BDAsmVal *lbl);
BDAsmExpr *bd_asmexpr_uniop(BDOpKind kind, BDAsmVal *val);
BDAsmExpr *bd_asmexpr_binop(BDOpKind kind, BDAsmVal *l, BDAsmVal *r);
BDAsmExpr *bd_asmexpr_if(BDOpKind kind, BDAsmVal *l, BDAsmVal *r, BDAsmIns *t, BDAsmIns *f);
BDAsmExpr *bd_asmexpr_call(BDAsmExprKind kind, BDAsmVal *fun, Vector *actuals);
void bd_asmexpr_destroy(BDAsmExpr *e);

BDAsmVal *bd_asmval_lbl(const char *lbl);
BDAsmVal *bd_asmval_reg(int index);
BDAsmVal *bd_asmval_mem(int offset);
BDAsmVal *bd_asmval_int(int imm);
BDAsmVal *bd_asmval_clone(BDAsmVal *val);
void bd_asmval_destroy(BDAsmVal *val);
char *bd_asmval_emit(BDAsmVal *val);

BDAsmExprFundef *bd_asmexpr_fundef(const char *name, BDType *type, Vector *formals, BDAsmIns *body);
void bd_asmexpr_fundef_destory(BDAsmExprFundef *fundef);

BDAsmIns *bd_asmins_concat(BDAsmIns *e1, BDAsmVal *lbl, BDType *type, BDAsmIns *e2);

#endif
