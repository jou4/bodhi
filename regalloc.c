#include "compile.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util/exception.h"
#include "util/mem.h"

#define ACC_REG_INDEX -1
#define NONE_REG_INDEX -2

static int common_regs_length = 3;

typedef enum {
    IN_REG,
    IN_STACK,
    REMOVED
} RegValState;

typedef struct {
    char *name;
    BDType *type;
    int begin;
    int end;
    BDAsmVal *val;
    RegValState state;
    int index;
} RegVal;

typedef struct {
    Env *env;
    Vector *regs;
    int current;
    int stack_i;
} RegAllocState;

typedef enum {
    RA_ALLOC,
    RA_SPILL
} RegAllocResultKind;

typedef struct {
    RegAllocResultKind kind;
    union {
        RegVal *val;
        int index;
    } u;
} RegAllocResult;

#define ERROR_NOREG 105
typedef struct {
    const char *name;
    BDType *type;
} NoRegError;

Vector *init_regs(int length)
{
    Vector *regs = vector_new();
    int i;
    for(i = 0; i < length; i++){
        vector_add(regs, NULL);
    }
    return regs;
}

void clean_regs(Env *env, int current)
{
    Vector *keys = env_keys(env);
    RegVal *regval;
    int i;
    char *key;
    for(i = 0; i < keys->length; i++){
        key = vector_get(keys, i);
        regval = env_get(env, key);
        if(regval && regval->end < current){
            regval->state = REMOVED;
            map_remove(env, key);
        }
    }
}

int disused_regs(Vector *regs)
{
    RegVal *regval;
    int index = -1;
    int i;
    for(i = 0; i < regs->length; i++){
        regval = vector_get(regs, i);
        if(regval == NULL){
            index = i;
            break;
        }
    }
    return index;
}

void set_reg(Vector *regs, int index, RegVal *regval)
{
    vector_set(regs, index, regval);
}

int _is_contain_lbl(const char *name, BDAsmVal *val)
{
    switch(val->kind){
        case AV_LBL:
            if(strcmp(name, val->u.u_lbl) == 0){
                return 1;
            }
            break;
        case AV_REG:
        case AV_MEM:
        case AV_IMMINT:
            break;
    }
    return 0;
}

int _find_live_range_end(const char *name, BDAsmExpr *e, int end, int count)
{
    switch(e->kind){
        case AE_NOP:
            return end;
        case AE_SET:
        case AE_MOV:
        case AE_UNIOP:
            if(_is_contain_lbl(name, e->u.u_uniop.val)){
                end = count;
            }
            return end;
        case AE_BINOP:
            if(_is_contain_lbl(name, e->u.u_binop.l) ||
                    _is_contain_lbl(name, e->u.u_binop.r)){
                end = count;
            }
            return end;
        case AE_IF:
        case AE_CALLCLS:
        case AE_CALLDIR:
            return end;
    }
}

int find_live_range_end(const char *name, BDAsmIns *ins, int end, int count)
{
    switch(ins->kind){
        case AI_ANS:
            return _find_live_range_end(name, ins->u.u_ans.expr, end, count);
        case AI_LET:
            end = _find_live_range_end(name, ins->u.u_let.val, end, count);
            return find_live_range_end(name, ins->u.u_let.body, end, count + 1);
    }
}

int latest_val(Vector *regs)
{
    int latest_end = -1;
    int latest_i = -1;
    int i;
    RegVal *regval;
    for(i = 0; i < regs->length; i++){
        regval = vector_get(regs, i);
        if(regval->end > latest_end){
            latest_end = regval->end;
            latest_i = i;
        }
    }

    return latest_i;
}

BDAsmVal *acc_reg()
{
    return bd_asmval_reg(ACC_REG_INDEX);
}

BDAsmVal *none_reg()
{
    return bd_asmval_reg(NONE_REG_INDEX);
}

BDAsmVal *find_reg(Env *env, const char *name, BDType *type)
{
    RegVal *regval = env_get(env, name);
    if(regval != NULL){
        return regval->val;
    }

    NoRegError *err = malloc(sizeof(NoRegError));
    err->name = name;
    err->type = type;
    throw(ERROR_NOREG, err);
}

