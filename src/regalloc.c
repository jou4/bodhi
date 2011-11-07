#include "compile.h"


typedef enum {
    POS_REG,
    POS_LCL,
    POS_FV,
    POS_ARG,
} LblPos;

typedef struct {
    BDType *type;
    LblPos pos;
    union {
        int offset;
        BDReg reg;
    } u;
} LblState;

typedef struct {
    int local_offset;
} AllocState;

LblState *lbl_state_reg(BDType *type, BDReg reg)
{
    LblState *st = malloc(sizeof(LblState));
    st->type = type;
    st->u.reg = reg;
    return st;
}

LblState *lbl_state_offset(BDType *type, LblPos pos, int offset)
{
    LblState *st = malloc(sizeof(LblState));
    st->type = type;
    st->pos = pos;
    st->u.offset = offset;
    return st;
}

AllocState *alloc_state()
{
    AllocState *st = malloc(sizeof(AllocState));
    st->local_offset = 0;
    return st;
}


BDReg find_reg(char *lbl, BDAExpr *e)
{
    // TODO
    return RNONE;
}


BDAExpr *resolve_lbl(Env *env, char *lbl, BDReg dst, BDAExpr *body)
{
    LblState *lst = env_get(env, lbl);

    if(lst == NULL){
        // global label.
        return bd_aexpr_let(
                bd_expr_ident_typevar(reg_name(dst)),
                bd_ainst_mov(lbl),
                body);
    }
    else{
        BDType *type = lst->type;
        BDAExpr *let;

        switch(lst->pos){
            case POS_REG:
                if(lst->u.reg == dst){
                    return body;
                }
                else{
                    return bd_aexpr_let(
                            bd_expr_ident(reg_name(dst), type),
                            bd_ainst_mov(reg_name(lst->u.reg)),
                            body);
                }
                break;
            case POS_LCL:
                return bd_aexpr_let(
                        bd_expr_ident(reg_name(dst), type),
                        bd_ainst_getlcl(type, lst->u.offset),
                        body);
            case POS_FV:
                return bd_aexpr_let(
                        bd_expr_ident(reg_name(dst), type),
                        bd_ainst_getfv(type, lst->u.offset),
                        body);
            case POS_ARG:
                return bd_aexpr_let(
                        bd_expr_ident(reg_name(dst), type),
                        bd_ainst_getarg(type, lst->u.offset),
                        body);
        }
    }
}

