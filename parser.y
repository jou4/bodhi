%{

#include <stdio.h>
#include "util/exception.h"
#include "util/vector.h"
#include "compile.h"

#define YYDEBUG 1
#define YYLEX_PARAM lexer->scanner, lexer

typedef struct {
   Vector *exprs;   // Vector<BDExpr1>
} Parser;

void prompt(int level);
void compile(BDExpr1 *e);

%}

%code requires {
#include "lexer.h"
#include "expr1.h"
}

%define api.pure
%error-verbose

%parse-param { Parser *ps }
%parse-param { Lexer *lexer }
%parse-param { char *stream_name }

%union {
  int i_val;
  double d_val;
  char *s_val;
  StringBuffer* buf;
  BDExpr1* t;
  BDExpr1Fundef *fundef;
  BDExprIdent *ident;
  Vector *vec;
}

%token <i_val> BOOL
%token <i_val> INT
%token <d_val> FLOAT
%token <s_val> IDENT
%token <buf> STRING_LITERAL
%token BR
%token NOT
%token MINUS
%token PLUS
%token AST
%token SLASH
%token EQUAL
%token EQUAL_EQUAL
%token LESS_GREATER
%token LESS_EQUAL
%token GREATER_EQUAL
%token LESS
%token GREATER
%token IF
%token THEN
%token ELSE
%token LET
%token IN
%token REC
%token COMMA
%token ARRAY_CREATE
%token DOT
%token LESS_MINUS
%token SEMICOLON
%token LPAREN
%token RPAREN

%right prec_let
%right SEMICOLON
%right prec_if
%right LESS_MINUS
%left  COMMA
%left  EQUAL EQUAL_EQUAL LESS_GREATER LESS GREATER LESS_EQUAL GREATER_EQUAL
%left  PLUS MINUS PLUS_DOT MINUS_DOT
%left  AST SLASH
%right prec_unary_minus
%left  prec_app
%left  DOT


%type <fundef> fundef
%type <vec> formal_args actual_args elems pat
%type <ident> formal_arg
%type <t> exp simple_exp
%start input

%%

input
:
| input line
;

line
: BR
| exp BR
    {
        compile($1);
        prompt(0);
    }
| error
    {
        YYABORT;
    }
;

simple_exp
: LPAREN exp RPAREN     { $$ = $2; }
| LPAREN elems RPAREN   { $$ = bd_expr1_tuple($2); }
| LPAREN RPAREN         { $$ = bd_expr1_unit(); }
| BOOL                  { $$ = bd_expr1_bool($1); }
| INT                   { $$ = bd_expr1_int($1); }
| FLOAT                 { $$ = bd_expr1_float($1); }
| IDENT                 { $$ = bd_expr1_var($1); }
/*| simple_exp DOT LPAREN exp RPAREN
                        { $$ = bd_expr1_get($1, $4); }*/
/*| STRING_LITERAL        { $$ = expr_string_literal(sb_to_string($1), $1->len); }*/
;

exp
: simple_exp            { $$ = $1; }
| NOT exp
    %prec prec_app      { $$ = bd_expr1_uniop(OP_NOT, $2); }
| MINUS exp
    %prec prec_unary_minus
                        { $$ = bd_expr1_uniop(OP_NEG, $2); }
| exp PLUS exp          { $$ = bd_expr1_binop(OP_ADD, $1, $3); }
| exp MINUS exp         { $$ = bd_expr1_binop(OP_SUB, $1, $3); }
| exp AST exp           { $$ = bd_expr1_binop(OP_MUL, $1, $3); }
| exp SLASH exp         { $$ = bd_expr1_binop(OP_DIV, $1, $3); }
| exp EQUAL_EQUAL exp   { $$ = bd_expr1_binop(OP_EQ, $1, $3); }
| exp LESS_GREATER exp  { $$ = bd_expr1_uniop(OP_NOT, bd_expr1_binop(OP_EQ, $1, $3)); }
| exp LESS exp          { $$ = bd_expr1_uniop(OP_NOT, bd_expr1_binop(OP_LE, $3, $1)); }
| exp GREATER exp       { $$ = bd_expr1_uniop(OP_NOT, bd_expr1_binop(OP_LE, $1, $3)); }
| exp LESS_EQUAL exp    { $$ = bd_expr1_binop(OP_LE, $1, $3); }
| exp GREATER_EQUAL exp { $$ = bd_expr1_binop(OP_LE, $3, $1); }
| IF exp THEN exp ELSE exp
    %prec prec_if       { $$ = bd_expr1_if($2, $4, $6); }
