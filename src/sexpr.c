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
        case E_FUN:
            bd_sexpr_fundef_destroy(e->u.u_fun.fundef);
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

BDSExpr *bd_sexpr_fun(BDSExprFundef *fundef)
{
    BDSExpr *e = bd_sexpr(E_FUN);
    e->u.u_fun.fundef = fundef;
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
            PRINT1(col, "%d", e->u.u_int);
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
                    PRINT(col, "-");
                    _bd_sexpr_show(e->u.u_uniop.val, col, depth);
                    break;
            }
            break;
        case E_BINOP:
            _bd_sexpr_show(e->u.u_binop.l, col, depth);
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
                case OP_EQ:
                    PRINT(col, " == ");
                    break;
                case OP_LE:
                    PRINT(col, " <= ");
                    break;
                case OP_CONS:
                    PRINT(col, " : ");
                    break;
            }
            _bd_sexpr_show(e->u.u_binop.r, col, depth);
            break;
        case E_IF:
            PRINT(col ,"if ");
            _bd_sexpr_show(e->u.u_if.pred, col, depth);

            PRINT(col ,"\n");
            bd_show_indent(depth + 1);

            PRINT(col ,"then ");
            _bd_sexpr_show(e->u.u_if.t, col, depth);

            PRINT(col, "\n");
            bd_show_indent(depth + 1);

            PRINT(col, "else ");
            _bd_sexpr_show(e->u.u_if.f, col, depth);
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
                BDSExprFundef *fundef = e->u.u_letrec.fundef;
                int i;

                PRINT(col, "let rec ");
                PRINT1(col, "%s", bd_expr_ident_show(fundef->ident));
                for(i = 0; i < fundef->formals->length; i++){
                    PRINT(col, " ");
                    PRINT1(col, "%s", bd_expr_ident_show(vector_get(fundef->formals, i)));
                }
                PRINT(col, " = ");
                _bd_sexpr_show(fundef->body, col, depth + 1);

                PRINT(col, " in ");

                DOBREAKLINE_NOSHIFT(col, depth);
                _bd_sexpr_show(e->u.u_letrec.body, col, depth);
            }
            break;
        case E_FUN:
            {
                BDSExprFundef *fundef = e->u.u_fun.fundef;
                int i;

                PRINT1(col, "(%s)", bd_type_show(fundef->ident->type));

                PRINT(col, "\\");
                for(i = 0; i < fundef->formals->length; i++){
                    if(i > 0){
                        PRINT(col, " ");
                    }
                    PRINT1(col, "%s", bd_expr_ident_show(vector_get(fundef->formals, i)));
                }
                PRINT(col, " -> ");

                BREAKLINE(col, depth);
                _bd_sexpr_show(fundef->body, col, depth);
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
    }
}

void bd_sexpr_show(BDSExpr *e)
{
    _bd_sexpr_show(e, 0, 0);
    printf("\n");
}

void bd_sprogram_init(BDSProgram *prog)
{
    prog->fundefs = vector_new();
    prog->datadefs = vector_new();
    prog->maindef = NULL;
}

void bd_sprogram_toplevels(Env *env, BDSProgram *prog)
{
    Vector *vec;
    int i;
    BDSExprFundef *def;

    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        env_set(env, def->ident->name, def->ident->type);
    }

    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        env_set(env, def->ident->name, def->ident->type);
    }
}

void bd_sprogram_fundef_show(BDSExprFundef *fundef)
{
    Vector *vec;
    int i, col = 0, depth = 0;

    PRINT1(col, "%s", bd_expr_ident_show(fundef->ident));

    vec = fundef->formals;
    if(vec != NULL){
        for(i = 0; i < vec->length; i++){
            PRINT1(col, " (%s)", bd_expr_ident_show(vector_get(vec, i)));
        }
    }
    PRINT(col, " = ");

    DOBREAKLINE(col, depth);
    _bd_sexpr_show(fundef->body, col, depth);

    PRINT(col, "\n");
}

void bd_sprogram_show(BDSProgram *prog)
{
    Vector *vec;
    int i;

    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        bd_sprogram_fundef_show(vector_get(vec, i));
    }
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        bd_sprogram_fundef_show(vector_get(vec, i));
    }
    bd_sprogram_fundef_show(prog->maindef);

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
