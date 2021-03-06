#ifndef _compile_h_
#define _compile_h_

#define OPTIMIZE_COUNT 10

#include <stdlib.h>
#include <stdio.h>

#include "config.h"
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

int compile(FILE *ch, BDSProgram *prog);
BDSProgram *bd_sanitize(BDSProgram *prog);
BDSProgram *bd_typing(BDSProgram *prog);
BDSProgram *bd_pattern_transform(BDSProgram *prog);
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
BDAProgram *bd_regalloc(BDAProgram *prog);
void bd_emit(FILE *ch, BDAProgram *prog);

// Error
#define ERROR 101
#define ERROR_INFER 102
#define ERROR_UNIFY 103
#define ERROR_INVALID_ARGUMENT 104

#endif
