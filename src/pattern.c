#include "compile.h"
#include <string.h>

int equal_pattern(BDSExpr *e1, BDSExpr *e2)
{
    if(e1->kind != e2->kind){
        return 0;
    }

    switch(e1->kind){
        case E_PATTERNVAR:
            if(strcmp(e1->u.u_var.name, e2->u.u_var.name) == 0){
                return 1;
            }
            return 0;
        case E_INT:
        case E_BOOL:
            if(e1->u.u_int == e2->u.u_int){
                return 1;
            }
            return 0;
        case E_CHAR:
            if(e1->u.u_char == e2->u.u_char){
                return 1;
            }
            return 0;
        case E_TUPLE:
            {
                Vector *elems1 = e1->u.u_tuple.elems;
                Vector *elems2 = e2->u.u_tuple.elems;

                int ret = 1;
                int i;
                for(i = 0; i < elems1->length; i++){
                    ret &= equal_pattern(vector_get(elems1, i), vector_get(elems2, i));
                }

                return ret;
            }
        default:
            break;
    }

    return 0;
}

/*
 * Check whether e1 and e2 is mismatching.
 *
 * ex)
 *  Var("a"), Int(1) ---> false
 *  Int(1), Var("a") ---> false
 *  Int(1), Int(2)   ---> false
 *  Tuple(a,b), Tuple(a,b,c) ---> true(mismatching)
 *
 */
int mismatching_pattern(BDSExpr *e1, BDSExpr *e2)
{
    if((e1->kind == E_INT && e2->kind == E_INT) ||
            (e1->kind == E_BOOL && e2->kind == E_BOOL) ||
            (e1->kind == E_CHAR && e2->kind == E_CHAR) ||
            (e1->kind == E_STR && e2->kind == E_STR) ||
            (e1->kind == E_FLOAT && e2->kind == E_FLOAT) ||
            (e1->kind == E_PATTERNVAR || e2->kind == E_PATTERNVAR)){
        return 0;
    }

    if(e1->kind == E_TUPLE && e2->kind == E_TUPLE){
        Vector *elems1 = e1->u.u_tuple.elems;
        Vector *elems2 = e2->u.u_tuple.elems;

        if(elems1->length != elems2->length){
            return 1;
        }

        int ret = 0;
        int i;
        for(i = 0; i < elems1->length; i++){
            ret |= mismatching_pattern(vector_get(elems1, i), vector_get(elems2, i));
        }

        return ret;
    }

    return 0;
}

void assert_mismatching_pattern(BDSExprMatchBranch *base, Vector *patterns)
{
    BDSExprMatchBranch *branch;
    int i;
    for(i = 0; i < patterns->length; i++){
        branch = vector_get(patterns, i);
        if(mismatching_pattern(base->pattern, branch->pattern)){
            printf("Error: Pattern match(es) are mismatching.\n");
            printf("\t");
            bd_sexpr_show(base->pattern);
            printf("\t");
            bd_sexpr_show(branch->pattern);
            exit(EXIT_FAILURE);
        }
    }
}

/*
 * Check whether e2 is overlapped by e1.
 *
 * ex)
 *  Var("a"), Int(1) ---> true
 *  Int(1), Var("a") ---> false
 *  Int(1), Int(2)   ---> false
 *  Int(1), Int(1)   ---> true
 *
 */
int overlapped_pattern(BDSExpr *e1, BDSExpr *e2)
{
    if(e1->kind == E_PATTERNVAR){
        return 1;
    }

    if(e1->kind != e2->kind){
        return 0;
    }

    switch(e1->kind){
        case E_PATTERNVAR:
            if(strcmp(e1->u.u_var.name, e2->u.u_var.name) == 0){
                return 1;
            }
            return 0;
        case E_INT:
        case E_BOOL:
            if(e1->u.u_int == e2->u.u_int){
                return 1;
            }
            return 0;
        case E_CHAR:
            if(e1->u.u_char == e2->u.u_char){
                return 1;
            }
            return 0;
        case E_TUPLE:
            {
                Vector *elems1 = e1->u.u_tuple.elems;
                Vector *elems2 = e2->u.u_tuple.elems;

                int ret = 1;
                int i;
                for(i = 0; i < elems1->length; i++){
                    ret &= overlapped_pattern(vector_get(elems1, i), vector_get(elems2, i));
                }

                return ret;
            }
        default:
            break;
    }

    return 0;
}

