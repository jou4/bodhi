#include <stdlib.h>
#include <stdio.h>
#include "program.h"

BDProgram1 *bd_program1(Vector *fundefs, BDExpr3 *expr)
{
    BDProgram1 *prog = malloc(sizeof(BDProgram1));
    prog->fundefs = fundefs;
    prog->expr = expr;
    return prog;
}

void bd_program1_show(BDProgram1 *prog)
{
    printf("Function declarations\n");

    Vector *fundefs = prog->fundefs;
    int i;

    for(i = 0; i < fundefs->length; i++){
        bd_expr3_fundef_show(vector_get(fundefs, i));
        printf("\n");
    }

    printf("\n");
    printf("Expressions\n");

    bd_expr3_show(prog->expr);
}

BDProgram2 *bd_program2(Vector *fundefs, Vector *codes)
{
    BDProgram2 *prog = malloc(sizeof(BDProgram2));
    prog->fundefs = fundefs;
    prog->codes = codes;
    return prog;
}

void bd_program2_show(BDProgram2 *prog)
{
}

