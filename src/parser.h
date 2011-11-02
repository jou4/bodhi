#ifndef _parser_h_
#define _parser_h_

#include "y.tab.h"

typedef struct {
    BDSProgram prog;
} Parser;

void prompt(int level);
void parser_init(Parser *ps);
void parser_destroy(Parser *ps);

#endif
