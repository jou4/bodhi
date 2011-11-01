#include "compile.h"

int expr2_size(BDExpr2 *e)
{
    switch(e->kind){
        case E_IF:
            return 1 + expr2_size(e->u.u_if.t) + expr2_size(e->u.u_if.f);
        case E_LET:
            return 1 + expr2_size(e->u.u_let.val) + expr2_size(e->u.u_let.body);
        case E_LETREC:
            return 1 + expr2_size(e->u.u_letrec.fundef->body) + expr2_size(e->u.u_letrec.body);
        case E_LETTUPLE:
            return 1 + expr2_size(e->u.u_lettuple.body);
        default:
            return 1;
    }
}

BDExpr2 *inline_expand(Env *env, int threashold, BDExpr2 *e)
{
    switch(e->kind){
        case E_IF:
            e->u.u_if.t = inline_expand(env, threashold, e->u.u_if.t);
            e->u.u_if.f = inline_expand(env, threashold, e->u.u_if.f);
            break;
        case E_LET:
            e->u.u_let.val = inline_expand(env, threashold, e->u.u_let.val);
            e->u.u_let.body = inline_expand(env, threashold, e->u.u_let.body);
            break;
        case E_LETREC:
            {
                BDExpr2Fundef *fundef = e->u.u_letrec.fundef;

                Env *local = env_local_new(env);
                if(expr2_size(fundef->body) <= threashold){
                    env_set(local, fundef->ident->name, fundef);
                }

                fundef->body = inline_expand(local, threashold, fundef->body);
                e->u.u_letrec.body = inline_expand(local, threashold, e->u.u_letrec.body);

                env_local_destroy(local);
            }
            break;
        case E_APP:
            {
                BDExpr2Fundef *fundef = env_get(env, e->u.u_app.fun);
                if(fundef){
                    Vector *actuals = e->u.u_app.actuals;
                    Vector *formals = fundef->formals;
                    BDExpr2 *body = fundef->body;

                    Env *alphalocal = env_new();
                    int i;
                    BDExprIdent *formal;
                    for(i = 0; i < actuals->length; i++){
                        formal = vector_get(formals, i);
                        env_set(alphalocal, formal->name, vector_get(actuals, i));
                    }

                    BDExpr2 *newexpr = bd_alpha(alphalocal, body);
                    env_destroy(alphalocal);

                    bd_expr2_destroy(e);

                    return newexpr;
                }
            }
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.body = inline_expand(env, threashold, e->u.u_lettuple.body);
            break;
    }
    return e;
}

BDExpr2 *bd_inline_expand(int threashold, BDExpr2 *e)
{
    Env *env = env_new();
    BDExpr2 *ret = inline_expand(env, threashold, e);
    env_destroy(env);

    printf("--- Inline expanded --- \n");
    bd_expr2_show(ret);
    printf("\n");

    return ret;
}
