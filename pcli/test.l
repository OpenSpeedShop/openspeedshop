%{
#include "y.tab.h"
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

"pcsamp"  	{ yylval.string = yytext; return EXP_PCSAMP;}
"usertime"  	{ yylval.string = yytext; return EXP_USERTIME;}
"mpi"  	    	{ yylval.string = yytext; return EXP_MPI;}
"fpe"  	    	{ yylval.string = yytext; return EXP_FPE;}
"hwc"  	    	{ yylval.string = yytext; return EXP_HWC;}
"io"  	    	{ yylval.string = yytext; return EXP_IO;}

"vtopn"  	{ return VIEW_TOPN;}
"vexcltime"  	{ return VIEW_EXCLTIME;}
"vio"	    	{ return VIEW_IO;}
"vfpe"  	{ return VIEW_FPE;}
"vhwc"  	{ return VIEW_HWC;}

"displaymode"  	{ return PARAM_DISPLAY_MODE;}
"displaymedia"	{ return PARAM_DISPLAY_MEDIA;}
"displayrefreshrate"  	{ return PARAM_DISPLAY_REFRESH_RATE;}
"expsavefile"  	{ return PARAM_EXP_SAVE_FILE;}
"samplingrate"	{ return PARAM_SAMPLING_RATE;}

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
"expsetparm"	{ return SETPARAM_HEAD; }
"expview"   	{ return VIEW_HEAD; }

"listexp"   	{ return LIST_EXP_HEAD; }
"listhosts" 	{ return LIST_HOSTS_HEAD; }
"listobj"   	{ return LIST_OBJ_HEAD; }
"listpids"  	{ return LIST_PIDS_HEAD; }
"listsrc"   	{ return LIST_SRC_HEAD; }
"listmetrics"	{ return LIST_METRICS_HEAD; }
"listparams"	{ return LIST_PARAMS_HEAD; }
"listreports"	{ return LIST_REPORTS_HEAD; }
"listtypes"	{ return LIST_TYPES_HEAD; }
"listbreaks"	{ return LIST_BREAKS_HEAD; }

"clearbreak"	{ return GEN_CLEAR_BREAK_HEAD; }
"exit"	    	{ return GEN_EXIT_HEAD; }
"opengui"   	{ return GEN_OPEN_GUI_HEAD; }
"help"	    	{ return GEN_HELP_HEAD; }
"history"   	{ return GEN_HISTORY_HEAD; }
"log"	    	{ return GEN_LOG_HEAD; }
"playback"  	{ return GEN_PLAYBACK_HEAD; }
"record"    	{ return GEN_RECORD_HEAD; }
"setbreak"  	{ return GEN_SETBREAK_HEAD; }

"quit"  	{ return JUST_QUIT_HEAD; }
"q" 	    	{ return JUST_QUIT_HEAD; }

"-r"	{ return RANK_ID; }
"-h"	{ return HOST_ID; }
"-p"	{ return PROCESS_ID; }
"-t"	{ return THREAD_ID; }
"-f"	{ return FILE_ID; }
"-x"	{ return EXP_ID; }
"-l"	{ return LINE_ID;}

0x[0-9a-f]+	{yylval.val = atoi(yytext); return(HEXNUMBER);}
[0-9]+   { yylval.val = atoi(yytext); return NUMBER; }
[a-zA-Z0-9_.]+  { yylval.string = yytext; return NAME; }

[\n]	{ return END_LINE; }

%%
#ifndef yywrap
int yywrap() { return 1; }
#endif
   
