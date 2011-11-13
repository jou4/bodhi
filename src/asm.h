#ifndef _asm_h_
#define _asm_h_

#include "expr.h"
#include "id.h"

#define SIZE_ALIGN 8
#define SIZE_CHAR 1
#define SIZE_INT 8
#define SIZE_FLOAT 8
#define SIZE_LBL 8

#define ARG_REG_NUM 6
#define FARG_REG_NUM 8
#define TAIL_JMP_THREASHOLD 0
#define STACK_ALIGN 16

typedef enum {
    RNONE,

    RACC,
    RBP,
    RSP,
    RHP,
    RARG1,
    RARG2,
    RARG3,
    RARG4,
    RARG5,
    RARG6,
    RFARG1,
    RFARG2,
    RFARG3,
    RFARG4,
    RFARG5,
    RFARG6,
    RFARG7,
    RFARG8,

    REXT1,
    REXT2,
    REXT3,
    REXT4,
    REXT5,

    RFACC,
    RFEXT1,
    RFEXT2,
    RFEXT3,
    RFEXT4,
    RFEXT5,
} BDReg;


#define reg_acc "%rax"
#define reg_hp "%r15"
#define reg_bp "%rbp"
#define reg_sp "%rsp"
#define reg_ip "%rip"

#define reg_arg1 "%rdi"
#define reg_arg2 "%rsi"
#define reg_arg3 "%rdx"
#define reg_arg4 "%rcx"
#define reg_arg5 "%r8"
#define reg_arg6 "%r9"

#define reg_farg1 "%xmm0"
#define reg_farg2 "%xmm1"
#define reg_farg3 "%xmm2"
#define reg_farg4 "%xmm3"
#define reg_farg5 "%xmm4"
#define reg_farg6 "%xmm5"
#define reg_farg7 "%xmm6"
#define reg_farg8 "%xmm7"

#define reg_ext1 "%r10"
#define reg_ext2 "%r11"
#define reg_ext3 "%r12"
#define reg_ext4 "%r13"
#define reg_ext5 "%r14"

char *reg_name(BDReg reg);
BDReg argreg(int offset);
BDReg fargreg(int offset);

typedef union {
	int i[2];
	double d;
} Dbl;

int gethi(double d);
int getlo(double d);

typedef enum {
    AE_ANS,
    AE_LET,
} BDAExprKind;

typedef enum {
    AI_NOP,

    AI_SET_C,
    AI_SET_I,

    AI_SETGLOBAL_C,
    AI_SETGLOBAL_I,
    AI_SETGLOBAL_F,
    AI_SETGLOBAL_L,

    AI_MOV,
    AI_MOV_F,
    AI_MOV_L,
    AI_MOVGLOBAL,
    AI_MOVGLOBAL_F,
    AI_MOVGLOBAL_L,

    AI_NEG,
    AI_ADD,
    AI_SUB,
    AI_MUL,
    AI_DIV,

    AI_FNEG,
    AI_FADD,
    AI_FSUB,
    AI_FMUL,
    AI_FDIV,

    AI_IFEQ,
    AI_IFLE,

    AI_CALLCLS,
    AI_CALLDIR,
    AI_CCALL,

    AI_CALLPTR,
    AI_CALL,
    AI_TAILCALLPOINT,

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

    AI_MAKECLS,
    AI_LOADFVS,
    AI_GETCLS_ENTRY,

    AI_PUSHFV_C,
    AI_PUSHFV_I,
    AI_PUSHFV_F,
    AI_PUSHFV_L,

    AI_GETFV_C,
    AI_GETFV_I,
    AI_GETFV_F,
    AI_GETFV_L,

    AI_MAKETUPLE,
    AI_LOADELMS,
    AI_PUSHELM,
    AI_GETELM,

    AI_MAKESTRING,

    AI_SAVE,
    AI_RESTORE,

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
        long u_int;
        double u_double;
        char u_char;
        Vector *u_elems;
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
            Vector *ilist;      // Vector<BDExprIdent>
            Vector *flist;      // Vector<BDExprIdent>
        } u_call;
    } u;
};

typedef struct {
    BDExprIdent *ident;
    Vector *iformals;
    Vector *fformals;
    Vector *freevars;
    BDAExpr *body;
} BDAExprDef;

typedef enum {
    AEC_CHAR,
    AEC_INT,
    AEC_FLOAT,
    AEC_STR,
} BDAExprConstKind;

typedef struct {
    char *lbl;
    BDType *type;
    BDAExprConstKind kind;
    union {
        char u_char;
        long u_int;
        double u_double;
        char *u_str;
    } u;
} BDAExprConst;

typedef struct {
    Vector *consts;     // Vector<BDAExprConst>
    Vector *globals;    // Vector<BDExprIdent>
    Vector *fundefs;    // Vector<BDAExprDef>
    BDAExprDef *maindef;
} BDAProgram;

