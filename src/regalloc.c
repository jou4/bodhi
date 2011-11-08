#include "compile.h"


typedef enum {
    POS_REG,
    POS_LCL,
    POS_FV,
    POS_ARG,
    POS_GLOBAL_FUNCTION,
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

LblState *lbl_state_global_function(BDType *type)
{
    LblState *st = malloc(sizeof(LblState));
    st->type = type;
    st->pos = POS_GLOBAL_FUNCTION;
    return st;
}

AllocState *alloc_state()
{
    AllocState *st = malloc(sizeof(AllocState));
    st->local_offset = 0;
    return st;
}

typedef struct {
    BDReg target;
    int reuse;
} TargetRegResult;

TargetRegResult *target_reg_result_new()
{
    TargetRegResult *result = malloc(sizeof(TargetRegResult));
    result->target = RNONE;
    result->reuse = 0;
    return result;
}

void target_reg_result_destroy(TargetRegResult *result)
{
    free(result);
}

void add_target_reg(TargetRegResult *result, BDReg target)
{
    result->target = target;
}

BDReg usable_reg(Env *regenv, BDReg *candidates)
{
    void *p;
    int i = 0;
    BDReg reg;
    while((reg = candidates[i++]) >= 0){
        p = env_get(regenv, reg_name(reg));
        if(p == NULL){
            return reg;
        }
    }

    // Spill
    return candidates[0];
}

int is_match_lbl(char *lbl, char *target)
{
    if(strcmp(lbl, target) == 0){
        return 1;
    }
    return 0;
}

BDReg extra_regs[] = {
    REXT1,
    REXT2,
    REXT3,
    REXT4,
    REXT5,
    -1
};

void target_reg_in_expr(Env *regenv, char *lbl, BDAExpr *e, TargetRegResult *result);

int target_reg_in_inst(Env *regenv, char *lbl, BDAInst *inst, TargetRegResult *result)
{
    switch(inst->kind){
        case AI_SETGLOBAL_C:
        case AI_SETGLOBAL_I:
        case AI_SETGLOBAL_F:
        case AI_SETGLOBAL_L:
            if(is_match_lbl(lbl, inst->u.u_bin.r)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;

        case AI_MOV:
            if(is_match_lbl(lbl, inst->lbl)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;

        case AI_NEG:
            if(is_match_lbl(lbl, inst->lbl)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;
        case AI_FNEG:
            // TODO
            return;

        case AI_ADD:
        case AI_SUB:
        case AI_MUL:
        case AI_DIV:
            if(is_match_lbl(lbl, inst->u.u_bin.l)){
                add_target_reg(result, RACC);
            }
            if(is_match_lbl(lbl, inst->u.u_bin.r)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;

        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:
            // TODO
            return 0;

        case AI_IFEQ:
        case AI_IFLE:
            {
                int ml = is_match_lbl(lbl, inst->u.u_if.l);
                int mr = is_match_lbl(lbl, inst->u.u_if.r);
                int wall = 0;

                if(ml){
                    add_target_reg(result, RACC);
                }
                if(mr){
                    add_target_reg(result, usable_reg(regenv, extra_regs));
                }

                TargetRegResult *r1 = target_reg_result_new();
                TargetRegResult *r2 = target_reg_result_new();

                target_reg_in_expr(regenv, lbl, inst->u.u_if.t, r1);
                target_reg_in_expr(regenv, lbl, inst->u.u_if.f, r2);

                if(ml || mr){
                    if(r1->target != RNONE || r2->target != RNONE){
                        result->reuse = 1;
                    }
                }
                else{
                    wall = 1;
                }

                target_reg_result_destroy(r1);
                target_reg_result_destroy(r2);

                return wall;
            }

        case AI_CALLCLS:
        case AI_CALLDIR:
        case AI_CCALL:
            {
                int wall = 0;

                int mf = is_match_lbl(lbl, inst->lbl);
                int maa = 0;
                if(mf){
                    add_target_reg(result, usable_reg(regenv, extra_regs));
                }

                int i;
                int ma;
                BDReg target;
                BDExprIdent *ident;
                Vector *vec;

                vec = inst->u.u_call.ilist;
                for(i = 0; i < vec->length; i++){
                    ident = vector_get(vec, i);
                    ma = is_match_lbl(lbl, ident->name);
                    if(ma){
                        target = argreg(i);
                        if(target != RACC){
                            add_target_reg(result, target);
                            maa = 1;
                        }
                    }
                }

                vec = inst->u.u_call.flist;
                for(i = 0; i < vec->length; i++){
                    ident = vector_get(vec, i);
                    ma = is_match_lbl(lbl, ident->name);
                    if(ma){
                        target = fargreg(i);
                        if(target != RACC){
                            add_target_reg(result, target);
                            maa = 1;
                        }
                    }
                }

                if(ma == 0 || maa == 0){
                    wall = 1;
                }

                return wall;
            }

        case AI_MAKECLS:
            return 1;
        case AI_LOADFVS:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, RARG1);
                    return 0;
                }
                return 1;
            }

        case AI_PUSHFV_C:
        case AI_PUSHFV_I:
        case AI_PUSHFV_F:
        case AI_PUSHFV_L:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, usable_reg(regenv, extra_regs));
                }

                return 0;
            }

        case AI_MAKETUPLE:
            return 1;
        case AI_LOADELMS:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, usable_reg(regenv, extra_regs));
                }
                return 0;
            }
        case AI_PUSHELM:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, RACC);
                }
                return 0;
            }
        case AI_GETELM:
            return 0;
    }
}

