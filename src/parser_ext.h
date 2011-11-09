#ifndef _parser_h_
#define _parser_h_

#include "parser.h"

typedef struct {
    BDSProgram prog;
} Parser;

void parser_init(Parser *ps);
void parser_destroy(Parser *ps);

#endif