void assert_overlapped_pattern(BDSExprMatchBranch *base, Vector *patterns)
{
    BDSExprMatchBranch *branch;
    int i;
    for(i = 0; i < patterns->length; i++){
        branch = vector_get(patterns, i);
        if(overlapped_pattern(base->pattern, branch->pattern)){
            printf("Error: Pattern match(es) are overlapped.\n");
            printf("\t");
            bd_sexpr_show(base->pattern);
            printf("\t");
            bd_sexpr_show(branch->pattern);
            exit(EXIT_FAILURE);
        }
    }
}

Vector *vector_slice(Vector *vec, int begin)
{
    Vector *new_vec = vector_new();
    int i;
    for(i = begin; i < vec->length; i++){
        vector_add(new_vec, vector_get(vec, i));
    }
    return new_vec;
}

BDSExpr *match_transform(BDSExpr *e);

BDSExpr *match_transform_match(BDSExpr *e, BDSExpr *cont)
{
    BDSExpr *target = e->u.u_match.target;
    Vector *branches = e->u.u_match.branches;

    int i;
    BDSExprMatchBranch *branch, *head_branch;
    BDSExpr *head_branch_pattern, *head_branch_body;
    Vector *remains;

    // Check whether the overlapped pattern exists.
    for(i = 0; i < branches->length - 1; i++){
        branch = vector_get(branches, i);
        remains = vector_slice(branches, i + 1);
        assert_mismatching_pattern(branch, remains);
        assert_overlapped_pattern(branch, remains);
        vector_destroy(remains);
    }

    // head of branches
    head_branch = vector_get(branches, 0);
    head_branch_pattern = head_branch->pattern;
    head_branch_body = head_branch->body;

    if(head_branch_pattern->kind == E_TUPLE){
        if(target->kind == E_TUPLE){
            // ex) match (a, b) with (1, 2) -> ..

            // dividing elements of match target into head & tail
            // ex) (a, b, c) ---> a & (b, c)
            Vector *elems = target->u.u_tuple.elems;
            BDSExpr *shifted_target = vector_get(elems, 0);
            Vector *remain_targets = vector_slice(elems, 1);
            BDSExpr *succeeding_target = bd_sexpr_tuple(remain_targets);

            Vector *transformed_branches = vector_new();
            Vector *remain_branches = branches;
            Vector *grouped_branches, *succeeding_branches, *cont_branches;

            Vector *tmp_remain_branches;
            BDSExprMatchBranch *base_branch, *tmp_branch;
            BDSExpr *base_pattern_item, *tmp_pattern_item;
            BDSExpr *new_cont;
            int break_flg;

            if(remain_targets->length > 0){
                // has remain target
                // ex) case (a, b, c) with ..
                while(remain_branches->length > 0){
                    grouped_branches = vector_new();
                    tmp_remain_branches = vector_new();
                    succeeding_branches = vector_new();
                    base_branch = vector_get(remain_branches, 0);
                    base_pattern_item = vector_get(base_branch->pattern->u.u_tuple.elems, 0);

                    // grouping branches
                    // ex)
                    //  match (a, b) with
                    //   | (1, 2) -> ...
                    //   | (1, 3) -> ...
                    //   | (2, 4) -> ...
                    //
                    //  --->
                    //   | (1, 2) -> ...
                    //   | (1, 3) -> ...
                    //  --
                    //   | (2, 4) -> ...
                    break_flg = 0;
                    for(i = 0; i < remain_branches->length; i++){
                        tmp_branch = vector_get(remain_branches, i);
                        if(break_flg){
                            vector_add(tmp_remain_branches, tmp_branch);
                        }
                        else{
                            tmp_pattern_item = vector_get(tmp_branch->pattern->u.u_tuple.elems, 0);
                            if(equal_pattern(base_pattern_item, tmp_pattern_item)){
                                vector_add(grouped_branches, tmp_branch);
                            }
                            else{
                                break_flg = 1;
                                vector_add(tmp_remain_branches, tmp_branch);
                            }
                        }
                    }

                    // making the succeeding branches of the grouped branches
                    // ex)
                    //  match (a, b) with
                    //   | (1, 2) -> ..
                    //   | (1, 3) -> ..
                    //   | (2, 4) -> ..
                    //
                    //  --->
                    //  match a with
                    //   | 1 -> match (b) with
                    //           | (2) -> ..
                    //           | (3) -> ..
                    //   | 2 -> ..
                    for(i = 0; i < grouped_branches->length; i++){
                        tmp_branch = vector_get(grouped_branches, i);
                        vector_add(succeeding_branches,
                                bd_sexpr_match_branch(
                                    bd_sexpr_tuple(vector_slice(tmp_branch->pattern->u.u_tuple.elems, 1)),
                                    match_transform(tmp_branch->body)));
                    }

                    if(base_pattern_item->kind != E_PATTERNVAR){
                        // finding PATTERNVAR and making continuation of error
                        // ex)
                        //  match (a, b) with
                        //   | (1, 2) -> ..
                        //   | (2, 3) -> ..
                        //   | (a, 4) -> ..   continuation when match failed with (1, 2) or (2, 3)
                        cont_branches = vector_new();
                        for(i = 0; i < tmp_remain_branches->length; i++){
                            tmp_branch = vector_get(tmp_remain_branches, i);
                            tmp_pattern_item = vector_get(tmp_branch->pattern->u.u_tuple.elems, 0);
                            if(tmp_pattern_item->kind == E_PATTERNVAR){
                                vector_add(cont_branches, tmp_branch);
                            }
                        }

                        if(cont_branches->length > 0){
                            new_cont = match_transform_match(
                                    bd_sexpr_match(target, cont_branches), cont);
                        }
                        else{
                            new_cont = cont;
                            vector_destroy(cont_branches);
                        }

                        vector_add(transformed_branches,
                                bd_sexpr_match_branch(
                                    base_pattern_item,
                                    match_transform_match(
                                        bd_sexpr_match(succeeding_target, succeeding_branches),
                                        new_cont)
                                    ));
                    }
                    else{
                        if(tmp_remain_branches->length > 0){
                            // passing all remains as continuation
                            // because can not be the pattern on same position.
                            // ex)
                            //  match (a, b) with
                            //   | (x, 2) -> ..
                            //   | (1, 3) -> ..
                            //
                            //  --->
                            //  match a with
                            //   | x -> match b with
                            //           | 2 -> ..
                            //           | _ -> match a with
                            //                   | 1 -> match b with
                            //                           | 3 -> ..
                            //
                            new_cont = match_transform_match(
                                    bd_sexpr_match(target, tmp_remain_branches),
                                    cont);
                            tmp_remain_branches = vector_new();
                        }
                        else{
                            new_cont = cont;
                        }

                        vector_add(transformed_branches,
                                bd_sexpr_match_branch(
                                    base_pattern_item,
                                    match_transform_match(
                                        bd_sexpr_match(succeeding_target, succeeding_branches),
                                        new_cont
                                        )));
                    }

                    // next loop
                    remain_branches = tmp_remain_branches;
                }
            }
            else{
                // has no remain target
                // ex) match (a) with ..
                for(i = 0; i < branches->length; i++){
                    tmp_branch = vector_get(branches, i);
                    vector_add(transformed_branches,
                            bd_sexpr_match_branch(
                                vector_get(tmp_branch->pattern->u.u_tuple.elems, 0),
                                match_transform(tmp_branch->body)
                                ));
                }
            }

            return match_transform_match(
                    bd_sexpr_match(shifted_target, transformed_branches), cont);
        }
        else{
            // ex) match a with (1, 2) -> ..

            // dividing branches into tuple pattern & remains
            // ex)
            //  match a with
            //   | (1, 2) -> ..
            //   | (2, 3) -> ..
            //   | x -> ..
            //
            //  --->
            //  match (b, c) with
            //   | (1, 2) -> ..
            //   | (2, 3) -> ..
            //  --
            //  match a with
            //   | x -> ..
            Vector *tuple_branches = vector_new();
            Vector *remain_branches = vector_new();
            BDSExprMatchBranch *tmp_branch;
            int break_flg = 0;
            for(i = 0; i < branches->length; i++){
                tmp_branch = vector_get(branches, i);
                if(break_flg){
                    vector_add(remain_branches, tmp_branch);
                }
                else{
                    if(tmp_branch->pattern->kind == E_TUPLE){
                        vector_add(tuple_branches, tmp_branch);
                    }
                    else{
                        break_flg = 1;
                        vector_add(remain_branches, tmp_branch);
                    }
                }
            }

            char *new_var;
            Vector *new_idents = vector_new();
            Vector *new_elems = vector_new();
            Vector *elems = head_branch_pattern->u.u_tuple.elems;
            for(i = 0; i < elems->length; i++){
                new_var = bd_generate_id(NULL);
                vector_add(new_idents, bd_expr_ident_typevar(new_var));
                vector_add(new_elems, bd_sexpr_var(new_var));
            }

            BDSExpr *new_target = bd_sexpr_tuple(new_elems);
            BDSExpr *new_cont = bd_sexpr_cont(NULL);
            BDSExpr *new_match = bd_sexpr_match(new_target, tuple_branches);
            BDSExpr *new_expr = match_transform_match(new_match, new_cont);

            if(remain_branches->length > 0){
                new_cont->u.u_cont.body = match_transform_match(
                        bd_sexpr_match(target, remain_branches), cont);
            }
            else{
                new_cont->u.u_cont.body = cont;
            }

            return bd_sexpr_lettuple(new_idents, target, new_expr);
        }
    }
    // TODO Variant
    else if(head_branch_pattern->kind == E_PATTERNVAR){
        // transform match-expr to let-expr
        // ex1)
        //  match a with
        //   _ -> e
        //
        //  --->
        //  e
        //
        // ex2)
        //  match a with
        //   x -> ..
        //
        //  --->
        //  let x = a in ..
        BDExprIdent *ident = head_branch_pattern->u.u_patvar.ident;
        if(is_trash_name(ident->name)){
            return match_transform(head_branch_body);
        }
        else{
            return bd_sexpr_let(ident, target,
                    match_transform(head_branch_body));
        }
    }
    else{
        int has_var = 0;
        BDSExprMatchBranch *tmp_branch;
        for(i = 0; i < branches->length; i++){
            tmp_branch = vector_get(branches, i);
            if(tmp_branch->pattern->kind == E_PATTERNVAR){
                has_var = 1;
            }
            tmp_branch->body = match_transform(tmp_branch->body);
        }

        if( ! has_var){
            // adding as cont when all-match failed
            vector_add(branches,
                    bd_sexpr_match_branch(bd_sexpr_pattern_var(bd_expr_ident_typevar("_")), cont));
        }

        return e;
    }
}

