#include <stdlib.h>
#include "nexpr.h"


BDNExprDef *bd_nexpr_def(BDExprIdent *ident, BDNExpr *body)
{
    BDNExprDef *def = malloc(sizeof(BDNExprDef));
    def->ident = ident;
    def->body = body;
    return def;
}

void bd_nexpr_def_destroy(BDNExprDef *def)
{
    bd_expr_ident_destroy(def->ident);
    bd_nexpr_destroy(def->body);
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
            bd_expr_ident_destroy(e->u.u_letrec.ident);
            bd_nexpr_destroy(e->u.u_letrec.fun);
            bd_nexpr_destroy(e->u.u_letrec.body);
            break;
        case E_FUN:
            vector_each(e->u.u_fun.formals, bd_expr_ident_destroy);
            bd_nexpr_destroy(e->u.u_fun.body);
            break;
        case E_APP:
        case E_CCALL:
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
		default:
			break;
    }
    free(e);
}

BDNExpr *bd_nexpr_unit()
{
    return bd_nexpr(E_UNIT);
}

BDNExpr *bd_nexpr_int(long val)
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

BDNExpr *bd_nexpr_fun(BDType *type, Vector *formals, BDNExpr *body)
{
    BDNExpr *e = bd_nexpr(E_FUN);
    e->u.u_fun.type = type;
    e->u.u_fun.formals = formals;
    e->u.u_fun.body = body;
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

BDNExpr *bd_nexpr_letrec(BDExprIdent *ident, BDNExpr *fun, BDNExpr *body)
{
    BDNExpr *e = bd_nexpr(E_LETREC);
    e->u.u_letrec.ident = ident;
    e->u.u_letrec.fun = fun;
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

BDNExpr *bd_nexpr_ccall(const char *fun, Vector *actuals)
{
    BDNExpr *e = bd_nexpr(E_CCALL);
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
                BDExprIdent *ident = e->u.u_letrec.ident;
                BDNExpr *fun = e->u.u_letrec.fun;
                BDNExpr *body = e->u.u_letrec.body;

                Set *s1 = bd_nexpr_freevars(fun);
                Set *s2 = bd_nexpr_freevars(body);
                Set *set = set_union(s1, s2);
                set_remove(set, ident->name);

                set_destroy(s1);
                set_destroy(s2);

                return set;
            }
        case E_FUN:
            {
                BDExprIdent *formal;
                int i;

                Vector *formals = e->u.u_fun.formals;
                BDNExpr *body = e->u.u_fun.body;

                Set *s1 = bd_nexpr_freevars(body);
                Set *s2 = set_new();

                for(i = 0; i < formals->length; i++){
                    formal = vector_get(formals, i);
                    set_add(s2, formal->name);
                }

                Set *set = set_diff(s1, s2);
                set_destroy(s1);
                set_destroy(s2);

                return set;
            }
            break;
        case E_APP:
        case E_CCALL:
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
    prog->defs = vector_new();
    prog->maindef = NULL;
}

void bd_nprogram_destroy(BDNProgram *prog)
{
#ifdef DEBUG
    printf("\n");
    printf("**************************\n");
    printf("TODO bd_nprogram_destroy\n");
    printf("**************************\n");
    printf("\n");
#endif
}

void _bd_nexpr_show(BDNExpr *e, int col, int depth)
{
    if(e == NULL){ return; }

    BREAKLINE(col, depth);

    switch(e->kind){
        case E_UNIT:
            PRINT(col, "()");
            break;
        case E_INT:
            PRINT1(col, "%ld", e->u.u_int);
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
                case OP_FNEG:
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
                case OP_FADD:
                    PRINT(col, " + ");
                    break;
                case OP_SUB:
                case OP_FSUB:
                    PRINT(col, " - ");
                    break;
                case OP_MUL:
                case OP_FMUL:
                    PRINT(col, " * ");
                    break;
                case OP_DIV:
                case OP_FDIV:
                    PRINT(col, " / ");
                    break;
                case OP_MOD:
                    PRINT(col, " %% ");
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
            _bd_nexpr_show(e->u.u_if.t, col, depth + 1);

            DOBREAKLINE_NOSHIFT(col, depth);
            PRINT(col, " else ");
            _bd_nexpr_show(e->u.u_if.f, col, depth + 1);
            break;
        case E_LET:
            PRINT(col, "let ");
            PRINT1(col, "%s", bd_expr_ident_show(e->u.u_let.ident));
            PRINT(col, " = ");
            _bd_nexpr_show(e->u.u_let.val, col, depth + 1);

            PRINT(col, " in ");

            DOBREAKLINE_NOSHIFT(col, depth);
            _bd_nexpr_show(e->u.u_let.body, col, depth);
            break;
        case E_VAR:
            PRINT1(col, "%s", e->u.u_var.name);
            break;
        case E_LETREC:
            {
                BDExprIdent *ident = e->u.u_letrec.ident;
                BDNExpr *fun = e->u.u_letrec.fun;
                BDNExpr *body = e->u.u_letrec.body;

                PRINT1(col, "let rec %s = ", bd_expr_ident_show(ident));
                _bd_nexpr_show(fun, col, depth + 1);

                PRINT(col, " in ");

                DOBREAKLINE_NOSHIFT(col, depth);
                _bd_nexpr_show(body, col, depth);
            }
            break;
        case E_FUN:
            {
                BDType *type = e->u.u_fun.type;
                Vector *formals = e->u.u_fun.formals;
                BDNExpr *body = e->u.u_fun.body;
                int i;

                PRINT1(col, "(%s)", bd_type_show(type));
                DOBREAKLINE_NOSHIFT(col, depth);

                PRINT(col, "\\");
                for(i = 0; i < formals->length; i++){
                    if(i > 0){
                        PRINT(col, " ");
                    }
                    PRINT1(col, "%s", bd_expr_ident_show(vector_get(formals, i)));
                }
                PRINT(col, " -> ");

                BREAKLINE(col, depth);
                _bd_nexpr_show(body, col, depth);
            }
            break;
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;
                char *x;

                PRINT(col, "(");
                PRINT1(col, "%s", e->u.u_app.fun);
                for(i = 0; i < actuals->length; i++){
                    x = vector_get(actuals, i);
                    PRINT1(col, " %s", x);
                }
                PRINT(col, ")");
            }
            break;
        case E_CCALL:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;
                char *x;

                PRINT(col, "(");
                PRINT1(col, "CCALL %s", e->u.u_app.fun);
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
                _bd_nexpr_show(e->u.u_lettuple.body, col, depth);
            }
            break;
		default:
			break;
    }
}

void bd_nexpr_show(BDNExpr *e)
{
    _bd_nexpr_show(e, 0, 0);
    printf("\n");
}

void bd_nprogram_def_show(BDNExprDef *def)
{
    int col = 0, depth = 0;

    PRINT1(col, "%s", bd_expr_ident_show(def->ident));
    PRINT(col, " = ");

    DOBREAKLINE(col, depth);
    _bd_nexpr_show(def->body, col, depth);

    PRINT(col, "\n");
}

void bd_nprogram_show(BDNProgram *prog)
{
    Vector *vec;
    int i;

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        bd_nprogram_def_show(vector_get(vec, i));
    }
    bd_nprogram_def_show(prog->maindef);

    printf("\n");
}

