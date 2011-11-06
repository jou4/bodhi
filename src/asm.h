#ifndef _asm_h_
#define _asm_h_

#include "expr.h"

typedef enum {
    AE_ANS,
    AE_LET,
} BDAExprKind;

typedef enum {
    AI_NOP,

    AI_SET_C,
    AI_SET_I,
    AI_SET_L,

    AI_MOV,

    AI_ADD,
    AI_SUB,
    AI_MUL,
    AI_DIV,

    AI_FADD,
    AI_FSUB,
    AI_FMUL,
    AI_FDIV,

    AI_IFEQ,
    AI_IFLE,

    AI_CALLCLS,
    AI_CALLDIR,
    AI_CCALL,

    AI_JMP,
    AI_JEQ,
    AI_JLE,

    AI_PUSHLCL_C,
    AI_PUSHLCL_I,
    AI_PUSHLCL_F,
    AI_PUSHLCL_L,

    AI_GETLCL_C,
    AI_GETLCL_I,
    AI_GETLCL_F,
    AI_GETLCL_L,

    AI_PUSHARG_C,
    AI_PUSHARG_I,
    AI_PUSHARG_F,
    AI_PUSHARG_L,

    AI_GETARG_C,
    AI_GETARG_I,
    AI_GETARG_F,
    AI_GETARG_L,
} BDAInstKind;

typedef struct BDAExpr BDAExpr;
typedef struct BDAInst BDAInst;

struct BDAExpr {
    BDAExprKind kind;
    union {
        struct {
            BDExprIdent *ident;
            BDAInst *val;
            BDAExpr *body;
        } u_let;
        struct {
            BDAInst *val;
        } u_ans;
    } u;
};

struct BDAInst {
    BDAInstKind kind;
    char *lbl;
    union {
        int u_int;
        double u_double;
        char u_char;
        char *u_rlbl;
        struct {
            char *l;
            char *r;
        } u_bin;
        struct {
            char *l;
            char *r;
            BDAExpr *t;
            BDAExpr *f;
        } u_if;
        struct {
            Vector *ilist;
            Vector *flist;
        } u_call;
    } u;
};

typedef struct {
    BDExprIdent *ident;
    Vector *iformals;
    Vector *fformals;
    BDAExpr *body;
} BDAExprDef;

typedef enum {
    AEC_INT,
    AEC_FLOAT,
    AEC_STR,
} BDAExprConstKind;

typedef struct {
    BDAExprConstKind kind;
    char *lbl;
    union {
        int u_int;
        double u_double;
        char *u_str;
    } u;
} BDAExprConst;

typedef struct {
    Vector *consts;     // Vector<BDAExprConst>
    Vector *datadefs;   // Vector<BDAExprDef>
    Vector *fundefs;    // Vector<BDAExprDef>
    BDAExprDef *maindef;
} BDAProgram;

void bd_aprogram_init(BDAProgram *prog);
void bd_aprogram_destroy(BDAProgram *prog);
void bd_aprogram_show(BDAProgram *prog);

BDAExprDef *bd_aexpr_def(BDExprIdent *ident, Vector *iformals, Vector *fformals, BDAExpr *body);
BDAExprConst *bd_aexpr_const(BDAExprConstKind kind, char *lbl);
BDAExprConst *bd_aexpr_const_int(char *lbl, int val);
BDAExprConst *bd_aexpr_const_float(char *lbl, double val);
BDAExprConst *bd_aexpr_const_str(char *lbl, char *val);

BDAExpr *bd_aexpr_let(BDExprIdent *ident, BDAInst *val, BDAExpr *body);
BDAExpr *bd_aexpr_ans(BDAInst *val);

