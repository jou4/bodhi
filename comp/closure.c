#include "util.h"
#include "compile.h"


static Vector *toplevels;


BDExpr3 *closure_transform(Env *env, Set *known, BDExpr2 *e)
{
    switch(e->kind){
        case E_UNIT:
            return bd_expr3_unit();
        case E_INT:
            return bd_expr3_int(e->u.u_int);
        case E_FLOAT:
            return bd_expr3_float(e->u.u_double);
        case E_UNIOP:
            return bd_expr3_uniop(e->u.u_uniop.kind, e->u.u_uniop.val);
        case E_BINOP:
            return bd_expr3_binop(e->u.u_binop.kind, e->u.u_binop.l, e->u.u_binop.r);
        case E_IF:
            return bd_expr3_if(e->u.u_if.kind, e->u.u_if.l, e->u.u_if.r,
                    closure_transform(env, known, e->u.u_if.t),
                    closure_transform(env, known, e->u.u_if.f));
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                BDExpr3 *newexpr = bd_expr3_let(
                        bd_expr_ident(ident->name, bd_type_clone(ident->type)),
                        closure_transform(env, known, e->u.u_let.val),
                        closure_transform(local, known, e->u.u_let.body)
                        );

                env_local_destroy(local);

                return newexpr;
            }
        case E_VAR:
            return bd_expr3_var(e->u.u_var.name);
        case E_LETREC:
            {
                BDExpr2Fundef *fundef = e->u.u_letrec.fundef;
                Vector *formals = fundef->formals;
                BDExprIdent *ident;
                int i;
                Env *local, *funlocal;
                Set *known1;
                BDExpr3 *e1, *e2, *newexpr;
                Set *lvset, *fvset, *s1, *s2;

                Vector *toplevels_backup = vector_clone(toplevels);

                // Try closure-transform assuming that the function has no free variables.
                // Set local variables to env and collect local vars in function.
                local = env_local_new(env);
                env_set(local, fundef->ident->name, fundef->ident->type);
                funlocal = env_local_new(local);
                lvset = set_new();
                set_add(lvset, fundef->ident->name);
                for(i = 0; i < formals->length; i++){
                    ident = vector_get(formals, i);
                    env_set(funlocal, ident->name, ident->type);
                    set_add(lvset, ident->name);
                }

                // Add the function to functions that can be called direct.
                known1 = set_clone(known);
                set_add(known1, fundef->ident->name);

                // Try.
                e1 = closure_transform(funlocal, known1, fundef->body);

                // Check that the function has free variables or not.
                s1 = bd_expr3_freevars(e1);
                fvset = set_diff(s1, lvset);

                if( ! set_is_empty(fvset)){
                    // The function has free variables.
                    // So revert state.
                    vector_destroy(toplevels);
                    toplevels = toplevels_backup;

                    set_destroy(known1);
                    known1 = known;

                    // Try again.
                    bd_expr3_destroy(e1);
                    e1 = closure_transform(funlocal, known1, fundef->body);

                    // Collect free variables.
                    set_destroy(s1);
                    set_destroy(fvset);
                    s1 = bd_expr3_freevars(e1);
                    fvset = set_diff(s1, lvset);
                }

                // Add function to toplevels.
                // Create free variables list.
                Vector *fvs = set_elements(fvset);
                Vector *newfreevars = vector_new();
                char *x;
                for(i = 0; i < fvs->length; i++){
                    x = vector_get(fvs, i);
                    vector_add(newfreevars, bd_expr_ident(x, bd_type_clone(env_get(local, x))));
                }

                // Create formal parameters list.
                Vector *newformals = vector_new();
                for(i = 0; i < formals->length; i++){
                    ident =  vector_get(formals, i);
                    vector_add(newformals, bd_expr_ident(ident->name, bd_type_clone(ident->type)));
                }

                vector_add(toplevels,
                        bd_expr3_fundef(
                            bd_expr_ident(fundef->ident->name, bd_type_clone(fundef->ident->type)),
                            newformals,
                            newfreevars,
                            e1));

                // Transform body.
                e2 = closure_transform(local, known1, e->u.u_letrec.body);
                s2 = bd_expr3_freevars(e2);

                if(set_has(s2, fundef->ident->name)){
                    // The function appear as variable in body.
                    // So create make-closure expression.
                    newexpr = bd_expr3_makecls(
                            bd_expr_ident(fundef->ident->name, bd_type_clone(fundef->ident->type)),
                            bd_expr_closure(fundef->ident->name, fvs),
                            e2);
                }
                else{
                    newexpr = e2;

                    vector_destroy(fvs);
                }

                // Clean.
                set_destroy(lvset);
                set_destroy(fvset);
                set_destroy(s1);
                set_destroy(s2);
                env_local_destroy(local);
                env_local_destroy(funlocal);

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
                    return bd_expr3_appdir(fun, newactuals);
                }
                else{
                    return bd_expr3_appcls(fun, newactuals);
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

                return bd_expr3_tuple(newelems);
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

                BDExpr3 *newexpr = bd_expr3_lettuple(newidents, e->u.u_lettuple.val,
                        closure_transform(local, known, e->u.u_lettuple.body));

                env_local_destroy(local);

                return newexpr;
            }
    }
}

BDProgram1 *bd_closure_transform(BDExpr2 *e)
{
    Env *env = env_new();
    Set *known = set_new();
    toplevels = vector_new();

    BDExpr3 *expr = closure_transform(env, known, e);
    bd_expr2_destroy(e);

    env_destroy(env);
    set_destroy(known);

    BDProgram1 *prog = bd_program1(toplevels, expr);

    printf("--- Closure transformed --- \n");
    bd_program1_show(prog);
    printf("\n");

    return prog;
}
