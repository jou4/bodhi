#include "compile.h"


BDAExpr *regalloc(BDAExpr *e, Set *globals, Set *locals)
{
}

BDAExprDef *regalloc_fundef(BDAExprDef *def)
{
}


extern Vector *primsigs;

BDAProgram *bd_regalloc(BDAProgram *prog)
{
    BDAProgram *aprog = malloc(sizeof(BDAProgram));
    bd_aprogram_init(aprog);
    consts = aprog->consts;

    Set *globals = set_new();
    Set *locals = set_new();
}