BDAExpr *regalloc_inst(AllocState *st, Env *env, BDAInst *inst, int tail)
{
    switch(inst->kind){
        case AI_NOP:
            return bd_aexpr_ans(bd_ainst_nop());

        case AI_SET_C:
            return bd_aexpr_ans(bd_ainst_setc(inst->u.u_char));
        case AI_SET_I:
            return bd_aexpr_ans(bd_ainst_seti(inst->u.u_int));

        case AI_SETGLOBAL:
            return resolve_lbl(env, inst->u.u_bin.r, RACC,
                    bd_aexpr_ans(bd_ainst_setglobal(inst->u.u_bin.l, reg_name(RACC))));

        case AI_MOV:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_mov(reg_name(RACC))));
        case AI_NEG:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_neg(reg_name(RACC))));
        case AI_ADD:
            {
                BDAExpr *body = bd_aexpr_ans(
                        bd_ainst_add(reg_name(RACC), reg_name(REXT1)));
                body = resolve_lbl(env, inst->u.u_bin.r, REXT1, body);
                return resolve_lbl(env, inst->u.u_bin.l, RACC, body);
            }
        case AI_SUB:
            {
                BDAExpr *body = bd_aexpr_ans(
                        bd_ainst_sub(reg_name(RACC), reg_name(REXT1)));
                body = resolve_lbl(env, inst->u.u_bin.r, REXT1, body);
                return resolve_lbl(env, inst->u.u_bin.l, RACC, body);
            }
        case AI_MUL:
            {
                BDAExpr *body = bd_aexpr_ans(
                        bd_ainst_mul(reg_name(RACC), reg_name(REXT1)));
                body = resolve_lbl(env, inst->u.u_bin.r, REXT1, body);
                return resolve_lbl(env, inst->u.u_bin.l, RACC, body);
            }
        case AI_DIV:
            {
                BDAExpr *body = bd_aexpr_ans(
                        bd_ainst_div(reg_name(RACC), reg_name(REXT1)));
                body = resolve_lbl(env, inst->u.u_bin.r, REXT1, body);
                return resolve_lbl(env, inst->u.u_bin.l, RACC, body);
            }

        case AI_FNEG:
        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:
            break;

        case AI_IFEQ:
        case AI_IFLE:
            break;

        case AI_CALLCLS:
        case AI_CALLDIR:
        case AI_CCALL:
            {
                BDAExpr *body;
                switch(inst->kind){
                    case AI_CALLCLS:
                        body = bd_aexpr_let(
                                bd_expr_ident("", bd_type_unit()),
                                bd_ainst_getcls_entry(reg_name(RACC)),
                                bd_aexpr_ans(bd_ainst_call(reg_name(RACC))));
                        body = resolve_lbl(env, inst->lbl, RACC, body);
                        break;
                    case AI_CALLDIR:
                    case AI_CCALL:
                        body = bd_aexpr_ans(bd_ainst_call(inst->lbl));
                        break;
                }

                Vector *ilist = inst->u.u_call.ilist;
                Vector *flist = inst->u.u_call.flist;
                BDExprIdent *ident;
                int i;

                for(i = flist->length - 1; i >= 0; i--){
                    ident = vector_get(flist, i);
                    body = resolve_lbl(env, ident->name, fargreg(i),
                            bd_aexpr_let(
                                bd_expr_ident("", bd_type_unit()),
                                bd_ainst_pusharg(ident->type, reg_name(fargreg(i)), i),
                                body));
                }

                for(i = ilist->length - 1; i >= 0; i--){
                    ident = vector_get(ilist, i);
                    body = resolve_lbl(env, ident->name, argreg(i),
                            bd_aexpr_let(
                                bd_expr_ident("", bd_type_unit()),
                                bd_ainst_pusharg(ident->type, reg_name(argreg(i)), i),
                                body));
                }

                if(tail && ilist->length + flist->length < 20){
                    body = bd_aexpr_let(
                            bd_expr_ident("", bd_type_unit()),
                            bd_ainst_tailcall_point(),
                            body);
                }

                return body;
            }
            break;

        case AI_MAKECLS:
            return bd_aexpr_ans(bd_ainst_makecls(inst->lbl, inst->u.u_int));
        case AI_LOADFVS:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_loadfvs(reg_name(RACC))));

        case AI_PUSHFV_C:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_pushfv_c(reg_name(RACC), inst->u.u_int)));
        case AI_PUSHFV_I:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_pushfv_i(reg_name(RACC), inst->u.u_int)));
        case AI_PUSHFV_F:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_pushfv_f(reg_name(RACC), inst->u.u_int)));
        case AI_PUSHFV_L:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_pushfv_l(reg_name(RACC), inst->u.u_int)));

        case AI_MAKETUPLE:
            {
                return bd_aexpr_ans(bd_ainst_maketuple(inst->u.u_int));
            }
            break;
        case AI_LOADELMS:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_loadelms(reg_name(RACC))));
        case AI_PUSHELM:
            return resolve_lbl(env, inst->lbl, RACC,
                    bd_aexpr_ans(bd_ainst_pushelm(reg_name(RACC), inst->u.u_int)));
        case AI_GETELM:
            return bd_aexpr_ans(bd_ainst_getelm(inst->u.u_int));
    }
}

