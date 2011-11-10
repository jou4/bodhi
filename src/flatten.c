#include "compile.h"

BDNExpr *flatten_nested_let(BDNExpr *e);

BDNExpr *flatten_insert(BDNExpr *origin, BDNExpr *val)
{
    switch(val->kind){
        case E_LET:
            val->u.u_let.body = flatten_insert(origin, val->u.u_let.body);
            return val;
        case E_LETREC:
            val->u.u_letrec.body = flatten_insert(origin, val->u.u_letrec.body);
            return val;
        case E_LETTUPLE:
            val->u.u_lettuple.body = flatten_insert(origin, val->u.u_lettuple.body);
            return val;
        default:
            origin->u.u_let.val = val;
            origin->u.u_let.body = flatten_nested_let(origin->u.u_let.body);
            return origin;
    }
}

BDNExpr *flatten_nested_let(BDNExpr *e)
{
    switch(e->kind){
        case E_IF:
            e->u.u_if.t = flatten_nested_let(e->u.u_if.t);
            e->u.u_if.f = flatten_nested_let(e->u.u_if.f);
            break;
        case E_LET:
            return flatten_insert(e, flatten_nested_let(e->u.u_let.val));
        case E_LETREC:
            e->u.u_letrec.fun = flatten_nested_let(e->u.u_letrec.fun);
            e->u.u_letrec.body = flatten_nested_let(e->u.u_letrec.body);
            break;
        case E_FUN:
            e->u.u_fun.body = flatten_nested_let(e->u.u_fun.body);
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.body = flatten_nested_let(e->u.u_lettuple.body);
            break;
		default:
			break;
    }
    return e;
}

BDNProgram *bd_flatten(BDNProgram *prog)
{
    Vector *vec;
    BDNExprDef *def;
    int i;

    // flatten defs
    vec = prog->defs;
    for(i = 0;i < vec->length; i++){
        def = vector_get(vec, i);
        def->body = flatten_nested_let(def->body);
    }

    // flatten maindef
    def = prog->maindef;
    def->body = flatten_nested_let(def->body);

    return prog;
}
