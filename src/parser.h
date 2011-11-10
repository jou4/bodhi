/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */

/* Line 2132 of yacc.c  */
#line 20 "parser.y"

#include "lexer.h"
#include "compile.h"



/* Line 2132 of yacc.c  */
#line 45 "parser.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     BOOL_LIT = 259,
     INT_LIT = 260,
     FLOAT_LIT = 261,
     CHAR_LIT = 262,
     STR_LIT = 263,
     BR = 264,
     NOT = 265,
     MINUS = 266,
     PLUS = 267,
     AST = 268,
     SLASH = 269,
     EQUAL = 270,
     EQUAL_EQUAL = 271,
     LESS_GREATER = 272,
     LESS_EQUAL = 273,
     GREATER_EQUAL = 274,
     LESS = 275,
     GREATER = 276,
     IF = 277,
     THEN = 278,
     ELSE = 279,
     LET = 280,
     IN = 281,
     REC = 282,
     COMMA = 283,
     COLON = 284,
     DOT = 285,
     LESS_MINUS = 286,
     SEMICOLON = 287,
     LPAREN = 288,
     RPAREN = 289,
     LBRACKET = 290,
     RBRACKET = 291,
     LBRACKET_BAR = 292,
     RBRACKET_BAR = 293,
     LBRACE = 294,
     RBRACE = 295,
     FUN = 296,
     ARROW = 297,
     MAIN = 298,
     DEF = 299,
     CCALL = 300,
     prec_let = 301,
     prec_lambda = 302,
     prec_if = 303,
     MINUS_DOT = 304,
     PLUS_DOT = 305,
     prec_unary_minus = 306,
     prec_app = 307
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2132 of yacc.c  */
#line 32 "parser.y"

  int i_val;
  double d_val;
  char c_val;
  char *s_val;
  StringBuffer* buf;
  BDSExpr* t;
  BDExprIdent *ident;
  Vector *vec;



/* Line 2132 of yacc.c  */
#line 127 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




