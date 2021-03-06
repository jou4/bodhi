#include "compile.h"


typedef struct {
    int local_offset;
} AllocState;

AllocState *alloc_state()
{
    AllocState *st = malloc(sizeof(AllocState));
    st->local_offset = 0;
    return st;
}


typedef enum {
    POS_REG,
    POS_LCL,
    POS_FV,
    POS_ARG,
    POS_DATA,
} LblPos;

typedef struct {
    BDType *type;
    LblPos pos;
    union {
        int offset;
        BDReg reg;
    } u;
} LblState;

LblState *lbl_state_reg(BDType *type, BDReg reg)
{
    LblState *st = malloc(sizeof(LblState));
    st->pos = POS_REG;
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

LblState *lbl_state_data(BDType *type)
{
    LblState *st = malloc(sizeof(LblState));
    st->pos = POS_DATA;
    st->type = type;
    return st;
}


typedef struct {
    BDReg target;
    int reuse;
    int lost;
} TargetRegResult;

TargetRegResult *target_reg_result_new()
{
    TargetRegResult *result = malloc(sizeof(TargetRegResult));
    result->target = RNONE;
    result->reuse = 0;
    result->lost = 0;
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

BDReg extra_fregs[] = {
    RFEXT1,
    RFEXT2,
    RFEXT3,
    -1
};

void target_reg_in_expr(Env *regenv, char *lbl, BDAExpr *e, TargetRegResult *result, int wallstop);

int target_reg_in_inst(Env *regenv, char *lbl, BDAInst *inst, TargetRegResult *result, int wallstop)
{
    switch(inst->kind){
        case AI_SETGLOBAL_C:
        case AI_SETGLOBAL_I:
        case AI_SETGLOBAL_F:
            if(is_match_lbl(lbl, inst->u.u_bin.r)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
				return 0;
            }
            return 1;
        case AI_SETGLOBAL_L:
            if(is_match_lbl(lbl, inst->u.u_bin.r)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
				return 0;
            }
			// For GC.
            return 1;

        case AI_MOV:
            if(is_match_lbl(lbl, inst->lbl)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;

        case AI_MOV_F:
            if(is_match_lbl(lbl, inst->lbl)){
                add_target_reg(result, usable_reg(regenv, extra_fregs));
            }
            return 0;

        case AI_NEG:
            if(is_match_lbl(lbl, inst->lbl)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;
        case AI_FNEG:
            if(is_match_lbl(lbl, inst->lbl)){
                add_target_reg(result, usable_reg(regenv, extra_fregs));
                return 0;
            }
            return 1;

        case AI_ADD:
        case AI_SUB:
        case AI_MUL:
        case AI_DIV:
        case AI_MOD:
            if(is_match_lbl(lbl, inst->u.u_bin.l)){
                add_target_reg(result, RACC);
            }
            else if(is_match_lbl(lbl, inst->u.u_bin.r)){
                add_target_reg(result, usable_reg(regenv, extra_regs));
            }
            return 0;

        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:
            if(is_match_lbl(lbl, inst->u.u_bin.l)){
                add_target_reg(result, RFACC);
            }
            else if(is_match_lbl(lbl, inst->u.u_bin.r)){
                add_target_reg(result, usable_reg(regenv, extra_fregs));
            }
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
                else if(mr){
                    add_target_reg(result, usable_reg(regenv, extra_regs));
                }

                TargetRegResult *r1 = target_reg_result_new();
                TargetRegResult *r2 = target_reg_result_new();

                target_reg_in_expr(regenv, lbl, inst->u.u_if.t, r1, wallstop);
                target_reg_in_expr(regenv, lbl, inst->u.u_if.f, r2, wallstop);

                if(ml || mr){
                    if(r1->target != RNONE || r2->target != RNONE || r1->lost || r2->lost){
                        result->reuse = 1;
                    }
                }
                else{
                    // notice to callee that the lbl is missed
                    result->lost = 1;
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
                        if(target != RNONE){
                            add_target_reg(result, target);
                            maa = 1;
                            break;
                        }
                    }
                }

                vec = inst->u.u_call.flist;
                for(i = 0; i < vec->length; i++){
                    ident = vector_get(vec, i);
                    ma = is_match_lbl(lbl, ident->name);
                    if(ma){
                        target = fargreg(i);
                        if(target != RNONE){
                            add_target_reg(result, target);
                            maa = 1;
                            break;
                        }
                    }
                }

                if(mf == 0 && maa == 0){
                    wall = 1;
                }

                return wall;
            }

        case AI_MAKECLS:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, RARG1);
                    return 0;
                }
                return 1;
            }
        case AI_LOADFVS:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, RARG1);
                    return 0;
                }
                return 1;
            }
        case AI_LOADFVS_SELF:
            return 0;
        case AI_GETCLS_ENTRY:
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

        case AI_MAKESTRING:
            {
                if(is_match_lbl(lbl, inst->lbl)){
                    add_target_reg(result, RARG1);
                    return 0;
                }
                return 1;
            }
        default:
            return 0;
    }
}

