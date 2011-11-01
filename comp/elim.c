#include "compile.h"

int has_side_effect(BDExpr2 *e)
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
        case E_LETTUPLE:
            return has_side_effect(e->u.u_lettuple.body);
        case E_APP:
            return 1;
    }
    return 0;
}

BDExpr2 *elim_disuse_decl(BDExpr2 *e)
{
    switch(e->kind){
        case E_IF:
            e->u.u_if.t = elim_disuse_decl(e->u.u_if.t);
            e->u.u_if.f = elim_disuse_decl(e->u.u_if.f);
            break;
        case E_LET:
            {
                BDExpr2 *e1 = elim_disuse_decl(e->u.u_let.val);
                BDExpr2 *e2 = elim_disuse_decl(e->u.u_let.body);

                Set *fvs = bd_expr2_freevars(e2);

                if(has_side_effect(e1) || set_has(fvs, e->u.u_let.ident->name)){
                    e->u.u_let.val = e1;
                    e->u.u_let.body = e2;

                    set_destroy(fvs);
                }
                else{
                    // eliminate variable
                    bd_expr_ident_destroy(e->u.u_let.ident);
                    bd_expr2_destroy(e1);
                    free(e);

                    set_destroy(fvs);

                    return e2;
                }
            }
            break;
        case E_LETREC:
            {
                BDExpr2 *e2 = elim_disuse_decl(e->u.u_letrec.body);

                Set *fvs = bd_expr2_freevars(e2);

                if(set_has(fvs, e->u.u_letrec.fundef->ident->name)){
                    e->u.u_letrec.fundef->body = elim_disuse_decl(e->u.u_letrec.fundef->body);
                    e->u.u_letrec.body = e2;

                    set_destroy(fvs);
                }
                else{
                    // eliminate function
                    bd_expr2_fundef_destroy(e->u.u_letrec.fundef);
                    free(e);

                    set_destroy(fvs);

                    return e2;
                }
            }
            break;
        case E_LETTUPLE:
            {
                BDExpr2 *e1 = elim_disuse_decl(e->u.u_lettuple.body);
                Set *fvs = bd_expr2_freevars(e1);

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
    }
    return e;
}

BDExpr2 *bd_elim(BDExpr2 *e)
{
    BDExpr2 *ret = elim_disuse_decl(e);

    printf("--- Disuse declaration eliminated --- \n");
    bd_expr2_show(ret);
    printf("\n");

    return ret;
}
