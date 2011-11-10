#include "compile.h"

int has_side_effect(BDNExpr *e)
{
    switch(e->kind){
        case E_LET:
            if(has_side_effect(e->u.u_let.val) || has_side_effect(e->u.u_let.body)){
                return 1;
            }
            break;
        case E_IF:
            if(has_side_effect(e->u.u_if.t) || has_side_effect(e->u.u_if.f)){
                return 1;
            }
            break;
        case E_LETREC:
            return has_side_effect(e->u.u_letrec.body);
		case E_FUN:
			return has_side_effect(e->u.u_fun.body);
        case E_LETTUPLE:
            return has_side_effect(e->u.u_lettuple.body);
        case E_APP:
            return 1;
		default:
			break;
    }
	return 0;
}

BDNExpr *elim_disuse_decl(BDNExpr *e)
{
    switch(e->kind){
        case E_IF:
            e->u.u_if.t = elim_disuse_decl(e->u.u_if.t);
            e->u.u_if.f = elim_disuse_decl(e->u.u_if.f);
            break;
        case E_LET:
            {
                BDNExpr *e1 = elim_disuse_decl(e->u.u_let.val);
                BDNExpr *e2 = elim_disuse_decl(e->u.u_let.body);

                Set *fvs = bd_nexpr_freevars(e2);

                if(has_side_effect(e1) || set_has(fvs, e->u.u_let.ident->name)){
                    e->u.u_let.val = e1;
                    e->u.u_let.body = e2;

                    set_destroy(fvs);
                }
                else{
                    // eliminate variable
                    bd_expr_ident_destroy(e->u.u_let.ident);
                    bd_nexpr_destroy(e1);
                    free(e);

                    set_destroy(fvs);

                    return e2;
                }
            }
            break;
        case E_LETREC:
            {
                BDNExpr *e2 = elim_disuse_decl(e->u.u_letrec.body);

                Set *fvs = bd_nexpr_freevars(e2);

                if(set_has(fvs, e->u.u_letrec.ident->name)){
                    e->u.u_letrec.fun = elim_disuse_decl(e->u.u_letrec.fun);
                    e->u.u_letrec.body = e2;

                    set_destroy(fvs);
                }
                else{
                    // eliminate function
                    bd_nexpr_destroy(e->u.u_letrec.fun);
                    free(e);

                    set_destroy(fvs);

                    return e2;
                }
            }
            break;
		case E_FUN:
			e->u.u_fun.body = elim_disuse_decl(e->u.u_fun.body);
			break;
        case E_LETTUPLE:
            {
                BDNExpr *e1 = elim_disuse_decl(e->u.u_lettuple.body);
                Set *fvs = bd_nexpr_freevars(e1);

                Vector *idents = e->u.u_lettuple.idents;
                BDExprIdent *ident;
                int i;
                int use = 0;

                for(i = 0; i < idents->length; i++){
                    ident = vector_get(idents, i);

                    if(set_has(fvs, ident->name)){
                        use = 1;
                        break;
                    }
                }

                if(use){
                    e->u.u_lettuple.body = e1;
                    set_destroy(fvs);
                }
                else{
                    // eliminate tuple
                    vector_each(idents, bd_expr_ident_destroy);
                    vector_destroy(idents);
                    free(e->u.u_lettuple.val);
                    free(e);

                    set_destroy(fvs);

                    return e1;
                }
            }
            break;
		default:
			break;
    }
    return e;
}

BDNProgram *bd_elim(BDNProgram *prog)
{
    int i;
    Vector *vec;
    BDNExprDef *def;

    vec = prog->defs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        def->body = elim_disuse_decl(def->body);
        vector_set(vec, i, def);
    }

    prog->maindef->body = elim_disuse_decl(prog->maindef->body);

	return prog;
}
