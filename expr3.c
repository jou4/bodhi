#include <stdlib.h>
#include "util/mem.h"
#include "expr3.h"


BDExpr3Fundef *bd_expr3_fundef(BDExprIdent *ident, Vector *formals, BDExpr3 *body)
{
    BDExpr3Fundef *fundef = malloc(sizeof(BDExpr3Fundef));
    fundef->ident = ident;
    fundef->formals = formals;
    fundef->body = body;
    return fundef;
}

void bd_expr3_fundef_destroy(BDExpr3Fundef *fundef)
{
    bd_expr_ident_destroy(fundef->ident);
    vector_each(fundef->formals, bd_expr_ident_destroy);
    bd_expr3_destroy(fundef->body);
}

BDExpr3 *bd_expr3(BDExprKind kind)
{
    BDExpr3 *e = malloc(sizeof(BDExpr3));
    e->kind = kind;
    return e;
}

void bd_expr3_destroy(BDExpr3 *e)
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
            bd_expr3_destroy(e->u.u_if.t);
            bd_expr3_destroy(e->u.u_if.f);
            break;
        case E_LET:
            bd_expr_ident_destroy(e->u.u_let.ident);
            bd_expr3_destroy(e->u.u_let.val);
            bd_expr3_destroy(e->u.u_let.body);
            break;
        case E_VAR:
            free(e->u.u_var.name);
            break;
        case E_MAKECLS:
            bd_expr_ident_destroy(e->u.u_makecls.ident);
            bd_expr_closure_destroy(e->u.u_makecls.closure);
            bd_expr3_destroy(e->u.u_makecls.body);
            break;
        case E_APPCLS:
        case E_APPDIR:
            free(e->u.u_app.fun);
            vector_each(e->u.u_app.actuals, free);
            break;
        case E_TUPLE:
            vector_each(e->u.u_tuple.elems, free);
            break;
        case E_LETTUPLE:
            vector_each(e->u.u_lettuple.idents, bd_expr_ident_destroy);
            free(e->u.u_lettuple.val);
            bd_expr3_destroy(e->u.u_lettuple.body);
            break;
    }
    free(e);
}

BDExpr3 *bd_expr3_unit()
{
    return bd_expr3(E_UNIT);
}

BDExpr3 *bd_expr3_int(int val)
{
    BDExpr3 *e = bd_expr3(E_INT);
    e->u.u_int = val;
    return e;
}

BDExpr3 *bd_expr3_float(double val)
{
    BDExpr3 *e = bd_expr3(E_FLOAT);
    e->u.u_double = val;
    return e;
}

BDExpr3 *bd_expr3_uniop(BDOpKind kind, const char *val)
{
    BDExpr3 *e = bd_expr3(E_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = mem_strdup(val);
    return e;
}

BDExpr3 *bd_expr3_binop(BDOpKind kind, const char *l, const char *r)
{
    BDExpr3 *e = bd_expr3(E_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = mem_strdup(l);
    e->u.u_binop.r = mem_strdup(r);
    return e;
}

BDExpr3 *bd_expr3_if(BDOpKind kind, const char *l, const char *r, BDExpr3 *t, BDExpr3 *f)
{
    BDExpr3 *e = bd_expr3(E_IF);
    e->u.u_if.kind = kind;
    e->u.u_if.l = mem_strdup(l);
    e->u.u_if.r = mem_strdup(r);
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDExpr3 *bd_expr3_let(BDExprIdent *ident, BDExpr3 *val, BDExpr3 *body)
{
    BDExpr3 *e = bd_expr3(E_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDExpr3 *bd_expr3_var(const char *name)
{
    BDExpr3 *e = bd_expr3(E_VAR);
    e->u.u_var.name = mem_strdup(name);
    return e;
}

BDExpr3 *bd_expr3_makecls(BDExprIdent *ident, BDExprClosure *closure, BDExpr3 *body)
{
    BDExpr3 *e = bd_expr3(E_MAKECLS);
    e->u.u_makecls.ident = ident;
    e->u.u_makecls.closure = closure;
    e->u.u_makecls.body = body;
    return e;
}

BDExpr3 *bd_expr3_appcls(const char *fun, Vector *actuals)
{
    BDExpr3 *e = bd_expr3(E_APPCLS);
    e->u.u_app.fun = mem_strdup(fun);
    e->u.u_app.actuals = actuals;
    return e;
}

BDExpr3 *bd_expr3_appdir(const char *fun, Vector *actuals)
{
    BDExpr3 *e = bd_expr3(E_APPDIR);
    e->u.u_app.fun = mem_strdup(fun);
    e->u.u_app.actuals = actuals;
    return e;
}

BDExpr3 *bd_expr3_tuple(Vector *elems)
{
    BDExpr3 *e = bd_expr3(E_TUPLE);
    e->u.u_tuple.elems = elems;
    return e;
}

BDExpr3 *bd_expr3_lettuple(Vector *idents, const char *val, BDExpr3 *body)
{
    BDExpr3 *e = bd_expr3(E_LETTUPLE);
    e->u.u_lettuple.idents = idents;
    e->u.u_lettuple.val = mem_strdup(val);
    e->u.u_lettuple.body = body;
    return e;
}


void _bd_expr3_show(BDExpr3 *e, int depth)
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
            _bd_expr3_show(e->u.u_if.t, 0);

            printf("\n");
            bd_show_indent(depth + 1);

            printf("else ");
            _bd_expr3_show(e->u.u_if.f, 0);
            break;
        case E_LET:
            printf("let ");
            bd_expr_ident_show(e->u.u_let.ident);
            printf(" = ");
            _bd_expr3_show(e->u.u_let.val, 0);

            printf(" in\n");
            _bd_expr3_show(e->u.u_let.body, depth + 1);
            break;
        case E_VAR:
            printf("%s", e->u.u_var.name);
            break;
        case E_MAKECLS:
            {
                BDExprIdent *ident = e->u.u_makecls.ident;
                BDExprClosure *cls = e->u.u_makecls.closure;

                char *entry = cls->entry;
                Vector *fvs = cls->freevars;
                int i;
                char *fv;

                printf("make closure %s", ident->name);
                printf("(");

                for(i = 0; i < fvs->length; i++){
                    if(i > 0){
                        printf(", ");
                    }
                    fv = vector_get(fvs, i);
                    printf("%s", fv);
                }

                printf(")\n");
                _bd_expr3_show(e->u.u_makecls.body, depth + 1);
            }
            break;
        case E_APPCLS:
        case E_APPDIR:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;
                char *x;

                if(e->kind == E_APPCLS){
                    printf("(closure)%s", e->u.u_app.fun);
                }
                else{
                    printf("(direct)%s", e->u.u_app.fun);
                }

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
                _bd_expr3_show(e->u.u_lettuple.body, depth + 1);
            }
            break;
    }
}

void bd_expr3_show(BDExpr3 *e)
{
    _bd_expr3_show(e, 0);
    printf("\n");
}

