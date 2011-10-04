#include "compile.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util/exception.h"

static char *acc_reg = "rax";
static char *common_regs[] = {"rbx", "rcx", "rsi", "rdi"};
static int common_regs_length = 4;

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
    RegValState state;
    int index;
} RegVal;

typedef struct {
    Env *env;
    Vector *regs;
    int current;
    int stack_i;
} RegAllocState;

#define ERROR_NOREG 105
typedef struct {
    RegVal *val;
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

void clean_regs(Vector *regs, int current)
{
    RegVal *regval;
    int i;
    for(i = 0; i < regs->length; i++){
        regval = vector_get(regs, i);
        if(regval && regval->end < current){
            regval->state = REMOVED;
            vector_set(regs, i, NULL);
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

int _find_live_range_end(const char *name, BDAsmExpr *e, int end, int count)
{
    switch(e->kind){
        case AE_NOP:
            return end;
        case AE_SET:
            return end;
        case AE_MOV:
            if(strcmp(name, e->u.u_lbl) == 0){
                end = count;
            }
            return end;
        case AE_UNIOP:
            if(strcmp(name, e->u.u_uniop.val) == 0){
                end = count;
            }
            return end;
        case AE_BINOP:
            if(strcmp(name, e->u.u_binop.l) == 0 ||
                    strcmp(name, e->u.u_binop.r) == 0){
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

int latest_val(Vector *regs, RegVal *val)
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

    if(val->end > latest_end){
        return -1;
    }
    return latest_i;
}

char *common_reg(int i)
{
    return common_regs[i];
}

char *find_reg(Env *env, const char *name)
{
    RegVal *val = env_get(env, name);
    if(val != NULL){
        return common_reg(val->index);
    }

    NoRegError *err = malloc(sizeof(NoRegError));
    err->val = val;
    throw(ERROR_NOREG, err);
}

BDAsmIns *_regalloc(RegAllocState *st, BDAsmExpr *e)
{
    switch(e->kind){
        case AE_NOP:
        case AE_SET:
            break;
        case AE_MOV:
            {
                char *name = e->u.u_lbl;
                e->u.u_lbl = find_reg(st->env, name);
                free(name);
            }
        case AE_UNIOP:
            {
                char *name = e->u.u_uniop.val;
                e->u.u_uniop.val = find_reg(st->env, name);
                free(name);
            }
            break;
        case AE_BINOP:
            {
                char *name;

                name = e->u.u_binop.l;
                e->u.u_binop.l = find_reg(st->env, name);
                free(name);
                name = e->u.u_binop.r;
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
                        /*
                        return regalloc(st, bd_asmins_let(
                                    bd_expr_ident(val->name, bd_type_clone(val->type)),
                                    bd_asmexpr_load(val->index * 8),
                                    ins
                                    ));
                                    */
                    }else{
                        throw(error_type, catch_error());
                    }
                }
            }
        case AI_LET:
            {
                BDExprIdent *ident = ins->u.u_let.ident;
                BDAsmExpr *val = ins->u.u_let.val;
                BDAsmIns *body = ins->u.u_let.body;

                RegVal *regval = malloc(sizeof(RegVal));
                regval->name = ident->name;
                regval->type = ident->type;
                regval->begin = st->current;
                regval->end = find_live_range_end(regval->name, body, regval->begin, regval->begin + 1);
                env_set(st->env, regval->name, regval);

                int disused_reg_i = disused_regs(st->regs);
                if(disused_reg_i < 0){
                    int latest_reg_i = latest_val(st->regs, regval);
                    int stack_i = st->stack_i++;
                    int stack_offset = stack_i * 8;
                    RegVal *spill_val;
                    if(latest_reg_i < 0){
                        // Spill itself.
                        spill_val = regval;
                        spill_val->state = IN_STACK;
                        spill_val->index = stack_i;

                        st->current++;

                        BDAsmIns *new_ins = bd_asmins_let(
                                bd_expr_ident(acc_reg, bd_type_clone(spill_val->type)),
                                val,
                                bd_asmins_let(
                                    bd_expr_ident(acc_reg, bd_type_clone(spill_val->type)),
                                    bd_asmexpr_store(acc_reg, stack_offset),
                                    regalloc(st, body)
                                ));

                        free(ins);
                        return new_ins;
                    }
                    else{
                        // Spill.
                        spill_val = vector_get(st->regs, latest_reg_i);
                        spill_val->state = IN_STACK;
                        spill_val->index = stack_i;

                        // Add to regs.
                        regval->state = IN_REG;
                        regval->index = disused_reg_i;
                        set_reg(st->regs, disused_reg_i, regval);

                        st->current++;

                        BDAsmIns *new_ins = bd_asmins_let(
                                bd_expr_ident(acc_reg, bd_type_clone(spill_val->type)),
                                bd_asmexpr_store(common_reg(spill_val->index), stack_offset),
                                bd_asmins_let(
                                    bd_expr_ident(common_reg(disused_reg_i), bd_type_clone(ident->type)),
                                    val,
                                    regalloc(st, body)
                                    ));

                        free(ins);
                        return new_ins;
                    }

                }
                else{
                    regval->state = IN_REG;
                    regval->index = disused_reg_i;
                    set_reg(st->regs, disused_reg_i, regval);

                    st->current++;

                    ident->name = common_reg(disused_reg_i);

                    BDAsmIns *new_ins = bd_asmins_let(ident, val, regalloc(st, body));

                    free(ins);
                    return new_ins;
                }
            }
            break;
    }
}

BDAsmProg *bd_register_allocate(BDAsmProg *prog)
{
    RegAllocState *st = malloc(sizeof(RegAllocState));
    st->env = env_new();
    st->regs = init_regs(common_regs_length);
    st->current = 0;
    st->stack_i = 0;

    BDAsmIns *ins = regalloc(st, prog->main);
    prog->main = ins;
    bd_asmprog_show(prog);
    return NULL;
}
