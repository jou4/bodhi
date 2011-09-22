#include <stdlib.h>
#include "util/mem.h"
#include "expr1.h"


BDExpr1Fundef *bd_expr1_fundef(BDExprIdent *ident, Vector *formals, BDExpr1 *body)
{
    BDExpr1Fundef *fundef = malloc(sizeof(BDExpr1Fundef));
    fundef->ident = ident;
    fundef->formals = formals;
    fundef->body = body;
    return fundef;
}

void bd_expr1_fundef_destroy(BDExpr1Fundef *fundef)
{
    bd_expr_ident_destroy(fundef->ident);
    vector_each(fundef->formals, bd_expr_ident_destroy);
    bd_expr1_destroy(fundef->body);
}

BDExpr1 *bd_expr1(BDExprKind kind)
{
    BDExpr1 *e = malloc(sizeof(BDExpr1));
    e->kind = kind;
    return e;
}

void bd_expr1_destroy(BDExpr1 *e)
{
    if(e == NULL){ return; }

    switch(e->kind){
        case E_UNIT:
        case E_BOOL:
        case E_INT:
        case E_FLOAT:
            break;
        case E_UNIOP:
            bd_expr1_destroy(e->u.u_uniop.val);
            break;
        case E_BINOP:
            bd_expr1_destroy(e->u.u_binop.l);
            bd_expr1_destroy(e->u.u_binop.r);
            break;
        case E_IF:
            bd_expr1_destroy(e->u.u_if.pred);
            bd_expr1_destroy(e->u.u_if.t);
            bd_expr1_destroy(e->u.u_if.f);
            break;
        case E_LET:
            bd_expr_ident_destroy(e->u.u_let.ident);
            bd_expr1_destroy(e->u.u_let.val);
            bd_expr1_destroy(e->u.u_let.body);
            break;
        case E_VAR:
            free(e->u.u_var.name);
            break;
        case E_LETREC:
            bd_expr1_fundef_destroy(e->u.u_letrec.fundef);
            bd_expr1_destroy(e->u.u_letrec.body);
            break;
        case E_APP:
            bd_expr1_destroy(e->u.u_app.fun);
            vector_each(e->u.u_app.actuals, bd_expr1_destroy);
            break;
        case E_TUPLE:
            vector_each(e->u.u_tuple.elems, bd_expr1_destroy);
            break;
        case E_LETTUPLE:
            vector_each(e->u.u_lettuple.idents, bd_expr_ident_destroy);
            bd_expr1_destroy(e->u.u_lettuple.val);
            bd_expr1_destroy(e->u.u_lettuple.body);
            break;
    }
    free(e);
}

BDExpr1 *bd_expr1_unit()
{
    return bd_expr1(E_UNIT);
}

BDExpr1 *bd_expr1_bool(int val)
{
    BDExpr1 *e = bd_expr1(E_BOOL);
    e->u.u_int = val;
    return e;
}

BDExpr1 *bd_expr1_int(int val)
{
    BDExpr1 *e = bd_expr1(E_INT);
    e->u.u_int = val;
    return e;
}

BDExpr1 *bd_expr1_float(double val)
{
    BDExpr1 *e = bd_expr1(E_FLOAT);
    e->u.u_double = val;
    return e;
}

