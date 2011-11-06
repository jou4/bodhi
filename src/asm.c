#include "asm.h"

void bd_aprogram_init(BDAProgram *prog)
{
    prog->datadefs = vector_new();
    prog->fundefs = vector_new();
    prog->maindef = NULL;
}

void bd_aprogram_destroy(BDAProgram *prog)
{
    printf("\n");
    printf("**************************\n");
    printf("TODO bd_aprogram_destroy\n");
    printf("**************************\n");
    printf("\n");
}

BDAExpr *bd_aexpr(BDAExprKind kind)
{
    BDAExpr *e = malloc(sizeof(BDAExpr));
    e->kind = kind;
    return e;
}

BDAExpr *bd_aexpr_let(BDExprIdent *ident, BDAInst *val, BDAExpr *body)
{
    BDAExpr *e = bd_aexpr(AE_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDAExpr *bd_aexpr_ans(BDAInst *val)
{
    BDAExpr *e = bd_aexpr(AE_ANS);
    e->u.u_ans.val = val;
    return e;
}

BDAInst *bd_ainst(BDAInstKind kind)
{
    BDAInst *inst = malloc(sizeof(BDAInst));
    inst->kind = kind;
    return inst;
}

BDAExprDef *bd_aexpr_def(BDExprIdent *ident, Vector *iformals, Vector *fformals, BDAExpr *body)
{
    BDAExprDef *def = malloc(sizeof(BDAExprDef));
    def->ident = ident;
    def->iformals = iformals;
    def->fformals = fformals;
    def->body = body;
    return def;
}

BDAExprConst *bd_aexpr_const(BDAExprConstKind kind, char *lbl)
{
    BDAExprConst *c = malloc(sizeof(BDAExprConst));
    c->kind = kind;
    c->lbl = lbl;
    return c;
}

BDAExprConst *bd_aexpr_const_int(char *lbl, int val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_INT, lbl);
    c->u.u_int = val;
    return c;
}

BDAExprConst *bd_aexpr_const_float(char *lbl, double val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_FLOAT, lbl);
    c->u.u_double = val;
    return c;
}

BDAExprConst *bd_aexpr_const_str(char *lbl, char *val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_STR, lbl);
    c->u.u_str = val;
    return c;
}


BDAInst *bd_ainst_setc(char val)
{
    BDAInst *inst = bd_ainst(AI_SET_C);
    inst->u.u_char = val;
    return inst;
}

BDAInst *bd_ainst_seti(int val)
{
    BDAInst *inst = bd_ainst(AI_SET_I);
    inst->u.u_int = val;
    return inst;
}

BDAInst *bd_ainst_setl(char *lbl)
{
    BDAInst *inst = bd_ainst(AI_SET_L);
    inst->lbl = lbl;
    return inst;
}

BDAInst *bd_ainst_mov(char *lbl)
{
    BDAInst *inst = bd_ainst(AI_MOV);
    inst->lbl = lbl;
    return inst;
}

BDAInst *_ainst_binreg(BDAInstKind kind, char *l, char *r)
{
    BDAInst *inst = bd_ainst(kind);
    inst->lbl = l;
    inst->u.u_rlbl = r;
    return inst;
}

BDAInst *_ainst_if(BDAInstKind kind, char *l, char *r, BDAExpr *t, BDAExpr *f)
{
    BDAInst *inst = bd_ainst(kind);
    inst->u.u_if.l = l;
    inst->u.u_if.r = r;
    inst->u.u_if.t = t;
    inst->u.u_if.f = f;
    return inst;
}

BDAInst *_ainst_call(BDAInstKind kind, char *lbl, Vector *ilist, Vector *flist)
{
    BDAInst *inst = bd_ainst(kind);
    inst->lbl = lbl;
    inst->u.u_call.ilist = ilist;
    inst->u.u_call.flist = flist;
    return inst;
}

BDAInst *bd_ainst_jmp(char *lbl)
{
    BDAInst *inst = bd_ainst(AI_JMP);
    inst->lbl = lbl;
    return inst;
}