void target_reg_in_expr(Env *regenv, char *lbl, BDAExpr *e, TargetRegResult *result, int wallstop)
{
    switch(e->kind){
        case AE_ANS:
            {
                target_reg_in_inst(regenv, lbl, e->u.u_ans.val, result, wallstop);
                return;
            }
            break;
        case AE_LET:
            {
                TargetRegResult *r1 = target_reg_result_new();
                int wall = target_reg_in_inst(regenv, lbl, e->u.u_let.val, r1, wallstop);
                if(wall && wallstop){
                    // There is wall(by function-call or if-expr) but not target-lbl.
                    target_reg_result_destroy(r1);
                    return;
                }
                else{
                    if(r1->target != RNONE){
                        // There is target-lbl and return use-register candidate.
                        result->target = r1->target;
                        if(r1->reuse || r1->lost){
                            result->reuse = 1;
                            target_reg_result_destroy(r1);
                            return;
                        }
                        else{
                            target_reg_result_destroy(r1);
                            r1 = target_reg_result_new();
                            target_reg_in_expr(regenv, lbl, e->u.u_let.body, r1, 0);
                            if(r1->target != RNONE || r1->lost){
                                result->reuse = 1;
                            }
                            target_reg_result_destroy(r1);
                        }
                    }
                    else{
                        target_reg_in_expr(regenv, lbl, e->u.u_let.body, result, wallstop);
                    }
                }
            }
            break;
		default:
			break;
    }
}

/*
 * The Rule of Finding Target Register
 *
 * Find the position that lable used nearly.
 * (ex. let A = 1 in A + 10)
 *
 * 1. If the label would be found and the label woule not be used in future,
 *    decide the register that will be used by the position.
 *
 *    ex. let A = 1 in A + 10  --> A : %rax
 *    ex. let A = 1 in 10 + A  --> A : %r10
 *    ex. let A = 1 in f A 10  --> A : %rdi
 *
 * 2. If the label would be found and the label woule be used too in future,
 *    the label would be saved to stack.
 *
 *    ex. let A = 1 in let _ = A + 10 in let _ = A + 20 in ... --> A : push to stack
 *
 * 3. If a function_call or a fork_point (ex, if expr) would be found before the position,
 *    the label would be saved to stack.
 *
 *    ex. let A = 1 in let B = f 10 in A + B  --> A : push to stack
 *
 *
 * However, if the label was already saved and that be necessary to restore, rule 2 and 3 will be ignored.
 * Because the label will be used immediately in future.
 *
*/
BDReg target_reg(Env *regenv, char *lbl, BDAExpr *e, int restore)
{
    TargetRegResult *result = target_reg_result_new();
    target_reg_in_expr(regenv, lbl, e, result, 1);

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
        return lbl;
    }

    Vector *keys = map_keys(regenv);
    int i;
    char *key, *val;
    for(i = 0; i < keys->length; i++){
        key = vector_get(keys, i);
        val = map_get(regenv, key);
        if(val != NULL && strcmp(val, lbl) == 0){
            return key;
        }
    }

    throw(ERROR, lbl);
}

void use_reg(Env *regenv, BDReg reg, char *lbl)
{
    env_set(regenv, reg_name(reg), lbl);
}

void free_reg(Env *regenv, char *reg)
{
    env_set(regenv, reg, NULL);
}

