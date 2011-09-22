#include "compile.h"


typedef struct {
    BDExpr2 *e;
    BDType *t;
} Pair;

Pair pair(BDExpr2 *e, BDType *t)
{
    Pair p;
    p.e = e;
    p.t = t;
    return p;
}


typedef struct {
    char *name;
    BDExpr2 *let;
} InsertLetResult;

InsertLetResult insert_let(Pair p)
{
    char *x;
    InsertLetResult ret;

    if(p.e->kind == E_VAR){
        ret.name = p.e->u.u_var.name;
        ret.let = NULL;
    }
    else{
        ret.name = bd_generate_id(p.t);
        ret.let = bd_expr2_let(bd_expr_ident(ret.name, bd_type_clone(p.t)), p.e, NULL);
    }

    return ret;
}

BDExpr2 *combine_let_body(BDExpr2 *e1, BDExpr2 *e2)
{
    if(e1 == NULL){
        return e2;
    }
    else{
        e1->u.u_let.body = e2;
    }
    return e1;
}


Pair normalize(Env *env, BDExpr1 *e);

Pair normalize_uniop(Env *env, BDExpr1 *src, BDExpr2 *dest, BDType *t)
{
    Pair p = normalize(env, src->u.u_uniop.val);
    InsertLetResult x = insert_let(p);

    dest->u.u_uniop.val = x.name;

    return pair(combine_let_body(x.let, dest), t);
}

Pair normalize_binop(Env *env, BDExpr1 *src, BDExpr2 *dest, BDType *t)
{
    Pair p1, p2;
    InsertLetResult x, y;

    p1 = normalize(env, src->u.u_binop.l);
    p2 = normalize(env, src->u.u_binop.r);

    x = insert_let(p1);
    y = insert_let(p2);

    dest->u.u_binop.l = x.name;
    dest->u.u_binop.r = y.name;

    return pair(combine_let_body(x.let, combine_let_body(y.let, dest)), t);
}

Pair normalize_if(Env *env, BDExpr1 *src, BDExpr2 *dest)
{
    Pair p1, p2, p3, p4;
    InsertLetResult x, y;

    p1 = normalize(env, src->u.u_if.pred->u.u_binop.l);
    p2 = normalize(env, src->u.u_if.pred->u.u_binop.r);
    p3 = normalize(env, src->u.u_if.t);
    p4 = normalize(env, src->u.u_if.f);

    x = insert_let(p1);
    y = insert_let(p2);

    dest->u.u_if.l = x.name;
    dest->u.u_if.r = y.name;
    dest->u.u_if.t = p3.e;
    dest->u.u_if.f = p4.e;

    return pair(combine_let_body(x.let, combine_let_body(y.let, dest)), p3.t);
}

