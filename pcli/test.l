/* //////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////// */

/*[0-7]+.[0-7]+.[0-7]+.[0-7]+  yylval.val = ipnum(yytext); return NUMBER; } */

%{
#include <stdio.h>
#include <stdlib.h>
//
// Note: This dummy comment is used to eliminate a false-positive found by
// the SGI source scanner when comparing against AT&T Unix SVR4.1ES source.
//

#include <ctype.h>
#include <string.h>
#include "y.tab.h"
extern int yylex();
extern char * copy_string(char *);
extern int ip_number(char *,int);
%}

%%
","     { return COMMA; }
";" 	{ return SEMICOLON; }
"::" 	{ return DOUBLE_COLON; }
":" 	{ return COLON; }
"." 	{ return DOT; }
"=" 	{ return EQUAL; }

"focus"     	{ return FOCUS;}
"all"	    	{ return ALL;}
"copy"	    	{ return COPY;}
"-gui"	    	{ return GUI;}
"kill"	    	{ return KILL;}
"-mpi"	    	{ return MPI;}

"expattach" 	{ return ATTACH_HEAD; }
"expclose"  	{ return CLOSE_HEAD; }
"expcreate" 	{ return CREATE_HEAD; }
"expdetach" 	{ return DETACH_HEAD; }
"expdisable" 	{ return DISABLE_HEAD; }
"expenable" 	{ return ENABLE_HEAD; }
"expfocus"  	{ return FOCUS_HEAD; }
"exppause"  	{ return PAUSE_HEAD; }
"exprestore"	{ return RESTORE_HEAD; }
"expgo"    	{ return GO_HEAD; }
"expsave"	{ return SAVE_HEAD; }
"expsetparam"	{ return SETPARAM_HEAD; }
"expview"   	{ return VIEW_HEAD; }

"listbreaks"	{ return LIST_BREAKS_HEAD; }
"listexp"   	{ return LIST_EXP_HEAD; }
"listhosts" 	{ return LIST_HOSTS_HEAD; }
"listmetrics"	{ return LIST_METRICS_HEAD; }
"listobj"   	{ return LIST_OBJ_HEAD; }
"listparams"	{ return LIST_PARAMS_HEAD; }
"listpids"  	{ return LIST_PIDS_HEAD; }
"listranks"	{ return LIST_RANKS_HEAD; }
"listsrc"   	{ return LIST_SRC_HEAD; }
"liststatus"   	{ return LIST_STATUS_HEAD; }
"listthreads"	{ return LIST_THREADS_HEAD; }
"listtypes"	{ return LIST_TYPES_HEAD; }
"listviews"	{ return LIST_VIEWS_HEAD; }

"clearbreak"	{ return GEN_CLEAR_BREAK_HEAD; }
"exit"	    	{ return GEN_EXIT_HEAD; }
"opengui"   	{ return GEN_OPEN_GUI_HEAD; }
"help"	    	{ return GEN_HELP_HEAD; }
"history"   	{ return GEN_HISTORY_HEAD; }
"log"	    	{ return GEN_LOG_HEAD; }
"playback"  	{ return GEN_PLAYBACK_HEAD; }
"record"    	{ return GEN_RECORD_HEAD; }
"setbreak"  	{ return GEN_SETBREAK_HEAD; }

"pcsamp"  	{ yylval.string = copy_string(yytext); return EXP_PCSAMP;}
"usertime"  	{ yylval.string = copy_string(yytext); return EXP_USERTIME;}
"mpi"  	    	{ yylval.string = copy_string(yytext); return EXP_MPI;}
"fpe"  	    	{ yylval.string = copy_string(yytext); return EXP_FPE;}
"hwc"  	    	{ yylval.string = copy_string(yytext); return EXP_HWC;}
"io"  	    	{ yylval.string = copy_string(yytext); return EXP_IO;}

"vtopn"  	{ yylval.string = copy_string(yytext); return VIEW_TOPN;}
"vexcltime"  	{ yylval.string = copy_string(yytext); return VIEW_EXCLTIME;}
"vio"	    	{ yylval.string = copy_string(yytext); return VIEW_IO;}
"vfpe"  	{ yylval.string = copy_string(yytext); return VIEW_FPE;}
"vhwc"  	{ yylval.string = copy_string(yytext); return VIEW_HWC;}

"displaymode"  	{ yylval.string = copy_string(yytext); return PARAM_DISPLAY_MODE;}
"displaymedia"	{ yylval.string = copy_string(yytext); return PARAM_DISPLAY_MEDIA;}
"displayrefreshrate"  	{ yylval.string = copy_string(yytext); return PARAM_DISPLAY_REFRESH_RATE;}
"expsavefile"  	{ yylval.string = copy_string(yytext); return PARAM_EXP_SAVE_FILE;}
"samplingrate"	{ yylval.string = copy_string(yytext); return PARAM_SAMPLING_RATE;}

"quit"  	{ return JUST_QUIT_HEAD; }

"-r"	{ return RANK_ID; }
"-h"	{ return HOST_ID; }
"-p"	{ return PROCESS_ID; }
"-t"	{ return THREAD_ID; }
"-f"	{ return FILE_ID; }
"-x"	{ return EXP_ID; }
"-l"	{ return LINE_ID;}
"-c"	{ return CLUSTER_ID;}

[0-9]+[.][0-9]+[.][0-9]+[.][0-9]+ {yylval.val = ip_number(yytext,4); return(IPNUMBER_32);}
0x[0-9a-f]+	{yylval.val = atoi(yytext); return(HEXNUMBER);}
[0-9]+   { yylval.val = atoi(yytext); return NUMBER; }
[a-zA-Z./][a-zA-Z0-9_./]+  { yylval.string = copy_string(yytext); return NAME; }

[\n]	{ return END_LINE; }

%%
#ifndef yywrap
int yywrap() { return 1; }
#endif
   
