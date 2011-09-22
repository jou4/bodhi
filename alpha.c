#include "compile.h"

BDExpr2 *bd_alpha(Env *env, BDExpr2 *e)
{
    switch(e->kind){
        case E_UNIT:
            return bd_expr2_unit();
        case E_INT:
            return bd_expr2_int(e->u.u_int);
        case E_FLOAT:
            return bd_expr2_float(e->u.u_double);
        case E_UNIOP:
            return bd_expr2_uniop(e->u.u_uniop.kind, env_get(env, e->u.u_uniop.val));
        case E_BINOP:
            return bd_expr2_binop(e->u.u_binop.kind, env_get(env, e->u.u_binop.l), env_get(env, e->u.u_binop.r));
        case E_IF:
            return bd_expr2_if(e->u.u_if.kind
                    , env_get(env, e->u.u_if.l), env_get(env, e->u.u_if.r)
                    , bd_alpha(env, e->u.u_if.t), bd_alpha(env, e->u.u_if.f));
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                char *oldname = ident->name;
                char *newname = bd_generate_id(ident->type);

                BDExprIdent *newident = bd_expr_ident(newname, bd_type_clone(ident->type));

                Env *local = env_local_new(env);
                env_set(local, oldname, newname);

                BDExpr2 *newexpr = bd_expr2_let(newident
                        , bd_alpha(local, e->u.u_let.val)
                        , bd_alpha(local, e->u.u_let.body));

                env_local_destroy(local);

                return newexpr;
            }
        case E_VAR:
            return bd_expr2_var(env_get(env, e->u.u_var.name));
        case E_LETREC:
            {
                BDExpr2Fundef *fundef = e->u.u_letrec.fundef;
                char *oldfunname = fundef->ident->name;
                char *newfunname = bd_generate_id(fundef->ident->type);

                // create local env of letrec-body
                Env *local = env_local_new(env);
                env_set(local, oldfunname, newfunname);

                // create local env of function-body
                Env *funlocal = env_local_new(local);
                Vector *formals = fundef->formals;
                Vector *newformals = vector_new();
                int i;
                BDExprIdent *formal;
                char *oldformalname, *newformalname;
                for(i = 0; i < formals->length; i++){
                    formal = vector_get(formals, i);
                    oldformalname = formal->name;
                    newformalname = bd_generate_id(formal->type);

                    vector_add(newformals, bd_expr_ident(newformalname, bd_type_clone(formal->type)));
                    env_set(funlocal, oldformalname, newformalname);
                }

                // convert function-body and create new-funciton
                BDExpr2Fundef *newfundef = bd_expr2_fundef(
                        bd_expr_ident(newfunname, bd_type_clone(fundef->ident->type))
                        , newformals
                        , bd_alpha(funlocal, fundef->body));

                // destroy local env of function-body
                env_local_destroy(funlocal);

                // convert letrec-body and create new-letrec
                BDExpr2 *newexpr = bd_expr2_letrec(
                        newfundef
                        , bd_alpha(local, e->u.u_letrec.body));

                // destroy local env of letrec-body
                env_local_destroy(local);

                return newexpr;
            }
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                Vector *newactuals = vector_new();
                int i;
                char *oldname;
                for(i = 0; i < actuals->length; i++){
                    oldname = vector_get(actuals, i);
                    vector_add(newactuals, env_get(env, oldname));
                }

                return bd_expr2_app(env_get(env, e->u.u_app.fun), newactuals);
            }
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                Vector *newelems = vector_new();
                int i;
                char *oldname;
                for(i = 0; i < elems->length; i++){
                    oldname = vector_get(elems, i);
                    vector_add(newelems, env_get(env, oldname));
                }

                return bd_expr2_tuple(newelems);
            }
        case E_LETTUPLE:
            {
                Env *local = env_local_new(env);

                Vector *idents = e->u.u_lettuple.idents;
                Vector *newidents = vector_new();
                BDExprIdent *ident;
                int i;
                char *oldname, *newname;
                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);
                    oldname = ident->name;
                    newname = bd_generate_id(ident->type);

                    vector_add(newidents, bd_expr_ident(newname, bd_type_clone(ident->type)));
                    env_set(local, oldname, newname);
                }

                BDExpr2 *newexpr = bd_expr2_lettuple(
                        newidents
                        , env_get(env, e->u.u_lettuple.val)
                        , bd_alpha(local, e->u.u_lettuple.body));

                env_local_destroy(local);

                return newexpr;
            }
            break;
    }
    return NULL;
}

BDExpr2 *bd_alpha_convert(BDExpr2 *e)
{
    Env *env = env_new();
    BDExpr2 *ret = bd_alpha(env, e);
    env_destroy(env);

    // destroy 'e'
    bd_expr2_destroy(e);

    printf("--- α converted --- \n");
    bd_expr2_show(ret);
    printf("\n");

    return ret;
}
