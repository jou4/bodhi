#include "program.h"

void bd_sprogram_init(BDSProgram *prog)
{
    prog->fundefs = vector_new();
    prog->datadefs = vector_new();
    prog->maindef = NULL;
}

void bd_sprogram_fundef_show(BDSExprFundef *fundef)
{
    int i;

    bd_expr_ident_show(fundef->ident);
    for(i = 0; i < fundef->formals->length; i++){
        printf(" ");
        printf("(");
        bd_expr_ident_show(vector_get(fundef->formals, i));
        printf(")");
    }
    printf(" = ");
    bd_sexpr_show(fundef->body);
}

void bd_sprogram_show(BDSProgram *prog)
{
    Vector *vec;
    int i;

    printf("--- Parsed ---\n");

    vec = prog->datadefs;
    for(i = 0; i < vec->length; i++){
        bd_sprogram_fundef_show(vector_get(vec, i));
    }
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        bd_sprogram_fundef_show(vector_get(vec, i));
    }
    bd_sprogram_fundef_show(prog->maindef);

    printf("\n");
}