BDAInst *bd_ainst(BDAInstKind kind);
#define bd_ainst_nop() bd_ainst(AI_NOP)
BDAInst *bd_ainst_setc(char val);
BDAInst *bd_ainst_seti(int val);
BDAInst *bd_ainst_setl(char *lbl);
BDAInst *bd_ainst_mov(char *lbl);
BDAInst *_ainst_binreg(BDAInstKind kind, char *l, char *r);
#define bd_ainst_add(l, r) _ainst_binreg(AI_ADD, l, r)
#define bd_ainst_sub(l, r) _ainst_binreg(AI_SUB, l, r)
#define bd_ainst_mul(l, r) _ainst_binreg(AI_MUL, l, r)
#define bd_ainst_div(l, r) _ainst_binreg(AI_DIV, l, r)
#define bd_ainst_fadd(l, r) _ainst_binreg(AI_FADD, l, r)
#define bd_ainst_fsub(l, r) _ainst_binreg(AI_FSUB, l, r)
#define bd_ainst_fmul(l, r) _ainst_binreg(AI_FMUL, l, r)
#define bd_ainst_fdiv(l, r) _ainst_binreg(AI_FDIV, l, r)
BDAInst *_ainst_if(BDAInstKind kind, char *l, char *r, BDAExpr *t, BDAExpr *f);
#define bd_ainst_ifeq(l, r, t, f) _ainst_if(AI_IFEQ, l, r, t, f)
#define bd_ainst_ifle(l, r, t, f) _ainst_if(AI_IFLE, l, r, t, f)
BDAInst *_ainst_call(BDAInstKind kind, char *lbl, Vector *ilist, Vector *flist);
#define bd_ainst_callcls(lbl) _ainst_call(AI_CALLCLS, lbl)
#define bd_ainst_calldir(lbl) _ainst_call(AI_CALLDIR, lbl)
#define bd_ainst_ccall(lbl) _ainst_call(AI_CCALL, lbl)
BDAInst *bd_ainst_jmp(char *lbl);
BDAInst *_ainst_jmpif(BDAInstKind kind, char *l, char *r, char *lbl);
#define bd_ainst_jeq(l, r, lbl) _ainst_jmpif(AI_JEQ, l, r, lbl)
#define bd_ainst_jle(l, r, lbl) _ainst_jmpif(AI_JLE, l, r, lbl)
BDAInst *_ainst_unireg(BDAInstKind kind, char *reg);
#define bd_ainst_pushlcl_c(reg) _ainst_unireg(AI_PUSHLCL_C, reg)
#define bd_ainst_pushlcl_i(reg) _ainst_unireg(AI_PUSHLCL_I, reg)
#define bd_ainst_pushlcl_f(reg) _ainst_unireg(AI_PUSHLCL_F, reg)
#define bd_ainst_pushlcl_l(reg) _ainst_unireg(AI_PUSHLCL_L, reg)
#define bd_ainst_getlcl_c(reg) _ainst_unireg(AI_GETLCL_C, reg)
#define bd_ainst_getlcl_i(reg) _ainst_unireg(AI_GETLCL_I, reg)
#define bd_ainst_getlcl_f(reg) _ainst_unireg(AI_GETLCL_F, reg)
#define bd_ainst_getlcl_l(reg) _ainst_unireg(AI_GETLCL_L, reg)
#define bd_ainst_pusharg_c(reg) _ainst_unireg(AI_PUSHARG_C, reg)
#define bd_ainst_pusharg_i(reg) _ainst_unireg(AI_PUSHARG_I, reg)
#define bd_ainst_pusharg_f(reg) _ainst_unireg(AI_PUSHARG_F, reg)
#define bd_ainst_pusharg_l(reg) _ainst_unireg(AI_PUSHARG_L, reg)
#define bd_ainst_getarg_c(reg) _ainst_unireg(AI_GETARG_C, reg)
#define bd_ainst_getarg_i(reg) _ainst_unireg(AI_GETARG_I, reg)
#define bd_ainst_getarg_f(reg) _ainst_unireg(AI_GETARG_F, reg)
#define bd_ainst_getarg_l(reg) _ainst_unireg(AI_GETARG_L, reg)


#endif
