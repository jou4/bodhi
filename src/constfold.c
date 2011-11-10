#include "compile.h"

int has_const(Env *env, char *name, BDExprKind kind){
    BDNExpr *e = env_get(env, name);
    if(e){
        if(e->kind == kind){
            return 1;
        }
    }
    return 0;
}

int find_const_int(Env *env, char *name){
    BDNExpr *e = env_get(env, name);
    if(e){
        if(e->kind == E_INT){
            return e->u.u_int;
        }
    }
    // TODO Exception
    return 0;
}

double find_const_float(Env *env, char *name){
    BDNExpr *e = env_get(env, name);
    if(e){
        if(e->kind == E_FLOAT){
            return e->u.u_double;
        }
    }
    // TODO Exception
    return 0;
}

Vector *find_const_tuple(Env *env, char *name){
    BDNExpr *e = env_get(env, name);
    if(e){
        if(e->kind == E_TUPLE){
            return e->u.u_tuple.elems;
        }
    }
    // TODO Exception
    return NULL;
}

BDNExpr *const_fold(Env *env, BDNExpr *e)
{
    BDNExpr *newexpr = NULL;
    char *x, *y;

    switch(e->kind){
        case E_VAR:
            x = e->u.u_var.name;
            if(has_const(env, x, T_INT)){
                newexpr = bd_nexpr_int(find_const_int(env, x));
                bd_nexpr_destroy(e);
                return newexpr;
            }
            break;
        case E_UNIOP:
            x = e->u.u_uniop.val;
            switch(e->u.u_uniop.kind){
                case OP_NEG:
                    if(has_const(env, x, T_INT)){
                        newexpr = bd_nexpr_int(find_const_int(env, x) * -1);
                        bd_nexpr_destroy(e);
                        return newexpr;
                    }
                    break;
            }
            break;
        case E_BINOP:
            x = e->u.u_binop.l;
            y = e->u.u_binop.r;
            switch(e->u.u_binop.kind){
                case OP_ADD:
                    if(has_const(env, x, T_INT) && has_const(env, y, T_INT)){
                        newexpr = bd_nexpr_int(find_const_int(env, x) + find_const_int(env, y));
                    }
                    break;
                case OP_SUB:
                    if(has_const(env, x, T_INT) && has_const(env, y, T_INT)){
                        newexpr = bd_nexpr_int(find_const_int(env, x) - find_const_int(env, y));
                    }
                    break;
                case OP_MUL:
                    if(has_const(env, x, T_INT) && has_const(env, y, T_INT)){
                        newexpr = bd_nexpr_int(find_const_int(env, x) * find_const_int(env, y));
                    }
                    break;
                case OP_DIV:
                    if(has_const(env, x, T_INT) && has_const(env, y, T_INT)){
                        newexpr = bd_nexpr_int(find_const_int(env, x) / find_const_int(env, y));
                    }
                    break;
            }

            if(newexpr != NULL){
                bd_nexpr_destroy(e);
                return newexpr;
            }
            break;
        case E_IF:
            x = e->u.u_if.l;
            y = e->u.u_if.r;
            switch(e->u.u_if.kind){
                case OP_EQ:
                    if(has_const(env, x, T_INT) && has_const(env, y, T_INT)){
                        if(find_const_int(env, x) == find_const_int(env, y)){
                            newexpr = const_fold(env, e->u.u_if.t);
                            bd_nexpr_destroy(e->u.u_if.f);
                        }
                        else{
                            newexpr = const_fold(env, e->u.u_if.f);
                            bd_nexpr_destroy(e->u.u_if.t);
                        }
                    }
                    break;
                case OP_LE:
                    if(has_const(env, x, T_INT) && has_const(env, y, T_INT)){
                        if(find_const_int(env, x) <= find_const_int(env, y)){
                            newexpr = const_fold(env, e->u.u_if.t);
                            bd_nexpr_destroy(e->u.u_if.f);
                        }
                        else{
                            newexpr = const_fold(env, e->u.u_if.f);
                            bd_nexpr_destroy(e->u.u_if.t);
                        }
                    }
                    break;
            }

            if(newexpr != NULL){
                free(e->u.u_if.l);
                free(e->u.u_if.r);
                free(e);
                return newexpr;
            }
            break;
        case E_LET:
            {
                x = e->u.u_let.ident->name;
                BDNExpr *e1 = const_fold(env, e->u.u_let.val);

                Env *local = env_local_new(env);
                env_set(local, x, e1);
                BDNExpr *e2 = const_fold(local, e->u.u_let.body);
                env_local_destroy(local);

                e->u.u_let.val = e1;
                e->u.u_let.body = e2;
            }
            break;
        case E_LETREC:
            e->u.u_letrec.fun = const_fold(env, e->u.u_letrec.fun);
            e->u.u_letrec.body = const_fold(env, e->u.u_letrec.body);
            break;
		case E_FUN:
            e->u.u_fun.body = const_fold(env, e->u.u_fun.body);
			break;
        case E_LETTUPLE:
            // TODO
            e->u.u_lettuple.body = const_fold(env, e->u.u_lettuple.body);
            break;
		default:
			break;
    }
    return e;
}

BDNProgram *bd_const_fold(BDNProgram *prog)
{
    Env *env = env_new();

    int i;
    Vector *vec;
    BDNExprDef *def;

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        def->body = const_fold(env, def->body);
        vector_set(vec, i, def);
    }

    prog->maindef->body = const_fold(env, prog->maindef->body);

    env_destroy(env);

    return prog;
}
