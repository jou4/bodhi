%{

#include <stdio.h>
#include "parser.h"
#include "util.h"

#define YYDEBUG 1
#define YYLEX_PARAM lexer->scanner, lexer

#define add_fundef(def) vector_add(ps->prog.fundefs, def)
#define add_datadef(def) vector_add(ps->prog.datadefs, def)
#define set_maindef(def) ps->prog.maindef = def

%}

%code requires {
#include "lexer.h"
#include "compile.h"
}

%define api.pure
%error-verbose

%parse-param { Parser *ps }
%parse-param { Lexer *lexer }
%parse-param { char *stream_name }

%union {
  int i_val;
  double d_val;
  char c_val;
  char *s_val;
  StringBuffer* buf;
  BDSExpr* t;
  BDSExprFundef *fundef;
  BDExprIdent *ident;
  Vector *vec;
}

%token <s_val> IDENT
%token <i_val> BOOL_LIT
%token <i_val> INT_LIT
%token <d_val> FLOAT_LIT
%token <c_val> CHAR_LIT
%token <buf> STR_LIT
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
%token COLON
%token DOT
%token LESS_MINUS
%token SEMICOLON
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token LBRACKET_BAR
%token RBRACKET_BAR
%token LBRACE
%token RBRACE
%token FUN
%token ARROW
%token MAIN
%token DEF

%right prec_let
%right SEMICOLON
%right COLON
%right prec_if
%right LESS_MINUS
%left  COMMA
%left  EQUAL EQUAL_EQUAL LESS_GREATER LESS GREATER LESS_EQUAL GREATER_EQUAL
%left  PLUS MINUS PLUS_DOT MINUS_DOT
%left  AST SLASH
%left  prec_unary_minus
%left  prec_lambda
%left  prec_app
%left  DOT


%type <fundef> fundef
%type <vec> formal_args actual_args t_elems pat
%type <ident> formal_arg
%type <t> exp simple_exp uniop_exp binop_exp l_elems
%start input

%%

input
: /* nothing */
| input toplevel
| error
    {
        YYABORT;
    }
;

toplevel
: DEF IDENT EQUAL exp
    { add_datadef(bd_sexpr_fundef(bd_expr_ident_typevar($2), NULL, $4)); }
| DEF IDENT formal_args EQUAL exp
    { add_fundef(bd_sexpr_fundef(bd_expr_ident_typevar($2), $3, $5)); }
| DEF MAIN EQUAL exp
    { set_maindef(bd_sexpr_fundef(bd_expr_ident("main", bd_type_unit()), NULL, $4)); }
;

simple_exp
: LPAREN exp RPAREN
    { $$ = $2; }
| LPAREN t_elems RPAREN
    { $$ = bd_sexpr_tuple($2); }
| LPAREN RPAREN
    { $$ = bd_sexpr_unit(); }
| LBRACKET l_elems RBRACKET
    { $$ = $2; }
| LBRACKET RBRACKET
    { $$ = bd_sexpr_nil(); }
| IDENT
    { $$ = bd_sexpr_var($1); }
| BOOL_LIT
    { $$ = bd_sexpr_bool($1); }
| INT_LIT
    { $$ = bd_sexpr_int($1); }
| FLOAT_LIT
    { $$ = bd_sexpr_float($1); }
| CHAR_LIT
    { $$ = bd_sexpr_char($1); }
| STR_LIT
    { $$ = bd_sexpr_str(sb_to_string($1)); }
;

exp
: simple_exp
| uniop_exp
| binop_exp
| IF exp THEN exp ELSE exp
    %prec prec_if
    { $$ = bd_sexpr_if($2, $4, $6); }
| LET IDENT EQUAL exp IN exp
    %prec prec_let
    { $$ = bd_sexpr_let(bd_expr_ident_typevar($2), $4, $6); }
| LET REC fundef IN exp
    %prec prec_let
    { $$ = bd_sexpr_letrec($3, $5); }
| simple_exp actual_args
    %prec prec_app
    { $$ = bd_sexpr_app($1, $2); }
| LET LPAREN pat RPAREN EQUAL exp IN exp
    %prec prec_let
    { $$ = bd_sexpr_lettuple($3, $6, $8); }
/*
| FUN IDENT formal_args ARROW exp
    %prec prec_lambda
    { $$ = NULL; }
*/
;

uniop_exp
: NOT exp
    %prec prec_app
    { $$ = bd_sexpr_uniop(OP_NOT, $2); }
| MINUS simple_exp
    %prec prec_unary_minus
    { $$ = bd_sexpr_uniop(OP_NEG, $2); }
;

binop_exp
: exp PLUS exp
    { $$ = bd_sexpr_binop(OP_ADD, $1, $3); }
| exp MINUS exp
    { $$ = bd_sexpr_binop(OP_SUB, $1, $3); }
| exp AST exp
    { $$ = bd_sexpr_binop(OP_MUL, $1, $3); }
| exp SLASH exp
    { $$ = bd_sexpr_binop(OP_DIV, $1, $3); }
| exp EQUAL_EQUAL exp
    { $$ = bd_sexpr_binop(OP_EQ, $1, $3); }
| exp LESS_GREATER exp
    { $$ = bd_sexpr_uniop(OP_NOT, bd_sexpr_binop(OP_EQ, $1, $3)); }
| exp LESS exp
    { $$ = bd_sexpr_uniop(OP_NOT, bd_sexpr_binop(OP_LE, $3, $1)); }
| exp GREATER exp
    { $$ = bd_sexpr_uniop(OP_NOT, bd_sexpr_binop(OP_LE, $1, $3)); }
| exp LESS_EQUAL exp
    { $$ = bd_sexpr_binop(OP_LE, $1, $3); }
| exp GREATER_EQUAL exp
    { $$ = bd_sexpr_binop(OP_LE, $3, $1); }
| exp COLON exp
    { $$ = bd_sexpr_binop(OP_CONS, $1, $3); }
;


fundef
: IDENT formal_args EQUAL exp
    { $$ = bd_sexpr_fundef(bd_expr_ident_typevar($1), $2, $4); }
;

formal_args
: formal_arg
    { Vector *vec = vector_new(); vector_add(vec, $1); $$ = vec; }
| formal_args formal_arg
    { vector_add($1, $2); $$ = $1; }
;

formal_arg
: IDENT
    { $$ = bd_expr_ident_typevar($1); }
| LPAREN RPAREN
    { $$ = bd_expr_ident("_", bd_type_unit()); }
;

actual_args
: simple_exp
    %prec prec_app
    { Vector *vec = vector_new(); vector_add(vec, $1); $$ = vec; }
| actual_args simple_exp
    %prec prec_app
    { vector_add($1, $2); $$ = $1; }
;

t_elems
: t_elems COMMA exp
    { vector_add($1, $3); $$ = $1; }
| exp COMMA exp
    { Vector *vec = vector_new(); vector_add(vec, $1); vector_add(vec, $3); $$ = vec; }
;

l_elems
: exp COMMA l_elems
    { $$ = bd_sexpr_binop(OP_CONS, $1, $3); }
| exp
    { $$ = bd_sexpr_binop(OP_CONS, $1, bd_sexpr_nil()); }
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
    bd_sprogram_init(&ps->prog);
}

void parser_destroy(Parser *ps){
    // TODO
}
