#include "util.h"
#include "compile.h"

// Error

#define ERROR 101
#define ERROR_INFER 102
#define ERROR_UNIFY 103
#define ERROR_INVALID_ARGUMENT 104

typedef struct {
    BDSExpr *e;
    BDType *t1;
    BDType *t2;
} InferError;

InferError *infer_error(BDSExpr *e, BDType *t1, BDType *t2)
{
    InferError *err = (InferError *)malloc(sizeof(InferError));
    err->e = e;
    err->t1 = t1;
    err->t2 = t2;
    return err;
}

typedef struct {
    BDType *t1;
    BDType *t2;
} UnifyError;

UnifyError *unify_error(BDType *t1, BDType *t2)
{
    UnifyError *err = (UnifyError *)malloc(sizeof(UnifyError));
    err->t1 = t1;
    err->t2 = t2;
    return err;
}


// Polymorphic

BDType *deref_type(BDType *t, Vector *vars);
BDExprIdent *deref_id_type(BDExprIdent *ident, Vector *vars);
BDSExpr *deref_term(BDSExpr *e);
BDType *replace_freevars(BDType *t, Vector *freevars, Vector *freshvars);

Vector *unknown_typevars_in_env(Env *env)
{
    Vector *schemas = env_values(env);
    Vector *vars = vector_new();
    int i;
    BDType *s, *t;

    for(i = 0; i < schemas->length; i++){
        s = vector_get(schemas, i);
        t = s->u.u_schema.body;
        if(t->kind == T_VAR && t->u.u_var.content == NULL){
            vector_add(vars, t);
        }
    }

    vector_destroy(schemas);

    return vars;
}

BDType *create_type_schema(Env *env, BDType *t)
{
    Vector *unknownvars = vector_new();
    BDType *derefed_type = deref_type(t, unknownvars);
    Vector *envvars = unknown_typevars_in_env(env);
    Vector *freevars = vector_new();

    // TypeVar in env would not be Polymorphic-TypeVar.
    // Because it may be type-infered in other expression.
    int i;
    BDType *t2;
    for(i = 0; i < unknownvars->length; i++){
        t2 = vector_get(unknownvars, i);
        if(vector_index_of(envvars, t2) < 0){
            vector_add(freevars, t2);
        }
    }

    vector_destroy(unknownvars);
    vector_destroy(envvars);

    return bd_type_schema(freevars, derefed_type);
}

Vector *replace_freevars_in_vector(Vector *vec, Vector *freevars, Vector *freshvars)
{
    int i;
    Vector *newvec = vector_new();

    for(i = 0; i < vec->length; i++){
        vector_add(newvec, replace_freevars(vector_get(vec, i), freevars, freshvars));
    }

    return newvec;
}

BDType *replace_freevars(BDType *t, Vector *freevars, Vector *freshvars)
{
    switch(t->kind){
        case T_UNIT:
        case T_BOOL:
        case T_INT:
        case T_FLOAT:
        case T_CHAR:
        case T_STRING:
            return t;
        case T_FUN:
            {
                Vector *formals = t->u.u_fun.formals;
                BDType *ret = t->u.u_fun.ret;

                Vector *newformals = replace_freevars_in_vector(formals, freevars, freshvars);
                BDType *newret = replace_freevars(ret, freevars, freshvars);

                return bd_type_fun(newformals, newret);
            }
        case T_TUPLE:
            {
                Vector *elems = t->u.u_tuple.elems;
                Vector *newelems = replace_freevars_in_vector(elems, freevars, freshvars);
                return bd_type_tuple(newelems);
            }
        case T_LIST:
            {
                return bd_type_list(replace_freevars(t->u.u_list.elem, freevars, freshvars));
            }
        case T_ARRAY:
            {
                return bd_type_array(replace_freevars(t->u.u_array.elem, freevars, freshvars));
            }
        case T_VAR:
            {
                int i;
                BDType *free_var;

                for(i = 0; i < freevars->length; i++){
                    free_var = vector_get(freevars, i);
                    if(t == free_var){
                        return vector_get(freshvars, i);
                    }
                }

                if(t->u.u_var.content == NULL){
                    return t;
                }

                return replace_freevars(t->u.u_var.content, freevars, freshvars);
            }
    }
}

