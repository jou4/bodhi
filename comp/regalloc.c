#include "compile.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

#define ACC_REG_INDEX -1
#define NONE_REG_INDEX -2

static int common_regs_length = 3;

typedef struct {
    char *name;
    BDType *type;
    int begin;
    int end;
    BDAsmVal *val;
} RegVal;

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

typedef struct {
    BDAsmIns *ins;
    Env *env;
} RegAllocPair;

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
            vector_set(regs, reg_i, val);
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

RegAllocPair regalloc_pair(BDAsmIns *ins, Env *env)
{
    RegAllocPair pair = {ins, env};
    return pair;
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

RegAllocPair regalloc(Env *env, BDAsmIns *ins, int current);

RegAllocPair regalloc_and_restore(Env *env, BDAsmExpr *e, int current)
{
    try{
        return regalloc_pair(_regalloc(env, e, current + 1), env);
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
                        ), current - 1);
        }else{
            throw(error_type, catch_error());
        }
    }
}

RegAllocPair regalloc(Env *env, BDAsmIns *ins, int current)
{
    clean_regs(env, current);

    switch(ins->kind){
        case AI_ANS:
            return regalloc_and_restore(env, ins->u.u_ans.expr, current);
        case AI_LET:
            {
                BDAsmVal *lbl = ins->u.u_let.lbl;
                BDType *type = ins->u.u_let.type;
                BDAsmExpr *val = ins->u.u_let.val;
                BDAsmIns *body = ins->u.u_let.body;

                RegAllocPair pair = regalloc_and_restore(env, val, current);
                BDAsmIns *newval = pair.ins;
                Env *local = pair.env;

                RegAllocResult *alloc = allocatable_reg(local);

                RegVal *regval = malloc(sizeof(RegVal));
                regval->name = lbl->u.u_lbl;
                regval->type = type;
                regval->begin = current;
                regval->end = find_live_range_end(regval->name, body, regval->begin, regval->begin + 1);

                env_set(local, regval->name, regval);

                if(alloc->kind == RA_SPILL){
                    RegVal *target = alloc->u.val;
                    regval->val = bd_asmval_reg(target->val->u.u_reg.index);
                    map_remove(local, target->name);
                    RegAllocPair pair = regalloc(local, body, current + 1);
                    return regalloc_pair(bd_asmins_let(
                                none_reg(), bd_type_unit(),
                                bd_asmexpr_store(target->val, target->name),
                                bd_asmins_concat(
                                    newval,
                                    regval->val, type,
                                    pair.ins)),
                            local);
                }
                else{
                    regval->val = bd_asmval_reg(alloc->u.index);
                    RegAllocPair pair = regalloc(local, body, current + 1);
                    return regalloc_pair(bd_asmins_concat(
                                newval,
                                regval->val, type,
                                pair.ins),
                            local);
                }
            }
            break;
    }
}

BDAsmProg *bd_register_allocate(BDAsmProg *prog)
{
    Env *env = env_new();

    RegAllocPair pair = regalloc(env, prog->main, 0);
    prog->main = pair.ins;
    bd_asmprog_show(prog);

    return prog;
}
