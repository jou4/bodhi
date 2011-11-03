#include "compile.h"

char *add_prefix(char *prefix, char *name)
{
    StringBuffer *sb = sb_new();
    sb_append(sb, prefix);
    sb_append(sb, name);
    char *alt = sb_to_string(sb);
    sb_destroy(sb);
    return alt;
}

char *make_toplevel_name(char *name)
{
    return add_prefix("bohdi_", name);
}

char *make_cfunc_name(char *name)
{
    return add_prefix("_", name);
}

void convert_to_toplevel_name(BDNExprFundef *fundef)
{
    char *newname = make_toplevel_name(fundef->ident->name);
    free(fundef->ident->name);
    fundef->ident->name = newname;
}

char *find_alt_name(Env *env, char *name)
{
    char *alt = env_get(env, name);
    if(alt){
        return alt;
    }

    return make_toplevel_name(name);
}

void make_alt_fundef_formals(Env *env, Vector *idents, Vector *newidents)
{
    int i;
    BDExprIdent *ident;
    char *oldname, *newname;

    if(idents == NULL){
        return;
    }

    for(i = 0; i < idents->length; i++){
        ident = vector_get(idents, i);
        oldname = ident->name;
        newname = bd_generate_id(ident->type);

        vector_add(newidents, bd_expr_ident(newname, bd_type_clone(ident->type)));
        env_set(env, oldname, newname);
    }
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
                char *newname = bd_generate_id(ident->type);

                BDExprIdent *newident = bd_expr_ident(newname, bd_type_clone(ident->type));

                Env *local = env_local_new(env);
                env_set(local, oldname, newname);

                BDNExpr *newexpr = bd_nexpr_let(newident
                        , bd_alpha(local, e->u.u_let.val)
                        , bd_alpha(local, e->u.u_let.body));

                env_local_destroy(local);

                return newexpr;
            }
        case E_VAR:
            return bd_nexpr_var(find_alt_name(env, e->u.u_var.name));
        case E_LETREC:
            {
                BDNExprFundef *fundef = e->u.u_letrec.fundef;
                char *oldfunname = fundef->ident->name;
                char *newfunname = bd_generate_id(fundef->ident->type);

                // create local env of letrec-body
                Env *local = env_local_new(env);
                env_set(local, oldfunname, newfunname);

                // create local env of function-body
                Env *funlocal = env_local_new(local);
                Vector *newformals = vector_new();
                make_alt_fundef_formals(funlocal, fundef->formals, newformals);

                // convert function-body and create new-funciton
                BDNExprFundef *newfundef = bd_nexpr_fundef(
                        bd_expr_ident(newfunname, bd_type_clone(fundef->ident->type))
                        , newformals
                        , bd_alpha(funlocal, fundef->body));

                // destroy local env of function-body
                env_local_destroy(funlocal);

                // convert letrec-body and create new-letrec
                BDNExpr *newexpr = bd_nexpr_letrec(
                        newfundef
                        , bd_alpha(local, e->u.u_letrec.body));

                // destroy local env of letrec-body
                env_local_destroy(local);

                return newexpr;
            }
        case E_FUN:
            {
                BDNExprFundef *fundef = e->u.u_fun.fundef;

                // create local env of function-body
                Env *funlocal = env_local_new(env);
                Vector *newformals = vector_new();
                make_alt_fundef_formals(funlocal, fundef->formals, newformals);

                // convert function-body and create new-funciton
                BDNExprFundef *newfundef = bd_nexpr_fundef(
                        bd_expr_ident_clone(fundef->ident)
                        , newformals
                        , bd_alpha(funlocal, fundef->body));

                // destroy local env of function-body
                env_local_destroy(funlocal);

                return bd_nexpr_fun(newfundef);
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

                return bd_nexpr_ccall(make_cfunc_name(e->u.u_app.fun), newactuals);
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
    }
    return NULL;
}

BDNProgram *bd_alpha_convert(BDNProgram *prog)
{
    BDNProgram *nprog = malloc(sizeof(BDNProgram));
    bd_nprogram_init(nprog);

    Vector *vec, formals;
    BDNExpr *tmpfun, *fun;
    int i;
    char *newname;
    Env *env = env_new();

    // convert datadefs
    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        tmpfun = bd_nexpr_fun(vector_get(vec, i));
        fun = bd_alpha(env, tmpfun);

        // add
        vector_add(nprog->datadefs, fun->u.u_fun.fundef);
        // destory
        free(tmpfun);
        free(fun);
    }

    // convert fundefs
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        tmpfun = bd_nexpr_fun(vector_get(vec, i));
        fun = bd_alpha(env, tmpfun);

        // rename toplevels funciton
        convert_to_toplevel_name(fun->u.u_fun.fundef);
        // add
        vector_add(nprog->fundefs, fun->u.u_fun.fundef);
        // destroy
        free(tmpfun);
        free(fun);
    }

    // convert maindef
    tmpfun = bd_nexpr_fun(prog->maindef);
    fun = bd_alpha(env, tmpfun);
    // rename toplevels funciton
    convert_to_toplevel_name(fun->u.u_fun.fundef);
    // add
    nprog->maindef = fun->u.u_fun.fundef;
    // destroy
    free(tmpfun);
    free(fun);

    env_destroy(env);
    bd_nprogram_destroy(prog);

    printf("--- α converted --- \n");
    bd_nprogram_show(nprog);
    printf("\n");

    return prog;
}