/*
int store_reg(RegAllocState *st, RegVal *regval)
{
    int stack_i = st->stack_i++;
    regval->state = IN_STACK;
    regval->index = stack_i;
    regval->val = bd_asmval_mem(stack_i * -8);
    return stack_i;
}

BDAsmIns *_regalloc(RegAllocState *st, BDAsmExpr *e)
{
    switch(e->kind){
        case AE_NOP:
        case AE_SET:
            break;
        case AE_MOV:
        case AE_UNIOP:
            {
                char *name = e->u.u_uniop.val->u.u_lbl;
                BDAsmVal *val = find_reg(st->env, name);
                e->u.u_uniop.val = val;
                free(name);
            }
            break;
        case AE_BINOP:
            {
                char *name;

                name = e->u.u_binop.l->u.u_lbl;
                e->u.u_binop.l = find_reg(st->env, name);
                free(name);
                name = e->u.u_binop.r->u.u_lbl;
                e->u.u_binop.r = find_reg(st->env, name);
                free(name);
            }
            break;
        case AE_IF:
        case AE_CALLCLS:
        case AE_CALLDIR:
            break;
    }
    return bd_asmins_ans(e);
}


BDAsmIns *regalloc(RegAllocState *st, BDAsmIns *ins)
{
    // Clean regs.
    clean_regs(st->regs, st->current);

    switch(ins->kind){
        case AI_ANS:
            {
                st->current++;
                try{
                    return _regalloc(st, ins->u.u_ans.expr);
                }catch{
                    if(error_type == ERROR_NOREG){
                        NoRegError *err = catch_error();
                        RegVal *val = err->val;
                        return regalloc(st, bd_asmins_let(
                                    bd_asmval_lbl(val->name), bd_type_clone(val->type),
                                    bd_asmexpr_load(val->index * 8),
                                    ins
                                    ));
                    }else{
                        throw(error_type, catch_error());
                    }
                }
            }
        case AI_LET:
            {
                BDAsmVal *lbl = ins->u.u_let.lbl;
                BDType *type = ins->u.u_let.type;
                BDAsmExpr *val = ins->u.u_let.val;
                BDAsmIns *body = ins->u.u_let.body;

                RegVal *regval = malloc(sizeof(RegVal));
                regval->name = lbl->u.u_lbl;
                regval->type = type;
                regval->begin = st->current;
                regval->end = find_live_range_end(regval->name, body, regval->begin, regval->begin + 1);

                env_set(st->env, regval->name, regval);

                int disused_reg_i = disused_regs(st->regs);
                if(disused_reg_i < 0){
                    int latest_reg_i = latest_val(st->regs, regval);
                    RegVal *spill_val;
                    if(latest_reg_i < 0){
                        // Spill itself.
                        spill_val = regval;
                        store_reg(st, spill_val);

                        st->current++;

                        BDAsmIns *new_ins = bd_asmins_concat(
                                _regalloc(st, val),
                                acc_reg(), bd_type_clone(spill_val->type),
                                bd_asmins_let(
                                    none_reg(), bd_type_clone(spill_val->type),
                                    bd_asmexpr_store(acc_reg(), spill_val->val->u.u_mem.offset),
                                    regalloc(st, body)
                                ));

                        free(ins);
                        return new_ins;
                    }
                    else{
                        // Spill.
                        spill_val = vector_get(st->regs, latest_reg_i);
                        store_reg(st, spill_val);

                        // Add to regs.
                        regval->state = IN_REG;
                        regval->index = disused_reg_i;
                        regval->val = bd_asmval_reg(disused_reg_i);
                        set_reg(st->regs, disused_reg_i, regval);

                        st->current++;

                        BDAsmIns *new_ins = bd_asmins_let(
                                none_reg(), bd_type_clone(spill_val->type),
                                bd_asmexpr_store(bd_asmval_reg(latest_reg_i), spill_val->val->u.u_mem.offset),
                                bd_asmins_concat(
                                    _regalloc(st, val),
                                    bd_asmval_reg(disused_reg_i), bd_type_clone(type),
                                    regalloc(st, body)
                                    ));

                        free(ins);
                        return new_ins;
                    }

                }
                else{
                    regval->state = IN_REG;
                    regval->index = disused_reg_i;
                    regval->val = bd_asmval_reg(disused_reg_i);
                    set_reg(st->regs, disused_reg_i, regval);

                    st->current++;

                    BDAsmIns *new_ins = bd_asmins_concat(
                            _regalloc(st, val),
                            bd_asmval_reg(disused_reg_i), bd_type_clone(type),
                            regalloc(st, body));

                    free(ins);
                    return new_ins;
                }
            }
            break;
    }
}
*/

