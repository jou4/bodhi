#ifndef _lexer_h_
#define _lexer_h_

#include "util/string_buffer.h"

typedef struct {
    StringBuffer *buf;
    void *scanner;
} Lexer;

void lexer_init(Lexer *lexer);
void lexer_destroy(Lexer *lexer);

#endif
