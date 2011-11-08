#include "compile.h"

static Vector *toplevels;

typedef struct {
    char *newname;
    Vector *fvs;
} TransFunResult;

TransFunResult *closure_transform_fun(Env *env, Set *known, BDNExpr *e, BDExprIdent *ident);

BDCExpr *closure_transform(Env *env, Set *known, BDNExpr *e)
{
    switch(e->kind){
        case E_UNIT:
            return bd_cexpr_unit();
        case E_INT:
            return bd_cexpr_int(e->u.u_int);
        case E_FLOAT:
            return bd_cexpr_float(e->u.u_double);
        case E_CHAR:
            return bd_cexpr_char(e->u.u_char);
        case E_STR:
            return bd_cexpr_str(e->u.u_str);
        case E_NIL:
            return bd_cexpr_nil();
        case E_UNIOP:
            return bd_cexpr_uniop(e->u.u_uniop.kind, e->u.u_uniop.val);
        case E_BINOP:
            switch(e->u.u_binop.kind){
                case OP_CONS:
                    {
                        Vector *actuals = vector_new();
                        vector_add(actuals, mem_strdup(e->u.u_binop.l));
                        vector_add(actuals, mem_strdup(e->u.u_binop.r));
                        return bd_cexpr_appdir("_bodhi_cons", actuals);
                    }
                default:
                    return bd_cexpr_binop(e->u.u_binop.kind, e->u.u_binop.l, e->u.u_binop.r);
            }
        case E_IF:
            return bd_cexpr_if(e->u.u_if.kind, e->u.u_if.l, e->u.u_if.r,
                    closure_transform(env, known, e->u.u_if.t),
                    closure_transform(env, known, e->u.u_if.f));
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                BDCExpr *newexpr;

                if(e->u.u_let.val->kind == E_FUN){
                    // ex) let a = \x y -> x + y in a 3 7
                    TransFunResult *tr = closure_transform_fun(env, known, e->u.u_let.val, ident);
                    newexpr = closure_transform(local, known, e->u.u_let.body);
                    Set *s1 = bd_cexpr_freevars(newexpr);
                    if(set_has(s1, ident->name)){
                        // The function appear as variable in body.
                        // So create make-closure expression.
                        newexpr = bd_cexpr_makecls(
                                bd_expr_ident_clone(ident),
                                bd_expr_closure(tr->newname, tr->fvs),
                                newexpr);
                    }
                    else{
                        vector_destroy(tr->fvs);
                    }
                    set_destroy(s1);
                }
                else{
                    newexpr = bd_cexpr_let(
                            bd_expr_ident(ident->name, bd_type_clone(ident->type)),
                            closure_transform(env, known, e->u.u_let.val),
                            closure_transform(local, known, e->u.u_let.body)
                            );
                }

                env_local_destroy(local);

                return newexpr;
            }
        case E_VAR:
            return bd_cexpr_var(e->u.u_var.name);
        case E_LETREC:
            {
                BDExprIdent *ident = e->u.u_letrec.ident;
                BDNExpr *fun = e->u.u_letrec.fun;
                BDNExpr *body = e->u.u_letrec.body;

                BDCExpr *e1, *newexpr;
                Set *knownlocal, *s1;
                Vector *fvs;

                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                // Do closure-transform expression of the function.
                knownlocal = set_clone(known);
                TransFunResult *tr = closure_transform_fun(local, knownlocal, fun, ident);

                // Transform body.
                e1 = closure_transform(local, knownlocal, body);
                s1 = bd_cexpr_freevars(e1);

                if(set_has(s1, ident->name)){
                    // The function appear as variable in body.
                    // So create make-closure expression.
                    newexpr = bd_cexpr_makecls(
                            bd_expr_ident_clone(ident),
                            bd_expr_closure(tr->newname, tr->fvs),
                            e1);
                }
                else{
                    newexpr = e1;

                    vector_destroy(fvs);
                }

                // Clean.
                set_destroy(knownlocal);
                set_destroy(s1);
                env_local_destroy(local);

                return newexpr;
            }
        case E_APP:
            {
                char *fun = e->u.u_app.fun;
                Vector *actuals = e->u.u_app.actuals;
                Vector *newactuals = vector_new();
                int i;

                for(i = 0; i < actuals->length; i++){
                    vector_add(newactuals, mem_strdup(vector_get(actuals, i)));
                }

                if(set_has(known, fun)){
                    return bd_cexpr_appdir(fun, newactuals);
                }
                else{
                    return bd_cexpr_appcls(fun, newactuals);
                }
            }
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                Vector *newelems = vector_new();
                int i;

                for(i = 0; i < elems->length; i++){
                    vector_add(newelems, mem_strdup(vector_get(elems, i)));
                }

                return bd_cexpr_tuple(newelems);
            }
        case E_LETTUPLE:
            {
                Vector *idents = e->u.u_lettuple.idents;
                Vector *newidents = vector_new();
                int i;
                BDExprIdent *ident;

                Env *local = env_local_new(env);
                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);

                    vector_add(newidents, bd_expr_ident(ident->name, bd_type_clone(ident->type)));
                    env_set(local, ident->name, ident->type);
                }

                BDCExpr *newexpr = bd_cexpr_lettuple(newidents, e->u.u_lettuple.val,
                        closure_transform(local, known, e->u.u_lettuple.body));

                env_local_destroy(local);

                return newexpr;
            }
    }
}

