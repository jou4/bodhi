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

BDAsmIns *bd_asmins_let(BDExprIdent *ident, BDAsmExpr *val, BDAsmIns *body)
{
    BDAsmIns *ins = malloc(sizeof(BDAsmIns));
    ins->kind = AI_LET;
    ins->u.u_let.ident = ident;
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
            bd_expr_ident_destroy(ins->u.u_let.ident);
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

BDAsmExpr *bd_asmexpr_set(int val)
{
    BDAsmExpr *e = bd_asmexpr(AE_SET);
    e->u.u_int = val;
    return e;
}

BDAsmExpr *bd_asmexpr_mov(const char *lbl)
{
    BDAsmExpr *e = bd_asmexpr(AE_MOV);
    e->u.u_lbl = mem_strdup(lbl);
    return e;
}

BDAsmExpr *bd_asmexpr_uniop(BDOpKind kind, const char *val)
{
    BDAsmExpr *e = bd_asmexpr(AE_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = mem_strdup(val);
    return e;
}

BDAsmExpr *bd_asmexpr_binop(BDOpKind kind, const char *l, const char *r)
{
    BDAsmExpr *e = bd_asmexpr(AE_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = mem_strdup(l);
    e->u.u_binop.r = mem_strdup(r);
    return e;
}

BDAsmExpr *bd_asmexpr_if(BDOpKind kind, const char *l, const char *r, BDAsmIns *t, BDAsmIns *f)
{
    BDAsmExpr *e = bd_asmexpr(AE_IF);
    e->u.u_if.kind = kind;
    e->u.u_if.l = mem_strdup(l);
    e->u.u_if.r = mem_strdup(r);
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDAsmExpr *bd_asmexpr_call(BDAsmExprKind kind, const char *fun, Vector *actuals)
{
    BDAsmExpr *e = bd_asmexpr(kind);
    e->u.u_call.fun = mem_strdup(fun);
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

void asmexpr_show(BDAsmExpr *e)
{
    switch(e->kind){
        case AE_NOP:
            printf("NOP\n");
            break;
        case AE_SET:
            printf("SET\t%d\n", e->u.u_int);
            break;
        case AE_MOV:
            printf("MOV\t%s\n", e->u.u_lbl);
            break;
        case AE_UNIOP:
            {
                char *op;
                switch(e->u.u_uniop.kind){
                    case OP_NEG:
                        op = "NEG";
                        break;
                }
                printf("%s\t%s\n", op, e->u.u_uniop.val);
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
                printf("%s\t%s, %s\n", op, e->u.u_binop.l, e->u.u_binop.r);
            }
            break;
        case AE_IF:
            break;
        case AE_CALLCLS:
            break;
        case AE_CALLDIR:
            break;
    }
}

void asmins_show(BDAsmIns *ins)
{
    switch(ins->kind){
        case AI_ANS:
            asmexpr_show(ins->u.u_ans.expr);
            break;
        case AI_LET:
            printf("LET\t%s::%s = ", ins->u.u_let.ident->name, bd_type_show(ins->u.u_let.ident->type));
            asmexpr_show(ins->u.u_let.val);
            asmins_show(ins->u.u_let.body);
            break;
    }
}

void bd_asmprog_show(BDAsmProg *prog)
{
    asmins_show(prog->main);
}
