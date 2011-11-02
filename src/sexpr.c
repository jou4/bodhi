#include <stdlib.h>
#include "util.h"
#include "sexpr.h"


BDSExprFundef *bd_sexpr_fundef(BDExprIdent *ident, Vector *formals, BDSExpr *body)
{
    BDSExprFundef *fundef = malloc(sizeof(BDSExprFundef));
    fundef->ident = ident;
    fundef->formals = formals;
    fundef->body = body;
    return fundef;
}

void bd_sexpr_fundef_destroy(BDSExprFundef *fundef)
{
    bd_expr_ident_destroy(fundef->ident);
    vector_each(fundef->formals, bd_expr_ident_destroy);
    bd_sexpr_destroy(fundef->body);
}

BDSExpr *bd_sexpr(BDExprKind kind)
{
    BDSExpr *e = malloc(sizeof(BDSExpr));
    e->kind = kind;
    return e;
}

void bd_sexpr_destroy(BDSExpr *e)
{
    if(e == NULL){ return; }

    switch(e->kind){
        case E_UNIT:
        case E_BOOL:
        case E_INT:
        case E_FLOAT:
        case E_CHAR:
        case E_NIL:
            break;
        case E_STR:
            free(e->u.u_str);
            break;
        case E_UNIOP:
            bd_sexpr_destroy(e->u.u_uniop.val);
            break;
        case E_BINOP:
            bd_sexpr_destroy(e->u.u_binop.l);
            bd_sexpr_destroy(e->u.u_binop.r);
            break;
        case E_IF:
            bd_sexpr_destroy(e->u.u_if.pred);
            bd_sexpr_destroy(e->u.u_if.t);
            bd_sexpr_destroy(e->u.u_if.f);
            break;
        case E_LET:
            bd_expr_ident_destroy(e->u.u_let.ident);
            bd_sexpr_destroy(e->u.u_let.val);
            bd_sexpr_destroy(e->u.u_let.body);
            break;
        case E_VAR:
            free(e->u.u_var.name);
            break;
        case E_LETREC:
            bd_sexpr_fundef_destroy(e->u.u_letrec.fundef);
            bd_sexpr_destroy(e->u.u_letrec.body);
            break;
        case E_APP:
            bd_sexpr_destroy(e->u.u_app.fun);
            vector_each(e->u.u_app.actuals, bd_sexpr_destroy);
            break;
        case E_TUPLE:
            vector_each(e->u.u_tuple.elems, bd_sexpr_destroy);
            break;
        case E_LETTUPLE:
            vector_each(e->u.u_lettuple.idents, bd_expr_ident_destroy);
            bd_sexpr_destroy(e->u.u_lettuple.val);
            bd_sexpr_destroy(e->u.u_lettuple.body);
            break;
    }
    free(e);
}

BDSExpr *bd_sexpr_unit()
{
    return bd_sexpr(E_UNIT);
}

BDSExpr *bd_sexpr_bool(int val)
{
    BDSExpr *e = bd_sexpr(E_BOOL);
    e->u.u_int = val;
    return e;
}

BDSExpr *bd_sexpr_int(int val)
{
    BDSExpr *e = bd_sexpr(E_INT);
    e->u.u_int = val;
    return e;
}

BDSExpr *bd_sexpr_float(double val)
{
    BDSExpr *e = bd_sexpr(E_FLOAT);
    e->u.u_double = val;
    return e;
}

BDSExpr *bd_sexpr_char(char val)
{
    BDSExpr *e = bd_sexpr(E_CHAR);
    e->u.u_char = val;
    return e;
}

BDSExpr *bd_sexpr_str(char *val)
{
    BDSExpr *e = bd_sexpr(E_STR);
    e->u.u_str = val;
    return e;
}

BDSExpr *bd_sexpr_nil()
{
    BDSExpr *e = bd_sexpr(E_NIL);
    return e;
}

