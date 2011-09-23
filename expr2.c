#include <stdlib.h>
#include "util/mem.h"
#include "expr2.h"


BDExpr2Fundef *bd_expr2_fundef(BDExprIdent *ident, Vector *formals, BDExpr2 *body)
{
    BDExpr2Fundef *fundef = malloc(sizeof(BDExpr2Fundef));
    fundef->ident = ident;
    fundef->formals = formals;
    fundef->body = body;
    return fundef;
}

void bd_expr2_fundef_destroy(BDExpr2Fundef *fundef)
{
    bd_expr_ident_destroy(fundef->ident);
    vector_each(fundef->formals, bd_expr_ident_destroy);
    bd_expr2_destroy(fundef->body);
}

BDExpr2 *bd_expr2(BDExprKind kind)
{
    BDExpr2 *e = malloc(sizeof(BDExpr2));
    e->kind = kind;
    return e;
}

void bd_expr2_destroy(BDExpr2 *e)
{
    if(e == NULL){ return; }

    switch(e->kind){
        case E_UNIT:
        case E_BOOL:
        case E_INT:
        case E_FLOAT:
            break;
        case E_UNIOP:
            free(e->u.u_uniop.val);
            break;
        case E_BINOP:
            free(e->u.u_binop.l);
            free(e->u.u_binop.r);
            break;
        case E_IF:
            free(e->u.u_if.l);
            free(e->u.u_if.r);
            bd_expr2_destroy(e->u.u_if.t);
            bd_expr2_destroy(e->u.u_if.f);
            break;
        case E_LET:
            bd_expr_ident_destroy(e->u.u_let.ident);
            bd_expr2_destroy(e->u.u_let.val);
            bd_expr2_destroy(e->u.u_let.body);
            break;
        case E_VAR:
            free(e->u.u_var.name);
            break;
        case E_LETREC:
            bd_expr2_fundef_destroy(e->u.u_letrec.fundef);
            bd_expr2_destroy(e->u.u_letrec.body);
            break;
        case E_APP:
            free(e->u.u_app.fun);
            vector_each(e->u.u_app.actuals, free);
            break;
        case E_TUPLE:
            vector_each(e->u.u_tuple.elems, free);
            break;
        case E_LETTUPLE:
            vector_each(e->u.u_lettuple.idents, bd_expr_ident_destroy);
            free(e->u.u_lettuple.val);
            bd_expr2_destroy(e->u.u_lettuple.body);
            break;
    }
    free(e);
}

BDExpr2 *bd_expr2_unit()
{
    return bd_expr2(E_UNIT);
}

BDExpr2 *bd_expr2_int(int val)
{
    BDExpr2 *e = bd_expr2(E_INT);
    e->u.u_int = val;
    return e;
}

BDExpr2 *bd_expr2_float(double val)
{
    BDExpr2 *e = bd_expr2(E_FLOAT);
    e->u.u_double = val;
    return e;
}

