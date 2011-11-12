/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "parser.y"


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



/* Line 268 of yacc.c  */
#line 90 "parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 288 of yacc.c  */
#line 19 "parser.y"

#include "lexer.h"
#include "compile.h"



/* Line 288 of yacc.c  */
#line 121 "parser.c"

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

/* Line 301 of yacc.c  */
#line 31 "parser.y"

  long i_val;
  double d_val;
  char c_val;
  char *s_val;
  StringBuffer* buf;
  BDSExpr* t;
  BDExprIdent *ident;
  Vector *vec;



/* Line 301 of yacc.c  */
#line 203 "parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 215 "parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   331

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  53
/* YYNRULES -- Number of states.  */
#define YYNSTATES  111

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    14,    20,    25,    29,
      33,    36,    40,    43,    45,    47,    49,    51,    53,    55,
      57,    59,    61,    68,    75,    84,    87,    91,   100,   105,
     108,   111,   115,   119,   123,   127,   131,   135,   139,   143,
     147,   151,   155,   157,   160,   162,   165,   167,   170,   174,
     178,   182,   184,   188
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    -1,    54,    55,    -1,     1,    -1,    44,
       3,    15,    57,    -1,    44,     3,    60,    15,    57,    -1,
      44,    43,    15,    57,    -1,    33,    57,    34,    -1,    33,
      63,    34,    -1,    33,    34,    -1,    35,    64,    36,    -1,
      35,    36,    -1,     3,    -1,     4,    -1,     5,    -1,     6,
      -1,     7,    -1,     8,    -1,    56,    -1,    58,    -1,    59,
      -1,    22,    57,    23,    57,    24,    57,    -1,    25,     3,
      15,    57,    26,    57,    -1,    25,    27,     3,    60,    15,
      57,    26,    57,    -1,    56,    62,    -1,    45,     3,    62,
      -1,    25,    33,    65,    34,    15,    57,    26,    57,    -1,
      41,    60,    42,    57,    -1,    10,    57,    -1,    11,    56,
      -1,    57,    12,    57,    -1,    57,    11,    57,    -1,    57,
      13,    57,    -1,    57,    14,    57,    -1,    57,    16,    57,
      -1,    57,    17,    57,    -1,    57,    20,    57,    -1,    57,
      21,    57,    -1,    57,    18,    57,    -1,    57,    19,    57,
      -1,    57,    29,    57,    -1,    61,    -1,    60,    61,    -1,
       3,    -1,    33,    34,    -1,    56,    -1,    62,    56,    -1,
      63,    28,    57,    -1,    57,    28,    57,    -1,    57,    28,
      64,    -1,    57,    -1,    65,    28,     3,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   107,   107,   109,   110,   114,   116,   118,   123,   125,
     127,   129,   131,   133,   135,   137,   139,   141,   143,   148,
     149,   150,   151,   154,   157,   160,   163,   166,   169,   175,
     178,   184,   186,   188,   190,   192,   194,   196,   198,   200,
     202,   204,   210,   212,   217,   219,   224,   227,   233,   235,
     240,   242,   247,   249
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "BOOL_LIT", "INT_LIT",
  "FLOAT_LIT", "CHAR_LIT", "STR_LIT", "BR", "NOT", "MINUS", "PLUS", "AST",
  "SLASH", "EQUAL", "EQUAL_EQUAL", "LESS_GREATER", "LESS_EQUAL",
  "GREATER_EQUAL", "LESS", "GREATER", "IF", "THEN", "ELSE", "LET", "IN",
  "REC", "COMMA", "COLON", "DOT", "LESS_MINUS", "SEMICOLON", "LPAREN",
  "RPAREN", "LBRACKET", "RBRACKET", "LBRACKET_BAR", "RBRACKET_BAR",
  "LBRACE", "RBRACE", "FUN", "ARROW", "MAIN", "DEF", "CCALL", "prec_let",
  "prec_lambda", "prec_if", "MINUS_DOT", "PLUS_DOT", "prec_unary_minus",
  "prec_app", "$accept", "input", "toplevel", "simple_exp", "exp",
  "uniop_exp", "binop_exp", "formal_args", "formal_arg", "actual_args",
  "t_elems", "l_elems", "pat", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    54,    54,    54,    55,    55,    55,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    58,
      58,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    60,    60,    61,    61,    62,    62,    63,    63,
      64,    64,    65,    65
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     4,     5,     4,     3,     3,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     6,     6,     8,     2,     3,     8,     4,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     2,     1,     2,     1,     2,     3,     3,
       3,     1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     4,     0,     1,     0,     3,     0,     0,    44,     0,
       0,     0,    42,     0,    13,    14,    15,    16,    17,    18,
       0,     0,     0,     0,     0,     0,     0,     0,    19,     5,
      20,    21,    45,     0,    43,     7,    29,    30,     0,     0,
       0,     0,    10,     0,     0,    12,    51,     0,     0,     0,
      46,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     6,     0,     0,     0,    53,     0,     0,
       8,     0,     9,     0,    11,     0,    26,    47,    32,    31,
      33,    34,    35,    36,    39,    40,    37,    38,    41,     0,
       0,     0,     0,     0,    49,    48,    50,    28,     0,     0,
       0,    52,     0,    22,    23,     0,     0,     0,     0,    24,
      27
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     5,    28,    46,    30,    31,    11,    12,    51,
      44,    47,    68
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -35
static const yytype_int16 yypact[] =
{
      17,   -35,    10,   -35,    16,   -35,    25,    -8,   -35,   124,
     -22,    69,   -35,   124,   -35,   -35,   -35,   -35,   -35,   -35,
     124,   167,   124,     2,    72,   115,     3,    22,   167,   296,
     -35,   -35,   -35,   124,   -35,   296,   -35,   -35,   192,    11,
      24,    28,   -35,   165,   -20,   -35,   206,    -2,     0,   167,
     -35,   167,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   124,   296,   124,   124,     3,   -35,   -12,   124,
     -35,   124,   -35,   124,   -35,   124,   167,   -35,     7,     7,
     -35,   -35,    56,    56,    56,    56,    56,    56,   296,   225,
     239,    70,    34,    26,   296,   296,   -35,   296,   124,   124,
     124,   -35,   124,   310,   296,   258,   277,   124,   124,   296,
     296
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -35,   -35,   -35,   -19,    -9,   -35,   -35,    -3,   -10,    15,
     -35,   -34,   -35
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -3
static const yytype_int8 yytable[] =
{
      29,    34,    37,     8,    35,    39,     8,    13,    71,    50,
       3,    36,    32,    38,    72,    43,    92,    -2,     1,     6,
      54,    55,    93,    48,    63,    49,    65,    66,     8,    40,
      50,    67,    77,    10,    74,    41,    10,   101,    34,    96,
       9,   102,    75,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,     4,    89,    90,    77,    10,     7,
      94,    -2,    95,    91,    76,     0,    97,    52,    53,    54,
      55,     0,     8,     8,     0,    14,    15,    16,    17,    18,
      19,    34,    20,    21,    33,   100,     0,     0,     0,   103,
     104,   105,     0,   106,    22,     0,     0,    23,   109,   110,
       0,     0,    10,    10,     0,    24,    42,    25,     0,     0,
       0,     0,     0,    26,     0,     0,     0,    27,    14,    15,
      16,    17,    18,    19,     0,    20,    21,    14,    15,    16,
      17,    18,    19,     0,    20,    21,     0,    22,     0,     0,
      23,     0,     0,     0,     0,     0,    22,     0,    24,    23,
      25,    45,     0,     0,     0,     0,    26,    24,     0,    25,
      27,     0,     0,     0,     0,    26,     0,     0,     0,    27,
      14,    15,    16,    17,    18,    19,    52,    53,    54,    55,
       0,    56,    57,    58,    59,    60,    61,     0,     0,     0,
       0,     0,     0,    69,    62,     0,     0,     0,     0,    70,
      24,     0,    25,    52,    53,    54,    55,     0,    56,    57,
      58,    59,    60,    61,     0,    64,     0,    52,    53,    54,
      55,    62,    56,    57,    58,    59,    60,    61,     0,     0,
       0,     0,     0,     0,    73,    62,    52,    53,    54,    55,
       0,    56,    57,    58,    59,    60,    61,     0,     0,    98,
      52,    53,    54,    55,    62,    56,    57,    58,    59,    60,
      61,     0,     0,     0,     0,    99,     0,     0,    62,    52,
      53,    54,    55,     0,    56,    57,    58,    59,    60,    61,
       0,     0,     0,     0,   107,     0,     0,    62,    52,    53,
      54,    55,     0,    56,    57,    58,    59,    60,    61,     0,
       0,     0,     0,   108,     0,     0,    62,    52,    53,    54,
      55,     0,    56,    57,    58,    59,    60,    61,     0,     0,
       0,    52,    53,    54,    55,    62,    56,    57,    58,    59,
      60,    61
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-35))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
       9,    11,    21,     3,    13,     3,     3,    15,    28,    28,
       0,    20,    34,    22,    34,    24,    28,     0,     1,     3,
      13,    14,    34,    26,    33,     3,    15,     3,     3,    27,
      49,     3,    51,    33,    36,    33,    33,     3,    48,    73,
      15,    15,    42,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    44,    64,    65,    76,    33,    43,
      69,    44,    71,    66,    49,    -1,    75,    11,    12,    13,
      14,    -1,     3,     3,    -1,     3,     4,     5,     6,     7,
       8,    91,    10,    11,    15,    15,    -1,    -1,    -1,    98,
      99,   100,    -1,   102,    22,    -1,    -1,    25,   107,   108,
      -1,    -1,    33,    33,    -1,    33,    34,    35,    -1,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    -1,    45,     3,     4,
       5,     6,     7,     8,    -1,    10,    11,     3,     4,     5,
       6,     7,     8,    -1,    10,    11,    -1,    22,    -1,    -1,
      25,    -1,    -1,    -1,    -1,    -1,    22,    -1,    33,    25,
      35,    36,    -1,    -1,    -1,    -1,    41,    33,    -1,    35,
      45,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    45,
       3,     4,     5,     6,     7,     8,    11,    12,    13,    14,
      -1,    16,    17,    18,    19,    20,    21,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    34,
      33,    -1,    35,    11,    12,    13,    14,    -1,    16,    17,
      18,    19,    20,    21,    -1,    23,    -1,    11,    12,    13,
      14,    29,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    -1,    -1,    -1,    28,    29,    11,    12,    13,    14,
      -1,    16,    17,    18,    19,    20,    21,    -1,    -1,    24,
      11,    12,    13,    14,    29,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,    11,
      12,    13,    14,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    26,    -1,    -1,    29,    11,    12,
      13,    14,    -1,    16,    17,    18,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    29,    11,    12,    13,
      14,    -1,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    11,    12,    13,    14,    29,    16,    17,    18,    19,
      20,    21
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    54,     0,    44,    55,     3,    43,     3,    15,
      33,    60,    61,    15,     3,     4,     5,     6,     7,     8,
      10,    11,    22,    25,    33,    35,    41,    45,    56,    57,
      58,    59,    34,    15,    61,    57,    57,    56,    57,     3,
      27,    33,    34,    57,    63,    36,    57,    64,    60,     3,
      56,    62,    11,    12,    13,    14,    16,    17,    18,    19,
      20,    21,    29,    57,    23,    15,     3,     3,    65,    28,
      34,    28,    34,    28,    36,    42,    62,    56,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    60,    28,    34,    57,    57,    64,    57,    24,    26,
      15,     3,    15,    57,    57,    57,    57,    26,    26,    57,
      57
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (ps, lexer, stream_name, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, ps, lexer, stream_name); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Parser *ps, Lexer *lexer, char *stream_name)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, ps, lexer, stream_name)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    Parser *ps;
    Lexer *lexer;
    char *stream_name;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (ps);
  YYUSE (lexer);
  YYUSE (stream_name);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Parser *ps, Lexer *lexer, char *stream_name)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, ps, lexer, stream_name)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    Parser *ps;
    Lexer *lexer;
    char *stream_name;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, ps, lexer, stream_name);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, Parser *ps, Lexer *lexer, char *stream_name)
