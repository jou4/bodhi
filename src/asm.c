#include "asm.h"


char *reg_name(BDReg reg)
{
    switch(reg){
        case RACC:
            return reg_acc;
        case RBP:
            return reg_bp;
        case RSP:
            return reg_sp;
        case RHP:
            return reg_hp;
        case RARG1:
            return reg_arg1;
        case RARG2:
            return reg_arg2;
        case RARG3:
            return reg_arg3;
        case RARG4:
            return reg_arg4;
        case RARG5:
            return reg_arg5;
        case RARG6:
            return reg_arg6;
        case RFARG1:
            return reg_farg1;
        case RFARG2:
            return reg_farg2;
        case RFARG3:
            return reg_farg3;
        case RFARG4:
            return reg_farg4;
        case RFARG5:
            return reg_farg5;
        case RFARG6:
            return reg_farg6;
        case RFARG7:
            return reg_farg7;
        case RFARG8:
            return reg_farg8;
        case REXT1:
            return reg_ext1;
        case REXT2:
            return reg_ext2;
        case REXT3:
            return reg_ext3;
        case REXT4:
            return reg_ext4;
        case REXT5:
            return reg_ext5;
    }
    return NULL;
}

BDReg argreg(int offset){
    switch(offset){
        case 0:
            return RARG1;
        case 1:
            return RARG2;
        case 2:
            return RARG3;
        case 3:
            return RARG4;
        case 4:
            return RARG5;
        case 5:
            return RARG6;
        default:
            return RNONE;
    }
}

BDReg fargreg(int offset){
    switch(offset){
        case 0:
            return RFARG1;
        case 1:
            return RFARG2;
        case 2:
            return RFARG3;
        case 3:
            return RFARG4;
        case 4:
            return RFARG5;
        case 5:
            return RFARG6;
        case 6:
            return RFARG7;
        case 7:
            return RFARG8;
        default:
            return RNONE;
    }
}


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

BDAExprConst *bd_aexpr_const(BDAExprConstKind kind, char *lbl, BDType *type)
{
    BDAExprConst *c = malloc(sizeof(BDAExprConst));
    c->kind = kind;
    c->lbl = lbl;
    c->type = type;
    return c;
}

BDAExprConst *bd_aexpr_const_char(char *lbl, char val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_CHAR, lbl, bd_type_char());
    c->u.u_char = val;
    return c;
}

BDAExprConst *bd_aexpr_const_int(char *lbl, int val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_INT, lbl, bd_type_int());
    c->u.u_int = val;
    return c;
}

BDAExprConst *bd_aexpr_const_float(char *lbl, double val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_FLOAT, lbl, bd_type_float());
    c->u.u_double = val;
    return c;
}

BDAExprConst *bd_aexpr_const_str(char *lbl, char *val)
{
    BDAExprConst *c = bd_aexpr_const(AEC_STR, lbl, bd_type_string());
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

BDAInst *_ainst_jmpif(BDAInstKind kind, char *l, char *r, char *lbl)
{
    BDAInst *inst = bd_ainst(kind);
    inst->u.u_bin.l = l;
    inst->u.u_bin.r = r;
    inst->lbl = lbl;
    return inst;
}

BDAInst *bd_ainst_setglobal(BDType *type, char *l, char *r)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_setglobal_c(l, r);
        case T_INT:
            return bd_ainst_setglobal_i(l, r);
        case T_FLOAT:
            return bd_ainst_setglobal_f(l, r);
        default:
            return bd_ainst_setglobal_l(l, r);
    }
}

BDAInst *bd_ainst_pushlcl(BDType *type, char *reg, int offset)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_pushlcl_c(reg, offset);
        case T_INT:
            return bd_ainst_pushlcl_i(reg, offset);
        case T_FLOAT:
            return bd_ainst_pushlcl_f(reg, offset);
        default:
            return bd_ainst_pushlcl_l(reg, offset);
    }
}

BDAInst *bd_ainst_getlcl(BDType *type, int offset)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_getlcl_c(offset);
        case T_INT:
            return bd_ainst_getlcl_i(offset);
        case T_FLOAT:
            return bd_ainst_getlcl_f(offset);
        default:
            return bd_ainst_getlcl_l(offset);
    }
}

BDAInst *bd_ainst_pusharg(BDType *type, char *reg, int offset)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_pusharg_c(reg, offset);
        case T_INT:
            return bd_ainst_pusharg_i(reg, offset);
        case T_FLOAT:
            return bd_ainst_pusharg_f(reg, offset);
        default:
            return bd_ainst_pusharg_l(reg, offset);
    }
}

BDAInst *bd_ainst_getarg(BDType *type, int offset)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_getarg_c(offset);
        case T_INT:
            return bd_ainst_getarg_i(offset);
        case T_FLOAT:
            return bd_ainst_getarg_f(offset);
        default:
            return bd_ainst_getarg_l(offset);
    }
}

BDAInst *bd_ainst_pushfv(BDType *type, char *lbl, int offset)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_pushfv_c(lbl, offset);
        case T_INT:
            return bd_ainst_pushfv_i(lbl, offset);
        case T_FLOAT:
            return bd_ainst_pushfv_f(lbl, offset);
        default:
            return bd_ainst_pushfv_l(lbl, offset);
    }
}

