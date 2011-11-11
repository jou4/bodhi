#include <stdlib.h>
#include "util.h"
#include "cexpr.h"


BDCExprDef *bd_cexpr_def(BDExprIdent *ident, BDCExpr *body)
{
    BDCExprDef *def = malloc(sizeof(BDCExprDef));
    def->ident = ident;
    def->body = body;
    return def;
}

void bd_cexpr_def_destroy(BDCExprDef *def)
{
    bd_expr_ident_destroy(def->ident);
    bd_cexpr_destroy(def->body);
}

BDCExpr *bd_cexpr(BDExprKind kind)
{
    BDCExpr *e = malloc(sizeof(BDCExpr));
    e->kind = kind;
    return e;
}

void bd_cexpr_destroy(BDCExpr *e)
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
            bd_cexpr_destroy(e->u.u_if.t);
            bd_cexpr_destroy(e->u.u_if.f);
            break;
        case E_LET:
            bd_expr_ident_destroy(e->u.u_let.ident);
            bd_cexpr_destroy(e->u.u_let.val);
            bd_cexpr_destroy(e->u.u_let.body);
            break;
        case E_VAR:
            free(e->u.u_var.name);
            break;
        case E_FUN:
            vector_each(e->u.u_fun.formals, bd_expr_ident_destroy);
            vector_each(e->u.u_fun.freevars, bd_expr_ident_destroy);
            bd_cexpr_destroy(e->u.u_fun.body);
            break;
        case E_MAKECLS:
            bd_expr_ident_destroy(e->u.u_makecls.ident);
            bd_expr_closure_destroy(e->u.u_makecls.closure);
            bd_cexpr_destroy(e->u.u_makecls.body);
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
            bd_cexpr_destroy(e->u.u_lettuple.body);
            break;
		default:
			break;
    }
    free(e);
}

BDCExpr *bd_cexpr_unit()
{
    return bd_cexpr(E_UNIT);
}

BDCExpr *bd_cexpr_int(int val)
{
    BDCExpr *e = bd_cexpr(E_INT);
    e->u.u_int = val;
    return e;
}

BDCExpr *bd_cexpr_float(double val)
{
    BDCExpr *e = bd_cexpr(E_FLOAT);
    e->u.u_double = val;
    return e;
}

BDCExpr *bd_cexpr_char(char val)
{
    BDCExpr *e = bd_cexpr(E_CHAR);
    e->u.u_char = val;
    return e;
}

BDCExpr *bd_cexpr_str(char *val)
{
    BDCExpr *e = bd_cexpr(E_STR);
    e->u.u_str = val;
    return e;
}

BDCExpr *bd_cexpr_nil()
{
    BDCExpr *e = bd_cexpr(E_NIL);
    return e;
}

BDCExpr *bd_cexpr_uniop(BDOpKind kind, const char *val)
{
    BDCExpr *e = bd_cexpr(E_UNIOP);
    e->u.u_uniop.kind = kind;
    e->u.u_uniop.val = mem_strdup(val);
    return e;
}

BDCExpr *bd_cexpr_binop(BDOpKind kind, const char *l, const char *r)
{
    BDCExpr *e = bd_cexpr(E_BINOP);
    e->u.u_binop.kind = kind;
    e->u.u_binop.l = mem_strdup(l);
    e->u.u_binop.r = mem_strdup(r);
    return e;
}

BDCExpr *bd_cexpr_if(BDOpKind kind, const char *l, const char *r, BDCExpr *t, BDCExpr *f)
{
    BDCExpr *e = bd_cexpr(E_IF);
    e->u.u_if.kind = kind;
    e->u.u_if.l = mem_strdup(l);
    e->u.u_if.r = mem_strdup(r);
    e->u.u_if.t = t;
    e->u.u_if.f = f;
    return e;
}

BDCExpr *bd_cexpr_let(BDExprIdent *ident, BDCExpr *val, BDCExpr *body)
{
    BDCExpr *e = bd_cexpr(E_LET);
    e->u.u_let.ident = ident;
    e->u.u_let.val = val;
    e->u.u_let.body = body;
    return e;
}

