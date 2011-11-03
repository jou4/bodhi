#include <stdlib.h>
#include "nexpr.h"


BDNExprFundef *bd_nexpr_fundef(BDExprIdent *ident, Vector *formals, BDNExpr *body)
{
    BDNExprFundef *fundef = malloc(sizeof(BDNExprFundef));
    fundef->ident = ident;
    fundef->formals = formals;
    fundef->body = body;
    return fundef;
}

void bd_nexpr_fundef_destroy(BDNExprFundef *fundef)
{
    bd_expr_ident_destroy(fundef->ident);
    vector_each(fundef->formals, bd_expr_ident_destroy);
    bd_nexpr_destroy(fundef->body);
}

BDNExpr *bd_nexpr(BDExprKind kind)
{
    BDNExpr *e = malloc(sizeof(BDNExpr));
    e->kind = kind;
    return e;
}

void bd_nexpr_destroy(BDNExpr *e)
{
    if(e == NULL){ return; }

    switch(e->kind){
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
            bd_nexpr_destroy(e->u.u_if.t);
            bd_nexpr_destroy(e->u.u_if.f);
            break;
        case E_LET:
            bd_expr_ident_destroy(e->u.u_let.ident);
            bd_nexpr_destroy(e->u.u_let.val);
            bd_nexpr_destroy(e->u.u_let.body);
            break;
        case E_VAR:
            free(e->u.u_var.name);
            break;
        case E_LETREC:
            bd_nexpr_fundef_destroy(e->u.u_letrec.fundef);
            bd_nexpr_destroy(e->u.u_letrec.body);
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
            bd_nexpr_destroy(e->u.u_lettuple.body);
            break;
    }
    free(e);
}

BDNExpr *bd_nexpr_unit()
{
    return bd_nexpr(E_UNIT);
}

BDNExpr *bd_nexpr_int(int val)
{
    BDNExpr *e = bd_nexpr(E_INT);
    e->u.u_int = val;
    return e;
}

BDNExpr *bd_nexpr_float(double val)
{
    BDNExpr *e = bd_nexpr(E_FLOAT);
    e->u.u_double = val;
    return e;
}

BDNExpr *bd_nexpr_char(char val)
{
    BDNExpr *e = bd_nexpr(E_CHAR);
    e->u.u_char = val;
    return e;
}

BDNExpr *bd_nexpr_str(char *val)
{
    BDNExpr *e = bd_nexpr(E_STR);
    e->u.u_str = val;
    return e;
}

BDNExpr *bd_nexpr_nil()
{
    BDNExpr *e = bd_nexpr(E_NIL);
    return e;
}

