#include "stdio.h"
#include "stdlib.h"
#include "asm.h"
#include "util/mem.h"


BDAsmProg *bd_asmprog(Vector *fundefs, BDAsmIns *main)
{
    BDAsmProg *prog = malloc(sizeof(BDAsmProg));
    prog->fundefs = fundefs;
    prog->main = main;
    return prog;
}

void bd_asmprog_destroy(BDAsmProg *prog)
{
    vector_each(prog->fundefs, bd_asmexpr_fundef_destory);
    bd_asmins_destroy(prog->main);
    free(prog);
}

BDAsmIns *bd_asmins_ans(BDAsmExpr *e)
{
    BDAsmIns *ins = malloc(sizeof(BDAsmIns));
    ins->kind = AI_ANS;
    ins->u.u_ans.expr = e;
    return ins;
}

BDAsmIns *bd_asmins_let(BDAsmVal *lbl, BDType *type, BDAsmExpr *val, BDAsmIns *body)
{
    BDAsmIns *ins = malloc(sizeof(BDAsmIns));
    ins->kind = AI_LET;
    ins->u.u_let.lbl = lbl;
    ins->u.u_let.type = type;
    ins->u.u_let.val = val;
    ins->u.u_let.body = body;
    return ins;
}

void bd_asmins_destroy(BDAsmIns *ins)
{
    switch(ins->kind){
        case AI_ANS:
            bd_asmexpr_destroy(ins->u.u_ans.expr);
            break;
        case AI_LET:
            bd_asmval_destroy(ins->u.u_let.lbl);
            bd_type_destroy(ins->u.u_let.type);
            bd_asmexpr_destroy(ins->u.u_let.val);
            bd_asmins_destroy(ins->u.u_let.body);
            break;
    }
    free(ins);
}

BDAsmExpr *bd_asmexpr(BDAsmExprKind kind)
{
    BDAsmExpr *e = malloc(sizeof(BDAsmExpr));
    e->kind = kind;
    return e;
}

BDAsmExpr *bd_asmexpr_nop()
{
    return bd_asmexpr(AE_NOP);
}

BDAsmExpr *bd_asmexpr_set(BDAsmVal *val)
{
    BDAsmExpr *e = bd_asmexpr(AE_SET);
    e->u.u_uniop.val = val;
    return e;
}

BDAsmExpr *bd_asmexpr_mov(BDAsmVal *lbl)
{
    BDAsmExpr *e = bd_asmexpr(AE_MOV);
    e->u.u_uniop.val = lbl;
    return e;
}

BDAsmExpr *bd_asmexpr_store(BDAsmVal *lbl, const char *name)
{
    BDAsmExpr *e = bd_asmexpr(AE_STORE);
    e->u.u_store.lbl = lbl;
    e->u.u_store.name = mem_strdup(name);
    return e;
}

BDAsmExpr *bd_asmexpr_load(BDAsmVal *lbl)
{
    BDAsmExpr *e = bd_asmexpr(AE_LOAD);
    e->u.u_store.lbl = lbl;
    e->u.u_store.name = NULL;
    return e;
}

