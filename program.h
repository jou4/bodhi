#ifndef _program_h_
#define _program_h_

#include "util/vector.h"
#include "expr3.h"
#include "bytecode.h"
#include "show.h"

typedef struct {
    Vector *fundefs;
    BDExpr3 *main;
} BDProgram1;

BDProgram1 *bd_program1(Vector *fundefs, BDExpr3 *main);
void bd_program1_show(BDProgram1 *prog);

#endif