BDNExpr *bd_nexpr_uniop(BDOpKind kind, const char *val)
{
    BDNExpr *e = bd_nexpr(E_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = mem_strdup(val);
    return e;
}

BDNExpr *bd_nexpr_binop(BDOpKind kind, const char *l, const char *r)
{
    BDNExpr *e = bd_nexpr(E_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = mem_strdup(l);
    e->u.u_binop.r = mem_strdup(r);
    return e;
}

BDNExpr *bd_nexpr_if(BDOpKind kind, const char *l, const char *r, BDNExpr *t, BDNExpr *f)
{
    BDNExpr *e = bd_nexpr(E_IF);
    e->u.u_if.kind = kind;
    e->u.u_if.l = mem_strdup(l);
    e->u.u_if.r = mem_strdup(r);
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDNExpr *bd_nexpr_let(BDExprIdent *ident, BDNExpr *val, BDNExpr *body)
{
    BDNExpr *e = bd_nexpr(E_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDNExpr *bd_nexpr_var(const char *name)
{
    BDNExpr *e = bd_nexpr(E_VAR);
    e->u.u_var.name = mem_strdup(name);
    return e;
}

BDNExpr *bd_nexpr_letrec(BDNExprFundef *fundef, BDNExpr *body)
{
    BDNExpr *e = bd_nexpr(E_LETREC);
    e->u.u_letrec.fundef = fundef;
    e->u.u_letrec.body = body;
    return e;
}

BDNExpr *bd_nexpr_app(const char *fun, Vector *actuals)
{
    BDNExpr *e = bd_nexpr(E_APP);
    e->u.u_app.fun = mem_strdup(fun);
    e->u.u_app.actuals = actuals;
    return e;
}

BDNExpr *bd_nexpr_tuple(Vector *elems)
{
    BDNExpr *e = bd_nexpr(E_TUPLE);
    e->u.u_tuple.elems = elems;
    return e;
}

BDNExpr *bd_nexpr_lettuple(Vector *idents, const char *val, BDNExpr *body)
{
    BDNExpr *e = bd_nexpr(E_LETTUPLE);
    e->u.u_lettuple.idents = idents;
    e->u.u_lettuple.val = mem_strdup(val);
    e->u.u_lettuple.body = body;
    return e;
}

Set *bd_nexpr_freevars(BDNExpr *e)
{
    switch(e->kind){
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
                Set *s1 = bd_nexpr_freevars(e->u.u_if.t);
                Set *s2 = bd_nexpr_freevars(e->u.u_if.f);
                Set *set = set_union(s1, s2);
                set_add(set, e->u.u_if.l);
                set_add(set, e->u.u_if.r);

                set_destroy(s1);
                set_destroy(s2);

                return set;
            }
        case E_LET:
            {
                Set *s1 = bd_nexpr_freevars(e->u.u_let.val);
                Set *s2 = bd_nexpr_freevars(e->u.u_let.body);
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
                Set *s1 = bd_nexpr_freevars(e->u.u_letrec.fundef->body);
                Set *s2 = set_new();

                Vector *formals = e->u.u_letrec.fundef->formals;
                BDExprIdent *formal;
                int i;
                for(i = 0; i < formals->length; i++){
                    formal = vector_get(formals, i);
                    set_add(s2, formal->name);
                }

                Set *s3 = set_diff(s1, s2);
                Set *s4 = bd_nexpr_freevars(e->u.u_letrec.body);
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
                Set *s1 = bd_nexpr_freevars(e->u.u_lettuple.body);
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
        default:
            return set_new();
    }
}

void bd_nprogram_init(BDNProgram *prog)
{
    prog->fundefs = vector_new();
    prog->datadefs = vector_new();
    prog->maindef = NULL;
}

void bd_nprogram_fundef_show(BDNExprFundef *fundef)
{
    Vector *vec;
    int i;

    bd_expr_ident_show(fundef->ident);
    vec = fundef->formals;

    if(vec != NULL){
        for(i = 0; i < vec->length; i++){
            printf(" ");
            printf("(");
            bd_expr_ident_show(vector_get(vec, i));
            printf(")");
        }
    }
    printf(" = ");
    bd_nexpr_show(fundef->body);
}

void bd_nprogram_show(BDNProgram *prog)
{
    Vector *vec;
    int i;

    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        bd_nprogram_fundef_show(vector_get(vec, i));
    }
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        bd_nprogram_fundef_show(vector_get(vec, i));
    }
    bd_nprogram_fundef_show(prog->maindef);

    printf("\n");
}

void _bd_nexpr_show(BDNExpr *e, int depth)
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
        case E_CHAR:
            printf("'%c'", e->u.u_char);
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
                case OP_CONS:
                    printf(" : ");
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
            _bd_nexpr_show(e->u.u_if.t, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("else ");
            _bd_nexpr_show(e->u.u_if.f, 0);
            break;
        case E_LET:
            printf("let ");
            bd_expr_ident_show(e->u.u_let.ident);
            printf(" = ");
            _bd_nexpr_show(e->u.u_let.val, 0);

            printf(" in\n");
            _bd_nexpr_show(e->u.u_let.body, depth + 1);
            break;
        case E_VAR:
            printf("%s", e->u.u_var.name);
            break;
        case E_LETREC:
            {
                BDNExprFundef *fundef = e->u.u_letrec.fundef;
                int i;

                printf("let rec ");
                bd_expr_ident_show(fundef->ident);
                for(i = 0; i < fundef->formals->length; i++){
                    printf(" ");
                    bd_expr_ident_show(vector_get(fundef->formals, i));
                }
                printf(" = ");
                _bd_nexpr_show(fundef->body, 0);

                printf(" in\n");
                _bd_nexpr_show(e->u.u_letrec.body, depth + 1);
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
                _bd_nexpr_show(e->u.u_lettuple.body, depth + 1);
            }
            break;
    }
}

void bd_nexpr_show(BDNExpr *e)
{
    _bd_nexpr_show(e, 0);
    printf("\n");
}

