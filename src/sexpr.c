#include <stdlib.h>
#include "util.h"
#include "sexpr.h"


BDSExprDef *bd_sexpr_def(BDExprIdent *ident, BDSExpr *body)
{
    BDSExprDef *def = malloc(sizeof(BDSExprDef));
    def->ident = ident;
    def->body = body;
    return def;
}

void bd_sexpr_def_destroy(BDSExprDef *def)
{
    bd_expr_ident_destroy(def->ident);
    bd_sexpr_destroy(def->body);
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
            bd_expr_ident_destroy(e->u.u_letrec.ident);
            bd_sexpr_destroy(e->u.u_letrec.fun);
            bd_sexpr_destroy(e->u.u_letrec.body);
            break;
        case E_FUN:
            vector_each(e->u.u_fun.formals, bd_expr_ident_destroy);
            bd_sexpr_destroy(e->u.u_fun.body);
            break;
        case E_APP:
            bd_sexpr_destroy(e->u.u_app.fun);
            vector_each(e->u.u_app.actuals, bd_sexpr_destroy);
            break;
        case E_CCALL:
            free(e->u.u_ccall.fun);
            vector_each(e->u.u_ccall.actuals, bd_sexpr_destroy);
            break;
        case E_TUPLE:
            vector_each(e->u.u_tuple.elems, bd_sexpr_destroy);
            break;
        case E_LETTUPLE:
            vector_each(e->u.u_lettuple.idents, bd_expr_ident_destroy);
            bd_sexpr_destroy(e->u.u_lettuple.val);
            bd_sexpr_destroy(e->u.u_lettuple.body);
            break;
        case E_MATCH:
            bd_sexpr_destroy(e->u.u_match.target);
            vector_each(e->u.u_match.branches, bd_sexpr_match_branch_destroy);
            break;
        case E_PATTERNVAR:
            bd_expr_ident_destroy(e->u.u_patvar.ident);
            break;
		default:
			break;
    }
    free(e);
}

BDSExpr *bd_sexpr_unit()
{
    return bd_sexpr(E_UNIT);
}

BDSExpr *bd_sexpr_bool(long val)
{
    BDSExpr *e = bd_sexpr(E_BOOL);
    e->u.u_int = val;
    return e;
}

BDSExpr *bd_sexpr_int(long val)
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

