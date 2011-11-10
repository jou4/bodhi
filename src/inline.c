#include "compile.h"

int nexpr_size(BDNExpr *e)
{
    switch(e->kind){
        case E_IF:
            return 1 + nexpr_size(e->u.u_if.t) + nexpr_size(e->u.u_if.f);
        case E_LET:
            return 1 + nexpr_size(e->u.u_let.val) + nexpr_size(e->u.u_let.body);
        case E_LETREC:
            return 1 + nexpr_size(e->u.u_letrec.fun) + nexpr_size(e->u.u_letrec.body);
		case E_FUN:
			return 1 + nexpr_size(e->u.u_fun.body);
        case E_LETTUPLE:
            return 1 + nexpr_size(e->u.u_lettuple.body);
        default:
            return 1;
    }
}

BDNExpr *inline_expand(Env *env, int threashold, BDNExpr *e)
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
				BDExprIdent *ident = e->u.u_letrec.ident;
                BDNExpr *fun = e->u.u_letrec.fun;

                Env *local = env_local_new(env);
                if(nexpr_size(fun) <= threashold){
                    env_set(local, ident->name, fun);
                }

                e->u.u_letrec.fun = inline_expand(local, threashold, fun);
                e->u.u_letrec.body = inline_expand(local, threashold, e->u.u_letrec.body);

                env_local_destroy(local);
            }
            break;
		case E_FUN:
			e->u.u_fun.body = inline_expand(env, threashold, e->u.u_fun.body);
			break;
        case E_APP:
            {
                BDNExpr *fun = env_get(env, e->u.u_app.fun);
                if(fun && fun->kind == E_FUN){
                    Vector *actuals = e->u.u_app.actuals;
                    Vector *formals = fun->u.u_fun.formals;
                    BDNExpr *body = fun->u.u_fun.body;

                    Env *alphalocal = env_new();
                    int i;
                    BDExprIdent *formal;
                    for(i = 0; i < actuals->length; i++){
                        formal = vector_get(formals, i);
                        env_set(alphalocal, formal->name, vector_get(actuals, i));
                    }

                    BDNExpr *newexpr = bd_alpha(alphalocal, body);
                    env_destroy(alphalocal);

                    bd_nexpr_destroy(e);

                    return newexpr;
                }
            }
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.body = inline_expand(env, threashold, e->u.u_lettuple.body);
            break;
		default:
			break;
    }
    return e;
}

BDNProgram *bd_inline_expand(int threashold, BDNProgram *prog)
{
    Env *env = env_new();

    int i;
    Vector *vec;
    BDNExpr *e;
    BDNExprDef *def;

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        def->body = inline_expand(env, threashold, def->body);
        vector_set(vec, i, def);
    }

    prog->maindef->body = inline_expand(env, threashold, prog->maindef->body);

    env_destroy(env);

    return prog;
}
