/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     FOCUS = 258,
     ALL = 259,
     COPY = 260,
     GUI = 261,
     KILL = 262,
     MPI = 263,
     EXP_PCSAMP = 264,
     EXP_USERTIME = 265,
     EXP_MPI = 266,
     EXP_FPE = 267,
     EXP_HWC = 268,
     EXP_IO = 269,
     VIEW_TOPN = 270,
     VIEW_EXCLTIME = 271,
     VIEW_IO = 272,
     VIEW_FPE = 273,
     VIEW_HWC = 274,
     PARAM_DISPLAY_MODE = 275,
     PARAM_DISPLAY_MEDIA = 276,
     PARAM_DISPLAY_REFRESH_RATE = 277,
     PARAM_EXP_SAVE_FILE = 278,
     PARAM_SAMPLING_RATE = 279,
     ATTACH_HEAD = 280,
     CLOSE_HEAD = 281,
     CREATE_HEAD = 282,
     DETACH_HEAD = 283,
     DISABLE_HEAD = 284,
     ENABLE_HEAD = 285,
     FOCUS_HEAD = 286,
     PAUSE_HEAD = 287,
     RESTORE_HEAD = 288,
     GO_HEAD = 289,
     SAVE_HEAD = 290,
     SETPARAM_HEAD = 291,
     STOP_HEAD = 292,
     VIEW_HEAD = 293,
     LIST_EXP_HEAD = 294,
     LIST_HOSTS_HEAD = 295,
     LIST_OBJ_HEAD = 296,
     LIST_PIDS_HEAD = 297,
     LIST_SRC_HEAD = 298,
     LIST_METRICS_HEAD = 299,
     LIST_PARAMS_HEAD = 300,
     LIST_REPORTS_HEAD = 301,
     LIST_BREAKS_HEAD = 302,
     LIST_TYPES_HEAD = 303,
     GEN_CLEAR_BREAK_HEAD = 304,
     GEN_EXIT_HEAD = 305,
     GEN_OPEN_GUI_HEAD = 306,
     GEN_HELP_HEAD = 307,
     GEN_HISTORY_HEAD = 308,
     GEN_LOG_HEAD = 309,
     GEN_PLAYBACK_HEAD = 310,
     GEN_RECORD_HEAD = 311,
     GEN_SETBREAK_HEAD = 312,
     JUST_QUIT_HEAD = 313,
     RANK_ID = 314,
     HOST_ID = 315,
     RANK_PID = 316,
     THREAD_ID = 317,
     FILE_ID = 318,
     EXP_ID = 319,
     PROCESS_ID = 320,
     LINE_ID = 321,
     COMMA = 322,
     SEMICOLON = 323,
     DOUBLE_COLON = 324,
     COLON = 325,
     END_LINE = 326,
     DOT = 327,
     EQUAL = 328,
     HEXNUMBER = 329,
     NUMBER = 330,
     NAME = 331
   };
#endif
#define FOCUS 258
#define ALL 259
#define COPY 260
#define GUI 261
#define KILL 262
#define MPI 263
#define EXP_PCSAMP 264
#define EXP_USERTIME 265
#define EXP_MPI 266
#define EXP_FPE 267
#define EXP_HWC 268
#define EXP_IO 269
#define VIEW_TOPN 270
#define VIEW_EXCLTIME 271
#define VIEW_IO 272
#define VIEW_FPE 273
#define VIEW_HWC 274
#define PARAM_DISPLAY_MODE 275
#define PARAM_DISPLAY_MEDIA 276
#define PARAM_DISPLAY_REFRESH_RATE 277
#define PARAM_EXP_SAVE_FILE 278
#define PARAM_SAMPLING_RATE 279
#define ATTACH_HEAD 280
#define CLOSE_HEAD 281
#define CREATE_HEAD 282
#define DETACH_HEAD 283
#define DISABLE_HEAD 284
#define ENABLE_HEAD 285
#define FOCUS_HEAD 286
#define PAUSE_HEAD 287
#define RESTORE_HEAD 288
#define GO_HEAD 289
#define SAVE_HEAD 290
#define SETPARAM_HEAD 291
#define STOP_HEAD 292
#define VIEW_HEAD 293
#define LIST_EXP_HEAD 294
#define LIST_HOSTS_HEAD 295
#define LIST_OBJ_HEAD 296
#define LIST_PIDS_HEAD 297
#define LIST_SRC_HEAD 298
#define LIST_METRICS_HEAD 299
#define LIST_PARAMS_HEAD 300
#define LIST_REPORTS_HEAD 301
#define LIST_BREAKS_HEAD 302
#define LIST_TYPES_HEAD 303
#define GEN_CLEAR_BREAK_HEAD 304
#define GEN_EXIT_HEAD 305
#define GEN_OPEN_GUI_HEAD 306
#define GEN_HELP_HEAD 307
#define GEN_HISTORY_HEAD 308
#define GEN_LOG_HEAD 309
#define GEN_PLAYBACK_HEAD 310
#define GEN_RECORD_HEAD 311
#define GEN_SETBREAK_HEAD 312
#define JUST_QUIT_HEAD 313
#define RANK_ID 314
#define HOST_ID 315
#define RANK_PID 316
#define THREAD_ID 317
#define FILE_ID 318
#define EXP_ID 319
#define PROCESS_ID 320
#define LINE_ID 321
#define COMMA 322
#define SEMICOLON 323
#define DOUBLE_COLON 324
#define COLON 325
#define END_LINE 326
#define DOT 327
#define EQUAL 328
#define HEXNUMBER 329
#define NUMBER 330
#define NAME 331




/* Copy the first part of user declarations.  */
#line 1 "test.y"

#include <stdio.h>
#include <ctype.h>
#include "support.h"

void yyerror(const char *);
extern char *yytext;
extern int yylex();
extern int yywrap();
extern void exit(int);