BDCExpr *bd_cexpr_var(const char *name)
{
    BDCExpr *e = bd_cexpr(E_VAR);
    e->u.u_var.name = mem_strdup(name);
    return e;
}

BDCExpr *bd_cexpr_fun(BDType *type, Vector *formals, Vector *freevars, BDCExpr *body)
{
    BDCExpr *e = bd_cexpr(E_FUN);
    e->u.u_fun.type = type;
    e->u.u_fun.formals = formals;
    e->u.u_fun.freevars = freevars;
    e->u.u_fun.body = body;
    return e;
}

BDCExpr *bd_cexpr_makecls(BDExprIdent *ident, BDExprClosure *closure, BDCExpr *body)
{
    BDCExpr *e = bd_cexpr(E_MAKECLS);
    e->u.u_makecls.ident = ident;
    e->u.u_makecls.closure = closure;
    e->u.u_makecls.body = body;
    return e;
}

BDCExpr *bd_cexpr_appcls(const char *fun, Vector *actuals)
{
    BDCExpr *e = bd_cexpr(E_APPCLS);
    e->u.u_app.fun = mem_strdup(fun);
    e->u.u_app.actuals = actuals;
    return e;
}

BDCExpr *bd_cexpr_appdir(const char *fun, Vector *actuals)
{
    BDCExpr *e = bd_cexpr(E_APPDIR);
    e->u.u_app.fun = mem_strdup(fun);
    e->u.u_app.actuals = actuals;
    return e;
}

BDCExpr *bd_cexpr_tuple(Vector *elems)
{
    BDCExpr *e = bd_cexpr(E_TUPLE);
    e->u.u_tuple.elems = elems;
    return e;
}

BDCExpr *bd_cexpr_lettuple(Vector *idents, const char *val, BDCExpr *body)
{
    BDCExpr *e = bd_cexpr(E_LETTUPLE);
    e->u.u_lettuple.idents = idents;
    e->u.u_lettuple.val = mem_strdup(val);
    e->u.u_lettuple.body = body;
    return e;
}

Set *bd_cexpr_freevars(BDCExpr *e)
{
    Set *s;

    switch(e->kind){
        case E_UNIOP:
            s = set_new();
            set_add(s, e->u.u_uniop.val);
            return s;
        case E_BINOP:
            s = set_new();
            set_add(s, e->u.u_binop.l);
            set_add(s, e->u.u_binop.r);
            return s;
        case E_IF:
            {
                Set *s1 = bd_cexpr_freevars(e->u.u_if.t);
                Set *s2 = bd_cexpr_freevars(e->u.u_if.f);

                s = set_union(s1, s2);
                set_add(s, e->u.u_if.l);
                set_add(s, e->u.u_if.r);

                set_destroy(s1);
                set_destroy(s2);

                return s;
            }
        case E_LET:
            {
                Set *s1 = bd_cexpr_freevars(e->u.u_let.val);
                Set *s2 = bd_cexpr_freevars(e->u.u_let.body);

                set_remove(s2, e->u.u_let.ident->name);
                s = set_union(s1, s2);

                set_destroy(s1);
                set_destroy(s2);

                return s;
            }
        case E_VAR:
            s = set_new();
            set_add(s, e->u.u_var.name);
            return s;
        case E_MAKECLS:
            {
                BDExprClosure *cls = e->u.u_makecls.closure;
                Set *s1 = set_of_list(cls->freevars);
                Set *s2 = bd_cexpr_freevars(e->u.u_makecls.body);

                s = set_union(s1, s2);
                set_remove(s, e->u.u_makecls.ident->name);

                set_destroy(s1);
                set_destroy(s2);

                return s;
            }
        case E_APPCLS:
            s = set_of_list(e->u.u_app.actuals);
            set_add(s, e->u.u_app.fun);
            return s;
        case E_APPDIR:
            s = set_of_list(e->u.u_app.actuals);
            return s;
        case E_TUPLE:
            s = set_of_list(e->u.u_tuple.elems);
            return s;
        case E_LETTUPLE:
            {
                Set *s1 = bd_cexpr_freevars(e->u.u_lettuple.body);
                Set *s2 = set_new();
                Vector *idents = e->u.u_lettuple.idents;
                int i;
                BDExprIdent *ident;

                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);
                    set_add(s2, ident->name);
                }

                s = set_diff(s1, s2);
                set_add(s, e->u.u_lettuple.val);

                set_destroy(s1);
                set_destroy(s2);

                return s;
            }
		default:
			break;
    }
    return set_new();
}

