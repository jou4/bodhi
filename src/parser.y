%{

#include <stdio.h>
#include "parser_ext.h"
#include "util.h"

//#define YYDEBUG 1
#define YYLEX_PARAM lexer->scanner, lexer

#define add_def(def) vector_add(ps->prog.defs, def)
#define set_maindef(def) ps->prog.maindef = def

extern int yylex();
extern int yyerror();
extern int yyget_lineno();

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
  long i_val;
  double d_val;
  char c_val;
  char *s_val;
  StringBuffer* buf;
  BDSExpr* t;
  BDExprIdent *ident;
  BDSExprMatchBranch *branch;
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
%token MINUS_DOT
%token PLUS_DOT
%token AST_DOT
%token SLASH_DOT
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
%token CCALL
%token MATCH
%token WITH
%token VERTICAL_BAR

%right prec_match
%right prec_match_branch
%right prec_let
%right prec_lambda
%right prec_if
%right COLON
%right LESS_MINUS
%left  COMMA
%left  EQUAL EQUAL_EQUAL LESS_GREATER LESS GREATER LESS_EQUAL GREATER_EQUAL
%left  PLUS MINUS PLUS_DOT MINUS_DOT
%left  AST SLASH AST_DOT SLASH_DOT
%left  prec_unary_minus
%left  prec_app

%type <vec> formal_args actual_args tuple_elems lettuple_pattern match_branches pattern_tuple_elems
%type <ident> formal_arg
%type <branch> match_branch
%type <t> exp simple_exp uniop_exp binop_exp list_elems exp_seq match_pattern
%start input

%%

input
: /* nothing */
| input toplevel
| error { YYABORT; }
;

toplevel
: DEF IDENT EQUAL exp
    { add_def(bd_sexpr_def(bd_expr_ident_typevar($2), $4)); }
| DEF IDENT formal_args EQUAL exp
    { add_def(bd_sexpr_def(bd_expr_ident_typevar($2), bd_sexpr_fun(bd_type_var(NULL), $3, $5))); }
| DEF MAIN EQUAL exp
    { set_maindef(bd_sexpr_def(
		bd_expr_ident("main", bd_type_unit()), bd_sexpr_fun(bd_type_var(NULL), vector_new(), $4))); }
| DEF MAIN formal_args EQUAL exp
    { set_maindef(bd_sexpr_def(
		bd_expr_ident("main", bd_type_unit()), bd_sexpr_fun(bd_type_var(NULL), $3, $5))); }
;

simple_exp
: LPAREN exp RPAREN
    { $$ = $2; }
| LPAREN tuple_elems RPAREN
    { $$ = bd_sexpr_tuple($2); }
| LPAREN RPAREN
    { $$ = bd_sexpr_unit(); }
| LBRACKET list_elems RBRACKET
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
| LET LPAREN IDENT RPAREN EQUAL exp IN exp
    %prec prec_let
    { $$ = bd_sexpr_let(bd_expr_ident_typevar($3), $6, $8); }
| LET REC IDENT formal_args EQUAL exp IN exp
    %prec prec_let
    { $$ = bd_sexpr_letrec(bd_expr_ident_typevar($3), bd_sexpr_fun(bd_type_var(NULL), $4, $6), $8); }
| LET LPAREN lettuple_pattern RPAREN EQUAL exp IN exp
    %prec prec_let
    { $$ = bd_sexpr_lettuple($3, $6, $8); }
| simple_exp actual_args
    %prec prec_app
    { $$ = bd_sexpr_app($1, $2); }
| CCALL IDENT actual_args
    %prec prec_app
    { $$ = bd_sexpr_ccall($2, $3); }
| FUN formal_args ARROW exp
    %prec prec_lambda
    { $$ = bd_sexpr_fun(bd_type_var(NULL), $2, $4); }
| MATCH exp WITH match_branches
    %prec prec_match
    { $$ = bd_sexpr_match($2, $4); }
| LBRACE exp_seq RBRACE
    { $$ = $2; }
;

uniop_exp
: NOT exp
    %prec prec_app
    { $$ = bd_sexpr_uniop(OP_NOT, $2); }
| MINUS simple_exp
    %prec prec_unary_minus
    { $$ = bd_sexpr_uniop(OP_NEG, $2); }
| MINUS_DOT simple_exp
    %prec prec_unary_minus
    { $$ = bd_sexpr_uniop(OP_FNEG, $2); }
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
| exp PLUS_DOT exp
    { $$ = bd_sexpr_binop(OP_FADD, $1, $3); }
| exp MINUS_DOT exp
    { $$ = bd_sexpr_binop(OP_FSUB, $1, $3); }
| exp AST_DOT exp
    { $$ = bd_sexpr_binop(OP_FMUL, $1, $3); }
| exp SLASH_DOT exp
    { $$ = bd_sexpr_binop(OP_FDIV, $1, $3); }
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

tuple_elems
: tuple_elems COMMA exp
    { vector_add($1, $3); $$ = $1; }
| exp COMMA exp
    { Vector *vec = vector_new(); vector_add(vec, $1); vector_add(vec, $3); $$ = vec; }
;

list_elems
: exp COMMA list_elems
    { $$ = bd_sexpr_binop(OP_CONS, $1, $3); }
| exp
    { $$ = bd_sexpr_binop(OP_CONS, $1, bd_sexpr_nil()); }
;

lettuple_pattern
: lettuple_pattern COMMA IDENT
    { vector_add($1, bd_expr_ident_typevar($3)); $$ = $1; }
| IDENT COMMA IDENT
    {
        Vector *vec = vector_new();
        vector_add(vec, bd_expr_ident_typevar($1));
        vector_add(vec, bd_expr_ident_typevar($3));
        $$ = vec;
    }
;

exp_seq
: exp
    { $$ = $1; }
| exp SEMICOLON
    { $$ = $1; }
| exp SEMICOLON exp_seq
    { $$ = bd_sexpr_let(bd_expr_ident("_", bd_type_unit()), $1, $3); }
;

match_branches
: VERTICAL_BAR match_branch
    { Vector *vec = vector_new(); vector_add(vec, $2); $$ = vec; }
| match_branches VERTICAL_BAR match_branch
    { vector_add($1, $3); $$ = $1; }
;

match_branch
: match_pattern ARROW exp
    %prec prec_match_branch
    { $$ = bd_sexpr_match_branch($1, $3); }
;

match_pattern
: IDENT
    { $$ = bd_sexpr_pattern_var(bd_expr_ident_typevar($1)); }
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
| LPAREN pattern_tuple_elems RPAREN
    { $$ = bd_sexpr_tuple($2); }
| LPAREN match_pattern RPAREN
    { $$ = $2; }
;

pattern_tuple_elems
: pattern_tuple_elems COMMA match_pattern
    { vector_add($1, $3); $$ = $1; }
| match_pattern COMMA match_pattern
    { Vector *vec = vector_new(); vector_add(vec, $1); vector_add(vec, $3); $$ = vec; }
;


%%

int yyerror(Parser *ps, Lexer *lexer, const char *stream_name, const char *msg)
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