void bd_aprogram_init(BDAProgram *prog);
void bd_aprogram_destroy(BDAProgram *prog);
void bd_aprogram_show(BDAProgram *prog);

BDAExprDef *bd_aexpr_def(BDExprIdent *ident, Vector *iformals, Vector *fformals, Vector *freevars, BDAExpr *body);
BDAExprConst *bd_aexpr_const(BDAExprConstKind kind, char *lbl, BDType *type);
BDAExprConst *bd_aexpr_const_char(char *lbl, char val);
BDAExprConst *bd_aexpr_const_int(char *lbl, long val);
BDAExprConst *bd_aexpr_const_float(char *lbl, double val);
BDAExprConst *bd_aexpr_const_str(char *lbl, char *val);

BDAExpr *bd_aexpr_let(BDExprIdent *ident, BDAInst *val, BDAExpr *body);
BDAExpr *bd_aexpr_nonelet(BDAInst *val, BDAExpr *body);
BDAExpr *bd_aexpr_ans(BDAInst *val);

BDAInst *bd_ainst(BDAInstKind kind);
#define bd_ainst_nop() bd_ainst(AI_NOP)
BDAInst *bd_ainst_setc(char val);
BDAInst *bd_ainst_seti(long val);
BDAInst *bd_ainst_setglobal(BDType *type, char *l, char *r);
#define bd_ainst_setglobal_c(l, r) _ainst_binreg(AI_SETGLOBAL_C, l, r)
#define bd_ainst_setglobal_i(l, r) _ainst_binreg(AI_SETGLOBAL_I, l, r)
#define bd_ainst_setglobal_f(l, r) _ainst_binreg(AI_SETGLOBAL_F, l, r)
#define bd_ainst_setglobal_l(l, r) _ainst_binreg(AI_SETGLOBAL_L, l, r)
BDAInst *bd_ainst_mov(char *lbl);
#define bd_ainst_mov_f(lbl) _ainst_unireg(AI_MOV_F, lbl)
#define bd_ainst_mov_l(lbl) _ainst_unireg(AI_MOV_L, lbl)
#define bd_ainst_movglobal(lbl) _ainst_unireg(AI_MOVGLOBAL, lbl)
#define bd_ainst_movglobal_f(lbl) _ainst_unireg(AI_MOVGLOBAL_F, lbl)
#define bd_ainst_movglobal_l(lbl) _ainst_unireg(AI_MOVGLOBAL_L, lbl)
BDAInst *_ainst_unireg(BDAInstKind kind, char *reg);
BDAInst *_ainst_binreg(BDAInstKind kind, char *l, char *r);
#define bd_ainst_neg(lbl) _ainst_unireg(AI_NEG, lbl)
#define bd_ainst_add(l, r) _ainst_binreg(AI_ADD, l, r)
#define bd_ainst_sub(l, r) _ainst_binreg(AI_SUB, l, r)
#define bd_ainst_mul(l, r) _ainst_binreg(AI_MUL, l, r)
#define bd_ainst_div(l, r) _ainst_binreg(AI_DIV, l, r)
#define bd_ainst_fneg(lbl) _ainst_unireg(AI_FNEG, lbl)
#define bd_ainst_fadd(l, r) _ainst_binreg(AI_FADD, l, r)
#define bd_ainst_fsub(l, r) _ainst_binreg(AI_FSUB, l, r)
#define bd_ainst_fmul(l, r) _ainst_binreg(AI_FMUL, l, r)
#define bd_ainst_fdiv(l, r) _ainst_binreg(AI_FDIV, l, r)
BDAInst *_ainst_if(BDAInstKind kind, char *l, char *r, BDAExpr *t, BDAExpr *f);
#define bd_ainst_ifeq(l, r, t, f) _ainst_if(AI_IFEQ, l, r, t, f)
#define bd_ainst_ifle(l, r, t, f) _ainst_if(AI_IFLE, l, r, t, f)
BDAInst *_ainst_call(BDAInstKind kind, char *lbl, Vector *ilist, Vector *flist);
#define bd_ainst_call(lbl) _ainst_call(AI_CALL, lbl, NULL, NULL)
#define bd_ainst_callptr(lbl) _ainst_call(AI_CALLPTR, lbl, NULL, NULL)
#define bd_ainst_callcls(lbl, ilist, flist) _ainst_call(AI_CALLCLS, lbl, ilist, flist)
#define bd_ainst_calldir(lbl, ilist, flist) _ainst_call(AI_CALLDIR, lbl, ilist, flist)
#define bd_ainst_ccall(lbl, ilist, flist) _ainst_call(AI_CCALL, lbl, ilist, flist)
#define bd_ainst_tailcall_point() bd_ainst(AI_TAILCALLPOINT)
BDAInst *_ainst_jmpif(BDAInstKind kind, char *l, char *r, char *lbl);
#define bd_ainst_jeq(l, r, lbl) _ainst_jmpif(AI_JEQ, l, r, lbl)
#define bd_ainst_jle(l, r, lbl) _ainst_jmpif(AI_JLE, l, r, lbl)
BDAInst *bd_ainst_pushlcl(BDType *type, char *reg, int offset);
#define bd_ainst_pushlcl_c(reg, offset) _ainst_push_offset(AI_PUSHLCL_C, reg, offset)
#define bd_ainst_pushlcl_i(reg, offset) _ainst_push_offset(AI_PUSHLCL_I, reg, offset)
#define bd_ainst_pushlcl_f(reg, offset) _ainst_push_offset(AI_PUSHLCL_F, reg, offset)
#define bd_ainst_pushlcl_l(reg, offset) _ainst_push_offset(AI_PUSHLCL_L, reg, offset)
BDAInst *bd_ainst_getlcl(BDType *type, int offset);
#define bd_ainst_getlcl_c(offset) _ainst_get_offset(AI_GETLCL_C, offset)
#define bd_ainst_getlcl_i(offset) _ainst_get_offset(AI_GETLCL_I, offset)
#define bd_ainst_getlcl_f(offset) _ainst_get_offset(AI_GETLCL_F, offset)
#define bd_ainst_getlcl_l(offset) _ainst_get_offset(AI_GETLCL_L, offset)
BDAInst *bd_ainst_pusharg(BDType *type, char *reg, int offset);
#define bd_ainst_pusharg_c(reg, offset) _ainst_push_offset(AI_PUSHARG_C, reg, offset)
#define bd_ainst_pusharg_i(reg, offset) _ainst_push_offset(AI_PUSHARG_I, reg, offset)
#define bd_ainst_pusharg_f(reg, offset) _ainst_push_offset(AI_PUSHARG_F, reg, offset)
#define bd_ainst_pusharg_l(reg, offset) _ainst_push_offset(AI_PUSHARG_L, reg, offset)
BDAInst *bd_ainst_getarg(BDType *type, int offset);
#define bd_ainst_getarg_c(offset) _ainst_get_offset(AI_GETARG_C, offset)
#define bd_ainst_getarg_i(offset) _ainst_get_offset(AI_GETARG_I, offset)
#define bd_ainst_getarg_f(offset) _ainst_get_offset(AI_GETARG_F, offset)
#define bd_ainst_getarg_l(offset) _ainst_get_offset(AI_GETARG_L, offset)

