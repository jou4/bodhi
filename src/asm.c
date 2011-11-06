#include "asm.h"

void bd_aprogram_init(BDAProgram *prog)
{
    prog->consts = vector_new();
    prog->globals = vector_new();
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

BDAExprDef *bd_aexpr_def(BDExprIdent *ident, Vector *iformals, Vector *fformals, Vector *freevars, BDAExpr *body)
{
    BDAExprDef *def = malloc(sizeof(BDAExprDef));
    def->ident = ident;
    def->iformals = iformals;
    def->fformals = fformals;
    def->freevars = freevars;
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

BDAExprConst *bd_aexpr_const_char(char *lbl, char val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_CHAR, lbl);
    c->u.u_char = val;
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
    inst->u.u_bin.l = l;
    inst->u.u_bin.r = r;
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

BDAInst *bd_ainst_makecls(char *lbl, int size)
{
    BDAInst *inst = bd_ainst(AI_MAKECLS);
    inst->lbl = lbl;
    inst->u.u_int = size;
    return inst;
}

BDAInst *_ainst_push_offset(BDAInstKind kind, char *lbl, int offset)
{
    BDAInst *inst = bd_ainst(kind);
    inst->lbl = lbl;
    inst->u.u_int = offset;
    return inst;
}

BDAInst *_ainst_get_offset(BDAInstKind kind, int offset)
{
    BDAInst *inst = bd_ainst(kind);
    inst->u.u_int = offset;
    return inst;
}

BDAInst *bd_ainst_maketuple(Vector *elems)
{
    BDAInst *inst = bd_ainst(AI_MAKETUPLE);
    inst->u.u_elems = elems;
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

        case AI_SETGLOBAL:
            PRINT2(col, "SETGLOBAL %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
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

                PRINT1(col, " %s ", inst->lbl);

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

        case AI_MAKECLS:
            PRINT2(col, "MAKECLS %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_LOADFVS:
            PRINT1(col, "LOADFVS %s", inst->lbl);
            break;
        case AI_GETCLS_ENTRY:
            PRINT1(col, "GETCLS_ENTRY %s", inst->lbl);
            break;

        case AI_PUSHFV_C:
            PRINT2(col, "PUSHFV_C %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHFV_I:
            PRINT2(col, "PUSHFV_I %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHFV_F:
            PRINT2(col, "PUSHFV_F %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHFV_L:
            PRINT2(col, "PUSHFV_L %s, %d", inst->lbl, inst->u.u_int);
            break;

        case AI_GETFV_C:
            PRINT1(col, "GETFV_C %d", inst->u.u_int);
            break;
        case AI_GETFV_I:
            PRINT1(col, "GETFV_I %d", inst->u.u_int);
            break;
        case AI_GETFV_F:
            PRINT1(col, "GETFV_F %d", inst->u.u_int);
            break;
        case AI_GETFV_L:
            PRINT1(col, "GETFV_L %d", inst->u.u_int);
            break;

        case AI_MAKETUPLE:
            {
                PRINT(col, "MAKETUPLE");

                PRINT(col, "(");

                Vector *elems = inst->u.u_elems;
                BDExprIdent *ident;
                int i;
                for(i = 0; i < elems->length; i++){
                    ident = vector_get(elems, i);
                    PRINT1(col, " %s", bd_expr_ident_show(ident));
                }

                PRINT(col, " )");
            }
            break;
        case AI_LOADELMS:
            PRINT1(col, "LOADELMS %s", inst->lbl);
            break;
        case AI_PUSHELM:
            PRINT2(col, "PUSHELM %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_GETELM:
            PRINT1(col, "GETELM %d", inst->u.u_int);
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

                if(ident->type->kind == T_UNIT){
                    PRINT(col, "_ = ");
                }
                else{
                    PRINT1(col, "%s = ", bd_expr_ident_show(ident));
                }
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

void bd_aprogram_global_show(BDExprIdent *global)
{
    int i, col = 0, depth = 0;
    PRINT1(col, "%s\n", bd_expr_ident_show(global));
}

void bd_aprogram_def_show(BDAExprDef *def)
{
    Vector *vec;
    BDExprIdent *ident;
    int i, col = 0, depth = 0;

    PRINT1(col, "%s:\n", bd_expr_ident_show(def->ident));

    vec = def->iformals;
    if(vec != NULL){
        PRINT(col, "\t(");
        for(i = 0; i < vec->length; i++){
            ident = vector_get(vec, i);
            PRINT1(col, " %s", bd_expr_ident_show(ident));
        }
        PRINT(col, " )\n");
    }

    vec = def->fformals;
    if(vec != NULL){
        PRINT(col, "\t(");
        for(i = 0; i < vec->length; i++){
            ident = vector_get(vec, i);
            PRINT1(col, " %s", bd_expr_ident_show(ident));
        }
        PRINT(col, " )\n");
    }

    vec = def->freevars;
    if(vec != NULL){
        PRINT(col, "\t(");
        for(i = 0; i < vec->length; i++){
            ident = vector_get(vec, i);
            PRINT1(col, " %s", bd_expr_ident_show(ident));
        }
        PRINT(col, " )\n");
    }

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

    printf("*global*\n");
    vec = prog->globals;
    for(i = 0; i < vec->length; i++){
        bd_aprogram_global_show(vector_get(vec, i));
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


BDAExpr *bd_aexpr_concat(BDAExpr *e1, BDExprIdent *ident, BDAExpr *e2)
{
    BDAExpr *ret;
    switch(e1->kind){
        case AE_ANS:
            ret = bd_aexpr_let(bd_expr_ident_clone(ident), e1->u.u_ans.val, e2);
            break;
        case AE_LET:
            ret = bd_aexpr_let(
                    bd_expr_ident_clone(e1->u.u_let.ident),
                    e1->u.u_let.val,
                    bd_aexpr_concat(e1->u.u_let.body, ident, e2));
            break;
    }
    free(e1);
    return ret;
}