BDExpr1 *bd_expr1_uniop(BDOpKind kind, BDExpr1 *val)
{
    BDExpr1 *e = bd_expr1(E_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = val;
    return e;
}

BDExpr1 *bd_expr1_binop(BDOpKind kind, BDExpr1 *l, BDExpr1 *r)
{
    BDExpr1 *e = bd_expr1(E_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = l;
    e->u.u_binop.r = r;
    return e;
}

BDExpr1 *bd_expr1_if(BDExpr1 *pred, BDExpr1 *t, BDExpr1 *f)
{
    BDExpr1 *e = bd_expr1(E_IF);
    e->u.u_if.pred = pred;
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDExpr1 *bd_expr1_let(BDExprIdent *ident, BDExpr1 *val, BDExpr1 *body)
{
    BDExpr1 *e = bd_expr1(E_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDExpr1 *bd_expr1_var(const char *name)
{
    BDExpr1 *e = bd_expr1(E_VAR);
    e->u.u_var.name = mem_strdup(name);
    return e;
}

BDExpr1 *bd_expr1_letrec(BDExpr1Fundef *fundef, BDExpr1 *body)
{
    BDExpr1 *e = bd_expr1(E_LETREC);
    e->u.u_letrec.fundef = fundef;
    e->u.u_letrec.body = body;
    return e;
}

BDExpr1 *bd_expr1_app(BDExpr1 *fun, Vector *actuals)
{
    BDExpr1 *e = bd_expr1(E_APP);
    e->u.u_app.fun = fun;
    e->u.u_app.actuals = actuals;
    return e;
}

BDExpr1 *bd_expr1_tuple(Vector *elems)
{
    BDExpr1 *e = bd_expr1(E_TUPLE);
    e->u.u_tuple.elems = elems;
    return e;
}

BDExpr1 *bd_expr1_lettuple(Vector *idents, BDExpr1 *val, BDExpr1 *body)
{
    BDExpr1 *e = bd_expr1(E_LETTUPLE);
    e->u.u_lettuple.idents = idents;
    e->u.u_lettuple.val = val;
    e->u.u_lettuple.body = body;
    return e;
}


void _bd_expr1_show(BDExpr1 *e, int depth)
{
    if(e == NULL){ return; }

    bd_show_indent(depth);

    switch(e->kind){
        case E_UNIT:
            printf("()");
            break;
        case E_BOOL:
            if(e->u.u_int == 0){
                printf("False");
            }
            else{
                printf("True");
            }
            break;
        case E_INT:
            printf("%d", e->u.u_int);
            break;
        case E_FLOAT:
            printf("%.14g", e->u.u_double);
            break;
        case E_UNIOP:
            switch(e->u.u_uniop.kind){
                case OP_NOT:
                    printf("not(");
                    _bd_expr1_show(e->u.u_uniop.val, 0);
                    printf(")");
                    break;
                case OP_NEG:
                    printf("-(");
                    _bd_expr1_show(e->u.u_uniop.val, 0);
                    printf(")");
                    break;
            }
            break;
        case E_BINOP:
            _bd_expr1_show(e->u.u_binop.l, 0);
            switch(e->u.u_binop.kind){
                case OP_ADD:
                    printf(" + ");
                    break;
                case OP_SUB:
                    printf(" - ");
                    break;
                case OP_MUL:
                    printf(" * ");
                    break;
                case OP_DIV:
                    printf(" / ");
                    break;
                case OP_EQ:
                    printf(" == ");
                    break;
                case OP_LE:
                    printf(" <= ");
                    break;
            }
            _bd_expr1_show(e->u.u_binop.r, 0);
            break;
        case E_IF:
            printf("if ");
            _bd_expr1_show(e->u.u_if.pred, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("then ");
            _bd_expr1_show(e->u.u_if.t, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("else ");
            _bd_expr1_show(e->u.u_if.f, 0);
            break;
        case E_LET:
            printf("let ");
            bd_expr_ident_show(e->u.u_let.ident);
            printf(" = ");
            _bd_expr1_show(e->u.u_let.val, 0);

            printf(" in\n");
            _bd_expr1_show(e->u.u_let.body, depth + 1);
            break;
        case E_VAR:
            printf("%s", e->u.u_var.name);
            break;
        case E_LETREC:
            {
                BDExpr1Fundef *fundef = e->u.u_letrec.fundef;
                int i;

                printf("let rec ");
                bd_expr_ident_show(fundef->ident);
                for(i = 0; i < fundef->formals->length; i++){
                    printf(" ");
                    bd_expr_ident_show(vector_get(fundef->formals, i));
                }
                printf(" = ");
                _bd_expr1_show(fundef->body, 0);

                printf(" in\n");
                _bd_expr1_show(e->u.u_letrec.body, depth + 1);
            }
            break;
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;

                _bd_expr1_show(e->u.u_app.fun, 0);
                for(i = 0; i < actuals->length; i++){
                    printf(" ");
                    _bd_expr1_show(vector_get(actuals, i), 0);
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;

                printf("(");
                for(i = 0; i < elems->length; i++){
                    if(i > 0){
                        printf(", ");
                    }
                    _bd_expr1_show(vector_get(elems, i), 0);
                }
                printf(")");
            }
            break;
        case E_LETTUPLE:
            {
                Vector *idents = e->u.u_lettuple.idents;
                int i;

                printf("let ");
                printf("(");
                for(i = 0; i < idents->length; i++){
                    if(i > 0){
                        printf(", ");
                    }
                    bd_expr_ident_show(vector_get(idents, i));
                }
                printf(") = ");
                _bd_expr1_show(e->u.u_lettuple.val, 0);

                printf(" in\n");
                _bd_expr1_show(e->u.u_lettuple.body, depth + 1);
            }
            break;
    }
}

void bd_expr1_show(BDExpr1 *e)
{
    _bd_expr1_show(e, 0);
    printf("\n");
}


/*
int main()
{
    BDExpr1 *s1, *s2, *s3;
    Vector *vec1, *vec2;

    s1 = bd_expr1_unit();
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_bool(0);
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_int(100);
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_float(1.23);
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_uniop(OP_NOT, bd_expr1_bool(1));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_uniop(OP_NEG, bd_expr1_int(50));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_uniop(OP_NEG, bd_expr1_float(3.14));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_binop(OP_SUB, bd_expr1_int(100), bd_expr1_binop(OP_ADD, bd_expr1_int(10), bd_expr1_int(20)));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_if(bd_expr1_binop(OP_LE, bd_expr1_int(0), bd_expr1_int(1)), bd_expr1_int(10), bd_expr1_int(20));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    s1 = bd_expr1_let(bd_expr_ident(mem_strdup("abc"), bd_type_int()), bd_expr1_int(10),
            bd_expr1_binop(OP_ADD, bd_expr1_var(mem_strdup("abc")), bd_expr1_int(20)));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    vec1 = vector_new();
    vector_add(vec1, bd_expr_ident(mem_strdup("n"), bd_type_int()));
    vec2 = vector_new();
    vector_add(vec2, bd_expr1_int(100));
    s1 = bd_expr1_letrec(
            bd_expr1_fundef(bd_expr_ident(mem_strdup("f"), bd_type_int()), vec1,
                bd_expr1_binop(OP_ADD, bd_expr1_var(mem_strdup("n")), bd_expr1_int(1))),
            bd_expr1_app(bd_expr1_var(mem_strdup("f")), vec2));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    vec1 = vector_new();
    vector_add(vec1, bd_expr1_bool(1));
    vector_add(vec1, bd_expr1_int(10));
    vector_add(vec1, bd_expr1_float(1.23));
    s1 = bd_expr1_tuple(vec1);
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    vec1 = vector_new();
    vector_add(vec1, bd_expr_ident(mem_strdup("a"), bd_type_bool()));
    vector_add(vec1, bd_expr_ident(mem_strdup("b"), bd_type_int()));
    vector_add(vec1, bd_expr_ident(mem_strdup("c"), bd_type_float()));
    vec2 = vector_new();
    vector_add(vec2, bd_expr1_bool(1));
    vector_add(vec2, bd_expr1_int(10));
    vector_add(vec2, bd_expr1_float(1.23));
    s1 = bd_expr1_lettuple(vec1, bd_expr1_tuple(vec2), bd_expr1_var(mem_strdup("b")));
    bd_expr1_show(s1);
    bd_expr1_destroy(s1);

    //s1 = bd_expr1_array(bd_expr1_int(100), bd_expr1_bool(1));
    //bd_expr1_show(s1);
    //bd_expr1_destroy(s1);

    //s1 = bd_expr1_array(bd_expr1_int(100), bd_expr1_bool(1));
    //s2 = bd_expr1_get(s1, bd_expr1_int(50));
    //bd_expr1_show(s2, 0);
    //bd_expr1_destroy(s2);

    //s1 = bd_expr1_array(bd_expr1_int(100), bd_expr1_bool(1));
    //s2 = bd_expr1_put(s1, bd_expr1_int(50), bd_expr1_bool(0));
    //bd_expr1_show(s2, 0);
    //bd_expr1_destroy(s2);

    return 0;
}
*/