BDAExpr *regalloc(AllocState *st, Env *env, BDAExpr *e)
{
    switch(e->kind){
        case AE_ANS:
            return regalloc_inst(st, env, e->u.u_ans.val, 1);
        case AE_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                BDAInst *inst = e->u.u_let.val;
                BDAExpr *body = e->u.u_let.body;
                BDReg reg = find_reg(ident->name, body);

                BDAExpr *newinst = regalloc_inst(st, env, inst, 0);
                BDAExpr *newbody;

                if(ident->type->kind == T_UNIT){
                    return bd_aexpr_concat(newinst,
                            bd_expr_ident_clone(ident),
                            regalloc(st, env, body));
                }
                else if(reg == RNONE){
                    // Store
                    int offset = st->local_offset;
                    LblState *lst = lbl_state_offset(ident->type, POS_LCL, offset);
                    st->local_offset += bd_value_size(ident->type);
                    env_set(env, ident->name, lst);
                    newbody = regalloc(st, env, body);

                    BDAInst *storeinst = bd_ainst_pushlcl(ident->type, reg_name(RACC), offset);

                    return bd_aexpr_concat(newinst, bd_expr_ident(reg_name(RACC), ident->type),
                            bd_aexpr_let(
                                bd_expr_ident("", bd_type_unit()),
                                storeinst,
                                newbody));
                }
                else{
                    // TODO
                }
            }
            break;
    }
}

BDAExprDef *regalloc_fundef(BDAExprDef *def)
{
    BDExprIdent *ident = def->ident;
    Vector *iformals = def->iformals;
    Vector *fformals = def->fformals;
    Vector *freevars = def->freevars;
    BDAExpr *body = def->body;

    AllocState *st = alloc_state();
    Env *env = env_new();

    int i, offset;
    BDExprIdent *formal;
    Vector *vec;

    vec = iformals;
    if(vec != NULL){
        for(i = 0, offset = 0; i < vec->length; i++){
            formal = vector_get(vec, i);
            env_set(env, formal->name, lbl_state_offset(formal->type, POS_ARG, offset));
            offset++;
        }
    }

    vec = fformals;
    if(vec != NULL){
        for(i = 0, offset = 0; i < vec->length; i++){
            formal = vector_get(vec, i);
            env_set(env, formal->name, lbl_state_offset(formal->type, POS_ARG, offset));
            offset++;
        }
    }

    BDAExpr *result, *newbody, *fv2reg, *reg2lcl, *tail = NULL;
    vec = freevars;
    if(vec != NULL){
        for(i = 0, offset = 0; i < vec->length; i++){
            formal = vector_get(vec, i);

            reg2lcl = bd_aexpr_let(
                    bd_expr_ident("", bd_type_unit()),
                    bd_ainst_pushlcl(formal->type, reg_name(RACC), offset),
                    NULL);
            fv2reg = bd_aexpr_let(
                    bd_expr_ident(reg_name(RACC), formal->type),
                    bd_ainst_getfv(formal->type, offset),
                    reg2lcl);

            if(i > 0){
                tail->u.u_let.body = fv2reg;
                tail = reg2lcl;
            }
            else{
                result = fv2reg;
                tail = reg2lcl;
            }

            env_set(env, formal->name, lbl_state_offset(formal->type, POS_LCL, i));
            offset += bd_value_size(formal->type);
        }
    }

    newbody = regalloc(st, env, body);
    if(tail == NULL){
        result = newbody;
    }
    else{
        tail->u.u_let.body = newbody;
    }

    BDAExprDef *newdef = bd_aexpr_def(
            bd_expr_ident_clone(ident),
            bd_expr_idents_clone(iformals),
            bd_expr_idents_clone(fformals),
            bd_expr_idents_clone(freevars),
            result
            );

    env_destroy(env);

    return newdef;
}


extern Vector *primsigs;

BDAProgram *bd_regalloc(BDAProgram *prog)
{
    BDAProgram *aprog = malloc(sizeof(BDAProgram));
    bd_aprogram_init(aprog);

    AllocState *st = alloc_state();
    Env *env = env_new();
    BDAExprDef *def;

    Vector *vec;
    int i;

    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        vector_add(aprog->fundefs, regalloc_fundef(def));
    }

    def = prog->maindef;
    aprog->maindef = regalloc_fundef(def);
    aprog->consts = prog->consts;
    aprog->globals = prog->globals;

    return aprog;
}
