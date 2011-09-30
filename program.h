#ifndef _program_h_
#define _program_h_

#include "util/vector.h"
#include "expr3.h"
#include "bytecode.h"
#include "show.h"

typedef struct {
    Vector *fundefs;
    BDExpr3 *expr;
} BDProgram1;

typedef struct {
    Vector *fundefs;    // Vector<Vector<BDByteCode>>
    Vector *codes;      // Vector<BDByteCode>
} BDProgram2;


BDProgram1 *bd_program1(Vector *fundefs, BDExpr3 *expr);
void bd_program1_show(BDProgram1 *prog);

BDProgram2 *bd_program2(Vector *fundefs, Vector *codes);
void bd_program2_show(BDProgram2 *prog);

#endif
