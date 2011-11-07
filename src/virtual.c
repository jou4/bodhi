#include "compile.h"

Vector *consts;

void separete_idents(Vector *idents, Vector *ilist, Vector *flist)
{
    BDExprIdent *ident;
    int i;
    for(i = 0; i < idents->length; i++){
        ident = vector_get(idents, i);
        if(ident->type->kind == T_FLOAT){
            vector_add(flist, bd_expr_ident_clone(ident));
        }
        else{
            vector_add(ilist, bd_expr_ident_clone(ident));
        }
    }
}

BDAExpr *virtual_expr(Env *env, BDCExpr *e)
{
    switch(e->kind){
        case E_UNIT:
            return bd_aexpr_ans(bd_ainst_nop());
        case E_INT:
            return bd_aexpr_ans(bd_ainst_seti(e->u.u_int));
        case E_FLOAT:
            {
                char *lbl = bd_generate_id(bd_type_float());
                vector_add(consts, bd_aexpr_const_float(lbl, e->u.u_double));
                return bd_aexpr_ans(bd_ainst_mov(lbl));
            }
        case E_CHAR:
            return bd_aexpr_ans(bd_ainst_setc(e->u.u_char));
        case E_STR:
            {
                char *lbl = bd_generate_id(bd_type_string());
                vector_add(consts, bd_aexpr_const_str(lbl, e->u.u_str));
                return bd_aexpr_ans(bd_ainst_mov(lbl));
            }
        case E_NIL:
            return bd_aexpr_ans(bd_ainst_seti(0));
        case E_UNIOP:
            {
                return bd_aexpr_ans(bd_ainst_neg(e->u.u_uniop.val));
            }
            break;
        case E_BINOP:
            {
                switch(e->u.u_binop.kind){
                    case OP_ADD:
                        return bd_aexpr_ans(bd_ainst_add(e->u.u_binop.l, e->u.u_binop.r));
                    case OP_SUB:
                        return bd_aexpr_ans(bd_ainst_sub(e->u.u_binop.l, e->u.u_binop.r));
                    case OP_MUL:
                        return bd_aexpr_ans(bd_ainst_mul(e->u.u_binop.l, e->u.u_binop.r));
                    case OP_DIV:
                        return bd_aexpr_ans(bd_ainst_div(e->u.u_binop.l, e->u.u_binop.r));
                }
            }
            break;
        case E_IF:
            {
                switch(e->u.u_if.kind){
                    case OP_EQ:
                        return bd_aexpr_ans(
                                bd_ainst_ifeq(
                                    e->u.u_if.l,
                                    e->u.u_if.r,
                                    virtual_expr(env, e->u.u_if.t),
                                    virtual_expr(env, e->u.u_if.f)));
                    case OP_LE:
                        return bd_aexpr_ans(
                                bd_ainst_ifle(
                                    e->u.u_if.l,
                                    e->u.u_if.r,
                                    virtual_expr(env, e->u.u_if.t),
                                    virtual_expr(env, e->u.u_if.f)));
                }
            }
            break;
        case E_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                BDCExpr *val = e->u.u_let.val;
                BDCExpr *body = e->u.u_let.body;

                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);

                BDAExpr *e1 = virtual_expr(env, val);
                BDAExpr *e2 = virtual_expr(local, body);

                env_local_destroy(local);

                return bd_aexpr_concat(e1, ident, e2);
            }
            break;
        case E_VAR:
            {
                BDType *type = env_get(env, e->u.u_var.name);
                switch(type->kind){
                    case T_UNIT:
                        return bd_aexpr_ans(bd_ainst_nop());
                    default:
                        return bd_aexpr_ans(bd_ainst_mov(e->u.u_var.name));
                }
            }
            break;
        case E_MAKECLS:
            {
                BDExprIdent *ident = e->u.u_makecls.ident;
                BDExprClosure *closure = e->u.u_makecls.closure;
                BDCExpr *body = e->u.u_makecls.body;

                char *entry = closure->entry;
                Vector *freevars = closure->freevars;
                int i;

                Vector *es = vector_new();
                BDType *type;
                char *fv;
                int fvs_size = 0;
                for(i = 0; i < freevars->length; i++){
                    fv = vector_get(freevars, i);
                    type = env_get(env, fv);
                    vector_add(es, bd_ainst_pushfv(type, fv, fvs_size));
                    fvs_size += bd_value_size(type);
                }

                Env *local = env_local_new(env);
                env_set(local, ident->name, ident->type);
                BDAExpr *result = virtual_expr(local, body);
                env_local_destroy(local);

                for(i = es->length - 1; i >= 0; i--){
                    result = bd_aexpr_let(
                            bd_expr_ident("", bd_type_unit()),
                            vector_get(es, i),
                            result
                            );
                }
                vector_destroy(es);

                result = bd_aexpr_let(
                        bd_expr_ident_clone(ident),
                        bd_ainst_makecls(ident->name, fvs_size),
                        result);

                return result;
            }
            break;
        case E_APPCLS:
        case E_APPDIR:
        case E_CCALL:
            {
                char *fun = e->u.u_app.fun;
                Vector *actuals = e->u.u_app.actuals;
                Vector *idents = vector_new();

                int i;
                char *name;
                for(i = 0; i < actuals->length; i++){
                    name = vector_get(actuals, i);
                    vector_add(idents, bd_expr_ident(name, env_get(env, name)));
                }

                Vector *ilist = vector_new();
                Vector *flist = vector_new();
                separete_idents(idents, ilist, flist);

                vector_destroy(idents);

                switch(e->kind){
                    case E_APPCLS:
                        return bd_aexpr_let(
                                bd_expr_ident("_", bd_type_unit()),
                                bd_ainst_loadfvs(fun),
                                bd_aexpr_ans(bd_ainst_callcls(fun, ilist, flist)));
                    case E_APPDIR:
                        return bd_aexpr_ans(bd_ainst_calldir(fun, ilist, flist));
                    case E_CCALL:
                        return bd_aexpr_ans(bd_ainst_ccall(fun, ilist, flist));
                }
            }
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;

                char *elem;
                int i;

                char *newname = bd_generate_id(bd_type_tuple(NULL));

                elem = vector_get(elems, elems->length - 1);
                BDAExpr *result = bd_aexpr_ans(bd_ainst_mov(newname));

                for(i = elems->length - 1; i >= 0; i--){
                    elem = vector_get(elems, i);
                    result = bd_aexpr_let(
                            bd_expr_ident("", bd_type_unit()),
                            bd_ainst_pushelm(elem, i),
                            result);
                }

                return bd_aexpr_let(
                        bd_expr_ident_typevar(newname),
                        bd_ainst_maketuple(elems->length),
                        result);
            }
        case E_LETTUPLE:
            {
                Vector *idents = e->u.u_lettuple.idents;
                char *val = e->u.u_lettuple.val;
                BDCExpr *body = e->u.u_lettuple.body;

                Env *local = env_local_new(env);
                Vector *insts = vector_new();

                BDExprIdent *ident;
                int i;

                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);
                    env_set(local, ident->name, ident->type);
                    vector_add(insts, bd_ainst_getelm(i));
                }

                BDAExpr *result = virtual_expr(local, body);
                for(i = idents->length - 1; i >= 0; i--){
                    ident = vector_get(idents, i);
                    result = bd_aexpr_let(
                            bd_expr_ident_clone(ident),
                            vector_get(insts, i),
                            result);
                }

                vector_destroy(insts);
                env_local_destroy(local);

                result = bd_aexpr_let(
                        bd_expr_ident("", bd_type_unit()),
                        bd_ainst_loadelms(val),
                        result);

                return result;
            }
    }
}

