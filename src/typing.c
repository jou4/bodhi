#include "compile.h"

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
		default:
			return NULL;
    }
}

BDType *substitute_for_schema(BDType *t)
{
    Vector *freevars = t->u.u_schema.vars;

    if(freevars == NULL || freevars->length == 0){
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
                e->u.u_letrec.ident = deref_id_type(e->u.u_letrec.ident, NULL);
                e->u.u_letrec.fun = deref_term(e->u.u_letrec.fun);
                e->u.u_letrec.body = deref_term(e->u.u_letrec.body);
            }
            break;
        case E_FUN:
            {
                e->u.u_fun.type = deref_type(e->u.u_fun.type, NULL);

                int i;
                Vector *formals = e->u.u_fun.formals;
                for(i = 0; i < formals->length; i++){
                    vector_set(formals, i, deref_id_type(vector_get(formals, i), NULL));
                }

                e->u.u_fun.body = deref_term(e->u.u_fun.body);
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
		default:
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
		default:
			break;
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
            (k1 == T_FLOAT && k2 == T_FLOAT) ||
            (k1 == T_CHAR && k2 == T_CHAR) ||
            (k1 == T_STRING && k2 == T_STRING)
      )
    {
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

        /*if(c1 == NULL && c2 == NULL){
            // Priority is given to the right type.
            t1->u.u_var.content = t2;
            return;
        }
        else */if(c1 == c2){
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

BDType *typing_fun(Env *env, BDSExpr *fun);

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
                    BDType *expected = NULL;
                    BDType *t = typing(env, e->u.u_uniop.val);

                    switch(e->u.u_uniop.kind){
                        case OP_NOT:
                            expected = bd_type_bool();
                            break;
                        case OP_NEG:
                            expected = bd_type_int();
                            break;
                        case OP_FNEG:
                            expected = bd_type_float();
                            break;
						default:
							break;
                    }

                    unify(expected, t);

                    return expected;
                }
            case E_BINOP:
                switch(e->u.u_binop.kind){
                    case OP_ADD:
                    case OP_SUB:
                    case OP_MUL:
                    case OP_DIV:
                    case OP_MOD:
                    case OP_FADD:
                    case OP_FSUB:
                    case OP_FMUL:
                    case OP_FDIV:
                        {
                            BDType *expected = NULL;
                            BDType *t1 = typing(env, e->u.u_binop.l);
                            BDType *t2 = typing(env, e->u.u_binop.r);

                            switch(e->u.u_binop.kind){
                                case OP_ADD:
                                case OP_SUB:
                                case OP_MUL:
                                case OP_DIV:
                                case OP_MOD:
                                    expected = bd_type_int();
                                    break;
                                case OP_FADD:
                                case OP_FSUB:
                                case OP_FMUL:
                                case OP_FDIV:
                                    expected = bd_type_float();
                                    break;
                                default:
                                    printf("Unexpected operator was passed.\n");
                                    exit(EXIT_FAILURE);
                                    break;
                            }

                            unify(expected, t1);
                            unify(expected, t2);
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
					default:
						break;
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

                    if(is_trash_name(name)){
                        printf("'%s' is unavailable for var name.\n", name);
                        exit(EXIT_FAILURE);
                    }

                    BDType *schema = env_get(env, name);

                    if(schema != NULL){
                        // create type from schema for unify
                        return substitute_for_schema(schema);
                    }

                    char msg[100];
                    sprintf(msg, "'%s' is undefined.", name);

                    throw(ERROR, mem_strdup(msg));
                }
            case E_LETREC:
                {
                    BDExprIdent *ident = e->u.u_letrec.ident;
                    BDSExpr *fun = e->u.u_letrec.fun;
                    BDSExpr *body = e->u.u_letrec.body;

                    if(is_trash_name(ident->name)){
                        printf("'%s' is unavailable for function name.\n", ident->name);
                        exit(EXIT_FAILURE);
                    }

                    Env *local = env_local_new(env);
                    env_set(local, ident->name, bd_type_schema(NULL, ident->type));

                    BDType *funtype = typing_fun(local, fun);
                    unify(ident->type, funtype);
                    env_set(local, ident->name, create_type_schema(env, ident->type));

                    BDType *result = typing(local, body);
                    env_local_destroy(local);

                    return result;
                }
            case E_FUN:
                {
                    return typing_fun(env, e);
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

                    //unify(bd_type_fun(actual_types, expected), typing(env, e->u.u_app.fun));
                    unify(typing(env, e->u.u_app.fun), bd_type_fun(actual_types, expected));

                    return expected;
                }
            case E_CCALL:
                {
                    return bd_type_var(NULL);
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
                        env_set(local, ident->name, bd_type_schema(NULL, ident->type));
                    }

                    BDType *result = typing(local, body);
                    env_local_destroy(local);

                    return result;
                }
            case E_MATCH:
                {
                    BDSExpr *target = e->u.u_match.target;
                    Vector *branches = e->u.u_match.branches;

                    BDType *pattern_expected = typing(env, target);
                    BDType *expected = bd_type_var(NULL);

                    int i;
                    BDSExprMatchBranch *branch;
                    BDSExpr *pattern;
                    BDSExpr *body;
                    Env *local;

                    for(i = 0; i < branches->length; i++){
                        branch = vector_get(branches, i);
                        pattern = branch->pattern;
                        body = branch->body;

                        local = env_local_new(env);
                        unify(pattern_expected, typing(local, pattern));
                        unify(expected, typing(local, body));
                        env_local_destroy(local);
                    }

                    return expected;
                }
                break;
            case E_PATTERNVAR:
                {
                    BDExprIdent *ident = e->u.u_patvar.ident;
                    env_set(env, ident->name, bd_type_schema(NULL, ident->type));
                    return ident->type;
                }
                break;
			default:
				break;
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

BDType *typing_fun(Env *env, BDSExpr *fun)
{
    BDType *funtype = fun->u.u_fun.type;
    Vector *funformals = fun->u.u_fun.formals;
    BDSExpr *funbody = fun->u.u_fun.body;

    int i;
    BDExprIdent *ident;
    Vector *formal_types = vector_new();
    Env *funlocal = env_local_new(env);

    for(i = 0; i < funformals->length; i++){
        ident = vector_get(funformals, i);
        vector_add(formal_types, ident->type);
        env_set(funlocal, ident->name, bd_type_schema(NULL, ident->type));
    }

    unify(funtype, bd_type_fun(formal_types, typing(funlocal, funbody)));
    env_local_destroy(funlocal);

    return funtype;
}

void set_def_type(Env *env, BDSExprDef *def)
{
    env_set(env, def->ident->name, bd_type_schema(NULL, def->ident->type));
}

void set_defs_type(Env *env, Vector *defs)
{
    int i;
    for(i = 0; i < defs->length; i++){
        set_def_type(env, vector_get(defs, i));
    }
}

BDType *typing_def(Env *env, BDSExprDef *def)
{
    unify(def->ident->type, typing(env, def->body));
    def->ident->type = deref_type(def->ident->type, NULL);
    def->body = deref_term(def->body);

    // Reset infered type to env.
    env_set(env, def->ident->name, create_type_schema(env, def->ident->type));

    return def->ident->type;
}

void typing_defs(Env *env, Vector *defs)
{
    int i;
    for(i = 0; i < defs->length; i++){
        typing_def(env, vector_get(defs, i));
    }
}


extern Vector *primsigs;

BDSProgram *bd_typing(BDSProgram *prog)
{
    Env *env = env_new();
    BDSExprDef *def;
    int i;

    try{
        // add primitives
        PrimSig *sig;
        for(i = 0; i < primsigs->length; i++){
            sig = vector_get(primsigs, i);
            env_set(env, sig->name, create_type_schema(env, sig->type));
        }

        // add defs type
        set_defs_type(env, prog->defs);

        // typing defs
        typing_defs(env, prog->defs);

        // typing main
        def = prog->maindef;
        BDType *maintype = bd_return_type(deref_type(typing(env, def->body), NULL));
        def->body = deref_term(def->body);

        env_destroy(env);
        env = NULL;

        if(maintype->kind != T_UNIT && maintype->kind != T_VAR){
            throw(ERROR, "type of 'main' must be ().");
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
