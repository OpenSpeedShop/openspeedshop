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
     DATA = 260,
     GUI = 261,
     MPI = 262,
     EXP_PCSAMP = 263,
     EXP_USERTIME = 264,
     EXP_MPI = 265,
     EXP_FPE = 266,
     EXP_HWC = 267,
     EXP_IO = 268,
     VIEW_TOPN = 269,
     VIEW_EXCLTIME = 270,
     VIEW_IO = 271,
     VIEW_FPE = 272,
     VIEW_HWC = 273,
     PARAM_DISPLAY_MODE = 274,
     PARAM_DISPLAY_MEDIA = 275,
     PARAM_DISPLAY_REFRESH_RATE = 276,
     PARAM_EXP_SAVE_FILE = 277,
     PARAM_SAMPLING_RATE = 278,
     ATTACH_HEAD = 279,
     CLOSE_HEAD = 280,
     CONT_HEAD = 281,
     CREATE_HEAD = 282,
     DETACH_HEAD = 283,
     FOCUS_HEAD = 284,
     PAUSE_HEAD = 285,
     RESTORE_HEAD = 286,
     RUN_HEAD = 287,
     SAVEFILE_HEAD = 288,
     SETPARAM_HEAD = 289,
     STOP_HEAD = 290,
     VIEW_HEAD = 291,
     LIST_EXP_HEAD = 292,
     LIST_HOSTS_HEAD = 293,
     LIST_OBJ_HEAD = 294,
     LIST_PIDS_HEAD = 295,
     LIST_SRC_HEAD = 296,
     LIST_METRICS_HEAD = 297,
     LIST_PARAMS_HEAD = 298,
     LIST_REPORTS_HEAD = 299,
     LIST_BREAKS_HEAD = 300,
     GEN_CLEAR_BREAK_HEAD = 301,
     GEN_EXIT_HEAD = 302,
     GEN_OPEN_GUI_HEAD = 303,
     GEN_HELP_HEAD = 304,
     GEN_HISTORY_HEAD = 305,
     GEN_LOG_HEAD = 306,
     GEN_PLAYBACK_HEAD = 307,
     GEN_RECORD_HEAD = 308,
     GEN_SETBREAK_HEAD = 309,
     JUST_QUIT_HEAD = 310,
     RANK_ID = 311,
     HOST_ID = 312,
     RANK_PID = 313,
     THREAD_ID = 314,
     FILE_ID = 315,
     EXP_ID = 316,
     PROCESS_ID = 317,
     LINE_ID = 318,
     COMMA = 319,
     SEMICOLON = 320,
     DOUBLE_COLON = 321,
     COLON = 322,
     END_LINE = 323,
     DOT = 324,
     EQUAL = 325,
     HEXNUMBER = 326,
     NUMBER = 327,
     NAME = 328
   };
