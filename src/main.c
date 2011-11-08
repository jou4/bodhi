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
    char *in, *out;
    FILE *ic, *oc;
    int comp;

    if(argc > 1){
        in = argv[1];
    }
    else{
        in = "test/sample.bd";
    }

    ic = fopen(in, "r");
    if(ic == NULL){
        printf("Can not open the file.\n");
        return 1;
    }

    parser_init(&ps);
    lexer_init(&lexer);
    lexer_setin(&lexer, ic);

//yydebug = 1;
    if(yyparse(&ps, &lexer, "input") != 0){
        fclose(ic);
        return;
    }
    fclose(ic);

    out = outfile(in);
    oc = fopen(out, "w");
    //oc = stdout;

    comp = compile(oc, &ps.prog);
    fclose(oc);

    if(comp == 0){
        char cmd[100];
        sprintf(cmd, "gcc %s %s", out, "test/core.c");
        system(cmd);
    }

    return 0;
}
