#include "compile.h"

char *find_alt_name(Env *env, char *name)
{
    char *alt = env_get(env, name);
    if(alt){
        return mem_strdup(alt);
    }

    return mem_strdup(name);
}

BDNExpr *bd_alpha(Env *env, BDNExpr *e)
{
    switch(e->kind){
        case E_UNIT:
            return bd_nexpr_unit();
        case E_INT:
            return bd_nexpr_int(e->u.u_int);
        case E_FLOAT:
            return bd_nexpr_float(e->u.u_double);
        case E_CHAR:
            return bd_nexpr_char(e->u.u_char);
        case E_STR:
            {
                char *str = e->u.u_str;
                e->u.u_str = NULL;
                return bd_nexpr_str(str);
            }
        case E_NIL:
            return bd_nexpr_nil();
        case E_UNIOP:
            return bd_nexpr_uniop(e->u.u_uniop.kind, find_alt_name(env, e->u.u_uniop.val));
        case E_BINOP:
            return bd_nexpr_binop(
                    e->u.u_binop.kind,
                    find_alt_name(env, e->u.u_binop.l),
                    find_alt_name(env, e->u.u_binop.r));
        case E_IF:
            return bd_nexpr_if(e->u.u_if.kind
                    , find_alt_name(env, e->u.u_if.l), find_alt_name(env, e->u.u_if.r)
                    , bd_alpha(env, e->u.u_if.t), bd_alpha(env, e->u.u_if.f));
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                char *oldname = ident->name;
                char *newname;
                Env *local = env_local_new(env);

                if(is_trash_name(oldname)){
                    newname = "";
                }
                else{
                    newname = bd_generate_id(ident->type);
                    env_set(local, oldname, newname);
                }

                BDExprIdent *newident = bd_expr_ident(newname, bd_type_clone(ident->type));

                BDNExpr *newexpr = bd_nexpr_let(newident
                        , bd_alpha(env, e->u.u_let.val)
                        , bd_alpha(local, e->u.u_let.body));

                env_local_destroy(local);

                return newexpr;
            }
        case E_VAR:
            return bd_nexpr_var(find_alt_name(env, e->u.u_var.name));
        case E_LETREC:
            {
                BDExprIdent *ident = e->u.u_letrec.ident;
                BDNExpr *fun = e->u.u_letrec.fun;
                BDNExpr *body = e->u.u_letrec.body;

                char *oldname = ident->name;
                char *newname = bd_generate_id(ident->type);

                // set to local env
                Env *local = env_local_new(env);
                env_set(local, oldname, newname);

                // convert function
                BDNExpr *new_fun = bd_alpha(local, fun);

                // create new-letrec
                BDNExpr *newexpr = bd_nexpr_letrec(
                        bd_expr_ident(newname, bd_type_clone(ident->type)),
                        new_fun,
                        bd_alpha(local, body));

                // destroy local env
                env_local_destroy(local);

                return newexpr;
            }
        case E_FUN:
            {
                BDType *type = e->u.u_fun.type;
                Vector *formals = e->u.u_fun.formals;
                BDNExpr *body = e->u.u_fun.body;
                BDExprIdent *ident;
                char *oldname, *newname;
                int i;

                // create local env of function-body
                Env *funlocal = env_local_new(env);
                Vector *new_formals = vector_new();
                for(i = 0; i < formals->length; i++){
                    ident = vector_get(formals, i);
                    oldname = ident->name;
                    newname = bd_generate_id(ident->type);
                    vector_add(new_formals, bd_expr_ident(newname, bd_type_clone(ident->type)));
                    env_set(funlocal, oldname, newname);
                }

                // convert function-body and create new-funciton
                BDNExpr *new_fun = bd_nexpr_fun(
                        bd_type_clone(type),
                        new_formals,
                        bd_alpha(funlocal, body));

                // destroy local env of function-body
                env_local_destroy(funlocal);

                return new_fun;
            }
            break;
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                Vector *newactuals = vector_new();
                int i;
                char *oldname;
                for(i = 0; i < actuals->length; i++){
                    oldname = vector_get(actuals, i);
                    vector_add(newactuals, find_alt_name(env, oldname));
                }

                return bd_nexpr_app(find_alt_name(env, e->u.u_app.fun), newactuals);
            }
        case E_CCALL:
            {
                Vector *actuals = e->u.u_app.actuals;
                Vector *newactuals = vector_new();
                int i;
                char *oldname;
                for(i = 0; i < actuals->length; i++){
                    oldname = vector_get(actuals, i);
                    vector_add(newactuals, find_alt_name(env, oldname));
                }

                return bd_nexpr_ccall(bd_generate_cfunc_lbl(e->u.u_app.fun), newactuals);
            }
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                Vector *newelems = vector_new();
                int i;
                char *oldname;
                for(i = 0; i < elems->length; i++){
                    oldname = vector_get(elems, i);
                    vector_add(newelems, find_alt_name(env, oldname));
                }

                return bd_nexpr_tuple(newelems);
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

                BDNExpr *newexpr = bd_nexpr_lettuple(
                        newidents
                        , find_alt_name(env, e->u.u_lettuple.val)
                        , bd_alpha(local, e->u.u_lettuple.body));

                env_local_destroy(local);

                return newexpr;
            }
            break;
        default:
            break;
    }
    return NULL;
}

extern Vector *primsigs;

BDNProgram *bd_alpha_convert(BDNProgram *prog)
{
    BDNProgram *nprog = malloc(sizeof(BDNProgram));
    bd_nprogram_init(nprog);

    Vector *vec;
    BDNExprDef *def;
    int i;
    Env *env = env_new();

    // set primitive labels to env
    PrimSig *sig;
    for(i = 0; i < primsigs->length; i++){
        sig = vector_get(primsigs, i);
        env_set(env, sig->name, sig->lbl);
    }

    // set toplevel names to env
    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        env_set(env, def->ident->name, bd_generate_toplevel_lbl(def->ident->name));
    }

    // convert defs
    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        vector_add(nprog->defs,
                bd_nexpr_def(
                    bd_expr_ident(find_alt_name(env, def->ident->name), bd_type_clone(def->ident->type)),
                    bd_alpha(env, def->body)
                    ));
    }

    // convert maindef
    def = prog->maindef;
    nprog->maindef = bd_nexpr_def(
            bd_expr_ident(bd_generate_toplevel_lbl(def->ident->name), bd_type_clone(def->ident->type)),
            bd_alpha(env, def->body)
            );

    // destroy
    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        free(env_get(env, def->ident->name));
    }
    env_destroy(env);
    bd_nprogram_destroy(prog);

    return nprog;
}
