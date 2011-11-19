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
#line 19 "parser.y"

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
     MINUS_DOT = 270,
     PLUS_DOT = 271,
     AST_DOT = 272,
     SLASH_DOT = 273,
     EQUAL = 274,
     EQUAL_EQUAL = 275,
     LESS_GREATER = 276,
     LESS_EQUAL = 277,
     GREATER_EQUAL = 278,
     LESS = 279,
     GREATER = 280,
     IF = 281,
     THEN = 282,
     ELSE = 283,
     LET = 284,
     IN = 285,
     REC = 286,
     COMMA = 287,
     COLON = 288,
     DOT = 289,
     LESS_MINUS = 290,
     SEMICOLON = 291,
     LPAREN = 292,
     RPAREN = 293,
     LBRACKET = 294,
     RBRACKET = 295,
     LBRACKET_BAR = 296,
     RBRACKET_BAR = 297,
     LBRACE = 298,
     RBRACE = 299,
     FUN = 300,
     ARROW = 301,
     MAIN = 302,
     DEF = 303,
     CCALL = 304,
     MATCH = 305,
     WITH = 306,
     VERTICAL_BAR = 307,
     prec_match = 308,
     prec_match_branch = 309,
     prec_let = 310,
     prec_lambda = 311,
     prec_if = 312,
     prec_unary_minus = 313,
     prec_app = 314
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2132 of yacc.c  */
#line 31 "parser.y"

  long i_val;
  double d_val;
  char c_val;
  char *s_val;
  StringBuffer* buf;
  BDSExpr* t;
  BDExprIdent *ident;
  BDSExprMatchBranch *branch;
  Vector *vec;



/* Line 2132 of yacc.c  */
#line 135 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