void target_reg_in_expr(Env *regenv, char *lbl, BDAExpr *e, TargetRegResult *result)
{
    switch(e->kind){
        case AE_ANS:
            {
                target_reg_in_inst(regenv, lbl, e->u.u_ans.val, result);
                return;
            }
            break;
        case AE_LET:
            {
                TargetRegResult *r1 = target_reg_result_new();
                int wall = target_reg_in_inst(regenv, lbl, e->u.u_let.val, r1);
                if(wall){
                    // There is wall(by function-call or if-expr) but not target-lbl.
                    target_reg_result_destroy(r1);
                    return;
                }
                else{
                    if(r1->target != RNONE){
                        // There is target-lbl and return use-register candidate.
                        result->target = r1->target;
                        if(r1->reuse){
                            result->reuse = 1;
                            target_reg_result_destroy(r1);
                            return;
                        }
                        else{
                            target_reg_result_destroy(r1);
                            r1 = target_reg_result_new();
                            target_reg_in_expr(regenv, lbl, e->u.u_let.body, r1);
                            if(r1->target != RNONE){
                                result->reuse = 1;
                            }
                            target_reg_result_destroy(r1);
                        }
                    }
                    else{
                        target_reg_in_expr(regenv, lbl, e->u.u_let.body, result);
                    }
                }
            }
            break;
    }
}

BDReg target_reg(Env *regenv, char *lbl, BDAExpr *e, int restore)
{
    TargetRegResult *result = target_reg_result_new();
    target_reg_in_expr(regenv, lbl, e, result);

    BDReg target = result->target;

    if(restore == 0){
        if(result->reuse){
            return RNONE;
        }
    }
    return target;
}

typedef enum {
    AR_ALLOC,
    AR_SPILL,
    AR_STACK,
} AllocResultKind;

typedef struct {
    AllocResultKind kind;
    BDReg target;
} AllocResult;

AllocResult allocate_register(Env *regenv, char *lbl, BDAExpr *e, int restore)
{
    AllocResult result;

    BDReg target = target_reg(regenv, lbl, e, restore);
    if(target == RNONE){
        result.kind = AR_ALLOC;
        result.target = target;
    }
    else if(env_get(regenv, reg_name(target)) == NULL){
        result.kind = AR_ALLOC;
        result.target = target;
    }
    else{
        result.kind = AR_SPILL;
        result.target = target;
    }

    return result;
}