#else
static void
yy_reduce_print (yyvsp, yyrule, ps, lexer, stream_name)
    YYSTYPE *yyvsp;
    int yyrule;
    Parser *ps;
    Lexer *lexer;
    char *stream_name;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , ps, lexer, stream_name);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, ps, lexer, stream_name); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, Parser *ps, Lexer *lexer, char *stream_name)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, ps, lexer, stream_name)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    Parser *ps;
    Lexer *lexer;
    char *stream_name;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (ps);
  YYUSE (lexer);
  YYUSE (stream_name);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (Parser *ps, Lexer *lexer, char *stream_name);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (Parser *ps, Lexer *lexer, char *stream_name)
#else
int
yyparse (ps, lexer, stream_name)
    Parser *ps;
    Lexer *lexer;
    char *stream_name;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

/* Line 1821 of yacc.c  */
#line 110 "parser.y"
    { YYABORT; }
    break;

  case 5:

/* Line 1821 of yacc.c  */
#line 115 "parser.y"
    { add_def(bd_sexpr_def(bd_expr_ident_typevar((yyvsp[(2) - (4)].s_val)), (yyvsp[(4) - (4)].t))); }
    break;

  case 6:

/* Line 1821 of yacc.c  */
#line 117 "parser.y"
    { add_def(bd_sexpr_def(bd_expr_ident_typevar((yyvsp[(2) - (5)].s_val)), bd_sexpr_fun(bd_type_var(NULL), (yyvsp[(3) - (5)].vec), (yyvsp[(5) - (5)].t)))); }
    break;

  case 7:

/* Line 1821 of yacc.c  */
#line 119 "parser.y"
    { set_maindef(bd_sexpr_def(bd_expr_ident("main", bd_type_unit()), (yyvsp[(4) - (4)].t))); }
    break;

  case 8:

/* Line 1821 of yacc.c  */
#line 124 "parser.y"
    { (yyval.t) = (yyvsp[(2) - (3)].t); }
    break;

  case 9:

/* Line 1821 of yacc.c  */
#line 126 "parser.y"
    { (yyval.t) = bd_sexpr_tuple((yyvsp[(2) - (3)].vec)); }
    break;

  case 10:

/* Line 1821 of yacc.c  */
#line 128 "parser.y"
    { (yyval.t) = bd_sexpr_unit(); }
    break;

  case 11:

/* Line 1821 of yacc.c  */
#line 130 "parser.y"
    { (yyval.t) = (yyvsp[(2) - (3)].t); }
    break;

  case 12:

/* Line 1821 of yacc.c  */
#line 132 "parser.y"
    { (yyval.t) = bd_sexpr_nil(); }
    break;

  case 13:

/* Line 1821 of yacc.c  */
#line 134 "parser.y"
    { (yyval.t) = bd_sexpr_var((yyvsp[(1) - (1)].s_val)); }
    break;

  case 14:

/* Line 1821 of yacc.c  */
#line 136 "parser.y"
    { (yyval.t) = bd_sexpr_bool((yyvsp[(1) - (1)].i_val)); }
    break;

  case 15:

/* Line 1821 of yacc.c  */
#line 138 "parser.y"
    { (yyval.t) = bd_sexpr_int((yyvsp[(1) - (1)].i_val)); }
    break;

  case 16:

/* Line 1821 of yacc.c  */
#line 140 "parser.y"
    { (yyval.t) = bd_sexpr_float((yyvsp[(1) - (1)].d_val)); }
    break;

  case 17:

/* Line 1821 of yacc.c  */
#line 142 "parser.y"
    { (yyval.t) = bd_sexpr_char((yyvsp[(1) - (1)].c_val)); }
    break;

  case 18:

/* Line 1821 of yacc.c  */
#line 144 "parser.y"
    { (yyval.t) = bd_sexpr_str(sb_to_string((yyvsp[(1) - (1)].buf))); }
    break;

  case 22:

/* Line 1821 of yacc.c  */
#line 153 "parser.y"
    { (yyval.t) = bd_sexpr_if((yyvsp[(2) - (6)].t), (yyvsp[(4) - (6)].t), (yyvsp[(6) - (6)].t)); }
    break;

  case 23:

/* Line 1821 of yacc.c  */
#line 156 "parser.y"
    { (yyval.t) = bd_sexpr_let(bd_expr_ident_typevar((yyvsp[(2) - (6)].s_val)), (yyvsp[(4) - (6)].t), (yyvsp[(6) - (6)].t)); }
    break;

  case 24:

/* Line 1821 of yacc.c  */
#line 159 "parser.y"
    { (yyval.t) = bd_sexpr_letrec(bd_expr_ident_typevar((yyvsp[(3) - (8)].s_val)), bd_sexpr_fun(bd_type_var(NULL), (yyvsp[(4) - (8)].vec), (yyvsp[(6) - (8)].t)), (yyvsp[(8) - (8)].t)); }
    break;

  case 25:

/* Line 1821 of yacc.c  */
#line 162 "parser.y"
    { (yyval.t) = bd_sexpr_app((yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].vec)); }
    break;

  case 26:

/* Line 1821 of yacc.c  */
#line 165 "parser.y"
    { (yyval.t) = bd_sexpr_ccall((yyvsp[(2) - (3)].s_val), (yyvsp[(3) - (3)].vec)); }
    break;

  case 27:

/* Line 1821 of yacc.c  */
#line 168 "parser.y"
    { (yyval.t) = bd_sexpr_lettuple((yyvsp[(3) - (8)].vec), (yyvsp[(6) - (8)].t), (yyvsp[(8) - (8)].t)); }
    break;

  case 28:

/* Line 1821 of yacc.c  */
#line 171 "parser.y"
    { (yyval.t) = bd_sexpr_fun(bd_type_var(NULL), (yyvsp[(2) - (4)].vec), (yyvsp[(4) - (4)].t)); }
    break;

  case 29:

/* Line 1821 of yacc.c  */
#line 177 "parser.y"
    { (yyval.t) = bd_sexpr_uniop(OP_NOT, (yyvsp[(2) - (2)].t)); }
    break;

  case 30:

/* Line 1821 of yacc.c  */
#line 180 "parser.y"
    { (yyval.t) = bd_sexpr_uniop(OP_NEG, (yyvsp[(2) - (2)].t)); }
    break;

  case 31:

/* Line 1821 of yacc.c  */
#line 185 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_ADD, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 32:

/* Line 1821 of yacc.c  */
#line 187 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_SUB, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 33:

/* Line 1821 of yacc.c  */
#line 189 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_MUL, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 34:

/* Line 1821 of yacc.c  */
#line 191 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_DIV, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 35:

/* Line 1821 of yacc.c  */
#line 193 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_EQ, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 36:

/* Line 1821 of yacc.c  */
#line 195 "parser.y"
    { (yyval.t) = bd_sexpr_uniop(OP_NOT, bd_sexpr_binop(OP_EQ, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t))); }
    break;

  case 37:

/* Line 1821 of yacc.c  */
#line 197 "parser.y"
    { (yyval.t) = bd_sexpr_uniop(OP_NOT, bd_sexpr_binop(OP_LE, (yyvsp[(3) - (3)].t), (yyvsp[(1) - (3)].t))); }
    break;

  case 38:

/* Line 1821 of yacc.c  */
#line 199 "parser.y"
    { (yyval.t) = bd_sexpr_uniop(OP_NOT, bd_sexpr_binop(OP_LE, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t))); }
    break;

  case 39:

/* Line 1821 of yacc.c  */
#line 201 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_LE, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 40:

/* Line 1821 of yacc.c  */
#line 203 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_LE, (yyvsp[(3) - (3)].t), (yyvsp[(1) - (3)].t)); }
    break;

  case 41:

/* Line 1821 of yacc.c  */
#line 205 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_CONS, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 42:

/* Line 1821 of yacc.c  */
#line 211 "parser.y"
    { Vector *vec = vector_new(); vector_add(vec, (yyvsp[(1) - (1)].ident)); (yyval.vec) = vec; }
    break;

  case 43:

/* Line 1821 of yacc.c  */
#line 213 "parser.y"
    { vector_add((yyvsp[(1) - (2)].vec), (yyvsp[(2) - (2)].ident)); (yyval.vec) = (yyvsp[(1) - (2)].vec); }
    break;

  case 44:

/* Line 1821 of yacc.c  */
#line 218 "parser.y"
    { (yyval.ident) = bd_expr_ident_typevar((yyvsp[(1) - (1)].s_val)); }
    break;

  case 45:

/* Line 1821 of yacc.c  */
#line 220 "parser.y"
    { (yyval.ident) = bd_expr_ident("_", bd_type_unit()); }
    break;

  case 46:

/* Line 1821 of yacc.c  */
#line 226 "parser.y"
    { Vector *vec = vector_new(); vector_add(vec, (yyvsp[(1) - (1)].t)); (yyval.vec) = vec; }
    break;

  case 47:

/* Line 1821 of yacc.c  */
#line 229 "parser.y"
    { vector_add((yyvsp[(1) - (2)].vec), (yyvsp[(2) - (2)].t)); (yyval.vec) = (yyvsp[(1) - (2)].vec); }
    break;

  case 48:

/* Line 1821 of yacc.c  */
#line 234 "parser.y"
    { vector_add((yyvsp[(1) - (3)].vec), (yyvsp[(3) - (3)].t)); (yyval.vec) = (yyvsp[(1) - (3)].vec); }
    break;

  case 49:

/* Line 1821 of yacc.c  */
#line 236 "parser.y"
    { Vector *vec = vector_new(); vector_add(vec, (yyvsp[(1) - (3)].t)); vector_add(vec, (yyvsp[(3) - (3)].t)); (yyval.vec) = vec; }
    break;

  case 50:

/* Line 1821 of yacc.c  */
#line 241 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_CONS, (yyvsp[(1) - (3)].t), (yyvsp[(3) - (3)].t)); }
    break;

  case 51:

/* Line 1821 of yacc.c  */
#line 243 "parser.y"
    { (yyval.t) = bd_sexpr_binop(OP_CONS, (yyvsp[(1) - (1)].t), bd_sexpr_nil()); }
    break;

  case 52:

/* Line 1821 of yacc.c  */
#line 248 "parser.y"
    { vector_add((yyvsp[(1) - (3)].vec), bd_expr_ident_typevar((yyvsp[(3) - (3)].s_val))); (yyval.vec) = (yyvsp[(1) - (3)].vec); }
    break;

  case 53:

/* Line 1821 of yacc.c  */
#line 250 "parser.y"
    { Vector *vec = vector_new(); vector_add(vec, bd_expr_ident_typevar((yyvsp[(1) - (1)].s_val))); (yyval.vec) = vec; }
    break;



/* Line 1821 of yacc.c  */
#line 1930 "parser.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (ps, lexer, stream_name, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (ps, lexer, stream_name, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, ps, lexer, stream_name);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, ps, lexer, stream_name);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (ps, lexer, stream_name, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, ps, lexer, stream_name);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, ps, lexer, stream_name);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 254 "parser.y"


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