BDType *substitute_for_schema(BDType *t)
{
    Vector *freevars = t->u.u_schema.vars;

    if(freevars->length == 0){
        return t->u.u_schema.body;
    }

    Vector *freshvars = vector_new();

    int i;
    for(i = 0; i < freevars->length; i++){
        vector_add(freshvars, bd_type_var(NULL));
    }

    BDType *fresh_body = replace_freevars(t->u.u_schema.body, freevars, freshvars);
    vector_destroy(freshvars);

    return fresh_body;
}

// Unwrap TypeVar

BDType *deref_type(BDType *t, Vector *vars)
{
    switch(t->kind){
        case T_FUN:
            {
                Vector *formals = t->u.u_fun.formals;
                BDType *ret = t->u.u_fun.ret;

                int i;
                for(i = 0; i < formals->length; i++){
                    vector_set(formals, i, deref_type(vector_get(formals, i), vars));
                }

                t->u.u_fun.ret = deref_type(ret, vars);

                return t;
            }
        case T_TUPLE:
            {
                Vector *elems = t->u.u_tuple.elems;

                int i;
                for(i = 0; i < elems->length; i++){
                    vector_set(elems, i, deref_type(vector_get(elems, i), vars));
                }

                return t;
            }
        case T_VAR:
            {
                BDType *content = t->u.u_var.content;
                if(content == NULL){
                    if(vars != NULL){
                        if(vector_index_of(vars, t) < 0){
                            vector_add(vars, t);
                        }
                    }
                    return t;
                }

                t->u.u_var.content = deref_type(content, vars);
                return t->u.u_var.content;
            }
        default:
            return t;
    }
}

BDExprIdent *deref_id_type(BDExprIdent *s, Vector *vars)
{
    s->type = deref_type(s->type, vars);
    return s;
}

BDSExpr *deref_term(BDSExpr *e)
{
    switch(e->kind)
    {
        case E_UNIOP:
            e->u.u_uniop.val = deref_term(e->u.u_uniop.val);
            break;
        case E_BINOP:
            e->u.u_binop.l = deref_term(e->u.u_binop.l);
            e->u.u_binop.r = deref_term(e->u.u_binop.r);
            break;
        case E_IF:
            e->u.u_if.pred = deref_term(e->u.u_if.pred);
            e->u.u_if.t = deref_term(e->u.u_if.t);
            e->u.u_if.f = deref_term(e->u.u_if.f);
            break;
        case E_LET:
            e->u.u_let.ident = deref_id_type(e->u.u_let.ident, NULL);
            e->u.u_let.val = deref_term(e->u.u_let.val);
            e->u.u_let.body = deref_term(e->u.u_let.body);
            break;
        case E_LETREC:
            {
                BDSExprFundef *fundef = e->u.u_letrec.fundef;
                fundef->ident = deref_id_type(fundef->ident, NULL);

                int i;
                Vector *formals = fundef->formals;
                for(i = 0; i < formals->length; i++){
                    vector_set(formals, i, deref_id_type(vector_get(formals, i), NULL));
                }

                fundef->body = deref_term(fundef->body);

                e->u.u_letrec.body = deref_term(e->u.u_letrec.body);
            }
            break;
        case E_APP:
            {
                e->u.u_app.fun = deref_term(e->u.u_app.fun);

                Vector *actuals = e->u.u_app.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, deref_term(vector_get(actuals, i)));
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;
                for(i = 0; i < elems->length; i++){
                    vector_set(elems, i, deref_term(vector_get(elems, i)));
                }
            }
            break;
        case E_LETTUPLE:
            {
                Vector *idents = e->u.u_lettuple.idents;
                int i;
                for(i = 0; i < idents->length; i++){
                    vector_set(idents, i, deref_id_type(vector_get(idents, i), NULL));
                }

                e->u.u_lettuple.val = deref_term(e->u.u_lettuple.val);
                e->u.u_lettuple.body = deref_term(e->u.u_lettuple.body);
            }
            break;
    }
    return e;

}


