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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 15 "test.y"
typedef union YYSTYPE {
int val;
char *string;
} YYSTYPE;
/* Line 1249 of yacc.c.  */
#line 187 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