char *find_reg(Env *env, Env *regenv, char *lbl)
{
    if(lbl[0] == '%'){
        map_remove(regenv, lbl);
        return lbl;
    }

    Vector *keys = map_keys(regenv);
    int i;
    char *key, *val;
    for(i = 0; i < keys->length; i++){
        key = vector_get(keys, i);
        val = map_get(regenv, key);
        if(strcmp(val, lbl) == 0){
            printf("free %s, %s\n", lbl, key);
            map_remove(regenv, key);
            return key;
        }
    }

    throw(ERROR, lbl);
}

void *use_reg(Env *regenv, BDReg reg, char *lbl)
{
    printf("use_reg, %s, %s\n", lbl, reg_name(reg));
    env_set(regenv, reg_name(reg), lbl);
}

void free_reg(Env *regenv, BDReg reg)
{
    env_set(regenv, reg_name(reg), NULL);
}

BDAExpr *resolve_lbl(Env *env, char *lbl, BDReg dst, BDAExpr *body)
{
    LblState *lst = env_get(env, lbl);

    if(lst == NULL){
        // global label.
        return bd_aexpr_let(
                bd_expr_ident_typevar(reg_name(dst)),
                bd_ainst_movglobal(lbl),
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
            case POS_GLOBAL_FUNCTION:
                return bd_aexpr_let(
                        bd_expr_ident_typevar(reg_name(dst)),
                        bd_ainst_movglobal_l(lbl),
                        body);
        }
    }
}