BDAExpr *virtual_expr_datadef(Env *env, BDCExprDef *def)
{
    BDExprIdent *ident = def->ident;
    BDCExpr *body = def->body;

    // Add to consts.
    switch(body->kind){
        case E_CHAR:
            vector_add(consts, bd_aexpr_const_char(ident->name, body->u.u_char));
            return NULL;
        case E_INT:
            vector_add(consts, bd_aexpr_const_int(ident->name, body->u.u_int));
            return NULL;
        case E_FLOAT:
            vector_add(consts, bd_aexpr_const_float(ident->name, body->u.u_double));
            return NULL;
        case E_STR:
            vector_add(consts, bd_aexpr_const_str(ident->name, body->u.u_str));
            return NULL;
        case E_NIL:
            vector_add(consts, bd_aexpr_const_int(ident->name, 0));
            return NULL;
    }

    // Concat with maindef.
    return virtual_expr(env, body);
}

BDAExprDef *virtual_expr_fundef(Env *env, BDCExprDef *def)
{
    Env *local = env_local_new(env);

    BDExprIdent *ident = def->ident;
    BDCExpr *fun = def->body;

    BDType *type = fun->u.u_fun.type;
    Vector *formals = fun->u.u_fun.formals;
    Vector *fvs = fun->u.u_fun.freevars;
    BDCExpr *body = fun->u.u_fun.body;

    Vector *iformals = vector_new();
    Vector *fformals = vector_new();
    Vector *freevars = vector_new();

    separete_idents(formals, iformals, fformals);

    int i;
    BDExprIdent *formal;
    for(i = 0; i < formals->length; i++){
        formal = vector_get(formals, i);
        env_set(local, formal->name, formal->type);
    }

    for(i = 0; i < fvs->length; i++){
        formal = vector_get(fvs, i);
        env_set(local, formal->name, formal->type);
        vector_add(freevars, bd_expr_ident_clone(formal));
    }

    BDAExprDef *result = bd_aexpr_def(
            bd_expr_ident_clone(ident),
            iformals,
            fformals,
            freevars,
            virtual_expr(local, body)
            );

    env_local_destroy(local);

    return result;
}

