#include "config.h"
#include "compile.h"
#include "util.h"

int compile(FILE *ch, BDSProgram *sprog)
{
    BDNProgram *nprog;
    BDCProgram *cprog;
    BDAProgram *aprog;

    int i;

    try{

        if(sprog->maindef == NULL){
            printf("There is no main.\n");
            return 1;
        }

        bd_typing(sprog);

#ifdef DEBUG
        printf("--- Typing ---\n");
        BDSExprDef *def;
        Vector *vec = sprog->defs;
        for(i = 0; i < vec->length; i++){
            def = vector_get(vec, i);
            printf("%s", bd_expr_ident_show(def->ident));
            printf("\n");
        }
        printf("%s", bd_expr_ident_show(sprog->maindef->ident));
        printf("\n\n");
#endif

#ifdef DEBUG
        printf("--- Parsed ---\n");
        bd_sprogram_show(sprog);
#endif

        nprog = bd_knormalize(sprog);
#ifdef DEBUG
        printf("--- K normalized --- \n");
        bd_nprogram_show(nprog);
#endif

        nprog = bd_alpha_convert(nprog);
#ifdef DEBUG
        printf("--- α converted --- \n");
        bd_nprogram_show(nprog);
#endif

        for(i = 0; i < OPTIMIZE_COUNT; i++){

            nprog = bd_beta_reduce(nprog);
#ifdef DEBUG_OPTIMIZE
            printf("--- β reduced --- \n");
            bd_nprogram_show(nprog);
#endif

            nprog = bd_flatten(nprog);
#ifdef DEBUG_OPTIMIZE
            printf("--- Nested let flatten --- \n");
            bd_nprogram_show(nprog);
#endif

            nprog = bd_inline_expand(10, nprog);
#ifdef DEBUG_OPTIMIZE
            printf("--- Inline expanded --- \n");
            bd_nprogram_show(nprog);
#endif

            nprog = bd_const_fold(nprog);
#ifdef DEBUG_OPTIMIZE
            printf("--- Const folded --- \n");
            bd_nprogram_show(nprog);
#endif

            nprog = bd_elim(nprog);
#ifdef DEBUG_OPTIMIZE
            printf("--- Disuse declaration eliminated --- \n");
            bd_nprogram_show(nprog);
#endif

        }

        cprog = bd_closure_transform(nprog);
#ifdef DEBUG
        printf("--- Closure transformed --- \n");
        bd_cprogram_show(cprog);
#endif

        aprog = bd_virtual(cprog);
#ifdef DEBUG
        printf("--- Generated vm code --- \n");
        bd_aprogram_show(aprog);
#endif

        aprog = bd_regalloc(aprog);
#ifdef DEBUG
        printf("--- Allocated registers --- \n");
        bd_aprogram_show(aprog);
#endif

        bd_emit(ch, aprog);

        return 0;
    }

    return 1;
}