BDAExpr *regalloc_inst(AllocState *st, Env *env, Env *regenv, BDAInst *inst, int tail)
{
    switch(inst->kind){
        case AI_NOP:
            return bd_aexpr_ans(bd_ainst_nop());

        case AI_SET_C:
            return bd_aexpr_ans(bd_ainst_setc(inst->u.u_char));
        case AI_SET_I:
            return bd_aexpr_ans(bd_ainst_seti(inst->u.u_int));

        case AI_SETGLOBAL_C:
            return bd_aexpr_ans(bd_ainst_setglobal_c(inst->u.u_bin.l, find_reg(env, regenv, inst->u.u_bin.r)));
        case AI_SETGLOBAL_I:
            return bd_aexpr_ans(bd_ainst_setglobal_i(inst->u.u_bin.l, find_reg(env, regenv, inst->u.u_bin.r)));
        case AI_SETGLOBAL_F:
            return bd_aexpr_ans(bd_ainst_setglobal_f(inst->u.u_bin.l, find_reg(env, regenv, inst->u.u_bin.r)));
        case AI_SETGLOBAL_L:
            return bd_aexpr_ans(bd_ainst_setglobal_l(inst->u.u_bin.l, find_reg(env, regenv, inst->u.u_bin.r)));

        case AI_MOV:
            return bd_aexpr_ans(bd_ainst_mov(find_reg(env, regenv, inst->lbl)));

        case AI_NEG:
            return bd_aexpr_ans(bd_ainst_neg(find_reg(env, regenv, inst->lbl)));
        case AI_ADD:
            return bd_aexpr_ans(bd_ainst_add(find_reg(env, regenv, inst->u.u_bin.l), find_reg(env, regenv, inst->u.u_bin.r)));
        case AI_SUB:
            return bd_aexpr_ans(bd_ainst_sub(find_reg(env, regenv, inst->u.u_bin.l), find_reg(env, regenv, inst->u.u_bin.r)));
        case AI_MUL:
            return bd_aexpr_ans(bd_ainst_mul(find_reg(env, regenv, inst->u.u_bin.l), find_reg(env, regenv, inst->u.u_bin.r)));
        case AI_DIV:
            return bd_aexpr_ans(bd_ainst_div(find_reg(env, regenv, inst->u.u_bin.l), find_reg(env, regenv, inst->u.u_bin.r)));

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
                        {
                            body = bd_aexpr_let(
                                    bd_expr_ident("", bd_type_unit()),
                                    bd_ainst_getcls_entry(reg_name(RACC)),
                                    bd_aexpr_ans(bd_ainst_callptr(reg_name(RACC))));
                            body = resolve_lbl(env, inst->lbl, RACC, body);
                        }
                        break;
                    case AI_CCALL:
                        body = bd_aexpr_ans(bd_ainst_call(inst->lbl));
                        break;
                    case AI_CALLDIR:
                        {
                            LblState *lst = env_get(env, inst->lbl);
                            if(lst != NULL && lst->pos == POS_GLOBAL_FUNCTION){
                                body = bd_aexpr_ans(bd_ainst_call(inst->lbl));
                            }
                            else{
                                body = bd_aexpr_ans(bd_ainst_callptr(inst->lbl));
                            }
                        }
                        break;
                }

                Vector *ilist = inst->u.u_call.ilist;
                Vector *flist = inst->u.u_call.flist;
                BDExprIdent *ident;
                int i, stack = (ilist->length - 6) + (flist->length - 8) - 1;
                BDReg target;

                for(i = flist->length - 1; i >= 0; i--){
                    ident = vector_get(flist, i);
                    target = fargreg(i);

                    if(target == RNONE){
                        body = resolve_lbl(env, ident->name, RACC,
                                bd_aexpr_let(
                                    bd_expr_ident("", bd_type_unit()),
                                    bd_ainst_pusharg(ident->type, reg_name(RACC), stack),
                                    body));
                        stack--;
                    }
                    else{
                        body = bd_aexpr_let(
                                bd_expr_ident("", bd_type_unit()),
                                bd_ainst_pusharg(ident->type, find_reg(env, regenv, ident->name), i),
                                body);
                    }
                }

                for(i = ilist->length - 1; i >= 0; i--){
                    ident = vector_get(ilist, i);
                    target = argreg(i);

                    if(target == RNONE){
                        body = resolve_lbl(env, ident->name, RACC,
                                bd_aexpr_let(
                                    bd_expr_ident("", bd_type_unit()),
                                    bd_ainst_pusharg(ident->type, reg_name(RACC), stack),
                                    body));
                        stack--;
                    }
                    else{
                        body = bd_aexpr_let(
                                bd_expr_ident("", bd_type_unit()),
                                bd_ainst_pusharg(ident->type, find_reg(env, regenv, ident->name), i),
                                body);
                    }
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
            return bd_aexpr_ans(bd_ainst_loadfvs(find_reg(env, regenv, inst->lbl)));

        case AI_PUSHFV_C:
            return bd_aexpr_ans(bd_ainst_pushfv_c(find_reg(env, regenv, inst->lbl), inst->u.u_int));
        case AI_PUSHFV_I:
            return bd_aexpr_ans(bd_ainst_pushfv_i(find_reg(env, regenv, inst->lbl), inst->u.u_int));
        case AI_PUSHFV_F:
            return bd_aexpr_ans(bd_ainst_pushfv_f(find_reg(env, regenv, inst->lbl), inst->u.u_int));
        case AI_PUSHFV_L:
            return bd_aexpr_ans(bd_ainst_pushfv_l(find_reg(env, regenv, inst->lbl), inst->u.u_int));

        case AI_MAKETUPLE:
            {
                return bd_aexpr_ans(bd_ainst_maketuple(inst->u.u_int));
            }
            break;
        case AI_LOADELMS:
            return bd_aexpr_ans(bd_ainst_loadelms(find_reg(env, regenv, inst->lbl)));
        case AI_PUSHELM:
            return bd_aexpr_ans(bd_ainst_pushelm(find_reg(env, regenv, inst->lbl), inst->u.u_int));
        case AI_GETELM:
            return bd_aexpr_ans(bd_ainst_getelm(inst->u.u_int));
        case AI_RESTORE:
            {
                char *lbl = inst->lbl;

                LblState *lst = env_get(env, lbl);
                BDType *type = lst->type;
                BDAExpr *let;

                switch(lst->pos){
                    case POS_LCL:
                        return bd_aexpr_ans(bd_ainst_getlcl(type, lst->u.offset));
                    case POS_FV:
                        return bd_aexpr_ans(bd_ainst_getfv(type, lst->u.offset));
                    case POS_ARG:
                        return bd_aexpr_ans(bd_ainst_getarg(type, lst->u.offset));
                    default:
                        printf("abc\n");
                        break;
                }
            }
    }
}

