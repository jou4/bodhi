#include <regex.h>
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

char *outfile(char *infile)
{
    regex_t ex;
    size_t nmatch = 3;
    regmatch_t pmatch[nmatch];
    char *filename = malloc(sizeof(char) * 100);
    int i, j;

    if(regcomp(&ex, "([[:alpha:]/]+)", REG_EXTENDED) != 0){
        printf("regex compile failed.\n");
        exit(1);
    }

    if(regexec(&ex, infile, nmatch, pmatch, REG_NOTBOL|REG_NOTEOL)){
        // no match
        return "tmp.s";
    }
    else{
        for(i = pmatch[0].rm_so, j = 0; i < pmatch[0].rm_eo; i++, j++){
            filename[j] = infile[i];
        }
        filename[j++] = '.';
        filename[j++] = 's';
        filename[j++] = '\0';
        printf("%s\n", filename);
        return filename;
    }
}

int main(int argc, char **argv)
{
    primsigs = primitives();

    Parser ps;
    Lexer lexer;
    char *filepath;
    FILE *in, *out;

    if(argc > 1){
        filepath = argv[1];
    }
    else{
        filepath = "test/sample.bd";
    }

    in = fopen(filepath, "r");
    if(in == NULL){
        printf("Can not open the file.\n");
        return 1;
    }

    out = fopen(outfile(filepath), "w");
    //out = stdout;

    parser_init(&ps);
    lexer_init(&lexer);
    lexer_setin(&lexer, in);

//yydebug = 1;
    if(yyparse(&ps, &lexer, "input") == 0){
        compile(out, &ps.prog);
    }

    fclose(in);

    return 0;
}
