#include "compile.h"

BDExpr2 *flatten_nested_let(BDExpr2 *e);

BDExpr2 *flatten_insert(BDExpr2 *origin, BDExpr2 *val)
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

BDExpr2 *flatten_nested_let(BDExpr2 *e)
{
    switch(e->kind){
        case E_IF:
            e->u.u_if.t = flatten_nested_let(e->u.u_if.t);
            e->u.u_if.f = flatten_nested_let(e->u.u_if.f);
            break;
        case E_LET:
            return flatten_insert(e, flatten_nested_let(e->u.u_let.val));
        case E_LETREC:
            e->u.u_letrec.fundef->body = flatten_nested_let(e->u.u_letrec.fundef->body);
            e->u.u_letrec.body = flatten_nested_let(e->u.u_letrec.body);
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.body = flatten_nested_let(e->u.u_lettuple.body);
            break;
    }
    return e;
}

BDExpr2 *bd_flatten(BDExpr2 *e)
{
    BDExpr2 *ret = flatten_nested_let(e);

    printf("--- Nested let flatten --- \n");
    bd_expr2_show(ret);
    printf("\n");

    return ret;
}
