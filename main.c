#include "lexer.h"
#include "parser.h"

void prompt(int level)
{
    if(level == 0){
        printf("> ");
    }
    else{
        printf("... ");
    }
}

int main(int argc, char **argv)
{
    Parser ps;
    Lexer lexer;

    parser_init(&ps);
    lexer_init(&lexer);

    prompt(0);

//yydebug = 1;
    yyparse(&ps, &lexer, "input");

    return 0;
}