BDAExpr *resolve_lbl(Env *env, char *lbl, BDReg dst, BDAExpr *body)
{
    LblState *lst = env_get(env, lbl);

    if(lst == NULL){
        // global label.
        return bd_aexpr_let(
                bd_expr_ident_typevar(reg_name(dst)),
                bd_ainst_movglobal_l(lbl),
                body);
    }
    else{
        BDType *type = lst->type;

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
            case POS_DATA:
                if(type->kind == T_FLOAT){
                    return bd_aexpr_let(
                            bd_expr_ident_typevar(reg_name(dst)),
                            bd_ainst_movglobal_f(lbl),
                            body);
                }
                else{
                    return bd_aexpr_let(
                            bd_expr_ident_typevar(reg_name(dst)),
                            bd_ainst_movglobal(lbl),
                            body);
                }
			default:
				break;
        }
    }
    return NULL;
}

int use_stack_args_num(int args, int fargs)
{
    return (args - ARG_REG_NUM) + (fargs - FARG_REG_NUM);
}

BDAExpr *regalloc(AllocState *st, Env *env, Env *regenv, BDAExpr *e, int tail);

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
        case AI_SETGLOBAL_I:
        case AI_SETGLOBAL_F:
        case AI_SETGLOBAL_L:
            {
                char *l = inst->u.u_bin.l;
                char *r = find_reg(env, regenv, inst->u.u_bin.r);

                switch(inst->kind){
                    case AI_SETGLOBAL_C:
                        return bd_aexpr_ans(bd_ainst_setglobal_c(l, r));
                    case AI_SETGLOBAL_I:
                        return bd_aexpr_ans(bd_ainst_setglobal_i(l, r));
                    case AI_SETGLOBAL_F:
                        return bd_aexpr_ans(bd_ainst_setglobal_f(l, r));
                    case AI_SETGLOBAL_L:
                        return bd_aexpr_ans(bd_ainst_setglobal_l(l, r));
					default:
						break;
                }
            }
			break;

        case AI_MOV:
        case AI_MOV_F:
        case AI_NEG:
        case AI_FNEG:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);

                switch(inst->kind){
                    case AI_MOV:
                        return bd_aexpr_ans(bd_ainst_mov(lbl));
                    case AI_MOV_F:
                        return bd_aexpr_ans(bd_ainst_mov_f(lbl));
                    case AI_NEG:
                        return bd_aexpr_ans(bd_ainst_neg(lbl));
                    case AI_FNEG:
                        return bd_aexpr_ans(bd_ainst_fneg(lbl));
					default:
						break;
                }
            }
        case AI_MOVGLOBAL:
            return bd_aexpr_ans(bd_ainst_movglobal(inst->lbl));
        case AI_MOVGLOBAL_F:
            return bd_aexpr_ans(bd_ainst_movglobal_f(inst->lbl));
        case AI_MOVGLOBAL_L:
            return bd_aexpr_ans(bd_ainst_movglobal_l(inst->lbl));

        case AI_ADD:
        case AI_SUB:
        case AI_MUL:
        case AI_DIV:
        case AI_MOD:
            {
                char *l = find_reg(env, regenv, inst->u.u_bin.l);
                char *r = find_reg(env, regenv, inst->u.u_bin.r);

                free_reg(regenv, l);
                free_reg(regenv, r);

                switch(inst->kind){
                    case AI_ADD:
                        return bd_aexpr_ans(bd_ainst_add(l, r));
                    case AI_SUB:
                        return bd_aexpr_ans(bd_ainst_sub(l, r));
                    case AI_MUL:
                        return bd_aexpr_ans(bd_ainst_mul(l, r));
                    case AI_DIV:
                        return bd_aexpr_ans(bd_ainst_div(l, r));
                    case AI_MOD:
                        return bd_aexpr_ans(bd_ainst_mod(l, r));
					default:
						break;
                }
            }
			break;

        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:
            {
                char *l = find_reg(env, regenv, inst->u.u_bin.l);
                char *r = find_reg(env, regenv, inst->u.u_bin.r);

                free_reg(regenv, l);
                free_reg(regenv, r);

                switch(inst->kind){
                    case AI_FADD:
                        return bd_aexpr_ans(bd_ainst_fadd(l, r));
                    case AI_FSUB:
                        return bd_aexpr_ans(bd_ainst_fsub(l, r));
                    case AI_FMUL:
                        return bd_aexpr_ans(bd_ainst_fmul(l, r));
                    case AI_FDIV:
                        return bd_aexpr_ans(bd_ainst_fdiv(l, r));
					default:
						break;
                }
            }
            break;

        case AI_IFEQ:
        case AI_IFLE:
            {
                char *l = find_reg(env, regenv, inst->u.u_if.l);
                char *r = find_reg(env, regenv, inst->u.u_if.r);

                free_reg(regenv, l);
                free_reg(regenv, r);

                BDAExpr *t;
                BDAExpr *f;

                AllocState *local_st = malloc(sizeof(AllocState));
                Env *local_env;
                Env *local_regenv;

                *local_st = *st;
                local_env = env_local_new(env);
                local_regenv = env_local_new(regenv);
                t = regalloc(local_st, local_env, local_regenv, inst->u.u_if.t, tail);
                env_local_destroy(local_env);
                env_local_destroy(local_regenv);

                *local_st = *st;
                local_env = env_local_new(env);
                local_regenv = env_local_new(regenv);
                f = regalloc(local_st, local_env, local_regenv, inst->u.u_if.f, tail);
                env_local_destroy(local_env);
                env_local_destroy(local_regenv);

                free(local_st);

                switch(inst->kind){
                    case AI_IFEQ:
                        return bd_aexpr_ans(bd_ainst_ifeq(l, r, t, f));
                    case AI_IFLE:
                    default:
                        return bd_aexpr_ans(bd_ainst_ifle(l, r, t, f));
                }
            }
            break;

        case AI_CALLCLS:
        case AI_CALLDIR:
        case AI_CCALL:
            {
                BDAExpr *body;
                switch(inst->kind){
                    case AI_CALLCLS:
                        {
                            char *entry_reg = find_reg(env, regenv, inst->lbl);
                            free_reg(regenv, entry_reg);
                            body = bd_aexpr_ans(bd_ainst_callptr(entry_reg));
                        }
                        break;
                    case AI_CCALL:
                        body = bd_aexpr_ans(bd_ainst_call(inst->lbl));
                        break;
                    case AI_CALLDIR:
                    default:
                        {
                            LblState *lst = env_get(env, inst->lbl);
                            if(lst == NULL){
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
                int i, stack = use_stack_args_num(ilist->length, flist->length) - 1;
                BDReg target;
                char *reg;
                Vector *used_regs = vector_new();

                for(i = flist->length - 1; i >= 0; i--){
                    ident = vector_get(flist, i);
                    target = fargreg(i);

                    if(target == RNONE){
                        body = resolve_lbl(env, ident->name, RACC,
                                bd_aexpr_nonelet(
                                    bd_ainst_pusharg(ident->type, reg_name(RACC), stack),
                                    body));
                        stack--;
                    }
                    else{
                        reg = find_reg(env, regenv, ident->name);
                        vector_add(used_regs, reg);

                        body = bd_aexpr_nonelet(
                                bd_ainst_pusharg(ident->type, reg, i),
                                body);
                    }
                }

                for(i = ilist->length - 1; i >= 0; i--){
                    ident = vector_get(ilist, i);
                    target = argreg(i);

                    if(target == RNONE){
                        body = resolve_lbl(env, ident->name, RACC,
                                bd_aexpr_nonelet(
                                    bd_ainst_pusharg(ident->type, reg_name(RACC), stack),
                                    body));
                        stack--;
                    }
                    else{
                        reg = find_reg(env, regenv, ident->name);
                        vector_add(used_regs, reg);

                        body = bd_aexpr_nonelet(
                                bd_ainst_pusharg(ident->type, reg, i),
                                body);
                    }
                }

                if(tail && use_stack_args_num(ilist->length, flist->length) < TAIL_JMP_THREASHOLD){
                    body = bd_aexpr_nonelet(
                            bd_ainst_tailcall_point(),
                            body);
                }

                // Remove used_regs.
                for(i = 0; i < used_regs->length; i++){
                    reg = vector_get(used_regs, i);
                    free_reg(regenv, reg);
                }
                vector_destroy(used_regs);

                return body;
            }
            break;

        case AI_MAKECLS:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);
                return bd_aexpr_ans(bd_ainst_makecls(lbl, inst->u.u_int));
            }
        case AI_LOADFVS:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);
                return bd_aexpr_ans(bd_ainst_loadfvs(lbl));
            }
        case AI_LOADFVS_SELF:
            return bd_aexpr_ans(bd_ainst_loadfvs_self(inst->lbl));
        case AI_GETCLS_ENTRY:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);
                return bd_aexpr_ans(bd_ainst_getcls_entry(lbl));
            }

        case AI_PUSHFV_C:
        case AI_PUSHFV_I:
        case AI_PUSHFV_F:
        case AI_PUSHFV_L:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);

                switch(inst->kind){
                    case AI_PUSHFV_C:
                        return bd_aexpr_ans(bd_ainst_pushfv_c(lbl, inst->u.u_int));
                    case AI_PUSHFV_I:
                        return bd_aexpr_ans(bd_ainst_pushfv_i(lbl, inst->u.u_int));
                    case AI_PUSHFV_F:
                        return bd_aexpr_ans(bd_ainst_pushfv_f(lbl, inst->u.u_int));
                    case AI_PUSHFV_L:
                        return bd_aexpr_ans(bd_ainst_pushfv_l(lbl, inst->u.u_int));
					default:
						break;
                }
            }
			break;

        case AI_MAKETUPLE:
            return bd_aexpr_ans(bd_ainst_maketuple(inst->u.u_int));
        case AI_LOADELMS:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);
                return bd_aexpr_ans(bd_ainst_loadelms(lbl));
            }
        case AI_PUSHELM:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);
                return bd_aexpr_ans(bd_ainst_pushelm(lbl, inst->u.u_int));
            }
        case AI_GETELM:
            return bd_aexpr_ans(bd_ainst_getelm(inst->u.u_int));
        case AI_RESTORE:
            {
                char *lbl = inst->lbl;

                LblState *lst = env_get(env, lbl);

                if(lst == NULL){
                    return bd_aexpr_ans(bd_ainst_movglobal_l(lbl));
                }
                else{
                    BDType *type = lst->type;

                    switch(lst->pos){
                        case POS_LCL:
                            return bd_aexpr_ans(bd_ainst_getlcl(type, lst->u.offset));
                        case POS_FV:
                            return bd_aexpr_ans(bd_ainst_getfv(type, lst->u.offset));
                        case POS_ARG:
                            return bd_aexpr_ans(bd_ainst_getarg(type, lst->u.offset));
                        case POS_DATA:
                        default:
                            if(type->kind == T_FLOAT){
                                return bd_aexpr_ans(bd_ainst_movglobal_f(lbl));
                            }
                            else{
                                return bd_aexpr_ans(bd_ainst_movglobal(lbl));
                            }
                    }
                }
            }
            break;

        case AI_MAKESTRING:
            {
                char *lbl = find_reg(env, regenv, inst->lbl);
                free_reg(regenv, lbl);
                return bd_aexpr_ans(bd_ainst_makestring(lbl));
            }
            break;
		default:
			break;
    }
    return NULL;
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

                    if(ident->type == NULL){
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

                            if(restore == 0){
                                // Add LblState to env.
                                lst = lbl_state_reg(ident->type, reg);
                                env_set(local_env, ident->name, lst);
                            }

                            // Transform body.
                            newbody = regalloc(local_st, local_env, local_regenv, body, tail);

                            // Clean.
                            free(local_st);
                            env_destroy(local_env);
                            env_destroy(local_regenv);

                            return bd_aexpr_nonelet(
                                    saveinst,
                                    bd_aexpr_concat(
                                        newinst,
                                        bd_expr_ident(reg_name(reg), ident->type),
                                        newbody));
                        }
                        else{
                            if(reg == RNONE){
                                // Save self.
                                int offset = local_st->local_offset;
                                local_st->local_offset += SIZE_ALIGN;

                                if(restore == 0){
                                    // Add LblState to env.
                                    LblState *lst = lbl_state_offset(ident->type, POS_LCL, offset);
                                    env_set(local_env, ident->name, lst);
                                }

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
                                        bd_aexpr_nonelet(
                                            saveinst,
                                            newbody));
                            }
                            else{
                                // Reserve regenv.
                                use_reg(local_regenv, reg, ident->name);

                                if(restore == 0){
                                    // Add LblState to env.
                                    LblState *lst = lbl_state_reg(ident->type, reg);
                                    env_set(local_env, ident->name, lst);
                                }

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
			default:
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

    return NULL;
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

    int i;
    BDExprIdent *formal;
    Vector *vec;
    BDReg reg;

    BDAExpr *result, *reg2lcl, *tail = NULL;

    result = bd_aexpr_nonelet(bd_ainst_nop(), NULL);
    tail = result;

    vec = freevars;
    if(vec != NULL){
        // add reg_hp to top of local stack
        if(vec->length > 0){
            tail->u.u_let.body = bd_aexpr_nonelet(
                    bd_ainst_pushlcl(bd_type_int(), reg_name(RHP), st->local_offset),
                    NULL);
            tail = tail->u.u_let.body;

            st->local_offset += SIZE_ALIGN;
        }

        for(i = 0; i < vec->length; i++){
            formal = vector_get(vec, i);

            reg2lcl = bd_aexpr_nonelet(
                    bd_ainst_pushlcl(formal->type, reg_name(RACC), st->local_offset),
                    NULL);
            tail->u.u_let.body = bd_aexpr_let(
                    bd_expr_ident(reg_name(RACC), formal->type),
                    bd_ainst_getfv(formal->type, i * SIZE_ALIGN),
                    reg2lcl);
            tail = reg2lcl;

            env_set(local, formal->name, lbl_state_offset(formal->type, POS_LCL, st->local_offset));
            st->local_offset += SIZE_ALIGN;
        }
    }

    vec = iformals;
    if(vec != NULL){
        for(i = 0; i < vec->length; i++){
            formal = vector_get(vec, i);
            reg = argreg(i);

            if(reg == RNONE){
                env_set(local, formal->name, lbl_state_offset(formal->type, POS_ARG, i));
            }
            else{
                reg2lcl = bd_aexpr_nonelet(
                        bd_ainst_pushlcl(formal->type, reg_name(reg), st->local_offset),
                        NULL);
                tail->u.u_let.body = reg2lcl;
                tail = reg2lcl;

                env_set(local, formal->name, lbl_state_offset(formal->type, POS_LCL, st->local_offset));
                st->local_offset += SIZE_ALIGN;
            }
        }
    }

    vec = fformals;
    if(vec != NULL){
        for(i = 0; i < vec->length; i++){
            formal = vector_get(vec, i);
            reg = fargreg(i);

            if(reg == RNONE){
                env_set(local, formal->name, lbl_state_offset(formal->type, POS_ARG, i));
            }
            else{
                reg2lcl = bd_aexpr_nonelet(
                        bd_ainst_pushlcl(formal->type, reg_name(reg), st->local_offset),
                        NULL);
                tail->u.u_let.body = reg2lcl;
                tail = reg2lcl;

                env_set(local, formal->name, lbl_state_offset(formal->type, POS_LCL, st->local_offset));
                st->local_offset += SIZE_ALIGN;
            }
        }
    }

    tail->u.u_let.body = regalloc(st, local, regenv, body, 1);

    free(st);
    env_local_destroy(local);
    env_destroy(regenv);

    BDAExprDef *newdef = bd_aexpr_def(
            bd_expr_ident_clone(ident),
            bd_expr_idents_clone(iformals),
            bd_expr_idents_clone(fformals),
            bd_expr_idents_clone(freevars),
            result
            );

    return newdef;
}


extern Vector *primsigs;

BDAProgram *bd_regalloc(BDAProgram *prog)
{
    BDAProgram *aprog = malloc(sizeof(BDAProgram));
    bd_aprogram_init(aprog);

    // Not include primitive_functions, c_functions.
    Env *env = env_new();

    BDAExprConst *c;
    BDExprIdent *ident;
    BDAExprDef *def;
    Vector *vec;
    int i;

    // Add consts.
    vec = prog->consts;
    for(i = 0; i < vec->length; i++){
        c = vector_get(vec, i);
        env_set(env, c->lbl, lbl_state_data(c->type));
    }

    // Add globals.
    vec = prog->globals;
    for(i = 0; i < vec->length; i++){
        ident = vector_get(vec, i);
        env_set(env, ident->name, lbl_state_data(ident->type));
    }

    // Add functions.
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        ident = def->ident;
        env_set(env, ident->name, lbl_state_data(ident->type));
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