int yydebug = 1;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 15 "test.y"
typedef union YYSTYPE {
int val;
char *string;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 246 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 258 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   552

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  146
/* YYNRULES -- Number of rules. */
#define YYNRULES  388
/* YYNRULES -- Number of states. */
#define YYNSTATES  429

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   331

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    14,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    38,
      40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
      60,    62,    64,    66,    68,    70,    72,    74,    76,    78,
      80,    82,    85,    88,    90,    91,    93,    96,    98,   100,
     102,   105,   108,   110,   112,   113,   115,   117,   119,   122,
     125,   128,   131,   134,   137,   139,   142,   145,   147,   148,
     150,   153,   155,   158,   161,   163,   164,   166,   169,   171,
     173,   175,   178,   181,   183,   185,   188,   189,   191,   193,
     196,   199,   201,   203,   206,   207,   209,   211,   214,   217,
     219,   220,   222,   225,   227,   230,   233,   235,   237,   240,
     241,   243,   245,   248,   251,   253,   255,   258,   261,   263,
     265,   268,   269,   271,   273,   276,   279,   281,   282,   286,
     289,   292,   294,   297,   300,   302,   303,   306,   308,   311,
     314,   316,   317,   319,   321,   324,   327,   329,   330,   334,
     337,   340,   342,   345,   348,   350,   351,   353,   355,   358,
     361,   363,   364,   366,   369,   372,   374,   375,   377,   379,
     382,   385,   387,   388,   390,   393,   395,   398,   401,   403,
     404,   406,   409,   412,   416,   418,   421,   423,   426,   429,
     431,   432,   434,   436,   439,   442,   444,   445,   447,   449,
     452,   455,   457,   458,   460,   462,   465,   468,   470,   471,
     473,   475,   478,   481,   483,   484,   487,   490,   492,   493,
     495,   498,   500,   503,   506,   508,   509,   512,   515,   517,
     518,   520,   523,   525,   527,   530,   532,   534,   536,   538,
     540,   542,   544,   546,   548,   550,   552,   554,   556,   558,
     560,   562,   564,   566,   568,   570,   572,   574,   576,   578,
     580,   582,   584,   586,   588,   590,   592,   594,   596,   598,
     600,   602,   604,   606,   608,   610,   612,   614,   616,   618,
     620,   622,   624,   626,   628,   630,   632,   634,   636,   638,
     640,   642,   644,   646,   648,   650,   652,   655,   658,   660,
     661,   663,   666,   669,   671,   672,   674,   677,   680,   682,
     684,   687,   690,   692,   694,   697,   700,   701,   703,   705,
     708,   709,   711,   713,   715,   717,   720,   722,   725,   727,
     729,   731,   733,   735,   738,   740,   744,   746,   748,   751,
     753,   756,   759,   762,   764,   768,   770,   773,   775,   779,
     781,   785,   788,   790,   794,   796,   800,   803,   805,   809,
     811,   815,   817,   825,   829,   831,   833,   835,   837,   839,
     841,   843,   847,   849,   851,   853,   855,   857,   859,   861,
     863,   865,   867,   869,   873,   878,   881,   883,   885
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      78,     0,    -1,    -1,    78,    79,    -1,    71,    -1,    80,
      71,    -1,    81,    -1,    85,    -1,    89,    -1,    92,    -1,
      96,    -1,   100,    -1,   104,    -1,   108,    -1,   112,    -1,
     115,    -1,   119,    -1,   122,    -1,   125,    -1,   128,    -1,
     131,    -1,   134,    -1,   137,    -1,   140,    -1,   143,    -1,
     146,    -1,   149,    -1,   152,    -1,   155,    -1,   158,    -1,
     161,    -1,   165,    -1,   168,    -1,   171,    -1,   175,    -1,
     178,    -1,   181,    -1,   184,    -1,   187,    -1,   191,    -1,
       1,    -1,    82,    83,    -1,    82,     1,    -1,    25,    -1,
      -1,    84,    -1,    83,    84,    -1,   193,    -1,   192,    -1,
     216,    -1,    86,    87,    -1,    86,     1,    -1,    26,    -1,
      88,    -1,    -1,   192,    -1,     4,    -1,     3,    -1,     7,
     192,    -1,     7,     4,    -1,     7,     3,    -1,   192,     7,
      -1,     4,     7,    -1,     3,     7,    -1,     7,    -1,    90,
      91,    -1,    90,     1,    -1,    27,    -1,    -1,   193,    -1,
     193,   216,    -1,   216,    -1,    93,    94,    -1,    93,     1,
      -1,    28,    -1,    -1,    95,    -1,    94,    95,    -1,   193,
      -1,   192,    -1,   216,    -1,    97,    98,    -1,    97,     1,
      -1,    29,    -1,    99,    -1,    98,    99,    -1,    -1,   192,
      -1,     4,    -1,   101,   102,    -1,   101,     1,    -1,    30,
      -1,   103,    -1,   102,   103,    -1,    -1,   192,    -1,     4,
      -1,   105,   106,    -1,   105,     1,    -1,    31,    -1,    -1,
     107,    -1,   106,   107,    -1,   192,    -1,   109,   110,    -1,
     109,     1,    -1,    32,    -1,   111,    -1,   110,   111,    -1,
      -1,   192,    -1,     4,    -1,   113,   114,    -1,   113,     1,
      -1,    33,    -1,   198,    -1,   116,   117,    -1,   116,     1,
      -1,    34,    -1,   118,    -1,   117,   118,    -1,    -1,   192,
      -1,     4,    -1,   120,   121,    -1,   120,     1,    -1,    35,
      -1,    -1,   192,     5,   198,    -1,   192,   198,    -1,     5,
     198,    -1,   198,    -1,   123,   124,    -1,   123,     1,    -1,
      36,    -1,    -1,   192,   219,    -1,   219,    -1,   126,   127,
      -1,   126,     1,    -1,    37,    -1,    -1,   192,    -1,     4,
      -1,   129,   130,    -1,   129,     1,    -1,    38,    -1,    -1,
     192,     6,   218,    -1,     6,   218,    -1,   192,   218,    -1,
     218,    -1,   132,   133,    -1,   132,     1,    -1,    39,    -1,
      -1,   192,    -1,     4,    -1,   135,   136,    -1,   135,     1,
      -1,    40,    -1,    -1,   199,    -1,   138,   139,    -1,   138,
       1,    -1,    41,    -1,    -1,   192,    -1,   194,    -1,   141,
     142,    -1,   141,     1,    -1,    42,    -1,    -1,   198,    -1,
     198,     8,    -1,     8,    -1,   144,   145,    -1,   144,     1,
      -1,    43,    -1,    -1,   192,    -1,   192,   193,    -1,   192,
     220,    -1,   192,   193,   220,    -1,   193,    -1,   193,   220,
      -1,   220,    -1,   147,   148,    -1,   147,     1,    -1,    44,
      -1,    -1,   192,    -1,     4,    -1,   150,   151,    -1,   150,
       1,    -1,    45,    -1,    -1,   192,    -1,     4,    -1,   153,
     154,    -1,   153,     1,    -1,    46,    -1,    -1,   192,    -1,
       4,    -1,   156,   157,    -1,   156,     1,    -1,    47,    -1,
      -1,   192,    -1,     4,    -1,   159,   160,    -1,   159,     1,
      -1,    48,    -1,    -1,   162,   163,    -1,   162,     1,    -1,
      49,    -1,    -1,   164,    -1,   163,   164,    -1,    75,    -1,
     166,   167,    -1,   166,     1,    -1,    50,    -1,    -1,   169,
     170,    -1,   169,     1,    -1,    51,    -1,    -1,   172,    -1,
     172,   173,    -1,    52,    -1,   174,    -1,   173,   174,    -1,
      76,    -1,    25,    -1,    26,    -1,    27,    -1,    28,    -1,
      31,    -1,    32,    -1,    33,    -1,    34,    -1,    35,    -1,
      36,    -1,    37,    -1,    38,    -1,    39,    -1,    40,    -1,
      41,    -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      56,    -1,    57,    -1,     3,    -1,     4,    -1,     5,    -1,
       6,    -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,
      11,    -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,
      16,    -1,    17,    -1,    18,    -1,    19,    -1,    20,    -1,
      21,    -1,    22,    -1,    23,    -1,    24,    -1,    59,    -1,
      60,    -1,    65,    -1,    62,    -1,    63,    -1,    64,    -1,
      66,    -1,   176,   177,    -1,   176,     1,    -1,    53,    -1,
      -1,   198,    -1,   179,   180,    -1,   179,     1,    -1,    54,
      -1,    -1,   198,    -1,   182,   183,    -1,   182,     1,    -1,
      55,    -1,   198,    -1,   185,   186,    -1,   185,     1,    -1,
      56,    -1,   198,    -1,   188,   189,    -1,   188,     1,    -1,
      -1,    57,    -1,   190,    -1,   189,   190,    -1,    -1,   192,
      -1,   194,    -1,   222,    -1,    58,    -1,    64,    75,    -1,
     194,    -1,   193,   194,    -1,   195,    -1,   201,    -1,   204,
      -1,   207,    -1,   210,    -1,    60,   196,    -1,   197,    -1,
     196,    67,   197,    -1,    76,    -1,   213,    -1,   199,   200,
      -1,   200,    -1,    60,   196,    -1,    63,   203,    -1,    63,
     202,    -1,   203,    -1,   202,    67,   203,    -1,    76,    -1,
      65,   205,    -1,   206,    -1,   205,    67,   206,    -1,    75,
      -1,    75,    70,    75,    -1,    62,   208,    -1,   209,    -1,
     208,    67,   209,    -1,    75,    -1,    75,    70,    75,    -1,
      59,   211,    -1,   212,    -1,   211,    67,   212,    -1,    75,
      -1,    75,    70,    75,    -1,    75,    -1,    75,    72,    75,
      72,    75,    72,    75,    -1,   217,    69,   215,    -1,   215,
      -1,    20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,
      -1,   217,    -1,   216,    67,   217,    -1,     9,    -1,    10,
      -1,    11,    -1,    12,    -1,    13,    -1,    14,    -1,    15,
      -1,    16,    -1,    17,    -1,    18,    -1,    19,    -1,   214,
      73,    75,    -1,   219,   214,    73,    75,    -1,    66,   221,
      -1,    75,    -1,    75,    -1,    76,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,    65,    65,    66,    69,    70,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   111,   112,   114,   116,   117,   118,   120,   121,   122,
     126,   127,   129,   131,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   147,   148,   150,   152,   153,
     154,   155,   159,   160,   162,   164,   165,   166,   168,   169,
     170,   174,   175,   177,   179,   180,   182,   183,   184,   188,
     189,   191,   193,   194,   196,   197,   198,   202,   203,   205,
     207,   208,   209,   211,   215,   216,   218,   220,   221,   223,
     224,   225,   229,   230,   232,   234,   238,   239,   241,   243,
     244,   246,   247,   248,   252,   253,   255,   257,   258,   259,
     260,   261,   265,   266,   268,   270,   271,   272,   276,   277,
     279,   281,   282,   283,   287,   288,   290,   292,   293,   294,
     295,   296,   300,   301,   303,   305,   306,   307,   311,   312,
     314,   316,   317,   321,   322,   324,   326,   327,   328,   332,
     333,   335,   337,   338,   339,   340,   344,   345,   347,   349,
     350,   351,   352,   353,   354,   355,   356,   360,   361,   363,
     365,   366,   367,   371,   372,   374,   376,   377,   378,   382,
     383,   385,   387,   388,   389,   393,   394,   396,   398,   399,
     400,   404,   405,   407,   409,   413,   414,   416,   418,   419,
     420,   422,   426,   427,   429,   431,   435,   436,   438,   440,
     444,   445,   447,   449,   450,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,   466,
     467,   468,   469,   470,   471,   472,   473,   474,   475,   476,
     477,   478,   479,   480,   481,   482,   483,   485,   486,   487,
     488,   489,   490,   492,   493,   494,   495,   496,   497,   499,
     500,   501,   502,   503,   505,   506,   507,   508,   509,   511,
     512,   513,   514,   515,   516,   517,   521,   522,   524,   526,
     527,   531,   532,   534,   536,   537,   541,   542,   544,   546,
     550,   551,   553,   555,   559,   560,   562,   563,   565,   566,
     568,   569,   570,   571,   575,   582,   585,   586,   589,   590,
     591,   592,   593,   596,   599,   600,   603,   604,   607,   608,
     610,   613,   616,   619,   620,   623,   625,   628,   629,   632,
     633,   636,   639,   640,   643,   644,   647,   650,   651,   654,
     655,   658,   659,   663,   664,   667,   668,   669,   670,   671,
     674,   675,   678,   679,   680,   681,   682,   683,   686,   687,
     688,   689,   690,   693,   694,   706,   717,   720,   721
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "FOCUS", "ALL", "COPY", "GUI", "KILL", 
  "MPI", "EXP_PCSAMP", "EXP_USERTIME", "EXP_MPI", "EXP_FPE", "EXP_HWC", 
  "EXP_IO", "VIEW_TOPN", "VIEW_EXCLTIME", "VIEW_IO", "VIEW_FPE", 
  "VIEW_HWC", "PARAM_DISPLAY_MODE", "PARAM_DISPLAY_MEDIA", 
  "PARAM_DISPLAY_REFRESH_RATE", "PARAM_EXP_SAVE_FILE", 
  "PARAM_SAMPLING_RATE", "ATTACH_HEAD", "CLOSE_HEAD", "CREATE_HEAD", 
  "DETACH_HEAD", "DISABLE_HEAD", "ENABLE_HEAD", "FOCUS_HEAD", 
  "PAUSE_HEAD", "RESTORE_HEAD", "GO_HEAD", "SAVE_HEAD", "SETPARAM_HEAD", 
  "STOP_HEAD", "VIEW_HEAD", "LIST_EXP_HEAD", "LIST_HOSTS_HEAD", 
  "LIST_OBJ_HEAD", "LIST_PIDS_HEAD", "LIST_SRC_HEAD", "LIST_METRICS_HEAD", 
  "LIST_PARAMS_HEAD", "LIST_REPORTS_HEAD", "LIST_BREAKS_HEAD", 
  "LIST_TYPES_HEAD", "GEN_CLEAR_BREAK_HEAD", "GEN_EXIT_HEAD", 
  "GEN_OPEN_GUI_HEAD", "GEN_HELP_HEAD", "GEN_HISTORY_HEAD", 
  "GEN_LOG_HEAD", "GEN_PLAYBACK_HEAD", "GEN_RECORD_HEAD", 
  "GEN_SETBREAK_HEAD", "JUST_QUIT_HEAD", "RANK_ID", "HOST_ID", "RANK_PID", 
  "THREAD_ID", "FILE_ID", "EXP_ID", "PROCESS_ID", "LINE_ID", "COMMA", 
  "SEMICOLON", "DOUBLE_COLON", "COLON", "END_LINE", "DOT", "EQUAL", 
  "HEXNUMBER", "NUMBER", "NAME", "$accept", "ss_line", "command_line", 
  "command_desc", "exp_attach_com", "exp_attach_head", "exp_attach_args", 
  "exp_attach_arg", "exp_close_com", "exp_close_head", "exp_close_args", 
  "exp_close_arg", "exp_create_com", "exp_create_head", "exp_create_args", 
  "exp_detach_com", "exp_detach_head", "exp_detach_args", 
  "exp_detach_arg", "exp_disable_com", "exp_disable_head", 
  "exp_disable_args", "exp_disable_arg", "exp_enable_com", 
  "exp_enable_head", "exp_enable_args", "exp_enable_arg", "exp_focus_com", 
  "exp_focus_head", "exp_focus_args", "exp_focus_arg", "exp_pause_com", 
  "exp_pause_head", "exp_pause_args", "exp_pause_arg", "exp_restore_com", 
  "exp_restore_head", "exp_restore_arg", "exp_go_com", "exp_go_head", 
  "exp_go_args", "exp_go_arg", "exp_save_com", "exp_save_head", 
  "exp_save_arg", "exp_setparam_com", "exp_setparam_head", 
  "exp_setparam_arg", "exp_stop_com", "exp_stop_head", "exp_stop_arg", 
  "exp_view_com", "exp_view_head", "exp_view_arg", "list_exp_com", 
  "list_exp_head", "list_exp_arg", "list_hosts_com", "list_hosts_head", 
  "list_hosts_arg", "list_obj_com", "list_obj_head", "list_obj_arg", 
  "list_pids_com", "list_pids_head", "list_pids_arg", "list_src_com", 
  "list_src_head", "list_src_arg", "list_metrics_com", 
  "list_metrics_head", "list_metrics_arg", "list_params_com", 
  "list_params_head", "list_params_arg", "list_reports_com", 
  "list_reports_head", "list_reports_arg", "list_breaks_com", 
  "list_breaks_head", "list_breaks_arg", "list_types_com", 
  "list_types_head", "list_types_arg", "gen_clear_break_com", 
  "gen_clear_break_head", "gen_clear_break_args", "gen_clear_break_arg", 
  "gen_exit_com", "gen_exit_head", "gen_exit_arg", "gen_open_gui_com", 
  "gen_open_gui_head", "gen_open_gui_arg", "gen_help_com", 
  "gen_help_head", "gen_help_args", "gen_help_arg", "gen_history_com", 
  "gen_history_head", "gen_history_arg", "gen_log_com", "gen_log_head", 
  "gen_log_arg", "gen_playback_com", "gen_playback_head", 
  "gen_playback_arg", "gen_record_com", "gen_record_head", 
  "gen_record_arg", "gen_setbreak_com", "gen_setbreak_head", 
  "gen_setbreak_args", "gen_setbreak_arg", "just_quit_com", "expId_spec", 
  "host_file_rpt_list", "host_file_rpt", "host_list_spec", "host_list", 
  "host_name", "host_file", "host_spec", "file_spec", "file_list_spec", 
  "file_list", "file_name", "pid_list_spec", "pid_list", "pid_range", 
  "thread_list_spec", "thread_list", "thread_range", "rank_list_spec", 
  "rank_list", "rank_range", "ip_address", "expParam", "param_name", 
  "expType_list", "expType", "viewType", "param_list", "lineno_spec", 
  "lineno", "address_description", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    77,    78,    78,    79,    79,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    81,    81,    82,    83,    83,    83,    84,    84,    84,
      85,    85,    86,    87,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    89,    89,    90,    91,    91,
      91,    91,    92,    92,    93,    94,    94,    94,    95,    95,
      95,    96,    96,    97,    98,    98,    99,    99,    99,   100,
     100,   101,   102,   102,   103,   103,   103,   104,   104,   105,
     106,   106,   106,   107,   108,   108,   109,   110,   110,   111,
     111,   111,   112,   112,   113,   114,   115,   115,   116,   117,
     117,   118,   118,   118,   119,   119,   120,   121,   121,   121,
     121,   121,   122,   122,   123,   124,   124,   124,   125,   125,
     126,   127,   127,   127,   128,   128,   129,   130,   130,   130,
     130,   130,   131,   131,   132,   133,   133,   133,   134,   134,
     135,   136,   136,   137,   137,   138,   139,   139,   139,   140,
     140,   141,   142,   142,   142,   142,   143,   143,   144,   145,
     145,   145,   145,   145,   145,   145,   145,   146,   146,   147,
     148,   148,   148,   149,   149,   150,   151,   151,   151,   152,
     152,   153,   154,   154,   154,   155,   155,   156,   157,   157,
     157,   158,   158,   159,   160,   161,   161,   162,   163,   163,
     163,   164,   165,   165,   166,   167,   168,   168,   169,   170,
     171,   171,   172,   173,   173,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   175,   175,   176,   177,
     177,   178,   178,   179,   180,   180,   181,   181,   182,   183,
     184,   184,   185,   186,   187,   187,   188,   188,   189,   189,
     190,   190,   190,   190,   191,   192,   193,   193,   194,   194,
     194,   194,   194,   195,   196,   196,   197,   197,   198,   198,
     199,   200,   201,   202,   202,   203,   204,   205,   205,   206,
     206,   207,   208,   208,   209,   209,   210,   211,   211,   212,
     212,   213,   213,   214,   214,   215,   215,   215,   215,   215,
     216,   216,   217,   217,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   219,   219,   220,   221,   222,   222
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     0,     1,     2,     1,     1,     1,
       2,     2,     1,     1,     0,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     1,     0,     1,
       2,     1,     2,     2,     1,     0,     1,     2,     1,     1,
       1,     2,     2,     1,     1,     2,     0,     1,     1,     2,
       2,     1,     1,     2,     0,     1,     1,     2,     2,     1,
       0,     1,     2,     1,     2,     2,     1,     1,     2,     0,
       1,     1,     2,     2,     1,     1,     2,     2,     1,     1,
       2,     0,     1,     1,     2,     2,     1,     0,     3,     2,
       2,     1,     2,     2,     1,     0,     2,     1,     2,     2,
       1,     0,     1,     1,     2,     2,     1,     0,     3,     2,
       2,     1,     2,     2,     1,     0,     1,     1,     2,     2,
       1,     0,     1,     2,     2,     1,     0,     1,     1,     2,
       2,     1,     0,     1,     2,     1,     2,     2,     1,     0,
       1,     2,     2,     3,     1,     2,     1,     2,     2,     1,
       0,     1,     1,     2,     2,     1,     0,     1,     1,     2,
       2,     1,     0,     1,     1,     2,     2,     1,     0,     1,
       1,     2,     2,     1,     0,     2,     2,     1,     0,     1,
       2,     1,     2,     2,     1,     0,     2,     2,     1,     0,
       1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     0,
       1,     2,     2,     1,     0,     1,     2,     2,     1,     1,
       2,     2,     1,     1,     2,     2,     0,     1,     1,     2,
       0,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     2,     1,     3,     1,     1,     2,     1,
       2,     2,     2,     1,     3,     1,     2,     1,     3,     1,
       3,     2,     1,     3,     1,     3,     2,     1,     3,     1,
       3,     1,     7,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     4,     2,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       2,     0,     1,    40,    43,    52,    67,    74,    83,    91,
      99,   106,   114,   118,   126,   134,   140,   146,   154,   160,
     165,   171,   178,   189,   195,   201,   207,   213,   217,   224,
     228,   232,   298,   303,   308,   312,   317,   324,     4,     3,
       0,     6,     0,     7,     0,     8,     0,     9,     0,    10,
       0,    11,     0,    12,     0,    13,     0,    14,     0,    15,
       0,    16,     0,    17,     0,    18,     0,    19,     0,    20,
       0,    21,     0,    22,     0,    23,     0,    24,     0,    25,
       0,    26,     0,    27,     0,    28,     0,    29,     0,    30,
       0,    31,     0,    32,     0,    33,   230,    34,     0,    35,
       0,    36,     0,    37,     0,    38,     0,    39,     5,    42,
     372,   373,   374,   375,   376,   377,     0,     0,     0,     0,
       0,     0,    41,    45,    48,    47,   326,   328,   329,   330,
     331,   332,    49,   370,    51,    57,    56,    64,    50,    53,
      55,    66,    65,    69,    71,    73,    72,    76,    79,    78,
      80,    82,    88,    81,    84,    87,    90,    96,    89,    92,
      95,    98,    97,   101,   103,   105,   111,   104,   107,   110,
     113,     0,     0,   112,   115,     0,   339,   117,   123,   116,
     119,   122,   125,     0,   124,     0,   131,   133,   365,   366,
     367,   368,   369,   132,     0,     0,   364,     0,   137,   139,
     143,   138,   142,   145,     0,   378,   379,   380,   381,   382,
     144,     0,   151,   153,   157,   152,   156,   159,   158,   162,
     164,   163,   167,   168,   170,   175,   169,   173,   177,     0,
     176,   180,   184,   186,   188,   192,   187,   191,   194,   198,
     193,   197,   200,   204,   199,   203,   206,   210,   205,   209,
     212,   211,   216,   221,   215,   219,   223,   222,   227,   226,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   289,   290,   292,   293,   294,   291,   295,
     235,   231,   233,   297,   296,   300,   302,   301,   305,   307,
     306,   309,   311,   310,   313,   315,   387,   388,   314,   318,
     321,   322,   323,   359,   356,   357,   361,   336,   333,   334,
     337,   354,   351,   352,   345,   342,   343,   325,   349,   346,
     347,    46,   327,     0,    63,    62,    60,    59,    58,    61,
      70,    77,    85,    93,   102,   108,   340,   341,   338,   120,
     130,     0,   129,   136,     0,     0,     0,   149,     0,   150,
     174,   386,   385,   181,   182,   185,   220,   234,   319,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   371,   128,
     383,   363,     0,   148,   183,   360,   358,     0,   335,   355,
     353,   344,   350,   348,   384,     0,     0,     0,   362
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,    39,    40,    41,    42,   122,   123,    43,    44,
     138,   139,    45,    46,   142,    47,    48,   146,   147,    49,
      50,   153,   154,    51,    52,   158,   159,    53,    54,   162,
     163,    55,    56,   167,   168,    57,    58,   173,    59,    60,
     179,   180,    61,    62,   184,    63,    64,   193,    65,    66,
     201,    67,    68,   210,    69,    70,   215,    71,    72,   218,
      73,    74,   221,    75,    76,   226,    77,    78,   230,    79,
      80,   236,    81,    82,   240,    83,    84,   244,    85,    86,
     248,    87,    88,   251,    89,    90,   254,   255,    91,    92,
     257,    93,    94,   259,    95,    96,   321,   322,    97,    98,
     324,    99,   100,   327,   101,   102,   330,   103,   104,   333,
     105,   106,   338,   339,   107,   124,   125,   126,   127,   348,
     349,   174,   175,   176,   128,   355,   356,   129,   359,   360,
     130,   352,   353,   131,   344,   345,   350,   195,   196,   132,
     133,   212,   198,   233,   392,   342
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -221
static const short yypact[] =
{
    -221,   168,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
     -51,  -221,   115,  -221,   403,  -221,   389,  -221,   236,  -221,
     407,  -221,   408,  -221,   117,  -221,   423,  -221,    57,  -221,
     425,  -221,    30,  -221,   250,  -221,   429,  -221,    94,  -221,
     431,  -221,    18,  -221,   253,  -221,    43,  -221,   357,  -221,
     433,  -221,   435,  -221,   437,  -221,   439,  -221,    17,  -221,
      16,  -221,    27,  -221,    31,  -221,   329,  -221,    71,  -221,
      81,  -221,    59,  -221,   103,  -221,    21,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,   -54,   -60,   -50,   -42,
     -29,   -27,   452,  -221,  -221,   223,  -221,  -221,  -221,  -221,
    -221,  -221,   -17,  -221,  -221,    47,    49,   263,  -221,  -221,
      55,  -221,  -221,   471,   -17,  -221,   452,  -221,  -221,   223,
     -17,  -221,  -221,     1,  -221,  -221,  -221,  -221,   172,  -221,
    -221,  -221,     0,  -221,  -221,  -221,  -221,   178,  -221,  -221,
    -221,   -60,   -42,  -221,  -221,     5,  -221,  -221,  -221,   225,
    -221,  -221,  -221,     6,  -221,   248,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,   528,    42,  -221,    63,   528,  -221,
    -221,  -221,  -221,  -221,   510,  -221,  -221,  -221,  -221,  -221,
    -221,   440,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,    66,  -221,    60,
    -221,   -36,   -36,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,    61,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,   329,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,   216,  -221,
    -221,  -221,  -221,    68,    73,  -221,    75,  -221,    76,  -221,
    -221,    78,    82,  -221,  -221,    83,  -221,  -221,    84,    86,
    -221,  -221,  -221,   509,  -221,  -221,  -221,  -221,  -221,  -221,
     -17,  -221,  -221,  -221,  -221,  -221,    76,  -221,  -221,  -221,
    -221,     6,  -221,   528,    70,   523,    88,  -221,   510,  -221,
    -221,  -221,  -221,   -36,  -221,  -221,  -221,  -221,  -221,    87,
     -54,    89,   -60,    92,   -50,   -42,    95,   -27,  -221,  -221,
    -221,  -221,    96,  -221,  -221,  -221,  -221,    85,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,   108,   112,   110,  -221
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -221,  -221,  -221,  -221,  -221,  -221,  -221,    34,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,    41,  -221,
    -221,  -221,    38,  -221,  -221,  -221,    80,  -221,  -221,  -221,
      72,  -221,  -221,  -221,    74,  -221,  -221,  -221,  -221,  -221,
    -221,    77,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,   -14,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,   -69,  -221,  -221,
    -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
    -221,  -221,  -221,   -83,  -221,    -7,   -39,   -73,  -221,    97,
    -145,   -62,   186,   102,  -221,  -221,  -170,  -221,  -221,  -138,
    -221,  -221,  -120,  -221,  -221,  -113,  -221,  -194,   -95,   -38,
     -61,  -198,    99,  -220,  -221,  -221
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -321
static const short yytable[] =
{
     186,   223,   377,   197,   386,   152,   387,   143,   144,   149,
     150,   394,   395,   389,   227,   346,   347,   252,   250,   217,
     108,   343,   335,   116,   117,   351,   118,   119,   256,   121,
     229,   182,   258,   341,   354,   183,   325,   140,   328,   232,
     331,   148,   334,   155,   224,   160,   357,   164,   358,   169,
     363,   225,   362,   181,   364,   185,   365,   194,   170,   202,
     329,   211,   369,   216,   120,   120,   171,   222,   172,   172,
     362,   231,   323,   237,   390,   241,   362,   245,   171,   249,
     116,   117,   326,   118,   119,   120,   121,  -218,  -214,  -161,
     171,   253,  -320,   172,   120,   203,   336,   337,  -225,   340,
     204,  -127,  -229,   171,   332,   370,   172,   149,   150,   205,
     206,   207,   208,   209,  -172,   384,   109,   171,   161,   171,
     172,   380,   172,   382,   110,   111,   112,   113,   114,   115,
     368,   171,   385,   197,   172,   391,   253,   197,   399,   148,
     400,   171,  -299,   402,   172,   410,   155,   401,   403,   404,
     405,   160,  -304,   407,   406,   164,   361,   425,   120,   362,
     169,   412,   415,   171,   417,  -147,   172,   419,     2,     3,
     422,   424,   181,   414,   116,   117,   157,   118,   119,   120,
     121,   120,   166,   426,   427,   428,   -44,   371,  -100,   386,
     413,   372,   393,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,  -316,  -316,   178,
    -316,  -316,  -316,  -316,   374,   421,   120,   145,   373,    38,
     396,   375,   120,  -316,  -316,   110,   111,   112,   113,   114,
     115,   187,   397,   381,   220,   398,   379,   418,   219,   110,
     111,   112,   113,   114,   115,   341,   366,   367,   376,   423,
     188,   189,   190,   191,   192,   116,   117,   378,   118,   119,
     120,   121,   116,   117,   420,   118,   119,   416,   121,   120,
     411,   336,   337,   383,     0,   116,   117,     0,   118,   119,
     120,   121,   408,     0,     0,     0,     0,   -75,   171,     0,
       0,   172,   116,   117,   120,   118,   119,   120,   121,   409,
     362,  -135,   197,     0,  -166,     0,     0,   120,     0,     0,
       0,   340,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   228,     0,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,     0,   313,   314,
     141,   315,   316,   317,   318,   319,     0,     0,   110,   111,
     112,   113,   114,   115,   134,   320,   135,   136,   151,   156,
     137,   152,   157,     0,     0,     0,   116,   117,     0,   118,
     119,   120,   121,   229,   165,     0,   177,   166,  -179,   178,
     199,     0,   213,   200,   234,   214,   238,   235,   242,   239,
     246,   243,     0,   247,     0,     0,   388,     0,   116,   117,
       0,   118,   119,     0,   121,   205,   206,   207,   208,   209,
     -68,   110,   111,   112,   113,   114,   115,   120,     0,     0,
       0,   120,   120,     0,   -54,     0,     0,     0,   -86,   -94,
     110,   111,   112,   113,   114,   115,     0,   120,     0,   120,
       0,     0,     0,   120,  -109,   120,  -121,   120,     0,   120,
    -141,   120,  -155,   120,  -190,     0,  -196,     0,  -202,     0,
    -208,   116,   117,     0,   118,   119,   120,   121,   110,   111,
     112,   113,   114,   115,     0,   205,   206,   207,   208,   209,
     116,   117,     0,   118,   119,     0,   121,   110,   111,   112,
     113,   114,   115,   188,   189,   190,   191,   192,   188,   189,
     190,   191,   192
};

static const short yycheck[] =
{
      62,    74,   172,    64,   198,     4,   204,    46,    46,    48,
      48,   231,   232,   211,    76,    75,    76,     1,     1,     1,
      71,    75,     1,    59,    60,    75,    62,    63,     1,    65,
      66,     1,     1,   106,    76,     5,    98,    44,   100,    78,
     102,    48,   104,    50,     1,    52,    75,    54,    75,    56,
      67,     8,   125,    60,     7,    62,     7,    64,     1,    66,
       1,    68,     7,    70,    64,    64,    60,    74,    63,    63,
     143,    78,     1,    80,     8,    82,   149,    84,    60,    86,
      59,    60,     1,    62,    63,    64,    65,    71,    71,    71,
      60,    75,    71,    63,    64,     1,    75,    76,    71,   106,
       6,    71,    71,    60,     1,   143,    63,   146,   146,    15,
      16,    17,    18,    19,    71,    73,     1,    60,     1,    60,
      63,   183,    63,   185,     9,    10,    11,    12,    13,    14,
     137,    60,    69,   194,    63,    75,    75,   198,    70,   146,
      67,    60,    71,    67,    63,    75,   153,    72,    70,    67,
      67,   158,    71,    67,    70,   162,   122,    72,    64,   232,
     167,    73,    75,    60,    75,    71,    63,    75,     0,     1,
      75,    75,   179,   393,    59,    60,     4,    62,    63,    64,
      65,    64,     4,    75,    72,    75,    71,   146,    71,   383,
     388,   153,   231,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,     4,
      62,    63,    64,    65,   162,   405,    64,     1,   158,    71,
     254,   167,    64,    75,    76,     9,    10,    11,    12,    13,
      14,     1,   321,     5,     1,   338,   179,   402,    72,     9,
      10,    11,    12,    13,    14,   338,     3,     4,   171,   407,
      20,    21,    22,    23,    24,    59,    60,   175,    62,    63,
      64,    65,    59,    60,   404,    62,    63,   400,    65,    64,
     385,    75,    76,   194,    -1,    59,    60,    -1,    62,    63,
      64,    65,   363,    -1,    -1,    -1,    -1,    71,    60,    -1,
      -1,    63,    59,    60,    64,    62,    63,    64,    65,   381,
     393,    71,   383,    -1,    71,    -1,    -1,    64,    -1,    -1,
      -1,   338,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     1,    -1,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
       1,    62,    63,    64,    65,    66,    -1,    -1,     9,    10,
      11,    12,    13,    14,     1,    76,     3,     4,     1,     1,
       7,     4,     4,    -1,    -1,    -1,    59,    60,    -1,    62,
      63,    64,    65,    66,     1,    -1,     1,     4,    71,     4,
       1,    -1,     1,     4,     1,     4,     1,     4,     1,     4,
       1,     4,    -1,     4,    -1,    -1,     6,    -1,    59,    60,
      -1,    62,    63,    -1,    65,    15,    16,    17,    18,    19,
      71,     9,    10,    11,    12,    13,    14,    64,    -1,    -1,
      -1,    64,    64,    -1,    71,    -1,    -1,    -1,    71,    71,
       9,    10,    11,    12,    13,    14,    -1,    64,    -1,    64,
      -1,    -1,    -1,    64,    71,    64,    71,    64,    -1,    64,
      71,    64,    71,    64,    71,    -1,    71,    -1,    71,    -1,
      71,    59,    60,    -1,    62,    63,    64,    65,     9,    10,
      11,    12,    13,    14,    -1,    15,    16,    17,    18,    19,
      59,    60,    -1,    62,    63,    -1,    65,     9,    10,    11,
      12,    13,    14,    20,    21,    22,    23,    24,    20,    21,
      22,    23,    24
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    78,     0,     1,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    71,    79,
      80,    81,    82,    85,    86,    89,    90,    92,    93,    96,
      97,   100,   101,   104,   105,   108,   109,   112,   113,   115,
     116,   119,   120,   122,   123,   125,   126,   128,   129,   131,
     132,   134,   135,   137,   138,   140,   141,   143,   144,   146,
     147,   149,   150,   152,   153,   155,   156,   158,   159,   161,
     162,   165,   166,   168,   169,   171,   172,   175,   176,   178,
     179,   181,   182,   184,   185,   187,   188,   191,    71,     1,
       9,    10,    11,    12,    13,    14,    59,    60,    62,    63,
      64,    65,    83,    84,   192,   193,   194,   195,   201,   204,
     207,   210,   216,   217,     1,     3,     4,     7,    87,    88,
     192,     1,    91,   193,   216,     1,    94,    95,   192,   193,
     216,     1,     4,    98,    99,   192,     1,     4,   102,   103,
     192,     1,   106,   107,   192,     1,     4,   110,   111,   192,
       1,    60,    63,   114,   198,   199,   200,     1,     4,   117,
     118,   192,     1,     5,   121,   192,   198,     1,    20,    21,
      22,    23,    24,   124,   192,   214,   215,   217,   219,     1,
       4,   127,   192,     1,     6,    15,    16,    17,    18,    19,
     130,   192,   218,     1,     4,   133,   192,     1,   136,   199,
       1,   139,   192,   194,     1,     8,   142,   198,     1,    66,
     145,   192,   193,   220,     1,     4,   148,   192,     1,     4,
     151,   192,     1,     4,   154,   192,     1,     4,   157,   192,
       1,   160,     1,    75,   163,   164,     1,   167,     1,   170,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    59,    60,    62,    63,    64,    65,    66,
      76,   173,   174,     1,   177,   198,     1,   180,   198,     1,
     183,   198,     1,   186,   198,     1,    75,    76,   189,   190,
     192,   194,   222,    75,   211,   212,    75,    76,   196,   197,
     213,    75,   208,   209,    76,   202,   203,    75,    75,   205,
     206,    84,   194,    67,     7,     7,     3,     4,   192,     7,
     216,    95,    99,   103,   107,   111,   196,   203,   200,   118,
     198,     5,   198,   219,    73,    69,   214,   218,     6,   218,
       8,    75,   221,   193,   220,   220,   164,   174,   190,    70,
      67,    72,    67,    70,    67,    67,    70,    67,   217,   198,
      75,   215,    73,   218,   220,    75,   212,    75,   197,    75,
     209,   203,    75,   206,    75,    72,    75,    72,    75
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
#line 69 "test.y"
    {/* printf("OSS > "); */ }
    break;

  case 5:
#line 70 "test.y"
    {dump_command(); /* printf("OSS > "); */ }
    break;

  case 6:
#line 73 "test.y"
    {set_command_type(CMD_EXP_ATTACH);}
    break;

  case 7:
#line 74 "test.y"
    {set_command_type(CMD_EXP_CLOSE);}
    break;

  case 8:
#line 75 "test.y"
    {set_command_type(CMD_EXP_CREATE);}
    break;

  case 9:
#line 76 "test.y"
    {set_command_type(CMD_EXP_DETACH);}
    break;

  case 10:
#line 77 "test.y"
    {set_command_type(CMD_EXP_DISABLE);}
    break;

  case 11:
#line 78 "test.y"
    {set_command_type(CMD_EXP_ENABLE);}
    break;

  case 12:
#line 79 "test.y"
    {set_command_type(CMD_EXP_FOCUS);}
    break;

  case 13:
#line 80 "test.y"
    {set_command_type(CMD_EXP_PAUSE);}
    break;

  case 14:
#line 81 "test.y"
    {set_command_type(CMD_EXP_RESTORE);}
    break;

  case 15:
#line 82 "test.y"
    {set_command_type(CMD_EXP_GO);}
    break;

  case 16:
#line 83 "test.y"
    {set_command_type(CMD_EXP_SAVE);}
    break;

  case 17:
#line 84 "test.y"
    {set_command_type(CMD_EXP_SETPARAM);}
    break;

  case 18:
#line 85 "test.y"
    {set_command_type(CMD_EXP_STOP);}
    break;

  case 19:
#line 86 "test.y"
    {set_command_type(CMD_EXP_VIEW);}
    break;

  case 20:
#line 87 "test.y"
    {set_command_type(CMD_LIST_EXP);}
    break;

  case 21:
#line 88 "test.y"
    {set_command_type(CMD_LIST_HOSTS);}
    break;

  case 22:
#line 89 "test.y"
    {set_command_type(CMD_LIST_OBJ);}
    break;

  case 23:
#line 90 "test.y"
    {set_command_type(CMD_LIST_PIDS);}
    break;

  case 24:
#line 91 "test.y"
    {set_command_type(CMD_LIST_SRC);}
    break;

  case 25:
#line 92 "test.y"
    {set_command_type(CMD_LIST_METRICS);}
    break;

  case 26:
#line 93 "test.y"
    {set_command_type(CMD_LIST_PARAMS);}
    break;

  case 27:
#line 94 "test.y"
    {set_command_type(CMD_LIST_REPORTS);}
    break;

  case 28:
#line 95 "test.y"
    {set_command_type(CMD_LIST_BREAKS);}
    break;

  case 29:
#line 96 "test.y"
    {set_command_type(CMD_LIST_TYPES);}
    break;

  case 30:
#line 97 "test.y"
    {set_command_type(CMD_CLEAR_BREAK);}
    break;

  case 31:
#line 98 "test.y"
    {set_command_type(CMD_EXIT);exit(0);}
    break;

  case 32:
#line 99 "test.y"
    {set_command_type(CMD_OPEN_GUI);}
    break;

  case 33:
#line 100 "test.y"
    {set_command_type(CMD_HELP);}
    break;

  case 34:
#line 101 "test.y"
    {set_command_type(CMD_HISTORY);}
    break;

  case 35:
#line 102 "test.y"
    {set_command_type(CMD_LOG);}
    break;

  case 36:
#line 103 "test.y"
    {set_command_type(CMD_PLAYBACK);}
    break;

  case 37:
#line 104 "test.y"
    {set_command_type(CMD_RECORD);}
    break;

  case 38:
#line 105 "test.y"
    {set_command_type(CMD_SETBREAK);}
    break;

  case 39:
#line 106 "test.y"
    {exit(0);}
    break;

  case 40:
#line 107 "test.y"
    {/* set_error(yylval.string,command_name[CMD_HEAD_ERROR]);*/}
    break;

  case 42:
#line 112 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_ATTACH]);}
    break;

  case 51:
#line 127 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_CLOSE]);}
    break;

  case 66:
#line 148 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_CREATE]);}
    break;

  case 73:
#line 160 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_DETACH]);}
    break;

  case 82:
#line 175 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_DISABLE]);}
    break;

  case 90:
#line 189 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_ENABLE]);}
    break;

  case 98:
#line 203 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_FOCUS]);}
    break;

  case 105:
#line 216 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_PAUSE]);}
    break;

  case 113:
#line 230 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_RESTORE]);}
    break;

  case 117:
#line 239 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_GO]);}
    break;

  case 125:
#line 253 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_SAVE]);}
    break;

  case 133:
#line 266 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_SETPARAM]);}
    break;

  case 139:
#line 277 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_STOP]);}
    break;

  case 145:
#line 288 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_VIEW]);}
    break;

  case 153:
#line 301 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_EXP]);}
    break;

  case 159:
#line 312 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_HOSTS]);}
    break;

  case 164:
#line 322 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_OBJ]);}
    break;

  case 170:
#line 333 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_PIDS]);}
    break;

  case 177:
#line 345 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_SRC]);}
    break;

  case 188:
#line 361 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_METRICS]);}
    break;

  case 194:
#line 372 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_PARAMS]);}
    break;

  case 200:
#line 383 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_REPORTS]);}
    break;

  case 206:
#line 394 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_BREAKS]);}
    break;

  case 212:
#line 405 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_TYPES]);}
    break;

  case 216:
#line 414 "test.y"
    {set_error(yylval.string,command_name[CMD_CLEAR_BREAK]);}
    break;

  case 219:
#line 419 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_BREAK);}
    break;

  case 220:
#line 420 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_BREAK);}
    break;

  case 223:
#line 427 "test.y"
    {set_error(yylval.string,command_name[CMD_EXIT]);}
    break;

  case 227:
#line 436 "test.y"
    {set_error(yylval.string,command_name[CMD_OPEN_GUI]);}
    break;

  case 235:
#line 452 "test.y"
    {push_help(yyvsp[0].string);}
    break;

  case 236:
#line 453 "test.y"
    {push_help(command_name[CMD_EXP_ATTACH]);}
    break;

  case 237:
#line 454 "test.y"
    {push_help(command_name[CMD_EXP_CLOSE]);}
    break;

  case 238:
#line 455 "test.y"
    {push_help(command_name[CMD_EXP_CREATE]);}
    break;

  case 239:
#line 456 "test.y"
    {push_help(command_name[CMD_EXP_DETACH]);}
    break;

  case 240:
#line 457 "test.y"
    {push_help(command_name[CMD_EXP_FOCUS]);}
    break;

  case 241:
#line 458 "test.y"
    {push_help(command_name[CMD_EXP_PAUSE]);}
    break;

  case 242:
#line 459 "test.y"
    {push_help(command_name[CMD_EXP_RESTORE]);}
    break;

  case 243:
#line 460 "test.y"
    {push_help(command_name[CMD_EXP_GO]);}
    break;

  case 244:
#line 461 "test.y"
    {push_help(command_name[CMD_EXP_SAVE]);}
    break;

  case 245:
#line 462 "test.y"
    {push_help(command_name[CMD_EXP_SETPARAM]);}
    break;

  case 246:
#line 463 "test.y"
    {push_help(command_name[CMD_EXP_STOP]);}
    break;

  case 247:
#line 464 "test.y"
    {push_help(command_name[CMD_EXP_VIEW]);}
    break;

  case 248:
#line 465 "test.y"
    {push_help(command_name[CMD_LIST_EXP]);}
    break;

  case 249:
#line 466 "test.y"
    {push_help(command_name[CMD_LIST_HOSTS]);}
    break;

  case 250:
#line 467 "test.y"
    {push_help(command_name[CMD_LIST_OBJ]);}
    break;

  case 251:
#line 468 "test.y"
    {push_help(command_name[CMD_LIST_PIDS]);}
    break;

  case 252:
#line 469 "test.y"
    {push_help(command_name[CMD_LIST_SRC]);}
    break;

  case 253:
#line 470 "test.y"
    {push_help(command_name[CMD_LIST_METRICS]);}
    break;

  case 254:
#line 471 "test.y"
    {push_help(command_name[CMD_LIST_PARAMS]);}
    break;

  case 255:
#line 472 "test.y"
    {push_help(command_name[CMD_LIST_REPORTS]);}
    break;

  case 256:
#line 473 "test.y"
    {push_help(command_name[CMD_LIST_BREAKS]);}
    break;

  case 257:
#line 474 "test.y"
    {push_help(command_name[CMD_LIST_TYPES]);}
    break;

  case 258:
#line 475 "test.y"
    {push_help(command_name[CMD_CLEAR_BREAK]);}
    break;

  case 259:
#line 476 "test.y"
    {push_help(command_name[CMD_EXIT]);}
    break;

  case 260:
#line 477 "test.y"
    {push_help(command_name[CMD_OPEN_GUI]);}
    break;

  case 261:
#line 478 "test.y"
    {push_help(command_name[CMD_HELP]);}
    break;

  case 262:
#line 479 "test.y"
    {push_help(command_name[CMD_HISTORY]);}
    break;

  case 263:
#line 480 "test.y"
    {push_help(command_name[CMD_LOG]);}
    break;

  case 264:
#line 481 "test.y"
    {push_help(command_name[CMD_PLAYBACK]);}
    break;

  case 265:
#line 482 "test.y"
    {push_help(command_name[CMD_RECORD]);}
    break;

  case 266:
#line 483 "test.y"
    {push_help(command_name[CMD_SETBREAK]);}
    break;

  case 267:
#line 485 "test.y"
    {push_help(general_name[H_GEN_FOCUS]);}
    break;

  case 268:
#line 486 "test.y"
    {push_help(general_name[H_GEN_ALL]);}
    break;

  case 269:
#line 487 "test.y"
    {push_help(general_name[H_GEN_COPY]);}
    break;

  case 270:
#line 488 "test.y"
    {push_help(general_name[H_GEN_GUI]);}
    break;

  case 271:
#line 489 "test.y"
    {push_help(general_name[H_GEN_KILL]);}
    break;

  case 272:
#line 490 "test.y"
    {push_help("-mpi");}
    break;

  case 273:
#line 492 "test.y"
    {push_help(experiment_name[H_EXP_PCSAMP]);}
    break;

  case 274:
#line 493 "test.y"
    {push_help(experiment_name[H_EXP_USERTIME]);}
    break;

  case 275:
#line 494 "test.y"
    {push_help(experiment_name[H_EXP_MPI]);}
    break;

  case 276:
#line 495 "test.y"
    {push_help(experiment_name[H_EXP_FPE]);}
    break;

  case 277:
#line 496 "test.y"
    {push_help(experiment_name[H_EXP_HWC]);}
    break;

  case 278:
#line 497 "test.y"
    {push_help(experiment_name[H_EXP_IO]);}
    break;

  case 279:
#line 499 "test.y"
    {push_help(viewtype_name[H_VIEW_TOPN]);}
    break;

  case 280:
#line 500 "test.y"
    {push_help(viewtype_name[H_VIEW_EXCLTIME]);}
    break;

  case 281:
#line 501 "test.y"
    {push_help(viewtype_name[H_VIEW_IO]);}
    break;

  case 282:
#line 502 "test.y"
    {push_help(viewtype_name[H_VIEW_FPE]);}
    break;

  case 283:
#line 503 "test.y"
    {push_help(viewtype_name[H_VIEW_HWC]);}
    break;

  case 284:
#line 505 "test.y"
    {push_help(paramtype_name[H_PARAM_DMODE]);}
    break;

  case 285:
#line 506 "test.y"
    {push_help(paramtype_name[H_PARAM_DMEDIA]);}
    break;

  case 286:
#line 507 "test.y"
    {push_help(paramtype_name[H_PARAM_DREFRESH]);}
    break;

  case 287:
#line 508 "test.y"
    {push_help(paramtype_name[H_PARAM_XSAVEFILE]);}
    break;

  case 288:
#line 509 "test.y"
    {push_help(paramtype_name[H_PARAM_SAMPRATE]);}
    break;

  case 289:
#line 511 "test.y"
    {push_help("-r");}
    break;

  case 290:
#line 512 "test.y"
    {push_help("-h");}
    break;

  case 291:
#line 513 "test.y"
    {push_help("-p");}
    break;

  case 292:
#line 514 "test.y"
    {push_help("-t");}
    break;

  case 293:
#line 515 "test.y"
    {push_help("-f");}
    break;

  case 294:
#line 516 "test.y"
    {push_help("-x");}
    break;

  case 295:
#line 517 "test.y"
    {push_help("-l");}
    break;

  case 297:
#line 522 "test.y"
    {set_error(yylval.string,command_name[CMD_HISTORY]);}
    break;

  case 302:
#line 532 "test.y"
    {set_error(yylval.string,command_name[CMD_LOG]);}
    break;

  case 307:
#line 542 "test.y"
    {set_error(yylval.string,command_name[CMD_PLAYBACK]);}
    break;

  case 311:
#line 551 "test.y"
    {set_error(yylval.string,command_name[CMD_RECORD]);}
    break;

  case 315:
#line 560 "test.y"
    {set_error(yylval.string,command_name[CMD_SETBREAK]);}
    break;

  case 323:
#line 571 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_ADDRESS);}
    break;

  case 325:
#line 582 "test.y"
    {set_exp_id(yyvsp[0].val);}
    break;

  case 336:
#line 603 "test.y"
    {push_host_name(yyvsp[0].string);}
    break;

  case 337:
#line 604 "test.y"
    {push_host_ip(yyvsp[0].val);}
    break;

  case 345:
#line 623 "test.y"
    {push_file(yyvsp[0].string);}
    break;

  case 349:
#line 632 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_PID);}
    break;

  case 350:
#line 633 "test.y"
    {push_32bit_range(yyvsp[-2].val,yyvsp[0].val,TABLE_PID);}
    break;

  case 354:
#line 643 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_THREAD);}
    break;

  case 355:
#line 644 "test.y"
    {push_32bit_range(yyvsp[-2].val,yyvsp[0].val,TABLE_THREAD);}
    break;

  case 359:
#line 654 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_RANK);}
    break;

  case 360:
#line 655 "test.y"
    {push_32bit_range(yyvsp[-2].val,yyvsp[0].val,TABLE_RANK);}
    break;

  case 362:
#line 660 "test.y"
    { yyval.val = (yyvsp[-6].val<<24) | (yyvsp[-4].val<<16) | (yyvsp[-2].val<<8) | yyvsp[0].val;}
    break;

  case 365:
#line 667 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 366:
#line 668 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 367:
#line 669 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 368:
#line 670 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 369:
#line 671 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 372:
#line 678 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 373:
#line 679 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 374:
#line 680 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 375:
#line 681 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 376:
#line 682 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 377:
#line 683 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 378:
#line 686 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 379:
#line 687 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 380:
#line 688 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 381:
#line 689 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 382:
#line 690 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 387:
#line 720 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_ADDRESS);}
    break;

  case 388:
#line 721 "test.y"
    {push_string(yyvsp[0].string,NAME_ADDR);}
    break;


    }

/* Line 991 of yacc.c.  */
#line 2502 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__) \
    && !defined __cplusplus
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 725 "test.y"



/***************************************************/
void 
yyerror(const char *s)
{
    printf("%s\n",s);
}

/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
#if 0
int
main ()
{    
    memset(&command,0,sizeof(command_t));
    cmd_init();
    printf("OSS > ");

    return yyparse();
}

#endif