// Typing

int occur(BDType *t1, BDType *t2);
void unify(BDType *t1, BDType *t2);

int occur_vector(BDType *t1, Vector *vec)
{
    int i;
    BDType *t2;
    for(i = 0; i < vec->length; i++){
        t2 = vector_get(vec, i);
        if(t1 == t2 || occur(t1, t2)){
            return 1;
        }
    }
    return 0;
}

int occur(BDType *t1, BDType *t2)
{
    BDTypeKind k2 = t2->kind;

    if(t1 == t2){
        return 1;
    }

    switch(k2){
        case T_FUN:
            if(occur_vector(t1, t2->u.u_fun.formals)){
                return 1;
            }
            return occur(t1, t2->u.u_fun.ret);
        case T_TUPLE:
            return occur_vector(t1, t2->u.u_tuple.elems);
        case T_VAR:
            {
                BDType *c2 = t2->u.u_var.content;
                if(t1 == c2){
                    return 1;
                }
                if(c2 == NULL){
                    return 0;
                }
                return occur(t1, c2);
            }
    }

    return 0;
}

void unify_vector(Vector *v1, Vector *v2)
{
    int l1 = v1->length;
    int l2 = v2->length;

    if(l1 != l2){
        throw(ERROR_INVALID_ARGUMENT, error_new("not match element num"));
    }

    int i;
    for(i = 0; i < l1; i++){
        unify(vector_get(v1, i), vector_get(v2, i));
    }
}

void unify(BDType *t1, BDType *t2)
{
    BDTypeKind k1, k2;
    k1 = t1->kind;
    k2 = t2->kind;

    if((k1 == T_UNIT && k2 == T_UNIT) ||
            (k1 == T_BOOL && k2 == T_BOOL) ||
            (k1 == T_INT && k2 == T_INT) ||
            (k1 == T_FLOAT && k2 == T_FLOAT)){
        return;
    }

    if(k1 == T_FUN && k2 == T_FUN){
        try{
            unify_vector(t1->u.u_fun.formals, t2->u.u_fun.formals);
        }catch{
            if(error_type == ERROR_INVALID_ARGUMENT){
                throw(ERROR_UNIFY, unify_error(t1, t2));
            }else{
                throw(error_type, catch_error());
            }
        }
        unify(t1->u.u_fun.ret, t2->u.u_fun.ret);
        return;
    }

    if(k1 == T_TUPLE && k2 == T_TUPLE){
        try{
            unify_vector(t1->u.u_tuple.elems, t2->u.u_tuple.elems);
        }catch{
            if(error_type == ERROR_INVALID_ARGUMENT){
                throw(ERROR_UNIFY, unify_error(t1, t2));
            }else{
                throw(error_type, catch_error());
            }
        }
        return;
    }

    if(k1 == T_LIST && k2 == T_LIST){
        unify(t1->u.u_list.elem, t2->u.u_list.elem);
        return;
    }

    if(k1 == T_ARRAY && k2 == T_ARRAY){
        unify(t1->u.u_array.elem, t2->u.u_array.elem);
        return;
    }

    if(k1 == T_VAR && k2 == T_VAR){
        BDType *c1 = t1->u.u_var.content;
        BDType *c2 = t2->u.u_var.content;

        if(c1 == c2){
            return;
        }
    }

    if(k1 == T_VAR){
        BDType *c1 = t1->u.u_var.content;

        if(c1 != NULL){
            unify(c1, t2);
            return;
        }
        else{
            if(occur(t1, t2)){
                throw(ERROR_UNIFY, unify_error(t1, t2));
            }
            t1->u.u_var.content = t2;
            return;
        }
    }

    if(k2 == T_VAR){
        BDType *c2 = t2->u.u_var.content;

        if(c2 != NULL){
            unify(t1, c2);
            return;
        }
        else{
            if(occur(t2, t1)){
                throw(ERROR_UNIFY, unify_error(t2, t1));
            }
            t2->u.u_var.content = t1;
            return;
        }
    }

    throw(ERROR_UNIFY, unify_error(t1, t2));
}

