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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 15 "test.y"
typedef union YYSTYPE {
int val;
char *string;
} YYSTYPE;
/* Line 1249 of yacc.c.  */
#line 193 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