Pair normalize(Env *env, BDExpr1 *e)
{
    switch(e->kind){
        case E_UNIT:
            return pair(bd_expr2_unit(), bd_type_unit());
        case E_BOOL:
        case E_INT:
            return pair(bd_expr2_int(e->u.u_int), bd_type_int());
        case E_FLOAT:
            return pair(bd_expr2_float(e->u.u_double), bd_type_float());
        case E_UNIOP:
            switch(e->u.u_uniop.kind){
                case OP_NOT:
                    return normalize(env, bd_expr1_if(e->u.u_uniop.val, bd_expr1_bool(0), bd_expr1_bool(1)));
                case OP_NEG:
                    return normalize_uniop(env, e, bd_expr2_uniop(e->u.u_uniop.kind, NULL), bd_type_int());
            }
        case E_BINOP:
            switch(e->u.u_binop.kind){
                case OP_ADD:
                case OP_SUB:
                case OP_MUL:
                case OP_DIV:
                    return normalize_binop(env, e, bd_expr2_binop(e->u.u_binop.kind, NULL, NULL), bd_type_int());
                case OP_EQ:
                case OP_LE:
                    return normalize(env, bd_expr1_if(e, bd_expr1_bool(1), bd_expr1_bool(0)));
            }
        case E_IF:
            {
                BDExpr1 *pred = e->u.u_if.pred;

                if(pred->kind == E_UNIOP && pred->u.u_uniop.kind == OP_NOT){
                    return normalize(env, bd_expr1_if(pred->u.u_uniop.val, e->u.u_if.t, e->u.u_if.f));
                }
                else if(pred->kind == E_BINOP){
                    return normalize_if(env, e, bd_expr2_if(pred->u.u_binop.kind, NULL, NULL, NULL, NULL));
                }

                BDExpr1 *alt_if = bd_expr1_if(
                        bd_expr1_binop(OP_EQ, pred, bd_expr1_bool(0)),
                        e->u.u_if.f,
                        e->u.u_if.t);
                return normalize(env, alt_if);
            }
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;

                Pair p1 = normalize(env, e->u.u_let.val);

                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                Pair p2 = normalize(local, e->u.u_let.body);

                return pair(bd_expr2_let(
                            bd_expr_ident(ident->name, bd_type_clone(ident->type)), p1.e, p2.e),
                        p2.t);
            }
        case E_VAR:
            {
                char *name = e->u.u_var.name;
                void *type;

                type = env_get(env, name);
                if(type == NULL){
                    // TODO External reference
                }

                return pair(bd_expr2_var(name), bd_type_clone(type));
            }
        case E_LETREC:
            {
                BDExpr1Fundef *fundef = e->u.u_letrec.fundef;
                BDExprIdent *ident = fundef->ident;
                Vector *formals = fundef->formals;
                Vector *new_formals = vector_new();

                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                Env *funlocal = env_local_new(local);
                int i;
                BDExprIdent *formal;
                for(i = 0; i < formals->length; i++){
                    formal = vector_get(formals, i);
                    env_set(funlocal, formal->name, formal->type);
                    vector_add(new_formals, bd_expr_ident(formal->name, bd_type_clone(formal->type)));
                }

                Pair p1 = normalize(funlocal, fundef->body);
                Pair p2 = normalize(local, e->u.u_letrec.body);

                env_local_destroy(funlocal);
                env_local_destroy(local);

                return pair(bd_expr2_letrec(
                            bd_expr2_fundef(
                                bd_expr_ident(ident->name, bd_type_clone(ident->type)),
                                new_formals,
                                p1.e),
                            p2.e),
                        p2.t);
            }
        case E_APP:
            {
                Pair pf, p;
                InsertLetResult f, x;
                Vector *lets = vector_new();

                pf = normalize(env, e->u.u_app.fun);
                f = insert_let(pf);

                Vector *actuals = e->u.u_app.actuals;
                Vector *new_actuals = vector_new();
                int i;
                BDExpr1 *tmp;
                for(i = 0; i < actuals->length; i++){
                    p = normalize(env, vector_get(actuals, i));
                    x = insert_let(p);

                    vector_add(lets, x.let);
                    vector_add(new_actuals, x.name);
                }

                BDExpr2 *curr = NULL;
                curr = bd_expr2_app(f.name, new_actuals);

                for(i = lets->length - 1; i >= 0; i--){
                    curr = combine_let_body(vector_get(lets, i), curr);
                }

                return pair(combine_let_body(f.let, curr), pf.t->u.u_fun.ret);
            }
        case E_TUPLE:
            {
                Pair p;
                InsertLetResult x;
                Vector *lets = vector_new();
                Vector *elems = e->u.u_tuple.elems;
                Vector *new_elems = vector_new();
                Vector *new_types = vector_new();
                int i;

                for(i = 0; i < elems->length; i++){
                    p = normalize(env, vector_get(elems, i));
                    x = insert_let(p);

                    vector_add(lets, x.let);
                    vector_add(new_elems, x.name);
                    vector_add(new_types, p.t);
                }

                BDExpr2 *curr = bd_expr2_tuple(new_elems);
                for(i = elems->length - 1; i >= 0; i--){
                    curr = combine_let_body(vector_get(lets, i), curr);
                }

                return pair(curr, bd_type_tuple(new_types));
            }
        case E_LETTUPLE:
            {
                Pair p;
                InsertLetResult x;

                p = normalize(env, e->u.u_lettuple.val);
                x = insert_let(p);

                Env *local = env_local_new(env);
                Vector *idents = e->u.u_lettuple.idents;
                Vector *new_idents = vector_new();
                int i;
                BDExprIdent *ident;
                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);
                    env_set(local, ident->name, ident->type);

                    vector_add(new_idents, bd_expr_ident(ident->name, bd_type_clone(ident->type)));
                }

                p = normalize(local, e->u.u_lettuple.body);
                env_local_destroy(local);

                return pair(combine_let_body(
                            x.let,
                            bd_expr2_lettuple(new_idents, x.name, p.e)),
                        p.t);
            }
    }
}

BDExpr2 *bd_knormalize(BDExpr1 *e)
{
    Env *env = env_new();
    Pair p = normalize(env, e);
    env_destroy(env);

    printf("--- K normalized --- \n");
    bd_expr2_show(p.e);
    printf("\n");

    return p.e;
}