BDSExpr *bd_sexpr_fun(BDType *type, Vector *formals, BDSExpr *body)
{
    BDSExpr *e = bd_sexpr(E_FUN);
    e->u.u_fun.type = type;
    e->u.u_fun.formals = formals;
    e->u.u_fun.body = body;
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

BDSExpr *bd_sexpr_letrec(BDExprIdent *ident, BDSExpr *fun, BDSExpr *body)
{
    BDSExpr *e = bd_sexpr(E_LETREC);
    e->u.u_letrec.ident = ident;
    e->u.u_letrec.fun = fun;
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

BDSExpr *bd_sexpr_ccall(const char *fun, Vector *actuals)
{
    BDSExpr *e = bd_sexpr(E_CCALL);
    e->u.u_ccall.fun = mem_strdup(fun);
    e->u.u_ccall.actuals = actuals;
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

BDSExpr *bd_sexpr_match(BDSExpr *target, Vector *branches)
{
    BDSExpr *e = bd_sexpr(E_MATCH);
    e->u.u_match.target = target;
    e->u.u_match.branches = branches;
    return e;
}

BDSExpr *bd_sexpr_pattern_var(BDExprIdent *ident)
{
    BDSExpr *e = bd_sexpr(E_PATTERNVAR);
    e->u.u_patvar.ident = ident;
    return e;
}

BDSExprMatchBranch *bd_sexpr_match_branch(BDSExpr *pattern, BDSExpr *body)
{
    BDSExprMatchBranch *branch = malloc(sizeof(BDSExprMatchBranch));
    branch->pattern = pattern;
    branch->body = body;
    return branch;
}

void bd_sexpr_match_branch_destroy(BDSExprMatchBranch *branch)
{
    bd_sexpr_destroy(branch->pattern);
    bd_sexpr_destroy(branch->body);
}


void _bd_sexpr_show(BDSExpr *e, int col, int depth)
{
    if(e == NULL){ return; }

    BREAKLINE(col, depth);

    switch(e->kind){
        case E_UNIT:
            PRINT(col, "()");
            break;
        case E_BOOL:
            if(e->u.u_int == 0){
                PRINT(col, "False");
            }
            else{
                PRINT(col, "True");
            }
            break;
        case E_INT:
            PRINT1(col, "%ld", e->u.u_int);
            break;
        case E_FLOAT:
            PRINT1(col, "%.14g", e->u.u_double);
            break;
        case E_CHAR:
            switch(e->u.u_char){
                case '\n':
                    PRINT(col, "'\\n'");
                    break;
                default:
                    PRINT1(col, "'%c'", e->u.u_char);
            }
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
                    PRINT(col, "not ");
                    _bd_sexpr_show(e->u.u_uniop.val, col, depth);
                    break;
                case OP_NEG:
                case OP_FNEG:
                    PRINT(col, "-");
                    _bd_sexpr_show(e->u.u_uniop.val, col, depth);
                    break;
				default:
					break;
            }
            break;
        case E_BINOP:
            _bd_sexpr_show(e->u.u_binop.l, col, depth);
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
                case OP_EQ:
                    PRINT(col, " == ");
                    break;
                case OP_LE:
                    PRINT(col, " <= ");
                    break;
                case OP_CONS:
                    PRINT(col, " : ");
                    break;
				default:
					break;
            }
            _bd_sexpr_show(e->u.u_binop.r, col, depth);
            break;
        case E_IF:
            PRINT(col ,"if ");
            _bd_sexpr_show(e->u.u_if.pred, col, depth);

            DOBREAKLINE_NOSHIFT(col, depth);
            PRINT(col, " then ");
            _bd_sexpr_show(e->u.u_if.t, col, depth + 1);

            DOBREAKLINE_NOSHIFT(col, depth);
            PRINT(col, " else ");
            _bd_sexpr_show(e->u.u_if.f, col, depth + 1);
            break;
        case E_LET:
            PRINT(col, "let ");
            PRINT1(col, "%s", bd_expr_ident_show(e->u.u_let.ident));
            PRINT(col, " = ");
            _bd_sexpr_show(e->u.u_let.val, col, depth);

            PRINT(col, " in ");

            DOBREAKLINE_NOSHIFT(col, depth);
            _bd_sexpr_show(e->u.u_let.body, col, depth);
            break;
        case E_VAR:
            PRINT1(col, "%s", e->u.u_var.name);
            break;
        case E_LETREC:
            {
                BDExprIdent *ident = e->u.u_letrec.ident;
                BDSExpr *fun = e->u.u_letrec.fun;
                BDSExpr *body = e->u.u_letrec.body;

                PRINT1(col, "let rec %s = ", bd_expr_ident_show(ident));
                _bd_sexpr_show(fun, col, depth + 1);

                PRINT(col, " in ");

                DOBREAKLINE_NOSHIFT(col, depth);
                _bd_sexpr_show(body, col, depth);
            }
            break;
        case E_FUN:
            {
                BDType *type = e->u.u_fun.type;
                Vector *formals = e->u.u_fun.formals;
                BDSExpr *body = e->u.u_fun.body;
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
                _bd_sexpr_show(body, col, depth);
            }
            break;
        case E_APP:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;

                PRINT(col, "(");
                _bd_sexpr_show(e->u.u_app.fun, col, depth);
                for(i = 0; i < actuals->length; i++){
                    PRINT(col, " ");
                    _bd_sexpr_show(vector_get(actuals, i), col, depth);
                }
                PRINT(col, ")");
            }
            break;
        case E_CCALL:
            {
                Vector *actuals = e->u.u_app.actuals;
                int i;

                PRINT(col, "(");
                PRINT1(col, "CCALL %s", e->u.u_ccall.fun);
                for(i = 0; i < actuals->length; i++){
                    PRINT(col, " ");
                    _bd_sexpr_show(vector_get(actuals, i), col, depth);
                }
                PRINT(col, ")");
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;

                PRINT(col, "(");
                for(i = 0; i < elems->length; i++){
                    if(i > 0){
                        PRINT(col, ", ");
                    }
                    _bd_sexpr_show(vector_get(elems, i), col, depth);
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
                PRINT(col, ") = ");
                _bd_sexpr_show(e->u.u_lettuple.val, col, depth + 1);

                PRINT(col, " in ");

                DOBREAKLINE_NOSHIFT(col, depth);
                _bd_sexpr_show(e->u.u_lettuple.body, col, depth + 1);
            }
            break;
        case E_MATCH:
            {
                BDSExpr *target = e->u.u_match.target;
                Vector *branches = e->u.u_match.branches;
                int i;
                BDSExprMatchBranch *branch;
                BDSExpr *pattern, *body;

                PRINT(col, "match ");
                _bd_sexpr_show(target, col, depth + 1);
                PRINT(col, " with");
                DOBREAKLINE(col, depth);

                for(i = 0; i < branches->length; i++){
                    if(i > 0){
                        DOBREAKLINE_NOSHIFT(col, depth);
                    }
                    branch = vector_get(branches, i);
                    pattern = branch->pattern;
                    body = branch->body;

                    _bd_sexpr_show(pattern, col, depth);
                    PRINT(col, " -> ");
                    _bd_sexpr_show(body, col, depth + 1);
                }
            }
            break;
        case E_PATTERNVAR:
            PRINT1(col, "%s", bd_expr_ident_show(e->u.u_patvar.ident));
		default:
			break;
    }
}

void bd_sexpr_show(BDSExpr *e)
{
    _bd_sexpr_show(e, 0, 0);
    printf("\n");
}

void bd_sprogram_init(BDSProgram *prog)
{
    prog->defs = vector_new();
    prog->maindef = NULL;
}

void bd_sprogram_destroy(BDSProgram *prog)
{
#ifdef DEBUG
    printf("\n");
    printf("**************************\n");
    printf("TODO bd_sprogram_destroy\n");
    printf("**************************\n");
    printf("\n");
#endif
}

void bd_sprogram_def_show(BDSExprDef *def)
{
    int col = 0, depth = 0;

    PRINT1(col, "%s", bd_expr_ident_show(def->ident));
    PRINT(col, " = ");

    DOBREAKLINE(col, depth);
    _bd_sexpr_show(def->body, col, depth);

    PRINT(col, "\n");
}

void bd_sprogram_show(BDSProgram *prog)
{
    Vector *vec;
    int i;

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        bd_sprogram_def_show(vector_get(vec, i));
    }
    bd_sprogram_def_show(prog->maindef);

    printf("\n");
}