BDType *typing(Env *env, BDSExpr *e)
{
    try{
        switch(e->kind){
            case E_UNIT:
                return bd_type_unit();
            case E_BOOL:
                return bd_type_bool();
            case E_INT:
                return bd_type_int();
            case E_FLOAT:
                return bd_type_float();
            case E_CHAR:
                return bd_type_char();
            case E_STR:
                return bd_type_string();
            case E_NIL:
                return bd_type_list(bd_type_var(NULL));
            case E_UNIOP:
                {
                    BDType *expected;

                    switch(e->u.u_uniop.kind){
                        case OP_NOT:
                            expected = bd_type_bool();
                            break;
                        case OP_NEG:
                            expected = bd_type_int();
                            break;
                    }

                    unify(expected, typing(env, e->u.u_uniop.val));

                    return expected;
                }
            case E_BINOP:
                switch(e->u.u_binop.kind){
                    case OP_ADD:
                    case OP_SUB:
                    case OP_MUL:
                    case OP_DIV:
                        {
                            BDType *expected = bd_type_int();
                            unify(expected, typing(env, e->u.u_binop.l));
                            unify(expected, typing(env, e->u.u_binop.r));
                            return expected;
                        }
                    case OP_CONS:
                        {
                            BDType *elem = typing(env, e->u.u_binop.l);
                            BDType *expected = bd_type_list(elem);
                            unify(expected, typing(env, e->u.u_binop.r));
                            return expected;
                        }
                    case OP_EQ:
                    case OP_LE:
                        unify(typing(env, e->u.u_binop.l), typing(env, e->u.u_binop.r));
                        return bd_type_bool();
                }
                break;
            case E_IF:
                {
                    unify(bd_type_bool(), typing(env, e->u.u_if.pred));
                    BDType *tt = typing(env, e->u.u_if.t);
                    BDType *tf = typing(env, e->u.u_if.f);
                    unify(tt, tf);
                    return tt;
                }
            case E_LET:
                {
                    // unify bind part
                    BDExprIdent *ident = e->u.u_let.ident;
                    Env *local = env_local_new(env);
                    unify(ident->type, typing(local, e->u.u_let.val));
                    env_local_destroy(local);

                    // create type-schema
                    BDType *schema = create_type_schema(env, ident->type);

                    // unify body part
                    local = env_local_new(env);
                    env_set(local, ident->name, schema);
                    BDType *result = typing(local, e->u.u_let.body);
                    env_local_destroy(local);

                    return result;
                }
            case E_VAR:
                {
                    char *name = e->u.u_var.name;
                    BDType *schema = env_get(env, name);

                    if(schema != NULL){
                        // create type from schema for unify
                        return substitute_for_schema(schema);
                    }

                    throw(ERROR, "undefined variable");
                }
            case E_LETREC:
                {
                    BDSExprFundef *fundef = e->u.u_letrec.fundef;
                    BDSExpr *body = e->u.u_letrec.body;

                    Env *local = env_local_new(env);
                    env_set(local, fundef->ident->name, bd_type_schema(vector_new(), fundef->ident->type));

                    int i;
                    BDExprIdent *ident;
                    Vector *formal_types = vector_new();
                    Env *funlocal = env_local_new(local);
                    for(i = 0; i < fundef->formals->length; i++){
                        ident = vector_get(fundef->formals, i);
                        vector_add(formal_types, ident->type);
                        env_set(funlocal, ident->name, bd_type_schema(vector_new(), ident->type));
                    }

                    unify(fundef->ident->type, bd_type_fun(formal_types, typing(funlocal, fundef->body)));
                    env_local_destroy(funlocal);
                    env_set(local, fundef->ident->name, create_type_schema(env, fundef->ident->type));

                    BDType *result = typing(local, body);
                    env_local_destroy(local);

                    return result;
                }
            case E_APP:
                {
                    BDType *expected = bd_type_var(NULL);
                    Vector *actuals = e->u.u_app.actuals;
                    Vector *actual_types = vector_new();

                    int i;
                    for(i = 0; i < actuals->length; i++){
                        vector_add(actual_types, typing(env, vector_get(actuals, i)));
                    }

                    unify(typing(env, e->u.u_app.fun), bd_type_fun(actual_types, expected));

                    return expected;
                }
            case E_TUPLE:
                {
                    Vector *elems = e->u.u_tuple.elems;
                    Vector *elem_types = vector_new();

                    int i;
                    for(i = 0; i < elems->length; i++){
                        vector_add(elem_types, typing(env, vector_get(elems, i)));
                    }

                    return bd_type_tuple(elem_types);
                }
            case E_LETTUPLE:
                {
                    Vector *idents = e->u.u_lettuple.idents;
                    BDSExpr *val = e->u.u_lettuple.val;
                    BDSExpr *body = e->u.u_lettuple.body;

                    Env *local = env_local_new(env);
                    Vector *elem_types = vector_new();
                    BDExprIdent *ident;

                    int i;
                    for(i = 0; i < idents->length; i++){
                        ident = vector_get(idents, i);
                        vector_add(elem_types, ident->type);
                    }

                    unify(bd_type_tuple(elem_types), typing(env, val));

                    for(i = 0; i < idents->length; i++){
                        ident = vector_get(idents, i);
                        env_set(local, ident->name, create_type_schema(env, ident->type));
                    }

                    BDType *result = typing(local, body);
                    env_local_destroy(local);

                    return result;
                }
        }
    }catch{
        if(error_type == ERROR_UNIFY){
            UnifyError *err = catch_error();
            throw(ERROR_INFER, infer_error(e, err->t1, err->t2));
        }
        else{
            throw(error_type, catch_error());
        }
    }
    return NULL;
}

