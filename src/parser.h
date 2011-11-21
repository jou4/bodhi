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
     PERCENT = 270,
     MINUS_DOT = 271,
     PLUS_DOT = 272,
     AST_DOT = 273,
     SLASH_DOT = 274,
     EQUAL = 275,
     EQUAL_EQUAL = 276,
     LESS_GREATER = 277,
     LESS_EQUAL = 278,
     GREATER_EQUAL = 279,
     LESS = 280,
     GREATER = 281,
     IF = 282,
     THEN = 283,
     ELSE = 284,
     LET = 285,
     IN = 286,
     REC = 287,
     COMMA = 288,
     COLON = 289,
     DOT = 290,
     LESS_MINUS = 291,
     SEMICOLON = 292,
     LPAREN = 293,
     RPAREN = 294,
     LBRACKET = 295,
     RBRACKET = 296,
     LBRACKET_BAR = 297,
     RBRACKET_BAR = 298,
     LBRACE = 299,
     RBRACE = 300,
     FUN = 301,
     ARROW = 302,
     MAIN = 303,
     DEF = 304,
     CCALL = 305,
     MATCH = 306,
     WITH = 307,
     VERTICAL_BAR = 308,
     prec_match = 309,
     prec_match_branch = 310,
     prec_let = 311,
     prec_lambda = 312,
     prec_if = 313,
     prec_unary_minus = 314,
     prec_app = 315
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
#line 136 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