BDSExpr *match_transform(BDSExpr *e)
{
    if(e == NULL){
        return e;
    }

    switch(e->kind){
        case E_UNIOP:
            e->u.u_uniop.val = match_transform(e->u.u_uniop.val);
            break;
        case E_BINOP:
            e->u.u_binop.l = match_transform(e->u.u_binop.l);
            e->u.u_binop.r = match_transform(e->u.u_binop.r);
            break;
        case E_IF:
            e->u.u_if.pred = match_transform(e->u.u_if.pred);
            e->u.u_if.t = match_transform(e->u.u_if.t);
            e->u.u_if.f = match_transform(e->u.u_if.f);
            break;
        case E_LET:
            e->u.u_let.val = match_transform(e->u.u_let.val);
            e->u.u_let.body = match_transform(e->u.u_let.body);
            break;
        case E_LETREC:
            e->u.u_letrec.fun = match_transform(e->u.u_letrec.fun);
            e->u.u_letrec.body = match_transform(e->u.u_letrec.body);
            break;
        case E_FUN:
            e->u.u_fun.body = match_transform(e->u.u_fun.body);
            break;
        case E_APP:
            {
                e->u.u_app.fun = match_transform(e->u.u_app.fun);

                Vector *actuals = e->u.u_app.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, match_transform(vector_get(actuals, i)));
                }
            }
            break;
        case E_CCALL:
            {
                Vector *actuals = e->u.u_ccall.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, match_transform(vector_get(actuals, i)));
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;

                for(i = 0; i < elems->length; i++){
                    vector_set(elems, i, match_transform(vector_get(elems, i)));
                }
            }
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.val = match_transform(e->u.u_lettuple.val);
            e->u.u_lettuple.body = match_transform(e->u.u_lettuple.body);
            break;
        case E_MATCH:
            return match_transform_match(e, bd_sexpr_error("Non-exhaustive patterns in match"));
        case E_CONT:
            e->u.u_cont.body = match_transform(e->u.u_cont.body);
            break;
        default:
            break;
    }

    return e;
}