BDAExpr *regalloc(AllocState *st, Env *env, Env *regenv, BDAExpr *e, int tail)
{
    AllocState *local_st = malloc(sizeof(AllocState));
    *local_st = *st;
    Env *local_env = env_local_new(env);
    Env *local_regenv = env_local_new(regenv);

    try{
        switch(e->kind){
            case AE_ANS:
                return regalloc_inst(local_st, local_env, local_regenv, e->u.u_ans.val, tail);
            case AE_LET:
                {
                    BDExprIdent *ident = e->u.u_let.ident;
                    BDAInst *inst = e->u.u_let.val;
                    BDAExpr *body = e->u.u_let.body;
                    BDAExpr *newinst = regalloc_inst(local_st, local_env, local_regenv, inst, 0);

                    int restore = 0;
                    if(inst->kind == AI_RESTORE){
                        restore = 1;
                    }

                    BDAExpr *newbody;

                    if(ident->type->kind == T_UNIT){
                        newbody = regalloc(local_st, local_env, local_regenv, body, tail);

                        // Clean.
                        free(local_st);
                        env_destroy(local_env);
                        env_destroy(local_regenv);

                        return bd_aexpr_concat(newinst,
                                bd_expr_ident_clone(ident),
                                newbody);
                    }
                    else{

                        AllocResult ar = allocate_register(local_regenv, ident->name, body, restore);
                        BDReg reg = ar.target;

                        if(ar.kind == AR_SPILL){
                            LblState *lst;

                            // Save
                            int offset = local_st->local_offset;
                            local_st->local_offset += SIZE_ALIGN;

                            // Update spilled lbl state.
                            char *savelbl = env_get(local_regenv, reg_name(reg));
                            lst = env_get(local_env, savelbl);
                            BDType *savelbltype = lst->type;
                            lst = lbl_state_offset(savelbltype, POS_LCL, offset);
                            env_set(local_env, savelbl, lst);
                            BDAInst *saveinst = bd_ainst_pushlcl(savelbltype, reg_name(reg), offset);

                            // Reserve regenv.
                            use_reg(local_regenv, reg, ident->name);

                            // Add LblState to env.
                            lst = lbl_state_reg(ident->type, reg);
                            env_set(local_env, ident->name, lst);

                            // Transform body.
                            newbody = regalloc(local_st, local_env, local_regenv, body, tail);

                            // Clean.
                            free(local_st);
                            env_destroy(local_env);
                            env_destroy(local_regenv);

                            return bd_aexpr_let(
                                    bd_expr_ident("", bd_type_unit()),
                                    saveinst,
                                    bd_aexpr_concat(
                                        newinst,
                                        bd_expr_ident(reg_name(reg), ident->type),
                                        newbody));
                        }
                        else{
                            if(reg == RNONE){
                                int offset = local_st->local_offset;
                                local_st->local_offset += SIZE_ALIGN;

                                // Add LblState to env.
                                LblState *lst = lbl_state_offset(ident->type, POS_LCL, offset);
                                env_set(local_env, ident->name, lst);

                                BDAInst *saveinst = bd_ainst_pushlcl(ident->type, reg_name(RACC), offset);

                                // Transform body.
                                newbody = regalloc(local_st, local_env, local_regenv, body, tail);

                                // Clean.
                                free(local_st);
                                env_destroy(local_env);
                                env_destroy(local_regenv);

                                return bd_aexpr_concat(
                                        newinst,
                                        bd_expr_ident(reg_name(RACC), ident->type),
                                        bd_aexpr_let(
                                            bd_expr_ident("", bd_type_unit()),
                                            saveinst,
                                            newbody));
                            }
                            else{
                                // Reserve regenv.
                                use_reg(local_regenv, reg, ident->name);

                                // Add LblState to env.
                                LblState *lst = lbl_state_reg(ident->type, reg);
                                env_set(local_env, ident->name, lst);

                                // Transform body.
                                newbody = regalloc(local_st, local_env, local_regenv, body, tail);

                                // Clean.
                                free(local_st);
                                env_destroy(local_env);
                                env_destroy(local_regenv);

                                return bd_aexpr_concat(
                                        newinst,
                                        bd_expr_ident(reg_name(reg), ident->type),
                                        newbody);
                            }

                        }
                    }
                }
                break;
        }
    }
    catch{
        // Clean.
        free(local_st);
        env_destroy(local_env);
        env_destroy(local_regenv);

        // Restore
        char *lbl = catch_error();
        return regalloc(st, env, regenv,
                bd_aexpr_let(
                    bd_expr_ident_typevar(lbl),
                    bd_ainst_restore(lbl),
                    e), tail);
    }
}

