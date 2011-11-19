#include "compile.h"

BDSExpr *sanitize(BDSExpr *e)
{
    switch(e->kind){
        case E_UNIOP:
            e->u.u_uniop.val = sanitize(e->u.u_uniop.val);
            break;
        case E_BINOP:
            e->u.u_binop.l = sanitize(e->u.u_binop.l);
            e->u.u_binop.r = sanitize(e->u.u_binop.r);
            break;
        case E_IF:
            e->u.u_if.pred = sanitize(e->u.u_if.pred);
            e->u.u_if.t = sanitize(e->u.u_if.t);
            e->u.u_if.f = sanitize(e->u.u_if.f);
            break;
        case E_LET:
            e->u.u_let.val = sanitize(e->u.u_let.val);
            e->u.u_let.body = sanitize(e->u.u_let.body);
            break;
        case E_LETREC:
            e->u.u_letrec.fun = sanitize(e->u.u_letrec.fun);
            e->u.u_letrec.body = sanitize(e->u.u_letrec.body);
            break;
        case E_FUN:
            e->u.u_fun.body = sanitize(e->u.u_fun.body);
            break;
        case E_APP:
            {
                e->u.u_app.fun = sanitize(e->u.u_app.fun);

                Vector *actuals = e->u.u_app.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, vector_get(actuals, i));
                }
            }
            break;
        case E_CCALL:
            {
                Vector *actuals = e->u.u_ccall.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, vector_get(actuals, i));
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;

                for(i = 0; i < elems->length; i++){
                    vector_set(elems, i, sanitize(vector_get(elems, i)));
                }
            }
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.val = sanitize(e->u.u_lettuple.val);
            e->u.u_lettuple.body = sanitize(e->u.u_lettuple.body);
            break;
        case E_MATCH:
            {
                BDSExpr *target = e->u.u_match.target;
                Vector *branches = e->u.u_match.branches;
                int i;

                BDSExprMatchBranch *branch;
                for(i = 0; i < branches->length; i++){
                    branch = vector_get(branches, i);
                    branch->body = sanitize(branch->body);
                }

                if(target->kind != E_VAR){
                    char *newvar = bd_generate_id(NULL);
                    e->u.u_match.target = bd_sexpr_var(newvar);
                    return bd_sexpr_let(
                            bd_expr_ident_typevar(newvar),
                            target,
                            e);
                }
            }
            break;
        default:
            break;
    }
    return e;
}

BDSProgram *bd_sanitize(BDSProgram *prog)
{
    Vector *defs = prog->defs;

    int i;
    BDSExprDef *def;

    for(i = 0; i < defs->length; i++){
        def = vector_get(defs, i);
        def->body = sanitize(def->body);
    }

    def = prog->maindef;
    def->body = sanitize(def->body);

    return prog;
}