BDSExpr *match_expand(BDSExpr *e)
{
    switch(e->kind){
        case E_UNIOP:
            e->u.u_uniop.val = match_expand(e->u.u_uniop.val);
            break;
        case E_BINOP:
            e->u.u_binop.l = match_expand(e->u.u_binop.l);
            e->u.u_binop.r = match_expand(e->u.u_binop.r);
            break;
        case E_IF:
            e->u.u_if.pred = match_expand(e->u.u_if.pred);
            e->u.u_if.t = match_expand(e->u.u_if.t);
            e->u.u_if.f = match_expand(e->u.u_if.f);
            break;
        case E_LET:
            e->u.u_let.val = match_expand(e->u.u_let.val);
            e->u.u_let.body = match_expand(e->u.u_let.body);
            break;
        case E_LETREC:
            e->u.u_letrec.fun = match_expand(e->u.u_letrec.fun);
            e->u.u_letrec.body = match_expand(e->u.u_letrec.body);
            break;
        case E_FUN:
            e->u.u_fun.body = match_expand(e->u.u_fun.body);
            break;
        case E_APP:
            {
                e->u.u_app.fun = match_expand(e->u.u_app.fun);

                Vector *actuals = e->u.u_app.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, match_expand(vector_get(actuals, i)));
                }
            }
            break;
        case E_CCALL:
            {
                Vector *actuals = e->u.u_ccall.actuals;
                int i;
                for(i = 0; i < actuals->length; i++){
                    vector_set(actuals, i, match_expand(vector_get(actuals, i)));
                }
            }
            break;
        case E_TUPLE:
            {
                Vector *elems = e->u.u_tuple.elems;
                int i;

                for(i = 0; i < elems->length; i++){
                    vector_set(elems, i, match_expand(vector_get(elems, i)));
                }
            }
            break;
        case E_LETTUPLE:
            e->u.u_lettuple.val = match_expand(e->u.u_lettuple.val);
            e->u.u_lettuple.body = match_expand(e->u.u_lettuple.body);
            break;
        case E_MATCH:
            {
                BDSExpr *target = e->u.u_match.target;
                Vector *branches = e->u.u_match.branches;
                int i;

                BDSExpr *new_expr = NULL;
                BDSExpr *if_expr = NULL;
                BDSExpr *transformed_expr = NULL;

                BDSExprMatchBranch *branch;
                BDExprIdent *ident;
                for(i = 0; i < branches->length; i++){
                    branch = vector_get(branches, i);
                    branch->body = match_expand(branch->body);

                    if(branch->pattern->kind == E_PATTERNVAR){
                        ident = branch->pattern->u.u_patvar.ident;
                        if(is_trash_name(ident->name)){
                            transformed_expr = branch->body;
                        }
                        else{
                            transformed_expr = bd_sexpr_let(ident, target, branch->body);
                        }
                    }
                    else{
                        transformed_expr = bd_sexpr_if(
                                bd_sexpr_binop(OP_EQ, target, branch->pattern), branch->body, NULL);
                    }

                    if(i == 0){
                        if_expr = transformed_expr;
                        new_expr = transformed_expr;
                    }
                    else{
                        if_expr->u.u_if.f = transformed_expr;
                        if_expr = transformed_expr;
                    }
                }

                return new_expr;
            }
            break;
        case E_CONT:
            return match_expand(e->u.u_cont.body);
        default:
            break;
    }
    return e;
}

BDSProgram *bd_pattern_transform(BDSProgram *prog)
{
    int i;
    BDSExprDef *def;

    Vector *defs = prog->defs;
    for(i = 0; i < defs->length; i++){
        def = vector_get(defs, i);
        def->body = match_expand(match_transform(def->body));
    }

    def = prog->maindef;
    def->body = match_expand(match_transform(def->body));

    return prog;
}