BDAsmExpr *bd_asmexpr_uniop(BDOpKind kind, BDAsmVal *val)
{
    BDAsmExpr *e = bd_asmexpr(AE_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = val;
    return e;
}

BDAsmExpr *bd_asmexpr_binop(BDOpKind kind, BDAsmVal *l, BDAsmVal *r)
{
    BDAsmExpr *e = bd_asmexpr(AE_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = l;
    e->u.u_binop.r = r;
    return e;
}

BDAsmExpr *bd_asmexpr_if(BDOpKind kind, BDAsmVal *l, BDAsmVal *r, BDAsmIns *t, BDAsmIns *f)
{
    BDAsmExpr *e = bd_asmexpr(AE_IF);
    e->u.u_if.kind = kind;
    e->u.u_if.l = l;
    e->u.u_if.r = r;
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDAsmExpr *bd_asmexpr_call(BDAsmExprKind kind, BDAsmVal *fun, Vector *actuals)
{
    BDAsmExpr *e = bd_asmexpr(kind);
    e->u.u_call.fun = fun;
    e->u.u_call.actuals = actuals;
    return e;
}

void bd_asmexpr_destroy(BDAsmExpr *e)
{
    switch(e->kind){
        case AE_NOP:
            break;
        case AE_UNIOP:
            break;
        case AE_BINOP:
            break;
        case AE_IF:
            break;
        case AE_CALLCLS:
        case AE_CALLDIR:
            break;
    }
}

BDAsmVal *bd_asmval_lbl(const char *lbl)
{
    BDAsmVal *val = malloc(sizeof(BDAsmVal));
    val->kind = AV_LBL;
    val->u.u_lbl = mem_strdup(lbl);
    return val;
}

BDAsmVal *bd_asmval_reg(int index)
{
    BDAsmVal *val = malloc(sizeof(BDAsmVal));
    val->kind = AV_REG;
    val->u.u_reg.index = index;
    return val;
}

BDAsmVal *bd_asmval_mem(int offset)
{
    BDAsmVal *val = malloc(sizeof(BDAsmVal));
    val->kind = AV_MEM;
    val->u.u_mem.offset = offset;
    return val;
}

BDAsmVal *bd_asmval_int(int imm)
{
    BDAsmVal *val = malloc(sizeof(BDAsmVal));
    val->kind = AV_IMMINT;
    val->u.u_int = imm;
    return val;
}

BDAsmVal *bd_asmval_clone(BDAsmVal *val)
{
    switch(val->kind){
        case AV_LBL:
            return bd_asmval_lbl(val->u.u_lbl);
        case AV_REG:
            return bd_asmval_reg(val->u.u_reg.index);
        case AV_MEM:
            return bd_asmval_mem(val->u.u_mem.offset);
        case AV_IMMINT:
            return bd_asmval_int(val->u.u_int);
    }
}

void bd_asmval_destroy(BDAsmVal *val)
{
    switch(val->kind){
        case AV_LBL:
            free(val->u.u_lbl);
            break;
        case AV_REG:
        case AV_MEM:
        case AV_IMMINT:
            break;
    }
    free(val);
}

BDAsmExprFundef *bd_asmexpr_fundef(const char *name, BDType *type, Vector *formals, BDAsmIns *body)
{
    BDAsmExprFundef *fundef = malloc(sizeof(BDAsmExprFundef));
    fundef->name = mem_strdup(name);
    fundef->type = type;
    fundef->formals = formals;
    fundef->body = body;
    return fundef;
}

void bd_asmexpr_fundef_destory(BDAsmExprFundef *fundef)
{
    free(fundef->name);
    bd_type_destroy(fundef->type);
    vector_each(fundef->formals, free);
    bd_asmins_destroy(fundef->body);
    free(fundef);
}

char *asmval_text(BDAsmVal *val)
{
    char *text = malloc(sizeof(char) * 20);
    switch(val->kind){
        case AV_LBL:
            sprintf(text, "%s", val->u.u_lbl);
            break;
        case AV_REG:
            sprintf(text, "R%d", val->u.u_reg.index);
            break;
        case AV_MEM:
            sprintf(text, "[bp+%d]", val->u.u_mem.offset);
            break;
        case AV_IMMINT:
            sprintf(text, "%d", val->u.u_int);
            break;
    }
    return text;
}

void asmexpr_show(BDAsmExpr *e)
{
    char *text1 = NULL;
    char *text2 = NULL;
    switch(e->kind){
        case AE_NOP:
            printf("NOP\n");
            break;
        case AE_SET:
            text1 = asmval_text(e->u.u_uniop.val);
            printf("SET\t%s\n", text1);
            break;
        case AE_MOV:
            text1 = asmval_text(e->u.u_uniop.val);
            printf("MOV\t%s\n", text1);
            break;
        case AE_UNIOP:
            {
                char *op;
                switch(e->u.u_uniop.kind){
                    case OP_NEG:
                        op = "NEG";
                        break;
                }
                text1 = asmval_text(e->u.u_uniop.val);
                printf("%s\t%s\n", op, text1);
            }
            break;
        case AE_BINOP:
            {
                char *op;
                switch(e->u.u_uniop.kind){
                    case OP_ADD:
                        op = "ADD";
                        break;
                    case OP_SUB:
                        op = "SUB";
                        break;
                    case OP_MUL:
                        op = "MUL";
                        break;
                    case OP_DIV:
                        op = "DIV";
                        break;
                }
                text1 = asmval_text(e->u.u_binop.l);
                text2 = asmval_text(e->u.u_binop.r);
                printf("%s\t%s, %s\n", op, text1, text2);
            }
            break;
        case AE_IF:
            break;
        case AE_CALLCLS:
            break;
        case AE_CALLDIR:
            break;
        case AE_STORE:
            text1 = asmval_text(e->u.u_store.lbl);
            printf("STORE\t%s(%s)\n", text1, e->u.u_store.name);
            break;
        case AE_LOAD:
            text1 = asmval_text(e->u.u_store.lbl);
            printf("LOAD\t%s\n", text1);
            break;
    }

    if(text1 != NULL){
        free(text1);
    }
    if(text2 != NULL){
        free(text2);
    }
}

void asmins_show(BDAsmIns *ins)
{
    switch(ins->kind){
        case AI_ANS:
            asmexpr_show(ins->u.u_ans.expr);
            break;
        case AI_LET:
            if(ins->u.u_let.type->kind == T_UNIT){
                printf("LET\t_ = ");
            }
            else{
                printf("LET\t%s::%s = ", asmval_text(ins->u.u_let.lbl), bd_type_show(ins->u.u_let.type));
            }
            asmexpr_show(ins->u.u_let.val);
            asmins_show(ins->u.u_let.body);
            break;
    }
}

void bd_asmprog_show(BDAsmProg *prog)
{
    asmins_show(prog->main);
}

static char *regs[] = {"rax", "rbx", "rcx", "rsi", "rdi", NULL};

char *bd_asmval_emit(BDAsmVal *val)
{
    switch(val->kind){
        case AV_LBL:
            return mem_strdup(val->u.u_lbl);
        case AV_REG:
            return mem_strdup(regs[val->u.u_reg.index]);
        case AV_MEM:
            break;
        case AV_IMMINT:
            {
                char *text = malloc(sizeof(char) * 10);
                sprintf(text, "%d", val->u.u_int);
                return text;
            }
    }
}

BDAsmIns *bd_asmins_concat(BDAsmIns *e1, BDAsmVal *lbl, BDType *type, BDAsmIns *e2)
{
    BDAsmIns *ret;
    switch(e1->kind){
        case AI_ANS:
            ret = bd_asmins_let(lbl, type, e1->u.u_ans.expr, e2);
            break;
        case AI_LET:
            ret = bd_asmins_let(e1->u.u_let.lbl, e1->u.u_let.type, e1->u.u_let.val,
                    bd_asmins_concat(e1->u.u_let.body, lbl, type, e2));
            break;
    }
    free(e1);
    return ret;
}
