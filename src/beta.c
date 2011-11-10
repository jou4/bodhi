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

BDNExpr *beta_reduce(Env *env, BDNExpr *e)
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
                BDNExpr *val = beta_reduce(env, e->u.u_let.val);
                if(val->kind == E_VAR){
                    // beta-reducing
                    Env *local = env_local_new(env);
                    env_set(local, e->u.u_let.ident->name, val->u.u_var.name);
                    BDNExpr *reduced = beta_reduce(local, e->u.u_let.body);
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
            e->u.u_letrec.fun = beta_reduce(env, e->u.u_letrec.fun);
            e->u.u_letrec.body = beta_reduce(env, e->u.u_letrec.body);
            break;
        case E_FUN:
            e->u.u_fun.body = beta_reduce(env, e->u.u_fun.body);
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
        default:
            break;
    }
    return e;
}

BDNProgram *bd_beta_reduce(BDNProgram *prog)
{
    Env *env = env_new();

    int i;
    Vector *vec;
    BDNExpr *e;
    BDNExprDef *def;

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        def->body = beta_reduce(env, def->body);
        vector_set(vec, i, def);
    }

    prog->maindef->body = beta_reduce(env, prog->maindef->body);

    env_destroy(env);

    return prog;
}