void bd_cprogram_init(BDCProgram *prog)
{
    prog->datadefs = vector_new();
    prog->fundefs = vector_new();
    prog->maindef = NULL;
}

void bd_cprogram_destroy(BDCProgram *prog)
{
    printf("\n");
    printf("**************************\n");
    printf("TODO bd_cprogram_destroy\n");
    printf("**************************\n");
    printf("\n");
}


void _bd_cexpr_show(BDCExpr *e, int col, int depth)
{
    if(e == NULL){ return; }

    BREAKLINE(col, depth);

    switch(e->kind){
        case E_UNIT:
            PRINT(col, "()");
            break;
        case E_INT:
            PRINT1(col, "%d", e->u.u_int);
            break;
        case E_FLOAT:
            PRINT1(col, "%.14g", e->u.u_double);
            break;
        case E_CHAR:
            PRINT1(col, "'%c'", e->u.u_char);
            break;
        case E_STR:
            PRINT1(col, "\"%s\"", e->u.u_str);
            break;
        case E_NIL:
            PRINT(col, "[]");
            break;
        case E_UNIOP:
            switch(e->u.u_uniop.kind){
                case OP_NOT:
                    PRINT1(col, "not %s", e->u.u_uniop.val);
                    break;
                case OP_NEG:
                    PRINT1(col, "-%s", e->u.u_uniop.val);
                    break;
				default:
					break;
            }
            break;
        case E_BINOP:
            PRINT1(col, "%s", e->u.u_binop.l);
            switch(e->u.u_binop.kind){
                case OP_ADD:
                    PRINT(col, " + ");
                    break;
                case OP_SUB:
                    PRINT(col, " - ");
                    break;
                case OP_MUL:
                    PRINT(col, " * ");
                    break;
                case OP_DIV:
                    PRINT(col, " / ");
                    break;
                case OP_CONS:
                    PRINT(col, " : ");
                    break;
				default:
					break;
            }
            PRINT1(col, "%s", e->u.u_binop.r);
            break;
        case E_IF:
            PRINT(col, "if ");

            PRINT1(col, "%s", e->u.u_binop.l);
            switch(e->u.u_if.kind){
                case OP_EQ:
                    PRINT(col, " == ");
                    break;
                case OP_LE:
                    PRINT(col, " <= ");
                    break;
				default:
					break;
            }
            PRINT1(col, "%s", e->u.u_binop.r);

            DOBREAKLINE_NOSHIFT(col, depth);
            PRINT(col, " then ");
            _bd_cexpr_show(e->u.u_if.t, col, depth + 1);

            DOBREAKLINE_NOSHIFT(col, depth);
            PRINT(col, " else ");
            _bd_cexpr_show(e->u.u_if.f, col, depth + 1);
            break;
        case E_LET:
            PRINT(col, "let ");
            PRINT1(col, "%s", bd_expr_ident_show(e->u.u_let.ident));
            PRINT(col, " = ");
            _bd_cexpr_show(e->u.u_let.val, col, depth + 1);

            PRINT(col, " in ");

            DOBREAKLINE_NOSHIFT(col, depth);
            _bd_cexpr_show(e->u.u_let.body, col, depth);
            break;
        case E_VAR:
            PRINT1(col, "%s", e->u.u_var.name);
            break;
        case E_MAKECLS:
            {
                BDExprIdent *ident = e->u.u_makecls.ident;
                BDExprClosure *cls = e->u.u_makecls.closure;

                char *entry = cls->entry;
                Vector *fvs = cls->freevars;
                int i;
                char *fv;

                PRINT1(col, "make closure *%s* = ", ident->name);
                PRINT(col, "(");

                PRINT1(col, "%s", entry);
                for(i = 0; i < fvs->length; i++){
                    PRINT(col, ", ");
                    fv = vector_get(fvs, i);
                    PRINT1(col, "%s", fv);
                }

                PRINT(col, ")");

                DOBREAKLINE_NOSHIFT(col, depth);
                _bd_cexpr_show(e->u.u_makecls.body, col, depth + 1);
            }
            break;
        case E_FUN:
            {
                BDType *type = e->u.u_fun.type;
                Vector *formals = e->u.u_fun.formals;
                Vector *freevars = e->u.u_fun.freevars;
                BDCExpr *body = e->u.u_fun.body;
                int i;

                PRINT1(col, "(%s)", bd_type_show(type));
                DOBREAKLINE_NOSHIFT(col, depth);

                PRINT(col, "\\");

                PRINT(col, "(* ");
                for(i = 0; i < freevars->length; i++){
                    if(i > 0){
                        PRINT(col, " ");
                    }
                    PRINT1(col, "%s", bd_expr_ident_show(vector_get(freevars, i)));
                }
                PRINT(col, " *)");

                PRINT(col, " ");

                for(i = 0; i < formals->length; i++){
                    if(i > 0){
                        PRINT(col, " ");
                    }
                    PRINT1(col, "%s", bd_expr_ident_show(vector_get(formals, i)));
                }
                PRINT(col, " -> ");

                BREAKLINE(col, depth);
                _bd_cexpr_show(body, col, depth);
            }
            break;
        case E_APPCLS:
        case E_APPDIR:
        case E_CCALL:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;
                char *x;

                PRINT(col, "(");
                if(e->kind == E_APPCLS){
                    PRINT1(col, "(closure)%s", e->u.u_app.fun);
                }
                else if(e->kind == E_APPDIR){
                    PRINT1(col, "(direct)%s", e->u.u_app.fun);
                }
                else if(e->kind == E_CCALL){
                    PRINT1(col, "(ccall)%s", e->u.u_app.fun);
                }

                for(i = 0; i < actuals->length; i++){
                    x = vector_get(actuals, i);
                    PRINT1(col, " %s", x);
                }
                PRINT(col, ")");
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;
                char *x;

                PRINT(col, "(");
                for(i = 0; i < elems->length; i++){
                    if(i > 0){
                        PRINT(col, ", ");
                    }
                    x = vector_get(elems, i);
                    PRINT1(col, "%s", x);
                }
                PRINT(col, ")");
            }
            break;
        case E_LETTUPLE:
            {
                Vector *idents = e->u.u_lettuple.idents;
                int i;

                PRINT(col, "let ");
                PRINT(col, "(");
                for(i = 0; i < idents->length; i++){
                    if(i > 0){
                        PRINT(col, ", ");
                    }
                    PRINT1(col, "%s", bd_expr_ident_show(vector_get(idents, i)));
                }
                PRINT1(col, ") = %s", e->u.u_lettuple.val);

                PRINT(col, " in ");

                DOBREAKLINE_NOSHIFT(col, depth);
                _bd_cexpr_show(e->u.u_lettuple.body, col, depth);
            }
            break;
		default:
			break;
    }
}

void bd_cexpr_show(BDCExpr *e)
{
    _bd_cexpr_show(e, 0, 0);
    printf("\n");
}

void bd_cprogram_def_show(BDCExprDef *def)
{
    int col = 0, depth = 0;

    PRINT1(col, "%s", bd_expr_ident_show(def->ident));
    PRINT(col, " = ");

    DOBREAKLINE(col, depth);
    _bd_cexpr_show(def->body, col, depth);

    PRINT(col, "\n");
}

void bd_cprogram_show(BDCProgram *prog)
{
    Vector *vec;
    int i;

    printf("*data*\n");
    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        bd_cprogram_def_show(vector_get(vec, i));
    }
    printf("\n");

    printf("*function*\n");
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        bd_cprogram_def_show(vector_get(vec, i));
    }

    bd_cprogram_def_show(prog->maindef);
    printf("\n");
}

