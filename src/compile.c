#include "compile.h"
#include "util.h"

void compile(BDSProgram *sprog)
{
    BDNProgram *nprog;
    BDCProgram *cprog;

    try{

        bd_typing(sprog);

        printf("--- Parsed ---\n");
        bd_sprogram_show(sprog);

        nprog = bd_knormalize(sprog);
        printf("--- K normalized --- \n");
        bd_nprogram_show(nprog);

        nprog = bd_alpha_convert(nprog);
        printf("--- α converted --- \n");
        bd_nprogram_show(nprog);

        nprog = bd_flatten(nprog);
        printf("--- Nested let flatten --- \n");
        bd_nprogram_show(nprog);

        cprog = bd_closure_transform(nprog);
        printf("--- Closure transformed --- \n");
        bd_cprogram_show(cprog);

        /*
        BDSExpr *e1 = bd_typing(e);
        BDExpr2 *e2;
        e2 = bd_knormalize(e1);
        e2 = bd_alpha_convert(e2);

        int i;
        for(i = 0; i < 10; i++){
            e2 = bd_beta_reduce(e2);
            e2 = bd_flatten(e2);
            e2 = bd_inline_expand(0, e2);
            //e2 = bd_const_fold(e2);
            e2 = bd_elim(e2);
        }

        BDProgram1 *prog1 = bd_closure_transform(e2);
        BDAsmProg *prog2 = bd_virtual(prog1);
        BDAsmProg *prog3 = bd_register_allocate(prog2);
        bd_emit(prog3);
        */
    }
}
