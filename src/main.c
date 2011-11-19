#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <regex.h>
#include "config.h"
#include "lexer.h"
#include "parser_ext.h"
#include "primitives.h"


Vector *primsigs;

extern int yyparse();

char *input = NULL;
char *output = NULL;
char *compiled_file = NULL;
int proc_compile = 1;
int proc_assemble = 1;
size_t heap_size = 64000000;

char *usage = "usage: bodhi [-Sv] [-o output] [-m heap memory size] [-L library directory] input";
char *version = "0.0.1";

char *libdir = NULL;
char *LIBS = "-lbdcore -lbdutil -lm";


char *basename(char *path)
{
    int i = 0, len = strlen(path);
    char *result = malloc(sizeof(char) * len);
    char *tmp, *begin = NULL, *end = NULL;

    for(tmp = path; (tmp = strchr(tmp, '/')) != NULL; begin = ++tmp){ }
    for(tmp = path; (tmp = strchr(tmp, '.')) != NULL; end = tmp, tmp++){ }

    if(begin == NULL){ begin = path; }
    if(end != NULL){ len = end - begin; }

    tmp = result;
    while(i < len){
        tmp[i++] = *begin;
        begin++;
    }

    return result;
}

void getopts(int argc, char **argv)
{
    int opt;

    while((opt = getopt(argc, argv, "Svo:m:L:")) >= 0){
        switch(opt){
            case 'S':
                // compile only
                proc_assemble = 0;
                break;
            case 'o':
                output = optarg;
                break;
			case 'm':
				heap_size = atol(optarg);
				break;
			case 'L':
				libdir = optarg;
				break;
            case 'v':
                fprintf(stdout, "%s\n", version);
                exit(EXIT_SUCCESS);
                break;
            case ':':
            case '?':
			default:
                fprintf(stderr, "%s\n", usage);
                exit(EXIT_FAILURE);
				break;
        }
    }

    while(optind < argc){
        input = argv[optind++];
        break;
    }

    if(input == NULL){
        fprintf(stderr, "%s\n", usage);
        exit(EXIT_FAILURE);
    }

    if(libdir == NULL){
        libdir = LIB_DIR;
    }

    char *bname = basename(input);
    char tmpstr[L_tmpnam];
    tmpnam(tmpstr);

    compiled_file = malloc(sizeof(char) * 100);
    sprintf(compiled_file, "%s.s", tmpstr);

    if(output == NULL){
        if(proc_assemble){
#ifdef DEBUG
            // Output assembly file to current directory.
            sprintf(compiled_file, "%s.s", bname);
#endif
            output = "a.out";
        }
        else{
            sprintf(compiled_file, "%s.s", bname);
            output = compiled_file;
        }
    }
    else{
        if( ! proc_assemble){
            sprintf(compiled_file, "%s", output);
        }
    }
}

int main(int argc, char **argv)
{
    primsigs = primitives();

    Parser ps;
    Lexer lexer;
    FILE *ic, *oc;
    int err = 0;

    getopts(argc, argv);

    ic = fopen(input, "r");
    if(ic == NULL){
        printf("Can not open the file.\n");
        return 1;
    }

    parser_init(&ps);
    lexer_init(&lexer);
    lexer_setin(&lexer, ic);

    err |= yyparse(&ps, &lexer, "input");
    fclose(ic);

    if(proc_compile && err == 0){
        oc = fopen(compiled_file, "w");
        err |= compile(oc, &ps.prog);
        fclose(oc);
    }

    if(proc_assemble && err == 0){
        char cmd[100];
        sprintf(cmd, "gcc -o %s %s -L%s %s", output, compiled_file, libdir, LIBS);
        system(cmd);
    }

    return 0;
}
