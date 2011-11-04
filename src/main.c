#include "lexer.h"
#include "parser.h"
#include "primitives.h"

Vector *primsigs;

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
    primsigs = primitives();

    Parser ps;
    Lexer lexer;
    FILE *in;

    if(argc > 1){
        in = fopen(argv[1], "r");
    }
    else{
        in = fopen("test/sample.txt", "r");
    }

    if(in == NULL){
        printf("Can not open the file.\n");
        return 1;
    }

    parser_init(&ps);
    lexer_init(&lexer);
    lexer_setin(&lexer, in);

//yydebug = 1;
    if(yyparse(&ps, &lexer, "input") == 0){
        compile(&ps.prog);
    }

    fclose(in);

    return 0;
}