BDAInst *bd_ainst_makecls(char *lbl, int size);
BDAInst *_ainst_push_offset(BDAInstKind kind, char *lbl, int offset);
BDAInst *_ainst_get_offset(BDAInstKind kind, int offset);
#define bd_ainst_loadfvs(lbl) _ainst_unireg(AI_LOADFVS, lbl)
#define bd_ainst_getcls_entry(lbl) _ainst_unireg(AI_GETCLS_ENTRY, lbl)
BDAInst *bd_ainst_pushfv(BDType *type, char *lbl, int offset);
#define bd_ainst_pushfv_c(lbl, offset) _ainst_push_offset(AI_PUSHFV_C, lbl, offset)
#define bd_ainst_pushfv_i(lbl, offset) _ainst_push_offset(AI_PUSHFV_I, lbl, offset)
#define bd_ainst_pushfv_f(lbl, offset) _ainst_push_offset(AI_PUSHFV_F, lbl, offset)
#define bd_ainst_pushfv_l(lbl, offset) _ainst_push_offset(AI_PUSHFV_L, lbl, offset)
BDAInst *bd_ainst_getfv(BDType *type, int offset);
#define bd_ainst_getfv_c(offset) _ainst_get_offset(AI_GETFV_C, offset)
#define bd_ainst_getfv_i(offset) _ainst_get_offset(AI_GETFV_I, offset)
#define bd_ainst_getfv_f(offset) _ainst_get_offset(AI_GETFV_F, offset)
#define bd_ainst_getfv_l(offset) _ainst_get_offset(AI_GETFV_L, offset)
BDAInst *bd_ainst_maketuple(int size);
#define bd_ainst_loadelms(lbl) _ainst_unireg(AI_LOADELMS, lbl)
#define bd_ainst_pushelm(lbl, offset) _ainst_push_offset(AI_PUSHELM, lbl, offset)
#define bd_ainst_getelm(offset) _ainst_get_offset(AI_GETELM, offset)
#define bd_ainst_makestring(lbl) _ainst_unireg(AI_MAKESTRING, lbl)

#define bd_ainst_save(lbl, reg) _ainst_binreg(AI_SAVE, lbl, reg)
#define bd_ainst_restore(lbl) _ainst_unireg(AI_RESTORE, lbl)

BDAExpr *bd_aexpr_concat(BDAExpr *e1, BDExprIdent *ident, BDAExpr *e2);
int bd_value_size(BDType *type);

#endif