BDAInst *_ainst_jmpif(BDAInstKind kind, char *l, char *r, char *lbl)
{
    BDAInst *inst = bd_ainst(kind);
    inst->u.u_bin.l = l;
    inst->u.u_bin.r = r;
    inst->lbl = lbl;
    return inst;
}

BDAInst *_ainst_unireg(BDAInstKind kind, char *reg)
{
    BDAInst *inst = bd_ainst(kind);
    inst->lbl = reg;
    return inst;
}

void _bd_aexpr_show(BDAExpr *e, int col, int depth);

void _bd_ainst_show(BDAInst *inst, int col, int depth)
{
    switch(inst->kind){
        case AI_NOP:
            PRINT(col, "NOP");
            break;

        case AI_SET_C:
            PRINT1(col, "SET '%c'", inst->u.u_char);
            break;
        case AI_SET_I:
            PRINT1(col, "SET %d", inst->u.u_int);
            break;
        case AI_SET_L:
            PRINT1(col, "SET %s", inst->lbl);
            break;

        case AI_MOV:
            PRINT1(col, "MOV %s", inst->lbl);
            break;

        case AI_ADD:
            PRINT2(col, "ADD %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_SUB:
            PRINT2(col, "SUB %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_MUL:
            PRINT2(col, "MUL %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_DIV:
            PRINT2(col, "DIV %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;

        case AI_FADD:
            PRINT2(col, "FADD %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_FSUB:
            PRINT2(col, "FSUB %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_FMUL:
            PRINT2(col, "FMUL %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_FDIV:
            PRINT2(col, "FDIV %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;

        case AI_IFEQ:
            PRINT2(col, "IF %s == %s\n", inst->u.u_if.l, inst->u.u_if.r);
            _bd_aexpr_show(inst->u.u_if.t, col, depth + 1);
            _bd_aexpr_show(inst->u.u_if.f, col, depth + 1);
            break;
        case AI_IFLE:
            PRINT2(col, "IF %s <= %s\n", inst->u.u_if.l, inst->u.u_if.r);
            _bd_aexpr_show(inst->u.u_if.t, col, depth + 1);
            _bd_aexpr_show(inst->u.u_if.f, col, depth + 1);
            break;

        case AI_CALLCLS:
        case AI_CALLDIR:
        case AI_CCALL:
            {
                switch(inst->kind){
                    case AI_CALLCLS:
                        PRINT(col, "CALLCLS");
                        break;
                    case AI_CALLDIR:
                        PRINT(col, "CALLDIR");
                        break;
                    case AI_CCALL:
                        PRINT(col, "CCALL");
                        break;
                }

                Vector *ilist = inst->u.u_call.ilist;
                Vector *flist = inst->u.u_call.flist;
                BDExprIdent *ident;
                int i;

                PRINT(col, "[");
                for(i = 0; i < ilist->length; i++){
                    if(i > 0){
                        PRINT(col, " ");
                    }
                    ident = vector_get(ilist, i);
                    PRINT1(col, "%s", bd_expr_ident_show(ident));
                }
                PRINT(col, "]");
                PRINT(col, " ");
                PRINT(col, "[");
                for(i = 0; i < flist->length; i++){
                    if(i > 0){
                        PRINT(col, " ");
                    }
                    ident = vector_get(flist, i);
                    PRINT1(col, "%s", bd_expr_ident_show(ident));
                }
                PRINT(col, "]");
            }
            break;

        case AI_JMP:
            PRINT1(col, "JMP %s", inst->lbl);
            break;
        case AI_JEQ:
            PRINT3(col, "JEQ %s, %s, %s", inst->u.u_bin.l, inst->u.u_bin.r, inst->lbl);
            break;
        case AI_JLE:
            PRINT3(col, "JLE %s, %s, %s", inst->u.u_bin.l, inst->u.u_bin.r, inst->lbl);
            break;

        case AI_PUSHLCL_C:
            PRINT1(col, "PUSHLCL_C %s", inst->lbl);
            break;
        case AI_PUSHLCL_I:
            PRINT1(col, "PUSHLCL_I %s", inst->lbl);
            break;
        case AI_PUSHLCL_F:
            PRINT1(col, "PUSHLCL_F %s", inst->lbl);
            break;
        case AI_PUSHLCL_L:
            PRINT1(col, "PUSHLCL_L %s", inst->lbl);
            break;

        case AI_GETLCL_C:
            PRINT1(col, "GETLCL_C %s", inst->lbl);
            break;
        case AI_GETLCL_I:
            PRINT1(col, "GETLCL_I %s", inst->lbl);
            break;
        case AI_GETLCL_F:
            PRINT1(col, "GETLCL_F %s", inst->lbl);
            break;
        case AI_GETLCL_L:
            PRINT1(col, "GETLCL_L %s", inst->lbl);
            break;

        case AI_PUSHARG_C:
            PRINT1(col, "PUSHARG_C %s", inst->lbl);
            break;
        case AI_PUSHARG_I:
            PRINT1(col, "PUSHARG_I %s", inst->lbl);
            break;
        case AI_PUSHARG_F:
            PRINT1(col, "PUSHARG_F %s", inst->lbl);
            break;
        case AI_PUSHARG_L:
            PRINT1(col, "PUSHARG_L %s", inst->lbl);
            break;

        case AI_GETARG_C:
            PRINT1(col, "GETARG_C %s", inst->lbl);
            break;
        case AI_GETARG_I:
            PRINT1(col, "GETARG_I %s", inst->lbl);
            break;
        case AI_GETARG_F:
            PRINT1(col, "GETARG_F %s", inst->lbl);
            break;
        case AI_GETARG_L:
            PRINT1(col, "GETARG_L %s", inst->lbl);
            break;
    }
}

void _bd_aexpr_show(BDAExpr *e, int col, int depth)
{
    PRINT(col, "\t");
    bd_show_indent(depth);

    switch(e->kind){
        case AE_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                BDAInst *val = e->u.u_let.val;
                BDAExpr *body = e->u.u_let.body;

                PRINT1(col, "%s = ", bd_expr_ident_show(ident));
                _bd_ainst_show(val, col, depth + 1);
                PRINT(col, " in \n");
                _bd_aexpr_show(body, col, depth);
            }
            break;
        case AE_ANS:
            {
                PRINT(col, "(");
                _bd_ainst_show(e->u.u_ans.val, col, depth);
                PRINT(col, ")");
            }
            break;
    }

    PRINT(col, "\n");
}

void bd_aprogram_const_show(BDAExprConst *c)
{
    int i, col = 0, depth = 0;

    PRINT1(col, "%s:\n", c->lbl);
    PRINT(col, "\t");

    switch(c->kind){
        case AEC_INT:
            PRINT1(col, "%d", c->u.u_int);
            break;
        case AEC_FLOAT:
            PRINT1(col, "%.14g", c->u.u_double);
            break;
        case AEC_STR:
            PRINT1(col, "%s", c->u.u_str);
            break;
    }

    PRINT(col, "\n");
}

void bd_aprogram_def_show(BDAExprDef *def)
{
    Vector *vec;
    int i, col = 0, depth = 0;

    PRINT1(col, "%s:\n", bd_expr_ident_show(def->ident));
    _bd_aexpr_show(def->body, col, depth);
    PRINT(col, "\n");
}

void bd_aprogram_show(BDAProgram *prog)
{
    Vector *vec;
    int i;

    printf("*const*\n");
    vec = prog->consts;
    for(i = 0; i < vec->length; i++){
        bd_aprogram_const_show(vector_get(vec, i));
    }
    printf("\n");

    printf("*data*\n");
    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        bd_aprogram_def_show(vector_get(vec, i));
    }
    printf("\n");

    printf("*function*\n");
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        bd_aprogram_def_show(vector_get(vec, i));
    }

    bd_aprogram_def_show(prog->maindef);
    printf("\n");
}