BDSExpr *bd_sexpr_uniop(BDOpKind kind, BDSExpr *val)
{
    BDSExpr *e = bd_sexpr(E_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = val;
    return e;
}

BDSExpr *bd_sexpr_binop(BDOpKind kind, BDSExpr *l, BDSExpr *r)
{
    BDSExpr *e = bd_sexpr(E_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = l;
    e->u.u_binop.r = r;
    return e;
}

BDSExpr *bd_sexpr_if(BDSExpr *pred, BDSExpr *t, BDSExpr *f)
{
    BDSExpr *e = bd_sexpr(E_IF);
    e->u.u_if.pred = pred;
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDSExpr *bd_sexpr_let(BDExprIdent *ident, BDSExpr *val, BDSExpr *body)
{
    BDSExpr *e = bd_sexpr(E_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDSExpr *bd_sexpr_var(const char *name)
{
    BDSExpr *e = bd_sexpr(E_VAR);
    e->u.u_var.name = mem_strdup(name);
    return e;
}

BDSExpr *bd_sexpr_letrec(BDSExprFundef *fundef, BDSExpr *body)
{
    BDSExpr *e = bd_sexpr(E_LETREC);
    e->u.u_letrec.fundef = fundef;
    e->u.u_letrec.body = body;
    return e;
}

BDSExpr *bd_sexpr_app(BDSExpr *fun, Vector *actuals)
{
    BDSExpr *e = bd_sexpr(E_APP);
    e->u.u_app.fun = fun;
    e->u.u_app.actuals = actuals;
    return e;
}

BDSExpr *bd_sexpr_tuple(Vector *elems)
{
    BDSExpr *e = bd_sexpr(E_TUPLE);
    e->u.u_tuple.elems = elems;
    return e;
}

BDSExpr *bd_sexpr_lettuple(Vector *idents, BDSExpr *val, BDSExpr *body)
{
    BDSExpr *e = bd_sexpr(E_LETTUPLE);
    e->u.u_lettuple.idents = idents;
    e->u.u_lettuple.val = val;
    e->u.u_lettuple.body = body;
    return e;
}


void _bd_sexpr_show(BDSExpr *e, int depth)
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
        case E_CHAR:
            switch(e->u.u_char){
                case '\n':
                    printf("'\\n'");
                    break;
                default:
                    printf("'%c'", e->u.u_char);
            }
            break;
        case E_STR:
            printf("\"%s\"", e->u.u_str);
            break;
        case E_NIL:
            printf("[]");
            break;
        case E_UNIOP:
            switch(e->u.u_uniop.kind){
                case OP_NOT:
                    printf("not(");
                    _bd_sexpr_show(e->u.u_uniop.val, 0);
                    printf(")");
                    break;
                case OP_NEG:
                    printf("-(");
                    _bd_sexpr_show(e->u.u_uniop.val, 0);
                    printf(")");
                    break;
            }
            break;
        case E_BINOP:
            _bd_sexpr_show(e->u.u_binop.l, 0);
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
            _bd_sexpr_show(e->u.u_binop.r, 0);
            break;
        case E_IF:
            printf("if ");
            _bd_sexpr_show(e->u.u_if.pred, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("then ");
            _bd_sexpr_show(e->u.u_if.t, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("else ");
            _bd_sexpr_show(e->u.u_if.f, 0);
            break;
        case E_LET:
            printf("let ");
            bd_expr_ident_show(e->u.u_let.ident);
            printf(" = ");
            _bd_sexpr_show(e->u.u_let.val, 0);

            printf(" in\n");
            _bd_sexpr_show(e->u.u_let.body, depth + 1);
            break;
        case E_VAR:
            printf("%s", e->u.u_var.name);
            break;
        case E_LETREC:
            {
                BDSExprFundef *fundef = e->u.u_letrec.fundef;
                int i;

                printf("let rec ");
                bd_expr_ident_show(fundef->ident);
                for(i = 0; i < fundef->formals->length; i++){
                    printf(" ");
                    bd_expr_ident_show(vector_get(fundef->formals, i));
                }
                printf(" = ");
                _bd_sexpr_show(fundef->body, 0);

                printf(" in\n");
                _bd_sexpr_show(e->u.u_letrec.body, depth + 1);
            }
            break;
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;

                _bd_sexpr_show(e->u.u_app.fun, 0);
                for(i = 0; i < actuals->length; i++){
                    printf(" ");
                    _bd_sexpr_show(vector_get(actuals, i), 0);
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
                    _bd_sexpr_show(vector_get(elems, i), 0);
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
                _bd_sexpr_show(e->u.u_lettuple.val, 0);

                printf(" in\n");
                _bd_sexpr_show(e->u.u_lettuple.body, depth + 1);
            }
            break;
    }
}

void bd_sexpr_show(BDSExpr *e)
{
    _bd_sexpr_show(e, 0);
    printf("\n");
}


/*
int main()
{
    BDSExpr *s1, *s2, *s3;
    Vector *vec1, *vec2;

    s1 = bd_sexpr_unit();
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_bool(0);
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_int(100);
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_float(1.23);
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_uniop(OP_NOT, bd_sexpr_bool(1));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_uniop(OP_NEG, bd_sexpr_int(50));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_uniop(OP_NEG, bd_sexpr_float(3.14));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_binop(OP_SUB, bd_sexpr_int(100), bd_sexpr_binop(OP_ADD, bd_sexpr_int(10), bd_sexpr_int(20)));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_if(bd_sexpr_binop(OP_LE, bd_sexpr_int(0), bd_sexpr_int(1)), bd_sexpr_int(10), bd_sexpr_int(20));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    s1 = bd_sexpr_let(bd_expr_ident(mem_strdup("abc"), bd_type_int()), bd_sexpr_int(10),
            bd_sexpr_binop(OP_ADD, bd_sexpr_var(mem_strdup("abc")), bd_sexpr_int(20)));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    vec1 = vector_new();
    vector_add(vec1, bd_expr_ident(mem_strdup("n"), bd_type_int()));
    vec2 = vector_new();
    vector_add(vec2, bd_sexpr_int(100));
    s1 = bd_sexpr_letrec(
            bd_sexpr_fundef(bd_expr_ident(mem_strdup("f"), bd_type_int()), vec1,
                bd_sexpr_binop(OP_ADD, bd_sexpr_var(mem_strdup("n")), bd_sexpr_int(1))),
            bd_sexpr_app(bd_sexpr_var(mem_strdup("f")), vec2));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    vec1 = vector_new();
    vector_add(vec1, bd_sexpr_bool(1));
    vector_add(vec1, bd_sexpr_int(10));
    vector_add(vec1, bd_sexpr_float(1.23));
    s1 = bd_sexpr_tuple(vec1);
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    vec1 = vector_new();
    vector_add(vec1, bd_expr_ident(mem_strdup("a"), bd_type_bool()));
    vector_add(vec1, bd_expr_ident(mem_strdup("b"), bd_type_int()));
    vector_add(vec1, bd_expr_ident(mem_strdup("c"), bd_type_float()));
    vec2 = vector_new();
    vector_add(vec2, bd_sexpr_bool(1));
    vector_add(vec2, bd_sexpr_int(10));
    vector_add(vec2, bd_sexpr_float(1.23));
    s1 = bd_sexpr_lettuple(vec1, bd_sexpr_tuple(vec2), bd_sexpr_var(mem_strdup("b")));
    bd_sexpr_show(s1);
    bd_sexpr_destroy(s1);

    //s1 = bd_sexpr_array(bd_sexpr_int(100), bd_sexpr_bool(1));
    //bd_sexpr_show(s1);
    //bd_sexpr_destroy(s1);

    //s1 = bd_sexpr_array(bd_sexpr_int(100), bd_sexpr_bool(1));
    //s2 = bd_sexpr_get(s1, bd_sexpr_int(50));
    //bd_sexpr_show(s2, 0);
    //bd_sexpr_destroy(s2);

    //s1 = bd_sexpr_array(bd_sexpr_int(100), bd_sexpr_bool(1));
    //s2 = bd_sexpr_put(s1, bd_sexpr_int(50), bd_sexpr_bool(0));
    //bd_sexpr_show(s2, 0);
    //bd_sexpr_destroy(s2);

    return 0;
}
*/