RegAllocResult *allocatable_reg(Env *env)
{
    int i;
    Vector *regs = init_regs(common_regs_length);
    Vector *live_regs = env_values(env);
    RegVal *val;
    int reg_i;
    for(i = 0; i < live_regs->length; i++){
        val = vector_get(live_regs, i);
        reg_i = val->val->u.u_reg.index;
        if(reg_i >= 0){
            vector_set(regs, i, val);
        }
    }

    val = NULL;
    reg_i = -1;
    for(i = 0; i < regs->length; i++){
        val = vector_get(regs, i);
        if(val == NULL){
            reg_i = i;
            break;
        }
    }

    RegAllocResult *ret = malloc(sizeof(RegAllocResult));
    if(reg_i < 0){
        ret->kind = RA_SPILL;
        ret->u.val = vector_get(regs, latest_val(regs));
    }
    else{
        ret->kind = RA_ALLOC;
        ret->u.index = reg_i;
    }
    return ret;
}

BDAsmIns *_regalloc(Env *env, BDAsmExpr *e, int current)
{
    switch(e->kind){
        case AE_NOP:
        case AE_SET:
            break;
        case AE_MOV:
        case AE_UNIOP:
            {
                BDAsmVal *val = find_reg(env, e->u.u_uniop.val->u.u_lbl, bd_type_int());
                return bd_asmins_ans(bd_asmexpr_uniop(e->u.u_uniop.kind, val));
            }
            break;
        case AE_BINOP:
            {
                BDAsmVal *l = find_reg(env, e->u.u_binop.l->u.u_lbl, bd_type_int());
                BDAsmVal *r = find_reg(env, e->u.u_binop.r->u.u_lbl, bd_type_int());
                return bd_asmins_ans(bd_asmexpr_binop(e->u.u_binop.kind, l, r));
            }
            break;
        case AE_IF:
        case AE_CALLCLS:
        case AE_CALLDIR:
            break;
    }
    return bd_asmins_ans(e);
}

BDAsmIns *regalloc(Env *env, BDAsmIns *ins, int current);

BDAsmIns *regalloc_and_restore(Env *env, BDAsmExpr *e, int current)
{
    try{
        return _regalloc(env, e, current);
    }catch{
        if(error_type == ERROR_NOREG){
            NoRegError *err = catch_error();
            const char *name = err->name;
            BDType *type = err->type;
            return regalloc(env,
                    bd_asmins_let(
                        bd_asmval_lbl(name), bd_type_clone(type),
                        bd_asmexpr_load(bd_asmval_lbl(name)),
                        bd_asmins_ans(e)
                        ), current - 2);
        }else{
            throw(error_type, catch_error());
        }
    }
}

BDAsmIns *regalloc(Env *env, BDAsmIns *ins, int current)
{
    clean_regs(env, current);

    switch(ins->kind){
        case AI_ANS:
            return regalloc_and_restore(env, ins->u.u_ans.expr, current + 1);
        case AI_LET:
            {
                BDAsmVal *lbl = ins->u.u_let.lbl;
                BDType *type = ins->u.u_let.type;
                BDAsmExpr *val = ins->u.u_let.val;
                BDAsmIns *body = ins->u.u_let.body;

                Env *local = env_local_new(env);
                BDAsmIns *newval = regalloc_and_restore(local, val, current + 1);

                RegVal *regval = malloc(sizeof(RegVal));
                regval->name = lbl->u.u_lbl;
                regval->type = type;
                regval->begin = current;
                regval->end = find_live_range_end(regval->name, body, regval->begin, regval->begin + 1);

                env_set(local, regval->name, regval);

                RegAllocResult *alloc = allocatable_reg(env);
                if(alloc->kind == RA_SPILL){
                    RegVal *target = alloc->u.val;
                    regval->val = bd_asmval_reg(target->val->u.u_reg.index);
                    map_remove(local, target->name);
                    return bd_asmins_let(
                            none_reg(), bd_type_unit(),
                            bd_asmexpr_store(target->val, target->name),
                            bd_asmins_concat(
                                newval,
                                regval->val, type,
                                regalloc(local, body, current + 1)));
                }
                else{
                    regval->val = bd_asmval_reg(alloc->u.index);
                    return bd_asmins_concat(
                            newval,
                            regval->val, type,
                            regalloc(local, body, current + 1));
                }
            }
            break;
    }
}

BDAsmProg *bd_register_allocate(BDAsmProg *prog)
{
    /*
    RegAllocState *st = malloc(sizeof(RegAllocState));
    st->env = env_new();
    st->regs = init_regs(common_regs_length);
    st->current = 0;
    st->stack_i = 0;
    */

    Env *env = env_new();

    BDAsmIns *ins = regalloc(env, prog->main, 0);
    prog->main = ins;
    bd_asmprog_show(prog);
    return NULL;
}