BDAExprDef *regalloc_fundef(Env *env, BDAExprDef *def)
{
    BDExprIdent *ident = def->ident;
    Vector *iformals = def->iformals;
    Vector *fformals = def->fformals;
    Vector *freevars = def->freevars;
    BDAExpr *body = def->body;

    AllocState *st = alloc_state();
    Env *local = env_local_new(env);
    Env *regenv = env_new();

    int i, offset;
    BDExprIdent *formal;
    Vector *vec;
    BDReg reg;
    char *use = "use";

    vec = iformals;
    if(vec != NULL){
        for(i = 0, offset = 0; i < vec->length; i++){
            formal = vector_get(vec, i);
            env_set(local, formal->name, lbl_state_offset(formal->type, POS_ARG, offset));
            offset++;

            reg = argreg(i);
            if(reg != RNONE){
                use_reg(regenv, reg, formal->name);
            }
        }
    }

    vec = fformals;
    if(vec != NULL){
        for(i = 0, offset = 0; i < vec->length; i++){
            formal = vector_get(vec, i);
            env_set(local, formal->name, lbl_state_offset(formal->type, POS_ARG, offset));
            offset++;

            reg = fargreg(i);
            if(reg != RNONE){
                use_reg(regenv, reg, formal->name);
            }
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

            env_set(local, formal->name, lbl_state_offset(formal->type, POS_LCL, i));
            offset += SIZE_ALIGN;
        }
    }

    newbody = regalloc(st, local, regenv, body, 1);
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

    env_local_destroy(local);

    return newdef;
}


extern Vector *primsigs;

BDAProgram *bd_regalloc(BDAProgram *prog)
{
    BDAProgram *aprog = malloc(sizeof(BDAProgram));
    bd_aprogram_init(aprog);

    AllocState *st = alloc_state();
    Env *env = env_new();

    BDAExprConst *c;
    BDExprIdent *ident;
    BDAExprDef *def;
    PrimSig *sig;
    Vector *vec;
    int i;

    // Add primitives that be called direct.
    for(i = 0; i < primsigs->length; i++){
        sig = vector_get(primsigs, i);
        env_set(env, sig->lbl, lbl_state_global_function(sig->type));
    }

    // Allocate.
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        vector_add(aprog->fundefs, regalloc_fundef(env, def));
    }

    def = prog->maindef;
    aprog->maindef = regalloc_fundef(env, def);
    aprog->consts = prog->consts;
    aprog->globals = prog->globals;

    env_destroy(env);

    return aprog;
}