#endif
#define FOCUS 258
#define ALL 259
#define DATA 260
#define GUI 261
#define MPI 262
#define EXP_PCSAMP 263
#define EXP_USERTIME 264
#define EXP_MPI 265
#define EXP_FPE 266
#define EXP_HWC 267
#define EXP_IO 268
#define VIEW_TOPN 269
#define VIEW_EXCLTIME 270
#define VIEW_IO 271
#define VIEW_FPE 272
#define VIEW_HWC 273
#define PARAM_DISPLAY_MODE 274
#define PARAM_DISPLAY_MEDIA 275
#define PARAM_DISPLAY_REFRESH_RATE 276
#define PARAM_EXP_SAVE_FILE 277
#define PARAM_SAMPLING_RATE 278
#define ATTACH_HEAD 279
#define CLOSE_HEAD 280
#define CONT_HEAD 281
#define CREATE_HEAD 282
#define DETACH_HEAD 283
#define FOCUS_HEAD 284
#define PAUSE_HEAD 285
#define RESTORE_HEAD 286
#define RUN_HEAD 287
#define SAVEFILE_HEAD 288
#define SETPARAM_HEAD 289
#define STOP_HEAD 290
#define VIEW_HEAD 291
#define LIST_EXP_HEAD 292
#define LIST_HOSTS_HEAD 293
#define LIST_OBJ_HEAD 294
#define LIST_PIDS_HEAD 295
#define LIST_SRC_HEAD 296
#define LIST_METRICS_HEAD 297
#define LIST_PARAMS_HEAD 298
#define LIST_REPORTS_HEAD 299
#define LIST_BREAKS_HEAD 300
#define GEN_CLEAR_BREAK_HEAD 301
#define GEN_EXIT_HEAD 302
#define GEN_OPEN_GUI_HEAD 303
#define GEN_HELP_HEAD 304
#define GEN_HISTORY_HEAD 305
#define GEN_LOG_HEAD 306
#define GEN_PLAYBACK_HEAD 307
#define GEN_RECORD_HEAD 308
#define GEN_SETBREAK_HEAD 309
#define JUST_QUIT_HEAD 310
#define RANK_ID 311
#define HOST_ID 312
#define RANK_PID 313
#define THREAD_ID 314
#define FILE_ID 315
#define EXP_ID 316
#define PROCESS_ID 317
#define LINE_ID 318
#define COMMA 319
#define SEMICOLON 320
#define DOUBLE_COLON 321
#define COLON 322
#define END_LINE 323
#define DOT 324
#define EQUAL 325
#define HEXNUMBER 326
#define NUMBER 327
#define NAME 328




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
#line 240 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 252 "y.tab.c"

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
#define YYLAST   479

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  139
/* YYNRULES -- Number of rules. */
#define YYNRULES  358
/* YYNRULES -- Number of states. */
#define YYNSTATES  411

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

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
      65,    66,    67,    68,    69,    70,    71,    72,    73
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
      81,    84,    86,    88,    91,    93,    95,    97,   100,   103,
     105,   107,   110,   111,   113,   115,   118,   121,   123,   125,
     128,   129,   131,   133,   136,   139,   141,   143,   146,   149,
     152,   154,   156,   159,   161,   163,   165,   168,   171,   173,
     175,   178,   180,   183,   186,   188,   190,   193,   194,   196,
     198,   201,   204,   206,   208,   211,   214,   216,   218,   221,
     222,   224,   226,   229,   232,   234,   238,   242,   245,   248,
     251,   254,   256,   259,   261,   264,   267,   269,   270,   272,
     274,   277,   280,   282,   286,   289,   292,   294,   297,   300,
     302,   303,   305,   307,   309,   312,   315,   317,   318,   320,
     323,   326,   328,   329,   331,   333,   336,   339,   341,   342,
     344,   347,   349,   352,   355,   357,   358,   360,   363,   366,
     370,   372,   375,   377,   380,   383,   385,   386,   388,   390,
     392,   395,   398,   400,   401,   403,   405,   407,   410,   413,
     415,   416,   418,   420,   422,   425,   428,   430,   431,   433,
     435,   438,   441,   443,   445,   448,   450,   453,   456,   458,
     459,   462,   465,   467,   468,   470,   473,   475,   477,   480,
     482,   484,   486,   488,   490,   492,   494,   496,   498,   500,
     502,   504,   506,   508,   510,   512,   514,   516,   518,   520,
     522,   524,   526,   528,   530,   532,   534,   536,   538,   540,
     542,   544,   546,   548,   550,   552,   554,   556,   558,   560,
     562,   564,   566,   568,   570,   572,   574,   576,   578,   580,
     582,   584,   586,   588,   590,   592,   594,   596,   598,   600,
     603,   606,   608,   609,   611,   614,   617,   619,   620,   622,
     625,   628,   630,   632,   635,   638,   640,   642,   645,   648,
     650,   652,   655,   657,   659,   661,   663,   666,   668,   671,
     673,   675,   677,   679,   681,   684,   686,   690,   692,   694,
     697,   699,   702,   705,   708,   710,   714,   716,   719,   721,
     725,   727,   731,   734,   736,   740,   742,   746,   749,   751,
     755,   757,   761,   763,   771,   775,   777,   779,   781,   783,
     785,   787,   789,   793,   795,   797,   799,   801,   803,   805,
     807,   809,   811,   813,   815,   819,   824,   827,   829
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      75,     0,    -1,    -1,    75,    76,    -1,    68,    -1,    77,
      68,    -1,    78,    -1,    82,    -1,    86,    -1,    90,    -1,
      93,    -1,    97,    -1,   101,    -1,   105,    -1,   108,    -1,
     112,    -1,   115,    -1,   118,    -1,   121,    -1,   124,    -1,
     127,    -1,   130,    -1,   133,    -1,   136,    -1,   139,    -1,
     142,    -1,   145,    -1,   148,    -1,   151,    -1,   155,    -1,
     158,    -1,   161,    -1,   165,    -1,   168,    -1,   171,    -1,
     174,    -1,   177,    -1,   181,    -1,     1,    -1,    79,    80,
      -1,    79,     1,    -1,    24,    -1,    81,    -1,    80,    81,
      -1,   183,    -1,   182,    -1,   206,    -1,    83,    84,    -1,
      83,     1,    -1,    25,    -1,    85,    -1,    84,    85,    -1,
      -1,   182,    -1,     4,    -1,    87,    88,    -1,    87,     1,
      -1,    26,    -1,    89,    -1,    88,    89,    -1,    -1,   182,
      -1,     4,    -1,    91,    92,    -1,    91,     1,    -1,    27,
      -1,   183,    -1,   183,   206,    -1,    94,    95,    -1,    94,
       1,    -1,    28,    -1,    96,    -1,    95,    96,    -1,   183,
      -1,   182,    -1,   206,    -1,    98,    99,    -1,    98,     1,
      -1,    29,    -1,   100,    -1,    99,   100,    -1,   182,    -1,
     102,   103,    -1,   102,     1,    -1,    30,    -1,   104,    -1,
     103,   104,    -1,    -1,   182,    -1,     4,    -1,   106,   107,
      -1,   106,     1,    -1,    31,    -1,   188,    -1,   109,   110,
      -1,   109,     1,    -1,    32,    -1,   111,    -1,   110,   111,
      -1,    -1,   182,    -1,     4,    -1,   113,   114,    -1,   113,
       1,    -1,    33,    -1,   182,     4,   188,    -1,   182,     5,
     188,    -1,     4,   188,    -1,     5,   188,    -1,   116,   117,
      -1,   116,     1,    -1,    34,    -1,   182,   209,    -1,   209,
      -1,   119,   120,    -1,   119,     1,    -1,    35,    -1,    -1,
     182,    -1,     4,    -1,   122,   123,    -1,   122,     1,    -1,
      36,    -1,   182,     6,   208,    -1,     6,   208,    -1,   182,
     208,    -1,   208,    -1,   125,   126,    -1,   125,     1,    -1,
      37,    -1,    -1,     3,    -1,   182,    -1,     4,    -1,   128,
     129,    -1,   128,     1,    -1,    38,    -1,    -1,   189,    -1,
     131,   132,    -1,   131,     1,    -1,    39,    -1,    -1,   182,
      -1,   184,    -1,   134,   135,    -1,   134,     1,    -1,    40,
      -1,    -1,   188,    -1,   188,     7,    -1,     7,    -1,   137,
     138,    -1,   137,     1,    -1,    41,    -1,    -1,   182,    -1,
     182,   183,    -1,   182,   210,    -1,   182,   183,   210,    -1,
     183,    -1,   183,   210,    -1,   210,    -1,   140,   141,    -1,
     140,     1,    -1,    42,    -1,    -1,     3,    -1,   182,    -1,
       4,    -1,   143,   144,    -1,   143,     1,    -1,    43,    -1,
      -1,     3,    -1,   182,    -1,     4,    -1,   146,   147,    -1,
     146,     1,    -1,    44,    -1,    -1,     3,    -1,   182,    -1,
       4,    -1,   149,   150,    -1,   149,     1,    -1,    45,    -1,
      -1,   182,    -1,     4,    -1,   152,   153,    -1,   152,     1,
      -1,    46,    -1,   154,    -1,   153,   154,    -1,    72,    -1,
     156,   157,    -1,   156,     1,    -1,    47,    -1,    -1,   159,
     160,    -1,   159,     1,    -1,    48,    -1,    -1,   162,    -1,
     162,   163,    -1,    49,    -1,   164,    -1,   163,   164,    -1,
      73,    -1,    24,    -1,    25,    -1,    26,    -1,    27,    -1,
      28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,    -1,
      33,    -1,    34,    -1,    35,    -1,    36,    -1,    37,    -1,
      38,    -1,    39,    -1,    40,    -1,    41,    -1,    42,    -1,
      43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,    -1,
      48,    -1,    49,    -1,    50,    -1,    51,    -1,    52,    -1,
      53,    -1,    54,    -1,     3,    -1,     4,    -1,     5,    -1,
       6,    -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,
      11,    -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,
      16,    -1,    17,    -1,    18,    -1,    19,    -1,    20,    -1,
      21,    -1,    22,    -1,    23,    -1,    56,    -1,    57,    -1,
      62,    -1,    59,    -1,    60,    -1,    61,    -1,    63,    -1,
     166,   167,    -1,   166,     1,    -1,    50,    -1,    -1,   188,
      -1,   169,   170,    -1,   169,     1,    -1,    51,    -1,    -1,
     188,    -1,   172,   173,    -1,   172,     1,    -1,    52,    -1,
     188,    -1,   175,   176,    -1,   175,     1,    -1,    53,    -1,
     188,    -1,   178,   179,    -1,   178,     1,    -1,    54,    -1,
     180,    -1,   179,   180,    -1,   182,    -1,   184,    -1,   212,
      -1,    55,    -1,    61,    72,    -1,   184,    -1,   183,   184,
      -1,   185,    -1,   191,    -1,   194,    -1,   197,    -1,   200,
      -1,    57,   186,    -1,   187,    -1,   186,    64,   187,    -1,
      73,    -1,   203,    -1,   189,   190,    -1,   190,    -1,    57,
     187,    -1,    60,   193,    -1,    60,   192,    -1,   193,    -1,
     192,    64,   193,    -1,    73,    -1,    62,   195,    -1,   196,
      -1,   195,    64,   196,    -1,    72,    -1,    72,    67,    72,
      -1,    59,   198,    -1,   199,    -1,   198,    64,   199,    -1,
      72,    -1,    72,    67,    72,    -1,    56,   201,    -1,   202,
      -1,   201,    64,   202,    -1,    72,    -1,    72,    67,    72,
      -1,    72,    -1,    72,    69,    72,    69,    72,    69,    72,
      -1,   207,    66,   205,    -1,   205,    -1,    19,    -1,    20,
      -1,    21,    -1,    22,    -1,    23,    -1,   207,    -1,   206,
      64,   207,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,    16,
      -1,    17,    -1,    18,    -1,   204,    70,    72,    -1,   209,
     204,    70,    72,    -1,    63,   211,    -1,    72,    -1,    72,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,    64,    64,    65,    68,    69,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   108,
     109,   111,   113,   114,   116,   117,   118,   122,   123,   125,
     127,   128,   130,   131,   132,   136,   137,   139,   141,   142,
     144,   145,   146,   150,   151,   153,   155,   156,   160,   161,
     163,   165,   166,   168,   169,   170,   174,   175,   177,   179,
     180,   182,   186,   187,   189,   191,   192,   194,   195,   196,
     200,   201,   203,   205,   209,   210,   212,   214,   215,   217,
     218,   219,   223,   224,   226,   228,   229,   230,   231,   235,
     236,   238,   240,   241,   245,   246,   248,   250,   251,   252,
     256,   257,   259,   261,   262,   263,   264,   268,   269,   271,
     273,   274,   275,   276,   280,   281,   283,   285,   286,   290,
     291,   293,   295,   296,   297,   301,   302,   304,   306,   307,
     308,   309,   313,   314,   316,   318,   319,   320,   321,   322,
     323,   324,   325,   329,   330,   332,   334,   335,   336,   337,
     341,   342,   344,   346,   347,   348,   349,   353,   354,   356,
     358,   359,   360,   361,   365,   366,   368,   370,   371,   372,
     376,   377,   379,   381,   382,   384,   388,   389,   391,   393,
     397,   398,   400,   402,   406,   407,   409,   411,   412,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   447,   448,   449,   450,   451,   453,   454,   455,   456,
     457,   458,   460,   461,   462,   463,   464,   466,   467,   468,
     469,   470,   472,   473,   474,   475,   476,   477,   478,   482,
     483,   485,   487,   488,   492,   493,   495,   497,   498,   502,
     503,   505,   507,   511,   512,   514,   516,   520,   521,   523,
     525,   526,   528,   529,   530,   534,   541,   544,   545,   548,
     549,   550,   551,   552,   555,   558,   559,   562,   563,   566,
     567,   569,   572,   575,   578,   579,   582,   584,   587,   588,
     591,   592,   595,   598,   599,   602,   603,   606,   609,   610,
     613,   614,   617,   618,   622,   623,   626,   627,   628,   629,
     630,   633,   634,   637,   638,   639,   640,   641,   642,   645,
     646,   647,   648,   649,   652,   653,   665,   676,   679
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "FOCUS", "ALL", "DATA", "GUI", "MPI", 
  "EXP_PCSAMP", "EXP_USERTIME", "EXP_MPI", "EXP_FPE", "EXP_HWC", "EXP_IO", 
  "VIEW_TOPN", "VIEW_EXCLTIME", "VIEW_IO", "VIEW_FPE", "VIEW_HWC", 
  "PARAM_DISPLAY_MODE", "PARAM_DISPLAY_MEDIA", 
  "PARAM_DISPLAY_REFRESH_RATE", "PARAM_EXP_SAVE_FILE", 
  "PARAM_SAMPLING_RATE", "ATTACH_HEAD", "CLOSE_HEAD", "CONT_HEAD", 
  "CREATE_HEAD", "DETACH_HEAD", "FOCUS_HEAD", "PAUSE_HEAD", 
  "RESTORE_HEAD", "RUN_HEAD", "SAVEFILE_HEAD", "SETPARAM_HEAD", 
  "STOP_HEAD", "VIEW_HEAD", "LIST_EXP_HEAD", "LIST_HOSTS_HEAD", 
  "LIST_OBJ_HEAD", "LIST_PIDS_HEAD", "LIST_SRC_HEAD", "LIST_METRICS_HEAD", 
  "LIST_PARAMS_HEAD", "LIST_REPORTS_HEAD", "LIST_BREAKS_HEAD", 
  "GEN_CLEAR_BREAK_HEAD", "GEN_EXIT_HEAD", "GEN_OPEN_GUI_HEAD", 
  "GEN_HELP_HEAD", "GEN_HISTORY_HEAD", "GEN_LOG_HEAD", 
  "GEN_PLAYBACK_HEAD", "GEN_RECORD_HEAD", "GEN_SETBREAK_HEAD", 
  "JUST_QUIT_HEAD", "RANK_ID", "HOST_ID", "RANK_PID", "THREAD_ID", 
  "FILE_ID", "EXP_ID", "PROCESS_ID", "LINE_ID", "COMMA", "SEMICOLON", 
  "DOUBLE_COLON", "COLON", "END_LINE", "DOT", "EQUAL", "HEXNUMBER", 
  "NUMBER", "NAME", "$accept", "ss_line", "command_line", "command_desc", 
  "exp_attach_com", "exp_attach_head", "exp_attach_args", 
  "exp_attach_arg", "exp_close_com", "exp_close_head", "exp_close_args", 
  "exp_close_arg", "exp_cont_com", "exp_cont_head", "exp_cont_args", 
  "exp_cont_arg", "exp_create_com", "exp_create_head", "exp_create_args", 
  "exp_detach_com", "exp_detach_head", "exp_detach_args", 
  "exp_detach_arg", "exp_focus_com", "exp_focus_head", "exp_focus_args", 
  "exp_focus_arg", "exp_pause_com", "exp_pause_head", "exp_pause_args", 
  "exp_pause_arg", "exp_restore_com", "exp_restore_head", 
  "exp_restore_arg", "exp_run_com", "exp_run_head", "exp_run_args", 
  "exp_run_arg", "exp_savefile_com", "exp_savefile_head", 
  "exp_savefile_arg", "exp_setparam_com", "exp_setparam_head", 
  "exp_setparam_arg", "exp_stop_com", "exp_stop_head", "exp_stop_arg", 
  "exp_view_com", "exp_view_head", "exp_view_arg", "list_exp_com", 
  "list_exp_head", "list_exp_arg", "list_hosts_com", "list_hosts_head", 
  "list_hosts_arg", "list_obj_com", "list_obj_head", "list_obj_arg", 
  "list_pids_com", "list_pids_head", "list_pids_arg", "list_src_com", 
  "list_src_head", "list_src_arg", "list_metrics_com", 
  "list_metrics_head", "list_metrics_arg", "list_params_com", 
  "list_params_head", "list_params_arg", "list_reports_com", 
  "list_reports_head", "list_reports_arg", "list_breaks_com", 
  "list_breaks_head", "list_breaks_arg", "gen_clear_break_com", 
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
     325,   326,   327,   328
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    74,    75,    75,    76,    76,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    78,
      78,    79,    80,    80,    81,    81,    81,    82,    82,    83,
      84,    84,    85,    85,    85,    86,    86,    87,    88,    88,
      89,    89,    89,    90,    90,    91,    92,    92,    93,    93,
      94,    95,    95,    96,    96,    96,    97,    97,    98,    99,
      99,   100,   101,   101,   102,   103,   103,   104,   104,   104,
     105,   105,   106,   107,   108,   108,   109,   110,   110,   111,
     111,   111,   112,   112,   113,   114,   114,   114,   114,   115,
     115,   116,   117,   117,   118,   118,   119,   120,   120,   120,
     121,   121,   122,   123,   123,   123,   123,   124,   124,   125,
     126,   126,   126,   126,   127,   127,   128,   129,   129,   130,
     130,   131,   132,   132,   132,   133,   133,   134,   135,   135,
     135,   135,   136,   136,   137,   138,   138,   138,   138,   138,
     138,   138,   138,   139,   139,   140,   141,   141,   141,   141,
     142,   142,   143,   144,   144,   144,   144,   145,   145,   146,
     147,   147,   147,   147,   148,   148,   149,   150,   150,   150,
     151,   151,   152,   153,   153,   154,   155,   155,   156,   157,
     158,   158,   159,   160,   161,   161,   162,   163,   163,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   165,
     165,   166,   167,   167,   168,   168,   169,   170,   170,   171,
     171,   172,   173,   174,   174,   175,   176,   177,   177,   178,
     179,   179,   180,   180,   180,   181,   182,   183,   183,   184,
     184,   184,   184,   184,   185,   186,   186,   187,   187,   188,
     188,   189,   190,   191,   192,   192,   193,   194,   195,   195,
     196,   196,   197,   198,   198,   199,   199,   200,   201,   201,
     202,   202,   203,   203,   204,   204,   205,   205,   205,   205,
     205,   206,   206,   207,   207,   207,   207,   207,   207,   208,
     208,   208,   208,   208,   209,   209,   210,   211,   212
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     2,     1,     1,     1,     2,     2,     1,
       1,     2,     0,     1,     1,     2,     2,     1,     1,     2,
       0,     1,     1,     2,     2,     1,     1,     2,     2,     2,
       1,     1,     2,     1,     1,     1,     2,     2,     1,     1,
       2,     1,     2,     2,     1,     1,     2,     0,     1,     1,
       2,     2,     1,     1,     2,     2,     1,     1,     2,     0,
       1,     1,     2,     2,     1,     3,     3,     2,     2,     2,
       2,     1,     2,     1,     2,     2,     1,     0,     1,     1,
       2,     2,     1,     3,     2,     2,     1,     2,     2,     1,
       0,     1,     1,     1,     2,     2,     1,     0,     1,     2,
       2,     1,     0,     1,     1,     2,     2,     1,     0,     1,
       2,     1,     2,     2,     1,     0,     1,     2,     2,     3,
       1,     2,     1,     2,     2,     1,     0,     1,     1,     1,
       2,     2,     1,     0,     1,     1,     1,     2,     2,     1,
       0,     1,     1,     1,     2,     2,     1,     0,     1,     1,
       2,     2,     1,     1,     2,     1,     2,     2,     1,     0,
       2,     2,     1,     0,     1,     2,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     1,     0,     1,     2,     2,     1,     0,     1,     2,
       2,     1,     1,     2,     2,     1,     1,     2,     2,     1,
       1,     2,     1,     1,     1,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     2,     1,     3,     1,     1,     2,
       1,     2,     2,     2,     1,     3,     1,     2,     1,     3,
       1,     3,     2,     1,     3,     1,     3,     2,     1,     3,
       1,     3,     1,     7,     3,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     4,     2,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short yydefact[] =
{
       2,     0,     1,    38,    41,    49,    57,    65,    70,    78,
      84,    92,    96,   104,   111,   116,   122,   129,   136,   141,
     147,   154,   165,   172,   179,   186,   192,   198,   202,   206,
     271,   276,   281,   285,   289,   295,     4,     3,     0,     6,
       0,     7,     0,     8,     0,     9,     0,    10,     0,    11,
       0,    12,     0,    13,     0,    14,     0,    15,     0,    16,
       0,    17,     0,    18,     0,    19,     0,    20,     0,    21,
       0,    22,     0,    23,     0,    24,     0,    25,     0,    26,
       0,    27,     0,    28,     0,    29,     0,    30,     0,    31,
     204,    32,     0,    33,     0,    34,     0,    35,     0,    36,
       0,    37,     5,    40,   343,   344,   345,   346,   347,   348,
       0,     0,     0,     0,     0,     0,    39,    42,    45,    44,
     297,   299,   300,   301,   302,   303,    46,   341,    48,    54,
      47,    50,    53,    56,    62,    55,    58,    61,    64,    63,
      66,    69,    68,    71,    74,    73,    75,    77,    76,    79,
      81,    83,    89,    82,    85,    88,    91,     0,     0,    90,
      93,     0,   310,    95,   101,    94,    97,   100,   103,     0,
       0,   102,     0,   110,   336,   337,   338,   339,   340,   109,
       0,     0,   335,     0,   113,   115,   119,   114,   118,   121,
       0,   349,   350,   351,   352,   353,   120,     0,   126,   128,
     131,   133,   127,   132,   135,   134,   138,   140,   139,   143,
     144,   146,   151,   145,   149,   153,     0,   152,   156,   160,
     162,   164,   167,   169,   163,   168,   171,   174,   176,   170,
     175,   178,   181,   183,   177,   182,   185,   189,   184,   188,
     191,   195,   190,   193,   197,   196,   201,   200,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     262,   263,   265,   266,   267,   264,   268,   209,   205,   207,
     270,   269,   273,   275,   274,   278,   280,   279,   282,   284,
     283,   286,   288,   358,   287,   290,   292,   293,   294,   330,
     327,   328,   332,   307,   304,   305,   308,   325,   322,   323,
     316,   313,   314,   296,   320,   317,   318,    43,   298,     0,
      51,    59,    67,    72,    80,    86,   311,   312,   309,    98,
     107,   108,     0,     0,   112,     0,     0,     0,   124,     0,
     125,   150,   357,   356,   157,   158,   161,   194,   208,   291,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   342,
     105,   106,   354,   334,     0,   123,   159,   331,   329,     0,
     306,   326,   324,   315,   321,   319,   355,     0,     0,     0,
     333
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,    37,    38,    39,    40,   116,   117,    41,    42,
     130,   131,    43,    44,   135,   136,    45,    46,   139,    47,
      48,   142,   143,    49,    50,   148,   149,    51,    52,   153,
     154,    53,    54,   159,    55,    56,   165,   166,    57,    58,
     171,    59,    60,   179,    61,    62,   187,    63,    64,   196,
      65,    66,   202,    67,    68,   205,    69,    70,   208,    71,
      72,   213,    73,    74,   217,    75,    76,   224,    77,    78,
     229,    79,    80,   234,    81,    82,   238,    83,    84,   242,
     243,    85,    86,   245,    87,    88,   247,    89,    90,   308,
     309,    91,    92,   311,    93,    94,   314,    95,    96,   317,
      97,    98,   320,    99,   100,   324,   325,   101,   118,   119,
     120,   121,   334,   335,   160,   161,   162,   122,   341,   342,
     123,   345,   346,   124,   338,   339,   125,   330,   331,   336,
     181,   182,   126,   127,   198,   184,   220,   373,   328
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -186
static const short yypact[] =
{
    -186,   166,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,   -16,  -186,
     360,  -186,    83,  -186,   102,  -186,    98,  -186,   373,  -186,
      53,  -186,   118,  -186,   224,  -186,   161,  -186,   222,  -186,
     379,  -186,   167,  -186,   235,  -186,    44,  -186,    20,  -186,
      34,  -186,    82,  -186,    72,  -186,    52,  -186,    56,  -186,
      77,  -186,   171,  -186,    14,  -186,    30,  -186,    40,  -186,
     303,  -186,   117,  -186,   180,  -186,   232,  -186,   241,  -186,
       7,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
     -11,   -22,    -7,   -36,     2,    10,   395,  -186,  -186,   417,
    -186,  -186,  -186,  -186,  -186,  -186,    -6,  -186,  -186,  -186,
      15,  -186,  -186,  -186,  -186,   112,  -186,  -186,  -186,  -186,
     402,  -186,   395,  -186,  -186,   417,    -6,  -186,    24,  -186,
    -186,  -186,  -186,   119,  -186,  -186,  -186,   -22,   -36,  -186,
    -186,    66,  -186,  -186,  -186,   234,  -186,  -186,  -186,   -14,
     -14,  -186,    67,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
     254,    60,  -186,    70,   254,  -186,  -186,  -186,  -186,  -186,
     229,  -186,  -186,  -186,  -186,  -186,  -186,   430,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,   130,  -186,    69,  -186,   409,   409,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,    71,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,   303,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,   199,  -186,  -186,  -186,  -186,    79,
      84,  -186,    78,  -186,    88,  -186,  -186,    86,    92,  -186,
    -186,    95,  -186,  -186,    94,   100,  -186,  -186,  -186,   415,
    -186,  -186,    -6,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,   -14,   -14,   254,    97,   374,   106,  -186,   229,
    -186,  -186,  -186,  -186,   409,  -186,  -186,  -186,  -186,  -186,
     110,   -11,   152,   -22,   185,    -7,   -36,   196,    10,  -186,
    -186,  -186,  -186,  -186,   197,  -186,  -186,  -186,  -186,   109,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,   198,   115,   200,
    -186
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -186,  -186,  -186,  -186,  -186,  -186,  -186,   162,  -186,  -186,
    -186,   149,  -186,  -186,  -186,   145,  -186,  -186,  -186,  -186,
    -186,  -186,   140,  -186,  -186,  -186,   137,  -186,  -186,  -186,
     134,  -186,  -186,  -186,  -186,  -186,  -186,   123,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
      49,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
      -9,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,  -186,
    -186,  -186,  -186,  -186,  -186,  -186,   -27,  -186,   -38,   -35,
     -70,  -186,  -186,  -152,   -69,   290,   139,  -186,  -186,  -156,
    -186,  -186,   -86,  -186,  -186,   -82,  -186,  -186,    -4,  -186,
    -177,     1,   -31,   -59,  -181,   195,  -185,  -186,  -186
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -278
static const short yytable[] =
{
     210,   183,   357,   214,   132,   356,   137,   367,   322,   368,
     144,   140,   150,   145,   155,   240,   370,   146,   167,   129,
     172,   204,   180,   312,   188,   315,   197,   318,   203,   321,
     327,   244,   209,   375,   376,   207,   218,   340,   225,   219,
     230,   246,   235,   157,   239,   199,   158,   200,   201,   348,
     332,   333,   102,   221,   147,   222,   223,   226,   349,   227,
     228,   329,   326,   110,   111,   337,   112,   113,   114,   115,
     348,   362,   363,   215,   343,   348,   114,   157,   231,   323,
     232,   233,   344,   211,   128,   114,   241,   129,  -137,   212,
     110,   111,   132,   112,   113,   114,   115,   137,  -199,   138,
     360,   361,  -142,   133,   144,   114,   134,   145,  -203,   352,
     150,   146,  -130,   114,   114,   155,   134,   114,   310,   151,
    -166,   183,   152,   152,  -173,   183,   158,   167,   110,   111,
     365,   112,   113,   114,   115,   216,   366,   371,   114,   157,
    -155,   372,   158,   241,   114,  -180,   380,   382,   381,   348,
    -148,   -52,   383,   384,   110,   111,   385,   112,   113,   386,
     115,   387,   163,   114,   388,   164,     2,     3,   185,   392,
     -60,   186,   236,   114,   157,   237,   394,   158,   407,   114,
     114,   313,   397,   374,   409,  -272,   -87,   367,   395,   396,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,   114,   168,   399,   156,   169,   170,   114,   -99,
     403,   400,   114,   316,    36,  -117,   189,   157,   164,  -187,
     158,   190,   319,   191,   192,   193,   194,   195,  -277,   191,
     192,   193,   194,   195,   327,   110,   111,   401,   112,   113,
     114,   115,   104,   105,   106,   107,   108,   109,   404,   406,
     408,   323,   410,   174,   175,   176,   177,   178,   347,   350,
     351,   157,   353,   114,   158,   354,   326,   355,   359,   157,
     389,   377,   158,   390,   391,   114,   114,   379,   157,   378,
     358,   158,   405,   402,   348,   183,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   206,   300,
     301,   103,   302,   303,   304,   305,   306,   393,   104,   105,
     106,   107,   108,   109,   141,   364,   307,   398,     0,     0,
     173,   104,   105,   106,   107,   108,   109,   104,   105,   106,
     107,   108,   109,   174,   175,   176,   177,   178,   174,   175,
     176,   177,   178,   104,   105,   106,   107,   108,   109,     0,
     104,   105,   106,   107,   108,   109,   110,   111,     0,   112,
     113,   114,   115,   104,   105,   106,   107,   108,   109,   110,
     111,     0,   112,   113,   114,   115,   369,     0,     0,     0,
     114,     0,     0,     0,   191,   192,   193,   194,   195,     0,
       0,   110,   111,     0,   112,   113,   114,   115,   110,   111,
       0,   112,   113,     0,   115,   110,   111,     0,   112,   113,
       0,   115,   216,   110,   111,     0,   112,   113,     0,   115
};

static const short yycheck[] =
{
      70,    60,   158,    72,    42,   157,    44,   184,     1,   190,
      48,    46,    50,    48,    52,     1,   197,    48,    56,     4,
      58,     1,    60,    92,    62,    94,    64,    96,    66,    98,
     100,     1,    70,   218,   219,     1,    74,    73,    76,    74,
      78,     1,    80,    57,    82,     1,    60,     3,     4,   119,
      72,    73,    68,     1,     1,     3,     4,     1,    64,     3,
       4,    72,   100,    56,    57,    72,    59,    60,    61,    62,
     140,     4,     5,     1,    72,   145,    61,    57,     1,    72,
       3,     4,    72,     1,     1,    61,    72,     4,    68,     7,
      56,    57,   130,    59,    60,    61,    62,   135,    68,     1,
     169,   170,    68,     1,   142,    61,     4,   142,    68,   140,
     148,   142,    68,    61,    61,   153,     4,    61,     1,     1,
      68,   180,     4,     4,    68,   184,    60,   165,    56,    57,
      70,    59,    60,    61,    62,    63,    66,     7,    61,    57,
      68,    72,    60,    72,    61,    68,    67,    69,    64,   219,
      68,    68,    64,    67,    56,    57,    64,    59,    60,    64,
      62,    67,     1,    61,    64,     4,     0,     1,     1,    72,
      68,     4,     1,    61,    57,     4,    70,    60,    69,    61,
      61,     1,    72,   218,    69,    68,    68,   364,   369,   374,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    61,     1,    72,     1,     4,     5,    61,    68,
     386,   383,    61,     1,    68,    68,     1,    57,     4,    68,
      60,     6,     1,    14,    15,    16,    17,    18,    68,    14,
      15,    16,    17,    18,   324,    56,    57,    72,    59,    60,
      61,    62,     8,     9,    10,    11,    12,    13,    72,    72,
      72,    72,    72,    19,    20,    21,    22,    23,   116,   130,
     135,    57,   142,    61,    60,   148,   324,   153,   165,    57,
     349,   242,    60,   362,   363,    61,    61,   324,    57,   308,
     161,    60,   388,   385,   374,   364,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    68,    56,
      57,     1,    59,    60,    61,    62,    63,   366,     8,     9,
      10,    11,    12,    13,     1,   180,    73,   381,    -1,    -1,
       1,     8,     9,    10,    11,    12,    13,     8,     9,    10,
      11,    12,    13,    19,    20,    21,    22,    23,    19,    20,
      21,    22,    23,     8,     9,    10,    11,    12,    13,    -1,
       8,     9,    10,    11,    12,    13,    56,    57,    -1,    59,
      60,    61,    62,     8,     9,    10,    11,    12,    13,    56,
      57,    -1,    59,    60,    61,    62,     6,    -1,    -1,    -1,
      61,    -1,    -1,    -1,    14,    15,    16,    17,    18,    -1,
      -1,    56,    57,    -1,    59,    60,    61,    62,    56,    57,
      -1,    59,    60,    -1,    62,    56,    57,    -1,    59,    60,
      -1,    62,    63,    56,    57,    -1,    59,    60,    -1,    62
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    75,     0,     1,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    68,    76,    77,    78,
      79,    82,    83,    86,    87,    90,    91,    93,    94,    97,
      98,   101,   102,   105,   106,   108,   109,   112,   113,   115,
     116,   118,   119,   121,   122,   124,   125,   127,   128,   130,
     131,   133,   134,   136,   137,   139,   140,   142,   143,   145,
     146,   148,   149,   151,   152,   155,   156,   158,   159,   161,
     162,   165,   166,   168,   169,   171,   172,   174,   175,   177,
     178,   181,    68,     1,     8,     9,    10,    11,    12,    13,
      56,    57,    59,    60,    61,    62,    80,    81,   182,   183,
     184,   185,   191,   194,   197,   200,   206,   207,     1,     4,
      84,    85,   182,     1,     4,    88,    89,   182,     1,    92,
     183,     1,    95,    96,   182,   183,   206,     1,    99,   100,
     182,     1,     4,   103,   104,   182,     1,    57,    60,   107,
     188,   189,   190,     1,     4,   110,   111,   182,     1,     4,
       5,   114,   182,     1,    19,    20,    21,    22,    23,   117,
     182,   204,   205,   207,   209,     1,     4,   120,   182,     1,
       6,    14,    15,    16,    17,    18,   123,   182,   208,     1,
       3,     4,   126,   182,     1,   129,   189,     1,   132,   182,
     184,     1,     7,   135,   188,     1,    63,   138,   182,   183,
     210,     1,     3,     4,   141,   182,     1,     3,     4,   144,
     182,     1,     3,     4,   147,   182,     1,     4,   150,   182,
       1,    72,   153,   154,     1,   157,     1,   160,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      56,    57,    59,    60,    61,    62,    63,    73,   163,   164,
       1,   167,   188,     1,   170,   188,     1,   173,   188,     1,
     176,   188,     1,    72,   179,   180,   182,   184,   212,    72,
     201,   202,    72,    73,   186,   187,   203,    72,   198,   199,
      73,   192,   193,    72,    72,   195,   196,    81,   184,    64,
      85,    89,   206,    96,   100,   104,   187,   193,   190,   111,
     188,   188,     4,     5,   209,    70,    66,   204,   208,     6,
     208,     7,    72,   211,   183,   210,   210,   154,   164,   180,
      67,    64,    69,    64,    67,    64,    64,    67,    64,   207,
     188,   188,    72,   205,    70,   208,   210,    72,   202,    72,
     187,    72,   199,   193,    72,   196,    72,    69,    72,    69,
      72
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
#line 68 "test.y"
    {/* printf("OSS > "); */ }
    break;

  case 5:
#line 69 "test.y"
    {dump_command(); /* printf("OSS > "); */ }
    break;

  case 6:
#line 72 "test.y"
    {set_command_type(CMD_EXP_ATTACH);}
    break;

  case 7:
#line 73 "test.y"
    {set_command_type(CMD_EXP_CLOSE);}
    break;

  case 8:
#line 74 "test.y"
    {set_command_type(CMD_EXP_CONT);}
    break;

  case 9:
#line 75 "test.y"
    {set_command_type(CMD_EXP_CREATE);}
    break;

  case 10:
#line 76 "test.y"
    {set_command_type(CMD_EXP_DETACH);}
    break;

  case 11:
#line 77 "test.y"
    {set_command_type(CMD_EXP_FOCUS);}
    break;

  case 12:
#line 78 "test.y"
    {set_command_type(CMD_EXP_PAUSE);}
    break;

  case 13:
#line 79 "test.y"
    {set_command_type(CMD_EXP_RESTORE);}
    break;

  case 14:
#line 80 "test.y"
    {set_command_type(CMD_EXP_RUN);}
    break;

  case 15:
#line 81 "test.y"
    {set_command_type(CMD_EXP_SAVEFILE);}
    break;

  case 16:
#line 82 "test.y"
    {set_command_type(CMD_EXP_SETPARAM);}
    break;

  case 17:
#line 83 "test.y"
    {set_command_type(CMD_EXP_STOP);}
    break;

  case 18:
#line 84 "test.y"
    {set_command_type(CMD_EXP_VIEW);}
    break;

  case 19:
#line 85 "test.y"
    {set_command_type(CMD_LIST_EXP);}
    break;

  case 20:
#line 86 "test.y"
    {set_command_type(CMD_LIST_HOSTS);}
    break;

  case 21:
#line 87 "test.y"
    {set_command_type(CMD_LIST_OBJ);}
    break;

  case 22:
#line 88 "test.y"
    {set_command_type(CMD_LIST_PIDS);}
    break;

  case 23:
#line 89 "test.y"
    {set_command_type(CMD_LIST_SRC);}
    break;

  case 24:
#line 90 "test.y"
    {set_command_type(CMD_LIST_METRICS);}
    break;

  case 25:
#line 91 "test.y"
    {set_command_type(CMD_LIST_PARAMS);}
    break;

  case 26:
#line 92 "test.y"
    {set_command_type(CMD_LIST_REPORTS);}
    break;

  case 27:
#line 93 "test.y"
    {set_command_type(CMD_LIST_BREAKS);}
    break;

  case 28:
#line 94 "test.y"
    {set_command_type(CMD_CLEAR_BREAK);}
    break;

  case 29:
#line 95 "test.y"
    {set_command_type(CMD_EXIT);exit(0);}
    break;

  case 30:
#line 96 "test.y"
    {set_command_type(CMD_OPEN_GUI);}
    break;

  case 31:
#line 97 "test.y"
    {set_command_type(CMD_HELP);}
    break;

  case 32:
#line 98 "test.y"
    {set_command_type(CMD_HISTORY);}
    break;

  case 33:
#line 99 "test.y"
    {set_command_type(CMD_LOG);}
    break;

  case 34:
#line 100 "test.y"
    {set_command_type(CMD_PLAYBACK);}
    break;

  case 35:
#line 101 "test.y"
    {set_command_type(CMD_RECORD);}
    break;

  case 36:
#line 102 "test.y"
    {set_command_type(CMD_SETBREAK);}
    break;

  case 37:
#line 103 "test.y"
    {exit(0);}
    break;

  case 40:
#line 109 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_ATTACH]);}
    break;

  case 48:
#line 123 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_CLOSE]);}
    break;

  case 56:
#line 137 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_CONT]);}
    break;

  case 64:
#line 151 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_CREATE]);}
    break;

  case 69:
#line 161 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_DETACH]);}
    break;

  case 77:
#line 175 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_FOCUS]);}
    break;

  case 83:
#line 187 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_PAUSE]);}
    break;

  case 91:
#line 201 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_RESTORE]);}
    break;

  case 95:
#line 210 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_RUN]);}
    break;

  case 103:
#line 224 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_SAVEFILE]);}
    break;

  case 110:
#line 236 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_SETPARAM]);}
    break;

  case 115:
#line 246 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_STOP]);}
    break;

  case 121:
#line 257 "test.y"
    {set_error(yylval.string,command_name[CMD_EXP_VIEW]);}
    break;

  case 128:
#line 269 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_EXP]);}
    break;

  case 135:
#line 281 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_HOSTS]);}
    break;

  case 140:
#line 291 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_OBJ]);}
    break;

  case 146:
#line 302 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_PIDS]);}
    break;

  case 153:
#line 314 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_SRC]);}
    break;

  case 164:
#line 330 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_METRICS]);}
    break;

  case 171:
#line 342 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_PARAMS]);}
    break;

  case 178:
#line 354 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_REPORTS]);}
    break;

  case 185:
#line 366 "test.y"
    {set_error(yylval.string,command_name[CMD_LIST_BREAKS]);}
    break;

  case 191:
#line 377 "test.y"
    {set_error(yylval.string,command_name[CMD_CLEAR_BREAK]);}
    break;

  case 193:
#line 381 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_BREAK);}
    break;

  case 194:
#line 382 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_BREAK);}
    break;

  case 197:
#line 389 "test.y"
    {set_error(yylval.string,command_name[CMD_EXIT]);}
    break;

  case 201:
#line 398 "test.y"
    {set_error(yylval.string,command_name[CMD_OPEN_GUI]);}
    break;

  case 209:
#line 414 "test.y"
    {push_help(yyvsp[0].string);}
    break;

  case 210:
#line 415 "test.y"
    {push_help(command_name[CMD_EXP_ATTACH]);}
    break;

  case 211:
#line 416 "test.y"
    {push_help(command_name[CMD_EXP_CLOSE]);}
    break;

  case 212:
#line 417 "test.y"
    {push_help(command_name[CMD_EXP_CONT]);}
    break;

  case 213:
#line 418 "test.y"
    {push_help(command_name[CMD_EXP_CREATE]);}
    break;

  case 214:
#line 419 "test.y"
    {push_help(command_name[CMD_EXP_DETACH]);}
    break;

  case 215:
#line 420 "test.y"
    {push_help(command_name[CMD_EXP_FOCUS]);}
    break;

  case 216:
#line 421 "test.y"
    {push_help(command_name[CMD_EXP_PAUSE]);}
    break;

  case 217:
#line 422 "test.y"
    {push_help(command_name[CMD_EXP_RESTORE]);}
    break;

  case 218:
#line 423 "test.y"
    {push_help(command_name[CMD_EXP_RUN]);}
    break;

  case 219:
#line 424 "test.y"
    {push_help(command_name[CMD_EXP_SAVEFILE]);}
    break;

  case 220:
#line 425 "test.y"
    {push_help(command_name[CMD_EXP_SETPARAM]);}
    break;

  case 221:
#line 426 "test.y"
    {push_help(command_name[CMD_EXP_STOP]);}
    break;

  case 222:
#line 427 "test.y"
    {push_help(command_name[CMD_EXP_VIEW]);}
    break;

  case 223:
#line 428 "test.y"
    {push_help(command_name[CMD_LIST_EXP]);}
    break;

  case 224:
#line 429 "test.y"
    {push_help(command_name[CMD_LIST_HOSTS]);}
    break;

  case 225:
#line 430 "test.y"
    {push_help(command_name[CMD_LIST_OBJ]);}
    break;

  case 226:
#line 431 "test.y"
    {push_help(command_name[CMD_LIST_PIDS]);}
    break;

  case 227:
#line 432 "test.y"
    {push_help(command_name[CMD_LIST_SRC]);}
    break;

  case 228:
#line 433 "test.y"
    {push_help(command_name[CMD_LIST_METRICS]);}
    break;

  case 229:
#line 434 "test.y"
    {push_help(command_name[CMD_LIST_PARAMS]);}
    break;

  case 230:
#line 435 "test.y"
    {push_help(command_name[CMD_LIST_REPORTS]);}
    break;

  case 231:
#line 436 "test.y"
    {push_help(command_name[CMD_LIST_BREAKS]);}
    break;

  case 232:
#line 437 "test.y"
    {push_help(command_name[CMD_CLEAR_BREAK]);}
    break;

  case 233:
#line 438 "test.y"
    {push_help(command_name[CMD_EXIT]);}
    break;

  case 234:
#line 439 "test.y"
    {push_help(command_name[CMD_OPEN_GUI]);}
    break;

  case 235:
#line 440 "test.y"
    {push_help(command_name[CMD_HELP]);}
    break;

  case 236:
#line 441 "test.y"
    {push_help(command_name[CMD_HISTORY]);}
    break;

  case 237:
#line 442 "test.y"
    {push_help(command_name[CMD_LOG]);}
    break;

  case 238:
#line 443 "test.y"
    {push_help(command_name[CMD_PLAYBACK]);}
    break;

  case 239:
#line 444 "test.y"
    {push_help(command_name[CMD_RECORD]);}
    break;

  case 240:
#line 445 "test.y"
    {push_help(command_name[CMD_SETBREAK]);}
    break;

  case 241:
#line 447 "test.y"
    {push_help(general_name[H_GEN_FOCUS]);}
    break;

  case 242:
#line 448 "test.y"
    {push_help(general_name[H_GEN_ALL]);}
    break;

  case 243:
#line 449 "test.y"
    {push_help(general_name[H_GEN_DATA]);}
    break;

  case 244:
#line 450 "test.y"
    {push_help(general_name[H_GEN_GUI]);}
    break;

  case 245:
#line 451 "test.y"
    {push_help("-mpi");}
    break;

  case 246:
#line 453 "test.y"
    {push_help(experiment_name[H_EXP_PCSAMP]);}
    break;

  case 247:
#line 454 "test.y"
    {push_help(experiment_name[H_EXP_USERTIME]);}
    break;

  case 248:
#line 455 "test.y"
    {push_help(experiment_name[H_EXP_MPI]);}
    break;

  case 249:
#line 456 "test.y"
    {push_help(experiment_name[H_EXP_FPE]);}
    break;

  case 250:
#line 457 "test.y"
    {push_help(experiment_name[H_EXP_HWC]);}
    break;

  case 251:
#line 458 "test.y"
    {push_help(experiment_name[H_EXP_IO]);}
    break;

  case 252:
#line 460 "test.y"
    {push_help(viewtype_name[H_VIEW_TOPN]);}
    break;

  case 253:
#line 461 "test.y"
    {push_help(viewtype_name[H_VIEW_EXCLTIME]);}
    break;

  case 254:
#line 462 "test.y"
    {push_help(viewtype_name[H_VIEW_IO]);}
    break;

  case 255:
#line 463 "test.y"
    {push_help(viewtype_name[H_VIEW_FPE]);}
    break;

  case 256:
#line 464 "test.y"
    {push_help(viewtype_name[H_VIEW_HWC]);}
    break;

  case 257:
#line 466 "test.y"
    {push_help(paramtype_name[H_PARAM_DMODE]);}
    break;

  case 258:
#line 467 "test.y"
    {push_help(paramtype_name[H_PARAM_DMEDIA]);}
    break;

  case 259:
#line 468 "test.y"
    {push_help(paramtype_name[H_PARAM_DREFRESH]);}
    break;

  case 260:
#line 469 "test.y"
    {push_help(paramtype_name[H_PARAM_XSAVEFILE]);}
    break;

  case 261:
#line 470 "test.y"
    {push_help(paramtype_name[H_PARAM_SAMPRATE]);}
    break;

  case 262:
#line 472 "test.y"
    {push_help("-r");}
    break;

  case 263:
#line 473 "test.y"
    {push_help("-h");}
    break;

  case 264:
#line 474 "test.y"
    {push_help("-p");}
    break;

  case 265:
#line 475 "test.y"
    {push_help("-t");}
    break;

  case 266:
#line 476 "test.y"
    {push_help("-f");}
    break;

  case 267:
#line 477 "test.y"
    {push_help("-x");}
    break;

  case 268:
#line 478 "test.y"
    {push_help("-l");}
    break;

  case 270:
#line 483 "test.y"
    {set_error(yylval.string,command_name[CMD_HISTORY]);}
    break;

  case 275:
#line 493 "test.y"
    {set_error(yylval.string,command_name[CMD_LOG]);}
    break;

  case 280:
#line 503 "test.y"
    {set_error(yylval.string,command_name[CMD_PLAYBACK]);}
    break;

  case 284:
#line 512 "test.y"
    {set_error(yylval.string,command_name[CMD_RECORD]);}
    break;

  case 288:
#line 521 "test.y"
    {set_error(yylval.string,command_name[CMD_SETBREAK]);}
    break;

  case 294:
#line 530 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_ADDRESS);}
    break;

  case 296:
#line 541 "test.y"
    {set_exp_id(yyvsp[0].val);}
    break;

  case 307:
#line 562 "test.y"
    {push_host_name(yyvsp[0].string);}
    break;

  case 308:
#line 563 "test.y"
    {push_host_ip(yyvsp[0].val);}
    break;

  case 314:
#line 578 "test.y"
    {push_file(yyvsp[0].string);}
    break;

  case 315:
#line 579 "test.y"
    {push_file(yyvsp[0].string);}
    break;

  case 316:
#line 582 "test.y"
    {push_file(yyvsp[0].string);}
    break;

  case 320:
#line 591 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_PID);}
    break;

  case 321:
#line 592 "test.y"
    {push_32bit_range(yyvsp[-2].val,yyvsp[0].val,TABLE_PID);}
    break;

  case 325:
#line 602 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_THREAD);}
    break;

  case 326:
#line 603 "test.y"
    {push_32bit_range(yyvsp[-2].val,yyvsp[0].val,TABLE_THREAD);}
    break;

  case 330:
#line 613 "test.y"
    {push_32bit_value(yyvsp[0].val,TABLE_RANK);}
    break;

  case 331:
#line 614 "test.y"
    {push_32bit_range(yyvsp[-2].val,yyvsp[0].val,TABLE_RANK);}
    break;

  case 333:
#line 619 "test.y"
    { yyval.val = (yyvsp[-6].val<<24) | (yyvsp[-4].val<<16) | (yyvsp[-2].val<<8) | yyvsp[0].val;}
    break;

  case 336:
#line 626 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 337:
#line 627 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 338:
#line 628 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 339:
#line 629 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 340:
#line 630 "test.y"
    {push_string(yyvsp[0].string,NAME_PARAM);}
    break;

  case 343:
#line 637 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 344:
#line 638 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 345:
#line 639 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 346:
#line 640 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 347:
#line 641 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 348:
#line 642 "test.y"
    {push_string(yyvsp[0].string,NAME_EXPERIMENT);}
    break;

  case 349:
#line 645 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 350:
#line 646 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 351:
#line 647 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 352:
#line 648 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;

  case 353:
#line 649 "test.y"
    {push_string(yyvsp[0].string,NAME_VIEW_TYPE);}
    break;


    }

/* Line 991 of yacc.c.  */
#line 2425 "y.tab.c"

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


#line 683 "test.y"



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


