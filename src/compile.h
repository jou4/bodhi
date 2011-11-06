#ifndef _compile_h_
#define _compile_h_

#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "type.h"
#include "id.h"
#include "env.h"
#include "primitives.h"
#include "expr.h"
#include "sexpr.h"
#include "nexpr.h"
#include "cexpr.h"
#include "asm.h"

void compile(BDSProgram *prog);
BDSProgram *bd_typing(BDSProgram *prog);
BDNProgram *bd_knormalize(BDSProgram *prog);
BDNProgram *bd_alpha_convert(BDNProgram *prog);
BDNExpr *bd_alpha(Env *env, BDNExpr *e);
BDNProgram *bd_beta_reduce(BDNProgram *prog);
BDNProgram *bd_flatten(BDNProgram *prog);
BDNProgram *bd_inline_expand(int threashold, BDNProgram *prog);
BDNProgram *bd_const_fold(BDNProgram *prog);
BDNProgram *bd_elim(BDNProgram *prog);
BDCProgram *bd_closure_transform(BDNProgram *prog);
BDAProgram *bd_virtual(BDCProgram *prog);
BDAProgram *bd_register_allocate(BDAProgram *prog);
void bd_emit(BDAProgram *prog);

#endif