BDAInst *bd_ainst_getfv(BDType *type, int offset)
{
    switch(type->kind){
        case T_CHAR:
            return bd_ainst_getfv_c(offset);
        case T_INT:
            return bd_ainst_getfv_i(offset);
        case T_FLOAT:
            return bd_ainst_getfv_f(offset);
        default:
            return bd_ainst_getfv_l(offset);
    }
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

BDAInst *bd_ainst_maketuple(int size)
{
    BDAInst *inst = bd_ainst(AI_MAKETUPLE);
    inst->u.u_int = size;
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

        case AI_SETGLOBAL_C:
            PRINT2(col, "SETGLOBAL_C %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_SETGLOBAL_I:
            PRINT2(col, "SETGLOBAL_I %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_SETGLOBAL_F:
            PRINT2(col, "SETGLOBAL_F %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_SETGLOBAL_L:
            PRINT2(col, "SETGLOBAL_L %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;

        case AI_MOV:
            PRINT1(col, "MOV %s", inst->lbl);
            break;
        case AI_MOVGLOBAL:
            PRINT1(col, "MOVGLOBAL %s", inst->lbl);
            break;
        case AI_MOVGLOBAL_L:
            PRINT1(col, "MOVGLOBAL_L %s", inst->lbl);
            break;

        case AI_NEG:
            PRINT1(col, "NEG %s", inst->lbl);
            break;
        case AI_FNEG:
            PRINT1(col, "FNEG %s", inst->lbl);
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
            PRINT(col, "\n");
            _bd_aexpr_show(inst->u.u_if.f, col, depth + 1);
            break;
        case AI_IFLE:
            PRINT2(col, "IF %s <= %s\n", inst->u.u_if.l, inst->u.u_if.r);
            _bd_aexpr_show(inst->u.u_if.t, col, depth + 1);
            PRINT(col, "\n");
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

        case AI_CALL:
            PRINT1(col, "CALL %s", inst->lbl);
            break;
        case AI_CALLPTR:
            PRINT1(col, "CALLPTR %s", inst->lbl);
            break;
        case AI_TAILCALLPOINT:
            PRINT(col, "TAILCALLPOINT");
            break;

        case AI_PUSHLCL_C:
            PRINT2(col, "PUSHLCL_C %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHLCL_I:
            PRINT2(col, "PUSHLCL_I %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHLCL_F:
            PRINT2(col, "PUSHLCL_F %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHLCL_L:
            PRINT2(col, "PUSHLCL_L %s, %d", inst->lbl, inst->u.u_int);
            break;

        case AI_GETLCL_C:
            PRINT1(col, "GETLCL_C %d", inst->u.u_int);
            break;
        case AI_GETLCL_I:
            PRINT1(col, "GETLCL_I %d", inst->u.u_int);
            break;
        case AI_GETLCL_F:
            PRINT1(col, "GETLCL_F %d", inst->u.u_int);
            break;
        case AI_GETLCL_L:
            PRINT1(col, "GETLCL_L %d", inst->u.u_int);
            break;

        case AI_PUSHARG_C:
            PRINT2(col, "PUSHARG_C %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHARG_I:
            PRINT2(col, "PUSHARG_I %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHARG_F:
            PRINT2(col, "PUSHARG_F %s, %d", inst->lbl, inst->u.u_int);
            break;
        case AI_PUSHARG_L:
            PRINT2(col, "PUSHARG_L %s, %d", inst->lbl, inst->u.u_int);
            break;

        case AI_GETARG_C:
            PRINT1(col, "GETARG_C %d", inst->u.u_int);
            break;
        case AI_GETARG_I:
            PRINT1(col, "GETARG_I %d", inst->u.u_int);
            break;
        case AI_GETARG_F:
            PRINT1(col, "GETARG_F %d", inst->u.u_int);
            break;
        case AI_GETARG_L:
            PRINT1(col, "GETARG_L %d", inst->u.u_int);
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
                PRINT1(col, "MAKETUPLE %d", inst->u.u_int);
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

        case AI_MAKESTRING:
            PRINT1(col, "MAKESTRING %s", inst->lbl);
            break;

        case AI_SAVE:
            PRINT2(col, "SAVE %s, %s", inst->u.u_bin.l, inst->u.u_bin.r);
            break;
        case AI_RESTORE:
            PRINT1(col, "RESTORE %s", inst->lbl);
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

                if(ident->type->kind != T_UNIT){
                    PRINT1(col, "%s = ", bd_expr_ident_show(ident));
                }
                _bd_ainst_show(val, col, depth + 1);
                PRINT(col, "\n");
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
            PRINT1(col, "\"%s\"", c->u.u_str);
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

int bd_value_size(BDType *type)
{
    switch(type->kind){
        case T_CHAR:
            return SIZE_CHAR;
        case T_INT:
            return SIZE_INT;
        case T_FLOAT:
            return SIZE_FLOAT;
        default:
            return SIZE_LBL;
    }
}
