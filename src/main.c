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
    FILE *in;

    if(argc > 1){
        in = fopen(argv[1], "r");
    }
    else{
        in = fopen("test/sample.txt", "r");
    }

    parser_init(&ps);
    lexer_init(&lexer);
    lexer_setin(&lexer, in);

//yydebug = 1;
    yyparse(&ps, &lexer, "input");

    compile(&ps.prog);

    fclose(in);

    return 0;
}
