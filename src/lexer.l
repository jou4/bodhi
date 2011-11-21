%{
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

#define YY_DECL int yylex \
               (YYSTYPE * yylval_param, yyscan_t yyscanner, Lexer *lexer)

%}

%option reentrant
%option bison-bridge
%option noyywrap
%option yylineno

br    [\n\r]
space [ \t]
digit [0-9]
lower [a-z]
upper [A-Z]


%start STRLIT
%start COMMENT

%%

<INITIAL>{space}    { }
<INITIAL>{br}       { }
<INITIAL>"("        { return(LPAREN); }
<INITIAL>")"        { return(RPAREN); }
<INITIAL>"["        { return(LBRACKET); }
<INITIAL>"]"        { return(RBRACKET); }
<INITIAL>"{"        { return(LBRACE); }
<INITIAL>"}"        { return(RBRACE); }
<INITIAL>":"        { return(COLON); }
<INITIAL>"True"     { yylval->i_val = 1; return(BOOL_LIT); }
<INITIAL>"False"    { yylval->i_val = 0; return(BOOL_LIT); }
<INITIAL>"not"      { return(NOT); }
<INITIAL>"main"     { return(MAIN); }
<INITIAL>"def"      { return(DEF); }
<INITIAL>"ccall"    { return(CCALL); }
<INITIAL>"match"    { return(MATCH); }
<INITIAL>"with"     { return(WITH); }
<INITIAL>"|"        { return(VERTICAL_BAR); }
<INITIAL>"\\"       { return(FUN); }
<INITIAL>"->"       { return(ARROW); }

<INITIAL>"+"        { return(PLUS); }
<INITIAL>"-"        { return(MINUS); }
<INITIAL>"*"        { return(AST); }
<INITIAL>"/"        { return(SLASH); }
<INITIAL>"%"        { return(PERCENT); }
<INITIAL>"+."       { return(PLUS_DOT); }
<INITIAL>"-."       { return(MINUS_DOT); }
<INITIAL>"*."       { return(AST_DOT); }
<INITIAL>"/."       { return(SLASH_DOT); }
<INITIAL>"="        { return(EQUAL); }
<INITIAL>"=="       { return(EQUAL_EQUAL); }
<INITIAL>"<>"       { return(LESS_GREATER); }
<INITIAL>"<="       { return(LESS_EQUAL); }
<INITIAL>">="       { return(GREATER_EQUAL); }
<INITIAL>"<"        { return(LESS); }
<INITIAL>">"        { return(GREATER); }
<INITIAL>"if"       { return(IF); }
<INITIAL>"then"     { return(THEN); }
<INITIAL>"else"     { return(ELSE); }
<INITIAL>"let"      { return(LET); }
<INITIAL>"in"       { return(IN); }
<INITIAL>"rec"      { return(REC); }
<INITIAL>","        { return(COMMA); }
<INITIAL>";"        { return(SEMICOLON); }

<INITIAL>'\\n' {
    yylval->c_val = '\n';
    return(CHAR_LIT);
}
<INITIAL>'(\\.|[^\\'])' {
    yylval->c_val = yytext[1];
    return(CHAR_LIT);
}

<INITIAL>{digit}+\.{digit}+ {
    yylval->d_val = atof(yytext);
    return(FLOAT_LIT);
}
<INITIAL>{digit}+ {
    yylval->i_val = atol(yytext);
    return(INT_LIT);
}
<INITIAL>[A-Za-z_][A-Za-z_0-9]* {
    sb_clear(lexer->buf);
    sb_append_n(lexer->buf, yytext, yyleng);
    yylval->s_val = sb_to_string(lexer->buf);

    return(IDENT);
}

 /* String literal */
<INITIAL>"\"" {
    BEGIN(STRLIT);
    sb_clear(lexer->buf);
}
<STRLIT>"\"" {
    BEGIN(INITIAL);
    yylval->buf = lexer->buf;
    return(STR_LIT);
}
<STRLIT>"\\\\" { sb_append_n(lexer->buf, "\\", 1); }
<STRLIT>"\\a"  { sb_append_n(lexer->buf, "\a", 1); }
<STRLIT>"\\b"  { sb_append_n(lexer->buf, "\b", 1); }
<STRLIT>"\\f"  { sb_append_n(lexer->buf, "\f", 1); }
<STRLIT>"\\n"  { sb_append_n(lexer->buf, "\n", 1); }
<STRLIT>"\\r"  { sb_append_n(lexer->buf, "\r", 1); }
<STRLIT>"\\t"  { sb_append_n(lexer->buf, "\t", 1); }
<STRLIT>"\\v"  { sb_append_n(lexer->buf, "\v", 1); }
<STRLIT>{br}   { YY_FATAL_ERROR("unexpected newline in string literal."); BEGIN(INITIAL); }
<STRLIT>.      { sb_append_n(lexer->buf, yytext, yyleng); }

 /* Comment */
<INITIAL>--[^\n]+ { }
<INITIAL>"{-"       { lexer->comment_depth++; BEGIN(COMMENT); }
<COMMENT>"{-"       { lexer->comment_depth++; }
<COMMENT>"-}"       { lexer->comment_depth--; if(lexer->comment_depth == 0){ BEGIN(INITIAL); } }
<COMMENT><<EOF>>    { YY_FATAL_ERROR("unexpected end of input."); }
<COMMENT>{br}       { }
<COMMENT>.          { }


<<EOF>> { return(EOF); }

%%

void lexer_init(Lexer *lexer)
{
    lexer->buf = sb_new();
    lexer->comment_depth = 0;
    yylex_init(&lexer->scanner);
}

void lexer_setin(Lexer *lexer, FILE *in)
{
    yyset_in(in, lexer->scanner);
}

void lexer_destroy(Lexer *lexer)
{
    sb_destroy(lexer->buf);
    yylex_destroy(lexer->scanner);
    free(lexer);
}