TransFunResult *closure_transform_fun(Env *env, Set *known, BDNExpr *e, BDExprIdent *ident)
{
    TransFunResult *result = malloc(sizeof(TransFunResult));

    BDType *type = e->u.u_fun.type;
    Vector *formals = e->u.u_fun.formals;
    BDNExpr *body = e->u.u_fun.body;

    Env *funlocal;
    Set *knownlocal, *lvset, *fvset, *s1;
    BDCExpr *e1;

    BDExprIdent *formal;
    int i;

    Vector *toplevels_backup = vector_clone(toplevels);

    // Try closure-transform assuming that the function has no free variables.
    // Set local variables to env and collect local vars in function.
    funlocal = env_local_new(env);
    lvset = set_new();
    set_add(lvset, ident->name);
    for(i = 0; i < formals->length; i++){
        formal = vector_get(formals, i);
        env_set(funlocal, formal->name, formal->type);
        set_add(lvset, formal->name);
    }

    // Add the function to functions that can be called direct.
    knownlocal = set_clone(known);
    set_add(knownlocal, ident->name);

    // Try.
    e1 = closure_transform(funlocal, knownlocal, body);

    // Check that the function has free variables or not.
    s1 = bd_cexpr_freevars(e1);
    fvset = set_diff(s1, lvset);

    if( ! set_is_empty(fvset)){
        // The function has free variables.
        // So revert state.
        vector_destroy(toplevels);
        toplevels = toplevels_backup;

        set_destroy(knownlocal);
        knownlocal = known;

        // Try again.
        bd_cexpr_destroy(e1);
        e1 = closure_transform(funlocal, knownlocal, body);

        // Collect free variables.
        set_destroy(s1);
        set_destroy(fvset);
        s1 = bd_cexpr_freevars(e1);
        fvset = set_diff(s1, lvset);
    }
    else{
        // Add function to original set of functions that can be called direct.
        set_add(known, ident->name);
    }

    // Add function to toplevels.
    // Create free variables list.
    Vector *fvs = set_elements(fvset);
    Vector *newfreevars = vector_new();
    char *x;
    for(i = 0; i < fvs->length; i++){
        x = vector_get(fvs, i);
        vector_add(newfreevars, bd_expr_ident(x, bd_type_clone(env_get(env, x))));
    }

    // Create formal parameters list.
    Vector *newformals = bd_expr_idents_clone(formals);

    // Add to toplevel functions.
    //char *newname = bd_generate_id(ident->type);
    char *newname = ident->name;
    vector_add(toplevels,
            bd_cexpr_def(
                bd_expr_ident(newname, bd_type_clone(ident->type)),
                bd_cexpr_fun(
                    bd_type_clone(ident->type),
                    newformals,
                    newfreevars,
                    e1)));

    // Clean.
    set_destroy(lvset);
    set_destroy(fvset);
    set_destroy(s1);
    env_local_destroy(funlocal);

    result->newname = newname;
    result->fvs = fvs;

    return result;
}

BDCExprDef *closure_transform_def(Env *env, Set *known, BDNExprDef *def)
{
    BDExprIdent *ident = def->ident;
    BDNExpr *body = def->body;

    BDCExpr *newexpr;

    if(body->kind == E_FUN){
        BDType *type = body->u.u_fun.type;
        Vector *formals = body->u.u_fun.formals;
        BDNExpr *funbody = body->u.u_fun.body;

        Env *local = env_local_new(env);

        BDExprIdent *formal;
        int i;

        for(i = 0; i < formals->length; i++){
            formal = vector_get(formals, i);
            env_set(local, formal->name, formal->type);
        }

        newexpr = bd_cexpr_fun(
                bd_type_clone(type),
                bd_expr_idents_clone(formals),
                vector_new(),
                closure_transform(local, known, funbody));

        env_local_destroy(local);
    }
    else{
        newexpr = closure_transform(env, known, body);
    }

    return bd_cexpr_def(
            bd_expr_ident_clone(def->ident),
            newexpr);
}


extern Vector *primsigs;

BDCProgram *bd_closure_transform(BDNProgram *prog)
{
    BDCProgram *cprog = malloc(sizeof(BDCProgram));
    bd_cprogram_init(cprog);

    PrimSig *sig;
    Vector *vec;
    BDNExprDef *def;
    BDCExprDef *newdef;
    int i;

    Env *env = env_new();
    Set *known = set_new();
    toplevels = vector_new();

    // Create set of functions that can be called direct.
    for(i = 0; i < primsigs->length; i++){
        sig = vector_get(primsigs, i);
        set_add(known, sig->lbl);
    }

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        set_add(known, def->ident->name);
    }

    // Do closure-transform each definition.
    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        if(def->body->kind == E_FUN){
            vector_add(cprog->fundefs, closure_transform_def(env, known, def));
        }
        else{
            vector_add(cprog->datadefs, closure_transform_def(env, known, def));
        }
    }

    // Do closure_transform main definition.
    def = prog->maindef;
    cprog->maindef = bd_cexpr_def(
            bd_expr_ident_clone(def->ident),
            closure_transform(env, known, def->body));

    // Append collected toplevel-functions to definitions.
    for(i = 0; i < toplevels->length; i++){
        vector_add(cprog->fundefs, vector_get(toplevels, i));
    }

    // Clean.
    env_destroy(env);
    set_destroy(known);
    vector_destroy(toplevels);

    return cprog;
}
