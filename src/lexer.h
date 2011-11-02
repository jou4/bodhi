#ifndef _lexer_h_
#define _lexer_h_

#include "util.h"

typedef struct {
    StringBuffer *buf;
    void *scanner;
} Lexer;

void lexer_init(Lexer *lexer);
void lexer_setin(Lexer *lexer, FILE *in);
void lexer_destroy(Lexer *lexer);

#endif
