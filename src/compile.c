#include "compile.h"
#include "util.h"

void compile(BDSProgram *prog)
{
    try{
        bd_sprogram_show(prog);
        bd_typing(prog);
        //bd_sprogram_show(prog);

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
