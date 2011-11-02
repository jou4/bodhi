#ifndef _program_h_
#define _program_h_

#include "util.h"
#include "expr.h"
#include "sexpr.h"

typedef struct {
    Vector *fundefs;
    Vector *datadefs;
    BDSExprFundef *maindef;
} BDSProgram;

void bd_sprogram_init(BDSProgram *prog);
void bd_sprogram_show(BDSProgram *prog);

#endif