BDExpr2 *bd_expr2_uniop(BDOpKind kind, const char *val)
{
    BDExpr2 *e = bd_expr2(E_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = mem_strdup(val);
    return e;
}

BDExpr2 *bd_expr2_binop(BDOpKind kind, const char *l, const char *r)
{
    BDExpr2 *e = bd_expr2(E_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = mem_strdup(l);
    e->u.u_binop.r = mem_strdup(r);
    return e;
}

BDExpr2 *bd_expr2_if(BDOpKind kind, const char *l, const char *r, BDExpr2 *t, BDExpr2 *f)
{
    BDExpr2 *e = bd_expr2(E_IF);
    e->u.u_if.kind = kind;
    e->u.u_if.l = mem_strdup(l);
    e->u.u_if.r = mem_strdup(r);
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDExpr2 *bd_expr2_let(BDExprIdent *ident, BDExpr2 *val, BDExpr2 *body)
{
    BDExpr2 *e = bd_expr2(E_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDExpr2 *bd_expr2_var(const char *name)
{
    BDExpr2 *e = bd_expr2(E_VAR);
    e->u.u_var.name = mem_strdup(name);
    return e;
}

BDExpr2 *bd_expr2_letrec(BDExpr2Fundef *fundef, BDExpr2 *body)
{
    BDExpr2 *e = bd_expr2(E_LETREC);
    e->u.u_letrec.fundef = fundef;
    e->u.u_letrec.body = body;
    return e;
}

BDExpr2 *bd_expr2_app(const char *fun, Vector *actuals)
{
    BDExpr2 *e = bd_expr2(E_APP);
    e->u.u_app.fun = mem_strdup(fun);
    e->u.u_app.actuals = actuals;
    return e;
}

BDExpr2 *bd_expr2_tuple(Vector *elems)
{
    BDExpr2 *e = bd_expr2(E_TUPLE);
    e->u.u_tuple.elems = elems;
    return e;
}

BDExpr2 *bd_expr2_lettuple(Vector *idents, const char *val, BDExpr2 *body)
{
    BDExpr2 *e = bd_expr2(E_LETTUPLE);
    e->u.u_lettuple.idents = idents;
    e->u.u_lettuple.val = mem_strdup(val);
    e->u.u_lettuple.body = body;
    return e;
}

Set *bd_expr2_freevars(BDExpr2 *e)
{
    switch(e->kind){
        case E_UNIT:
        case E_INT:
        case E_FLOAT:
            return set_new();
        case E_UNIOP:
            {
                Set *set = set_new();
                set_add(set, e->u.u_uniop.val);
                return set;
            }
        case E_BINOP:
            {
                Set *set = set_new();
                set_add(set, e->u.u_binop.l);
                set_add(set, e->u.u_binop.r);
                return set;
            }
        case E_IF:
            {
                Set *s1 = bd_expr2_freevars(e->u.u_if.t);
                Set *s2 = bd_expr2_freevars(e->u.u_if.f);
                Set *set = set_union(s1, s2);
                set_add(set, e->u.u_if.l);
                set_add(set, e->u.u_if.r);

                set_destroy(s1);
                set_destroy(s2);

                return set;
            }
        case E_LET:
            {
                Set *s1 = bd_expr2_freevars(e->u.u_let.val);
                Set *s2 = bd_expr2_freevars(e->u.u_let.body);
                set_remove(s2, e->u.u_let.ident->name);
                Set *set = set_union(s1, s2);

                set_destroy(s1);
                set_destroy(s2);

                return set;
            }
        case E_VAR:
            {
                Set *set = set_new();
                set_add(set, e->u.u_var.name);
                return set;
            }
        case E_LETREC:
            {
                Set *s1 = bd_expr2_freevars(e->u.u_letrec.fundef->body);
                Set *s2 = set_new();

                Vector *formals = e->u.u_letrec.fundef->formals;
                BDExprIdent *formal;
                int i;
                for(i = 0; i < formals->length; i++){
                    formal = vector_get(formals, i);
                    set_add(s2, formal->name);
                }

                Set *s3 = set_diff(s1, s2);
                Set *s4 = bd_expr2_freevars(e->u.u_letrec.body);
                Set *set = set_union(s3, s4);
                set_remove(set, e->u.u_letrec.fundef->ident->name);

                set_destroy(s1);
                set_destroy(s2);
                set_destroy(s3);
                set_destroy(s4);

                return set;
            }
        case E_APP:
            {
                Set *set = set_of_list(e->u.u_app.actuals);
                set_add(set, e->u.u_app.fun);
                return set;
            }
        case E_TUPLE:
            {
                Set *set = set_of_list(e->u.u_tuple.elems);
                return set;
            }
        case E_LETTUPLE:
            {
                Set *s1 = bd_expr2_freevars(e->u.u_lettuple.body);
                Set *s2 = set_new();

                Vector *idents = e->u.u_lettuple.idents;
                BDExprIdent *ident;
                int i;

                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);
                    set_add(s2, ident->name);
                }

                Set *set = set_diff(s1, s2);
                set_add(set, e->u.u_lettuple.val);

                set_destroy(s1);
                set_destroy(s2);

                return set;
            }
    }
}


void _bd_expr2_show(BDExpr2 *e, int depth)
{
    if(e == NULL){ return; }

    bd_show_indent(depth);

    switch(e->kind){
        case E_UNIT:
            printf("()");
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
                    printf("not %s", e->u.u_uniop.val);
                    break;
                case OP_NEG:
                    printf("-%s", e->u.u_uniop.val);
                    break;
            }
            break;
        case E_BINOP:
            printf("%s", e->u.u_binop.l);
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
            }
            printf("%s", e->u.u_binop.r);
            break;
        case E_IF:
            printf("if ");

            printf("%s", e->u.u_binop.l);
            switch(e->u.u_if.kind){
                case OP_EQ:
                    printf(" == ");
                    break;
                case OP_LE:
                    printf(" <= ");
                    break;
            }
            printf("%s", e->u.u_binop.r);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("then ");
            _bd_expr2_show(e->u.u_if.t, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("else ");
            _bd_expr2_show(e->u.u_if.f, 0);
            break;
        case E_LET:
            printf("let ");
            bd_expr_ident_show(e->u.u_let.ident);
            printf(" = ");
            _bd_expr2_show(e->u.u_let.val, 0);

            printf(" in\n");
            _bd_expr2_show(e->u.u_let.body, depth + 1);
            break;
        case E_VAR:
            printf("%s", e->u.u_var.name);
            break;
        case E_LETREC:
            {
                BDExpr2Fundef *fundef = e->u.u_letrec.fundef;
                int i;

                printf("let rec ");
                bd_expr_ident_show(fundef->ident);
                for(i = 0; i < fundef->formals->length; i++){
                    printf(" ");
                    bd_expr_ident_show(vector_get(fundef->formals, i));
                }
                printf(" = ");
                _bd_expr2_show(fundef->body, 0);

                printf(" in\n");
                _bd_expr2_show(e->u.u_letrec.body, depth + 1);
            }
            break;
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;
                char *x;

                printf("%s", e->u.u_app.fun);
                for(i = 0; i < actuals->length; i++){
                    x = vector_get(actuals, i);
                    printf(" %s", x);
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;
                char *x;

                printf("(");
                for(i = 0; i < elems->length; i++){
                    if(i > 0){
                        printf(", ");
                    }
                    x = vector_get(elems, i);
                    printf("%s", x);
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
                printf(") = %s in\n", e->u.u_lettuple.val);

                printf(" in\n");
                _bd_expr2_show(e->u.u_lettuple.body, depth + 1);
            }
            break;
    }
}

void bd_expr2_show(BDExpr2 *e)
{
    _bd_expr2_show(e, 0);
    printf("\n");
}