| LET IDENT EQUAL exp IN exp
    %prec prec_let      { $$ = bd_expr1_let(bd_expr_ident_typevar($2), $4, $6); }
| LET REC fundef IN exp
    %prec prec_let      { $$ = bd_expr1_letrec($3, $5); }
| simple_exp actual_args
    %prec prec_app      { $$ = bd_expr1_app($1, $2); }
| LET LPAREN pat RPAREN EQUAL exp IN exp
    %prec prec_let      { $$ = bd_expr1_lettuple($3, $6, $8); }
/*
| simple_exp DOT LPAREN exp RPAREN LESS_MINUS exp
                        { $$ = NULL; }
| ARRAY_CREATE simple_exp simple_exp
    %prec prec_app      { $$ = NULL; }
    */
| exp SEMICOLON exp
    {
        $$ = bd_expr1_let(
            bd_expr_ident(bd_generate_id(bd_type_unit()), bd_type_unit()),
            $1,
            $3);
    }
;

fundef
: IDENT formal_args EQUAL exp
    { $$ = bd_expr1_fundef(bd_expr_ident_typevar($1), $2, $4); }
;

formal_args
: formal_arg { Vector *vec = vector_new(); vector_add(vec, $1); $$ = vec; }
| formal_args formal_arg { vector_add($1, $2); $$ = $1; }
;

formal_arg
: IDENT { $$ = bd_expr_ident_typevar($1); }
| LPAREN RPAREN { $$ = bd_expr_ident("_", bd_type_unit()); }
;

actual_args
: simple_exp
    %prec prec_app
    { Vector *vec = vector_new(); vector_add(vec, $1); $$ = vec; }
| actual_args simple_exp
    %prec prec_app
    { vector_add($1, $2); $$ = $1; }
;

elems
: elems COMMA exp
    { vector_add($1, $3); $$ = $1; }
| exp COMMA exp
    { Vector *vec = vector_new(); vector_add(vec, $1); vector_add(vec, $3); $$ = vec; }
;

pat
: pat COMMA IDENT
    { vector_add($1, bd_expr_ident_typevar($3)); $$ = $1; }
| IDENT
    { Vector *vec = vector_new(); vector_add(vec, bd_expr_ident_typevar($1)); $$ = vec; }
;


%%

int yyerror(Parser *ps, Lexer *lexer, char *stream_name, char *msg)
{
    fprintf(stderr, "%s(%d): %s\n", stream_name, yyget_lineno(lexer->scanner), msg);
    return 0;
}

void parser_init(Parser *ps){
    ps->exprs = vector_new();
}

void parser_destroy(Parser *ps){
    // TODO
}

void prompt(int level)
{
    if(level == 0){
        printf("> ");
    }
    else{
        printf("... ");
    }
}

void compile(BDExpr1 *e)
{
    try{
        bd_expr1_show(e);
        BDExpr1 *e1 = bd_typing(e);
        BDExpr2 *e2 = bd_knormalize(e1);
        BDExpr2 *e3 = bd_alpha_convert(e2);
        BDExpr2 *e4 = bd_beta_reduce(e3);
        BDExpr2 *e5 = bd_inline_expand(5, e4);
        BDExpr2 *e6 = bd_const_fold(e5);
        BDExpr2 *e7 = bd_elim(e6);
        BDProgram1 *prog1 = bd_closure_transform(e7);
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
