%{
#include <stdio.h>
#include "y.tab.h"
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
<INITIAL>{br}       { return(BR); }
<INITIAL>"("        { return(LPAREN); }
<INITIAL>")"        { return(RPAREN); }
<INITIAL>"true"     { yylval->i_val = 1; return(BOOL); }
<INITIAL>"false"    { yylval->i_val = 0; return(BOOL); }
<INITIAL>"not"      { return(NOT); }

<INITIAL>"+"        { return(PLUS); }
<INITIAL>"-"        { return(MINUS); }
<INITIAL>"*"        { return(AST); }
<INITIAL>"/"        { return(SLASH); }
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
 /* TODO <INITIAL>"_"        { } */
 /*<INITIAL>"Array.create" { return(ARRAY_CREATE); } */
 /*<INITIAL>"."        { return(DOT); } */
 /*<INITIAL>"<-"       { return(LESS_MINUS); } */
 /*<INITIAL>";"        { return(SEMICOLON); } */


<INITIAL>{digit}+ {
    yylval->i_val = atoi(yytext);
    return(INT);
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
    return(STRING_LITERAL);
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
<INITIAL>\/\/[^\n]+ { }
<INITIAL>"/*"       { BEGIN(COMMENT); }
<COMMENT>"*/"       { BEGIN(INITIAL); }
<COMMENT><<EOF>>    { YY_FATAL_ERROR("unexpected end of input."); }
<COMMENT>{br}       { }
<COMMENT>.          { }


<<EOF>> { return(EOF); }

%%

void lexer_init(Lexer *lexer)
{
    lexer->buf = sb_new();
    yylex_init(&lexer->scanner);
}

void lexer_destroy(Lexer *lexer)
{
    sb_destroy(lexer->buf);
    yylex_destroy(lexer->scanner);
    free(lexer);
}