BDType *typing_fundef(Env *env, BDSExprFundef *fundef)
{
    int i;
    BDExprIdent *ident;
    Vector *formal_types = vector_new();
    Env *funlocal = env_local_new(env);
    for(i = 0; i < fundef->formals->length; i++){
        ident = vector_get(fundef->formals, i);
        vector_add(formal_types, ident->type);
        env_set(funlocal, ident->name, bd_type_schema(vector_new(), ident->type));
    }

    unify(fundef->ident->type, bd_type_fun(formal_types, typing(funlocal, fundef->body)));
}

BDSProgram *bd_typing(BDSProgram *prog)
{
    Env *env = env_new();
    Vector *vec;
    BDSExprFundef *def;
    int i;

    try{

        // add primitives

        // add datadefs
        vec = prog->datadefs;
        for(i = 0; i < vec->length; i++){
            def = vector_get(vec, i);
            env_set(env, def->ident->name, bd_type_schema(vector_new(), def->ident->type));
        }

        // add fundefs
        vec = prog->fundefs;
        for(i = 0; i < vec->length; i++){
            def = vector_get(vec, i);
            env_set(env, def->ident->name, bd_type_schema(vector_new(), def->ident->type));
        }

        // typing datadefs
        vec = prog->datadefs;
        for(i = 0; i < vec->length; i++){
            def = vector_get(vec, i);
            unify(def->ident->type, typing(env, def->body));
        }

        // typing fundefs
        vec = prog->fundefs;
        for(i = 0; i < vec->length; i++){
            def = vector_get(vec, i);
            typing_fundef(env, def);
        }

        // typing main
        BDType *type = typing(env, prog->maindef->body);
        env_destroy(env);
        env = NULL;

        if(type->kind != T_UNIT){
            //throw(ERROR, "type of 'main' must be ().");
        }

        return prog;
    }catch{
        if(env != NULL){
            env_destroy(env);
        }

        switch(error_type){
            case ERROR:
                printf("Error: %s\n", (char *)catch_error());
                break;
            case ERROR_INFER:
                {
                    InferError *err = catch_error();
                    printf("Error: This expression has type %s\n       but an expression was expected %s\n",
                            bd_type_show(err->t2),
                            bd_type_show(err->t1));
                }
                break;
        }
        throw(ERROR, "Typing error.");
    }
}
