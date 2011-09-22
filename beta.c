#include "compile.h"

char *try_find(Env *env, char *key)
{
    char *val = env_get(env, key);

    if(val == NULL){
        return key;
    }

    free(key);
    return val;
}

BDExpr2 *beta_reduce(Env *env, BDExpr2 *e)
{
    switch(e->kind)
    {
        case E_UNIOP:
            e->u.u_uniop.val = try_find(env, e->u.u_uniop.val);
            break;
        case E_BINOP:
            e->u.u_binop.l = try_find(env, e->u.u_binop.l);
            e->u.u_binop.r = try_find(env, e->u.u_binop.r);
            break;
        case E_IF:
            e->u.u_if.l = try_find(env, e->u.u_if.l);
            e->u.u_if.r = try_find(env, e->u.u_if.r);
            e->u.u_if.t = beta_reduce(env, e->u.u_if.t);
            e->u.u_if.f = beta_reduce(env, e->u.u_if.f);
            break;
        case E_LET:
            {
                BDExpr2 *val = beta_reduce(env, e->u.u_let.val);
                if(val->kind == E_VAR){
                    // beta-reducing
                    Env *local = env_local_new(env);
                    env_set(local, e->u.u_let.ident->name, val->u.u_var.name);
                    BDExpr2 *reduced = beta_reduce(local, e->u.u_let.body);
                    env_local_destroy(local);

                    free(e);
                    return reduced;
                }
                else{
                    e->u.u_let.val = val;
                    e->u.u_let.body = beta_reduce(env, e->u.u_let.body);
                }
            }
            break;
        case E_VAR:
            e->u.u_var.name = try_find(env, e->u.u_var.name);
            break;
        case E_LETREC:
            e->u.u_letrec.fundef->body = beta_reduce(env, e->u.u_letrec.fundef->body);
            e->u.u_letrec.body = beta_reduce(env, e->u.u_letrec.body);
            break;
        case E_APP:
            {
                e->u.u_app.fun = try_find(env, e->u.u_app.fun);
                Vector *actuals = e->u.u_app.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, try_find(env, vector_get(actuals, i)));
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;
                for(i = 0; i < elems->length; i++){
                    vector_set(elems, i, try_find(env, vector_get(elems, i)));
                }
            }
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.val = try_find(env, e->u.u_lettuple.val);
            e->u.u_lettuple.body = beta_reduce(env, e->u.u_lettuple.body);
            break;
    }
    return e;
}

BDExpr2 *bd_beta_reduce(BDExpr2 *e)
{
    Env *env = env_new();
    BDExpr2 *ret = beta_reduce(env, e);
    env_destroy(env);

    printf("--- β reduced --- \n");
    bd_expr2_show(ret);
    printf("\n");

    return ret;
}