BDAExpr *insert_initializer(BDAExpr *init, BDExprIdent *ident, BDAExpr *body)
{
    BDAExpr *newexpr;

    switch(init->kind){
        case AE_ANS:
            {
                char *tmpname = bd_generate_id(ident->type);
                printf("*****%s *****\n" , tmpname);
                newexpr = bd_aexpr_let(
                        bd_expr_ident(tmpname, bd_type_clone(ident->type)),
                        init->u.u_ans.val,
                        bd_aexpr_let(
                            bd_expr_ident("", bd_type_unit()),
                            bd_ainst_setglobal(ident->type, ident->name, tmpname),
                            body));
            }
            break;
        case AE_LET:
            newexpr = bd_aexpr_let(
                    bd_expr_ident_clone(init->u.u_let.ident),
                    init->u.u_let.val,
                    insert_initializer(
                        init->u.u_let.body,
                        ident,
                        body));
            break;
    }
    free(init);

    return newexpr;
}

extern Vector *primsigs;

BDAProgram *bd_virtual(BDCProgram *prog)
{
    BDAProgram *aprog = malloc(sizeof(BDAProgram));
    bd_aprogram_init(aprog);
    consts = aprog->consts;

    Env *env = env_new();
    Vector *inits = vector_new();
    Vector *initializers = vector_new();

    BDCExprDef *def;
    BDAExpr *init, *main;
    PrimSig *sig;
    Vector *vec;
    int i;

    // Add primitives.
    for(i = 0; i < primsigs->length; i++){
        sig = vector_get(primsigs, i);
        env_set(env, sig->lbl, sig->type);
    }

    // Add consts.
    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        env_set(env, def->ident->name, def->ident->type);
    }

    // Add functions.
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        env_set(env, def->ident->name, def->ident->type);
    }

    // consts or inits
    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        init = virtual_expr_datadef(env, def);
        if(init != NULL){
            vector_add(inits, def->ident);
            vector_add(initializers, init);
            vector_add(aprog->globals, bd_expr_ident_clone(def->ident));
        }
    }

    // Transform function definitions to VMCodes.
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        vector_add(aprog->fundefs, virtual_expr_fundef(env, def));
    }

    // Transform main definition and combine with initializes.
    def = prog->maindef;
    main = virtual_expr(env, def->body);

    for(i = inits->length - 1; i >= 0; i--){
        main = insert_initializer(
                vector_get(initializers, i),
                vector_get(inits, i),
                main);
    }

    aprog->maindef = bd_aexpr_def(
            bd_expr_ident_clone(def->ident),
            NULL,
            NULL,
            NULL,
            main);

    env_destroy(env);
    vector_destroy(inits);
    vector_destroy(initializers);

    return aprog;
}
