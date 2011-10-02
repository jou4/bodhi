#include "compile.h"
#include "util/mem.h"

BDAsmIns *virtual_expr_concat(BDAsmIns *e1, BDExprIdent *ident, BDAsmIns *e2)
{
    BDAsmIns *ret;
    switch(e1->kind){
        case AI_ANS:
            ret = bd_asmins_let(ident, e1->u.u_ans.expr, e2);
            break;
        case AI_LET:
            ret = bd_asmins_let(e1->u.u_let.ident, e1->u.u_let.val,
                    virtual_expr_concat(e1->u.u_let.body, ident, e2));
            break;
    }
    free(e1);
    return ret;
}

BDAsmIns *virtual_expr(Env *env, BDExpr3 *e)
{
    switch(e->kind){
        case E_UNIT:
            return bd_asmins_ans(bd_asmexpr_nop());
        case E_INT:
            return bd_asmins_ans(bd_asmexpr_set(e->u.u_int));
        //case E_FLOAT:
        case E_UNIOP:
            return bd_asmins_ans(bd_asmexpr_uniop(e->u.u_uniop.kind, e->u.u_uniop.val));
        case E_BINOP:
            return bd_asmins_ans(bd_asmexpr_binop(e->u.u_binop.kind, e->u.u_binop.l, e->u.u_binop.r));
        case E_IF:
            return bd_asmins_ans(bd_asmexpr_if(e->u.u_if.kind,
                        e->u.u_if.l, e->u.u_if.r,
                        virtual_expr(env, e->u.u_if.t), virtual_expr(env, e->u.u_if.f)));
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                BDAsmIns *e1 = virtual_expr(env, e->u.u_let.val);
                BDAsmIns *e2 = virtual_expr(local, e->u.u_let.body);

                env_local_destroy(local);
                return virtual_expr_concat(e1, ident, e2);
            }
            break;
        case E_VAR:
            {
                BDType *type = env_get(env, e->u.u_var.name);
                if(type->kind == T_UNIT){
                    return bd_asmins_ans(bd_asmexpr_nop());
                }
                else{
                    return bd_asmins_ans(bd_asmexpr_mov(e->u.u_var.name));
                }
            }
        case E_TUPLE:
        case E_LETTUPLE:
        case E_ARRAY:
        case E_MAKECLS:
        case E_APPCLS:
        case E_APPDIR:
            break;
    }
}

BDAsmExprFundef *virtual_fundef(BDExpr3Fundef *fundef)
{
    Env *env = env_new();

    Vector *formals = fundef->formals;
    Vector *freevars = fundef->freevars;

    Vector *newformals = vector_new();
    BDExprIdent *ident;
    int i;
    int offset = 0;
    for(i = 0; i < formals->length; i++){
        ident = vector_get(formals, i);
        vector_add(newformals, mem_strdup(ident->name));
        env_set(env, ident->name, ident->type);
    }

    for(i = 0; i < freevars->length; i++){
        ident = vector_get(freevars, i);
        vector_add(newformals, mem_strdup(ident->name));
        env_set(env, ident->name, ident->type);
    }

    BDAsmIns *body = virtual_expr(env, fundef->body);
}

BDAsmProg *bd_virtual(BDProgram1 *prog)
{
    Vector *vec = prog->fundefs;
    Vector *fundefs = vector_new();
    int i;
    for(i = 0; i < vec->length; i++){
        vector_add(fundefs, virtual_fundef(vector_get(vec, i)));
    }

    Env *env = env_new();
    BDAsmProg *newprog = bd_asmprog(fundefs, virtual_expr(env, prog->main));
    env_destroy(env);

    // TODO destroy prog.

    printf("--- Generated vm code. --- \n");
    bd_asmprog_show(newprog);
    printf("\n");

    return newprog;
}
