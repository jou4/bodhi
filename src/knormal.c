#include "compile.h"


typedef struct {
    BDNExpr *e;
    BDType *t;
} Pair;

Pair pair(BDNExpr *e, BDType *t)
{
    Pair p;
    p.e = e;
    p.t = t;
    return p;
}


typedef struct {
    char *name;
    BDNExpr *let;
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
        ret.let = bd_nexpr_let(bd_expr_ident(ret.name, bd_type_clone(p.t)), p.e, NULL);
    }

    return ret;
}

BDNExpr *combine_let_body(BDNExpr *e1, BDNExpr *e2)
{
    if(e1 == NULL){
        return e2;
    }
    else{
        e1->u.u_let.body = e2;
    }
    return e1;
}


Pair normalize(Env *env, BDSExpr *e);

Pair normalize_uniop(Env *env, BDSExpr *src, BDNExpr *dest, BDType *t)
{
    Pair p = normalize(env, src->u.u_uniop.val);
    InsertLetResult x = insert_let(p);

    dest->u.u_uniop.val = x.name;

    return pair(combine_let_body(x.let, dest), t);
}

Pair normalize_binop(Env *env, BDSExpr *src, BDNExpr *dest)
{
    Pair p1, p2;
    InsertLetResult x, y;
    BDType *t;

    p1 = normalize(env, src->u.u_binop.l);
    p2 = normalize(env, src->u.u_binop.r);

    x = insert_let(p1);
    y = insert_let(p2);

    dest->u.u_binop.l = x.name;
    dest->u.u_binop.r = y.name;

    switch(src->u.u_binop.kind){
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            t = p1.t;
            break;
        case OP_CONS:
            t = bd_type_list(p1.t);
            break;
    }

    return pair(combine_let_body(x.let, combine_let_body(y.let, dest)), t);
}

Pair normalize_if(Env *env, BDSExpr *src, BDNExpr *dest)
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

