#ifndef _compile_h_
#define _compile_h_

#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "type.h"
#include "id.h"
#include "env.h"
#include "primitives.h"
#include "program.h"
#include "expr.h"
#include "sexpr.h"
/*
_include "expr2.h"
_include "expr3.h"
_include "asm.h"
*/

void compile(BDSProgram *prog);
BDSProgram *bd_typing(BDSProgram *prog);
BDNProgram *bd_knormalize(BDSProgram *prog);
/*
BDExpr2 *bd_alpha_convert(BDExpr2 *e);
BDExpr2 *bd_alpha(Env *env, BDExpr2 *e);
BDExpr2 *bd_beta_reduce(BDExpr2 *e);
BDExpr2 *bd_flatten(BDExpr2 *e);
BDExpr2 *bd_inline_expand(int threashold, BDExpr2 *e);
BDExpr2 *bd_const_fold(BDExpr2 *e);
BDExpr2 *bd_elim(BDExpr2 *e);
BDProgram1 *bd_closure_transform(BDExpr2 *e);
BDAsmProg *bd_virtual(BDProgram1 *prog);
BDAsmProg *bd_register_allocate(BDAsmProg *prog);
void bd_emit(BDAsmProg *prog);
*/

#endif