Pair normalize(Env *env, BDSExpr *e)
{
    switch(e->kind){
        case E_UNIT:
            return pair(bd_nexpr_unit(), bd_type_unit());
        case E_BOOL:
        case E_INT:
            return pair(bd_nexpr_int(e->u.u_int), bd_type_int());
        case E_FLOAT:
            return pair(bd_nexpr_float(e->u.u_double), bd_type_float());
        case E_CHAR:
            return pair(bd_nexpr_char(e->u.u_char), bd_type_char());
        case E_STR:
            {
                char *str = e->u.u_str;
                e->u.u_str = NULL;
                return pair(bd_nexpr_str(str), bd_type_string());
            }
        case E_NIL:
            return pair(bd_nexpr_nil(), bd_type_list(bd_type_var(NULL)));
        case E_UNIOP:
            switch(e->u.u_uniop.kind){
                case OP_NOT:
                    return normalize(env, bd_sexpr_if(e->u.u_uniop.val, bd_sexpr_bool(0), bd_sexpr_bool(1)));
                case OP_NEG:
                    return normalize_uniop(env, e, bd_nexpr_uniop(e->u.u_uniop.kind, NULL), bd_type_int());
            }
            break;
        case E_BINOP:
            switch(e->u.u_binop.kind){
                case OP_ADD:
                case OP_SUB:
                case OP_MUL:
                case OP_DIV:
                    return normalize_binop(env, e, bd_nexpr_binop(e->u.u_binop.kind, NULL, NULL));
                case OP_EQ:
                case OP_LE:
                    return normalize(env, bd_sexpr_if(e, bd_sexpr_bool(1), bd_sexpr_bool(0)));
                case OP_CONS:
                    return normalize_binop(env, e, bd_nexpr_binop(e->u.u_binop.kind, NULL, NULL));
            }
            break;
        case E_IF:
            {
                BDSExpr *pred = e->u.u_if.pred;

                if(pred->kind == E_UNIOP && pred->u.u_uniop.kind == OP_NOT){
                    return normalize(env, bd_sexpr_if(pred->u.u_uniop.val, e->u.u_if.f, e->u.u_if.t));
                }
                else if(pred->kind == E_BINOP){
                    return normalize_if(env, e, bd_nexpr_if(pred->u.u_binop.kind, NULL, NULL, NULL, NULL));
                }

                BDSExpr *alt_if = bd_sexpr_if(
                        bd_sexpr_binop(OP_EQ, pred, bd_sexpr_bool(0)),
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

                return pair(bd_nexpr_let(
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

                return pair(bd_nexpr_var(name), bd_type_clone(type));
            }
        case E_LETREC:
            {
                BDSExprFundef *fundef = e->u.u_letrec.fundef;
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

                return pair(bd_nexpr_letrec(
                            bd_nexpr_fundef(
                                bd_expr_ident(ident->name, bd_type_clone(ident->type)),
                                new_formals,
                                p1.e),
                            p2.e),
                        p2.t);
            }
        case E_FUN:
            {
                BDSExprFundef *fundef = e->u.u_fun.fundef;
                BDExprIdent *ident = fundef->ident;
                Vector *formals = fundef->formals;
                Vector *new_formals = vector_new();

                Env *funlocal = env_local_new(env);
                int i;
                BDExprIdent *formal;
                for(i = 0; i < formals->length; i++){
                    formal = vector_get(formals, i);
                    env_set(funlocal, formal->name, formal->type);
                    vector_add(new_formals, bd_expr_ident(formal->name, bd_type_clone(formal->type)));
                }

                Pair p1 = normalize(funlocal, fundef->body);
                env_local_destroy(funlocal);

                return pair(bd_nexpr_fun(
                            bd_nexpr_fundef(
                                bd_expr_ident_clone(ident),
                                new_formals,
                                p1.e)),
                        p1.t);
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
                BDSExpr *tmp;
                for(i = 0; i < actuals->length; i++){
                    p = normalize(env, vector_get(actuals, i));
                    x = insert_let(p);

                    vector_add(lets, x.let);
                    vector_add(new_actuals, x.name);
                }

                BDNExpr *curr = NULL;
                curr = bd_nexpr_app(f.name, new_actuals);

                for(i = lets->length - 1; i >= 0; i--){
                    curr = combine_let_body(vector_get(lets, i), curr);
                }

                return pair(combine_let_body(f.let, curr), pf.t->u.u_fun.ret);
            }
        case E_CCALL:
            {
                Pair p;
                InsertLetResult f, x;
                Vector *lets = vector_new();

                Vector *actuals = e->u.u_app.actuals;
                Vector *new_actuals = vector_new();
                int i;
                BDSExpr *tmp;
                for(i = 0; i < actuals->length; i++){
                    p = normalize(env, vector_get(actuals, i));
                    x = insert_let(p);

                    vector_add(lets, x.let);
                    vector_add(new_actuals, x.name);
                }

                BDNExpr *curr = NULL;
                curr = bd_nexpr_ccall(e->u.u_ccall.fun, new_actuals);

                for(i = lets->length - 1; i >= 0; i--){
                    curr = combine_let_body(vector_get(lets, i), curr);
                }

                return pair(curr, bd_type_var(NULL));
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

                BDNExpr *curr = bd_nexpr_tuple(new_elems);
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
                            bd_nexpr_lettuple(new_idents, x.name, p.e)),
                        p.t);
            }
    }
}

void bd_idents_env(Env *env, Vector *idents)
{
    int i;
    BDExprIdent *ident;

    for(i = 0; i < idents->length; i++){
        ident = vector_get(idents, i);
        env_set(env, ident->name, ident->type);
    }
}

extern Vector *primsigs;

BDNProgram *bd_knormalize(BDSProgram *prog)
{
    Vector *vec;
    BDSExprFundef *def;
    BDNExprFundef *ndef;
    int i;
    Pair p;
    Env *env, *funlocal;

    BDNProgram *nprog = malloc(sizeof(BDNProgram));
    bd_nprogram_init(nprog);

    env = env_new();

    // add primitives
    PrimSig *sig;
    for(i = 0; i < primsigs->length; i++){
        sig = vector_get(primsigs, i);
        env_set(env, sig->name, sig->type);
    }

    // add toplevels
    bd_sprogram_toplevels(env, prog);

    // normalize datadefs
    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        p = normalize(env, def->body);
        ndef = bd_nexpr_fundef(
                bd_expr_ident_clone(def->ident),
                NULL,
                p.e);
        vector_add(nprog->datadefs, ndef);
    }

    // normalize fundefs
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        funlocal = env_local_new(env);
        bd_idents_env(funlocal, def->formals);
        p = normalize(funlocal, def->body);
        ndef = bd_nexpr_fundef(
                bd_expr_ident_clone(def->ident),
                bd_expr_idents_clone(def->formals),
                p.e);
        vector_add(nprog->fundefs, ndef);
        env_local_destroy(funlocal);
    }

    // normalize maindef
    def = prog->maindef;
    p = normalize(env, def->body);
    ndef = bd_nexpr_fundef(
            bd_expr_ident_clone(def->ident),
            NULL,
            p.e);
    nprog->maindef = ndef;

    env_destroy(env);

    printf("--- K normalized --- \n");
    bd_nprogram_show(nprog);
    printf("\n");

    return nprog;
}
