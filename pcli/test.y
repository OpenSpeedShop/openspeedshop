%{
#include <stdio.h>
#include <ctype.h>
#include "support.h"

void yyerror(const char *);
extern char *yytext;
extern int yylex();
extern int yywrap();
extern void exit(int);

%}

%union {
int val;
char *string;
}

%token FOCUS ALL COPY GUI KILL MPI

%token EXP_PCSAMP EXP_USERTIME EXP_MPI EXP_FPE EXP_HWC EXP_IO
%type <string> EXP_PCSAMP EXP_USERTIME EXP_MPI EXP_FPE EXP_HWC EXP_IO

%token VIEW_TOPN VIEW_EXCLTIME VIEW_IO VIEW_FPE VIEW_HWC
%type <string> VIEW_TOPN VIEW_EXCLTIME VIEW_IO VIEW_FPE VIEW_HWC

%token PARAM_DISPLAY_MODE PARAM_DISPLAY_MEDIA PARAM_DISPLAY_REFRESH_RATE
%token PARAM_EXP_SAVE_FILE PARAM_SAMPLING_RATE
%type <string> PARAM_DISPLAY_MODE PARAM_DISPLAY_MEDIA PARAM_DISPLAY_REFRESH_RATE
%type <string> PARAM_EXP_SAVE_FILE PARAM_SAMPLING_RATE

%token ATTACH_HEAD CLOSE_HEAD CREATE_HEAD DETACH_HEAD 
%token DISABLE_HEAD ENABLE_HEAD 
%token FOCUS_HEAD PAUSE_HEAD RESTORE_HEAD GO_HEAD SAVE_HEAD
%token SETPARAM_HEAD VIEW_HEAD 

%token LIST_EXP_HEAD LIST_HOSTS_HEAD LIST_OBJ_HEAD LIST_PIDS_HEAD
%token LIST_SRC_HEAD LIST_METRICS_HEAD LIST_PARAMS_HEAD LIST_REPORTS_HEAD
%token LIST_BREAKS_HEAD LIST_TYPES_HEAD

%token GEN_CLEAR_BREAK_HEAD GEN_EXIT_HEAD GEN_OPEN_GUI_HEAD GEN_HELP_HEAD
%token GEN_HISTORY_HEAD GEN_LOG_HEAD GEN_PLAYBACK_HEAD GEN_RECORD_HEAD
%token GEN_SETBREAK_HEAD

%token JUST_QUIT_HEAD

%token RANK_ID HOST_ID RANK_PID THREAD_ID FILE_ID EXP_ID PROCESS_ID
%token LINE_ID

%token COMMA SEMICOLON DOUBLE_COLON COLON END_LINE DOT EQUAL

%token <val> HEXNUMBER
%token <val> NUMBER
%token <string> NAME

%type <val> gen_clear_break_arg
%type <val> lineno
%type <val> ip_address
%type <val> address_description
%type <string> file_name gen_help_arg

%%

ss_line:    /* empty */
    	|  ss_line command_line
	;

command_line:  END_LINE {/* printf("OSS > "); */ }
    	|   command_desc END_LINE {dump_command(); /* printf("OSS > "); */ }
    	;

command_desc: exp_attach_com 	{set_command_type(CMD_EXP_ATTACH);}
    	    | exp_close_com 	{set_command_type(CMD_EXP_CLOSE);}
	    | exp_create_com 	{set_command_type(CMD_EXP_CREATE);}
	    | exp_detach_com 	{set_command_type(CMD_EXP_DETACH);}
	    | exp_disable_com 	{set_command_type(CMD_EXP_DISABLE);}
	    | exp_enable_com 	{set_command_type(CMD_EXP_ENABLE);}
	    | exp_focus_com 	{set_command_type(CMD_EXP_FOCUS);}
	    | exp_pause_com 	{set_command_type(CMD_EXP_PAUSE);}
	    | exp_restore_com 	{set_command_type(CMD_EXP_RESTORE);}
	    | exp_go_com   	{set_command_type(CMD_EXP_GO);}
	    | exp_save_com  	{set_command_type(CMD_EXP_SAVE);}
	    | exp_setparam_com  {set_command_type(CMD_EXP_SETPARAM);}
	    | exp_view_com  	{set_command_type(CMD_EXP_VIEW);}
	    | list_exp_com  	{set_command_type(CMD_LIST_EXP);}
	    | list_hosts_com 	{set_command_type(CMD_LIST_HOSTS);}
	    | list_obj_com  	{set_command_type(CMD_LIST_OBJ);}
	    | list_pids_com 	{set_command_type(CMD_LIST_PIDS);}
	    | list_src_com  	{set_command_type(CMD_LIST_SRC);}
	    | list_metrics_com  {set_command_type(CMD_LIST_METRICS);}
	    | list_params_com 	{set_command_type(CMD_LIST_PARAMS);}
	    | list_reports_com  {set_command_type(CMD_LIST_REPORTS);}
	    | list_breaks_com 	{set_command_type(CMD_LIST_BREAKS);}
	    | list_types_com 	{set_command_type(CMD_LIST_TYPES);}
	    | gen_clear_break_com 	{set_command_type(CMD_CLEAR_BREAK);}
	    | gen_exit_com      	{set_command_type(CMD_EXIT);exit(0);}
	    | gen_open_gui_com  	{set_command_type(CMD_OPEN_GUI);}
	    | gen_help_com      	{set_command_type(CMD_HELP);}
	    | gen_history_com   	{set_command_type(CMD_HISTORY);}
	    | gen_log_com 	    	{set_command_type(CMD_LOG);}
	    | gen_playback_com  	{set_command_type(CMD_PLAYBACK);}
	    | gen_record_com    	{set_command_type(CMD_RECORD);}
	    | gen_setbreak_com  	{set_command_type(CMD_SETBREAK);}
	    | just_quit_com 	 {exit(0);}
	    | error {/* set_error(yylval.string,command_name[CMD_HEAD_ERROR]);*/} 
    ; 

    	    /** EXP_ATTACH **/	    
exp_attach_com:     exp_attach_head   exp_attach_args 
    	    	|   exp_attach_head error {set_error(yylval.string,command_name[CMD_EXP_ATTACH]);} 
    	    	;
exp_attach_head:    ATTACH_HEAD   
    	    	;
exp_attach_args:    /* empty */
    	    	|   exp_attach_arg
    	    	|   exp_attach_args exp_attach_arg
    	    	;
exp_attach_arg:     host_file_rpt_list
    	    	|   expId_spec
		|   expType_list
    	    	;

    	    /** EXP_CLOSE **/
exp_close_com:	    exp_close_head   exp_close_args 
    	    	|   exp_close_head error {set_error(yylval.string,command_name[CMD_EXP_CLOSE]);} 
    	    	;
exp_close_head:     CLOSE_HEAD   
    	    	;
exp_close_args:     exp_close_arg
    	    	;
exp_close_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
		|   FOCUS	    	{push_string(general_name[H_GEN_FOCUS],NAME_DUNNO);}
    	    	|   KILL expId_spec 	{push_string(general_name[H_GEN_KILL],NAME_DUNNO);}
		|   KILL ALL	    	{push_string(general_name[H_GEN_KILL],NAME_DUNNO);
		    	    	    	 push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
		|   KILL FOCUS	    	{push_string(general_name[H_GEN_KILL],NAME_DUNNO);
		    	    	    	 push_string(general_name[H_GEN_FOCUS],NAME_DUNNO);}
    	    	|   expId_spec KILL 	{push_string(general_name[H_GEN_KILL],NAME_DUNNO);}
		|   ALL KILL	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);
		    	    	    	 push_string(general_name[H_GEN_KILL],NAME_DUNNO);}
		|   FOCUS KILL	    	{push_string(general_name[H_GEN_FOCUS],NAME_DUNNO);
		    	    	    	 push_string(general_name[H_GEN_KILL],NAME_DUNNO);}
 		|   KILL	    	{push_string(general_name[H_GEN_KILL],NAME_DUNNO);}
   	    	;

    	    /** EXP_CREATE **/
exp_create_com:     exp_create_head   exp_create_args 
    	    	|   exp_create_head error {set_error(yylval.string,command_name[CMD_EXP_CREATE]);} 
    	    	;
exp_create_head:    CREATE_HEAD   
    	    	;
exp_create_args:    /* empty */
    	    	|   host_file_rpt_list
    	    	|   host_file_rpt_list expType_list
    	    	|   expType_list
    	    	;

    	    /** EXP_DETACH **/
exp_detach_com:     exp_detach_head   exp_detach_args 
  	    	|   exp_detach_head error {set_error(yylval.string,command_name[CMD_EXP_DETACH]);} 
      	    	;
exp_detach_head:    DETACH_HEAD   
    	    	;
exp_detach_args:    /* empty */
    	    	|   exp_detach_arg
    	    	|   exp_detach_args exp_detach_arg
    	    	;
exp_detach_arg:     host_file_rpt_list
    	    	|   expId_spec
		|   expType_list
    	    	;

    	    /** EXP_DISABLE **/
exp_disable_com:    exp_disable_head   exp_disable_args 
  	    	|   exp_disable_head error {set_error(yylval.string,command_name[CMD_EXP_DISABLE]);} 
      	    	;
exp_disable_head:   DISABLE_HEAD   
    	    	;
exp_disable_args:   exp_disable_arg
    	    	|   exp_disable_args exp_disable_arg
    	    	;
exp_disable_arg:    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** EXP_ENABLE **/
exp_enable_com:	    exp_enable_head   exp_enable_args 
  	    	|   exp_enable_head error {set_error(yylval.string,command_name[CMD_EXP_ENABLE]);} 
      	    	;
exp_enable_head:    ENABLE_HEAD   
    	    	;
exp_enable_args:    exp_enable_arg
    	    	|   exp_enable_args exp_enable_arg
    	    	;
exp_enable_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** EXP_FOCUS **/
exp_focus_com:	    exp_focus_head   exp_focus_args 
  	    	|   exp_focus_head error {set_error(yylval.string,command_name[CMD_EXP_FOCUS]);} 
      	    	;
exp_focus_head:     FOCUS_HEAD   
    	    	;
exp_focus_args:     /* empty */
    	    	|   exp_focus_arg
    	    	|   exp_focus_args exp_focus_arg
    	    	;
exp_focus_arg:	    expId_spec
    	    	;

    	    /** EXP_PAUSE **/
exp_pause_com:	    exp_pause_head   exp_pause_args 
  	    	|   exp_pause_head error {set_error(yylval.string,command_name[CMD_EXP_PAUSE]);} 
      	    	;
exp_pause_head:     PAUSE_HEAD   
    	    	;
exp_pause_args:     exp_pause_arg
    	    	|   exp_pause_args exp_pause_arg
    	    	;
exp_pause_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** EXP_RESTORE **/
exp_restore_com:    exp_restore_head   exp_restore_arg
  	    	|   exp_restore_head error {set_error(yylval.string,command_name[CMD_EXP_RESTORE]);} 
      	    	;
exp_restore_head:   RESTORE_HEAD   
    	    	;
exp_restore_arg:    host_file
    	    	;

    	    /** EXP_GO **/
exp_go_com:	    exp_go_head   exp_go_args 
  	    	|   exp_go_head error {set_error(yylval.string,command_name[CMD_EXP_GO]);} 
      	    	;
exp_go_head:	    GO_HEAD   
    	    	;
exp_go_args:	    exp_go_arg
    	    	|   exp_go_args exp_go_arg
    	    	;
exp_go_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** EXP_SAVE **/
exp_save_com:   exp_save_head   exp_save_arg
  	    	|   exp_save_head error {set_error(yylval.string,command_name[CMD_EXP_SAVE]);} 
      	    	;
exp_save_head:  SAVE_HEAD   
    	    	;
exp_save_arg:	    /* empty */
    	    	|   expId_spec COPY host_file {push_string(general_name[H_GEN_COPY],NAME_DUNNO);}
    	    	|   expId_spec host_file
		|   COPY host_file {push_string(general_name[H_GEN_COPY],NAME_DUNNO);}
		|   host_file
    	    	;

    	    /** EXP_SETPARAM **/
exp_setparam_com:   exp_setparam_head   exp_setparam_arg
  	    	|   exp_setparam_head error {set_error(yylval.string,command_name[CMD_EXP_SETPARAM]);} 
      	    	;
exp_setparam_head:  SETPARAM_HEAD   
    	    	;
exp_setparam_arg:   /* empty */
    	    	|   expId_spec param_list
    	    	|   param_list
    	    	;

    	    /** EXP_VIEW **/
exp_view_com:	    exp_view_head   exp_view_arg
  	    	|   exp_view_head error {set_error(yylval.string,command_name[CMD_EXP_VIEW]);} 
      	    	;
exp_view_head:	    VIEW_HEAD
    	    	;
exp_view_arg:	    /* empty */
    	    	|   expId_spec GUI viewType {push_string(general_name[H_GEN_GUI],NAME_DUNNO);}
    	    	|   GUI viewType {push_string(general_name[H_GEN_GUI],NAME_DUNNO);}
    	    	|   expId_spec viewType
    	    	|   viewType
    	    	;

    	    /** LIST_EXP **/
list_exp_com:	    list_exp_head   list_exp_arg 
  	    	|   list_exp_head error {set_error(yylval.string,command_name[CMD_LIST_EXP]);} 
      	    	;
list_exp_head:    LIST_EXP_HEAD   
    	    	;
list_exp_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** LIST_HOSTS **/
list_hosts_com:     list_hosts_head   list_hosts_arg
  	    	|   list_hosts_head error {set_error(yylval.string,command_name[CMD_LIST_HOSTS]);} 
      	    	;
list_hosts_head:    LIST_HOSTS_HEAD   
    	    	;
list_hosts_arg:     /* empty */
    	    	|   host_spec
    	    	;

    	    /** LIST_OBJ **/
list_obj_com:	    list_obj_head   list_obj_arg
  	    	|   list_obj_head error {set_error(yylval.string,command_name[CMD_LIST_OBJ]);} 
      	    	;
list_obj_head:	    LIST_OBJ_HEAD   
    	    	;
list_obj_arg:	    /* empty */
    	    	|   expId_spec
		|   host_file_rpt
    	    	;

    	    /** LIST_PIDS **/
list_pids_com:	    list_pids_head   list_pids_arg
  	    	|   list_pids_head error {set_error(yylval.string,command_name[CMD_LIST_PIDS]);} 
      	    	;
list_pids_head:     LIST_PIDS_HEAD   
    	    	;
list_pids_arg:	    /* empty */
    	    	|   host_file
		|   host_file MPI
		|   MPI     	    	{push_string(general_name[H_GEN_MPI],NAME_DUNNO);}
    	    	;

    	    /** LIST_SRC **/
list_src_com:	    list_src_head   list_src_arg
  	    	|   list_src_head error {set_error(yylval.string,command_name[CMD_LIST_SRC]);} 
      	    	;
list_src_head:	    LIST_SRC_HEAD   
    	    	;
list_src_arg:	    /* empty */
    	    	|   expId_spec 
    	    	|   expId_spec host_file_rpt_list
    	    	|   expId_spec lineno_spec
    	    	|   expId_spec host_file_rpt_list  lineno_spec
		|   host_file_rpt_list
		|   host_file_rpt_list lineno_spec
 		|   lineno_spec
    	    	;

    	    /** LIST_METRICS **/
list_metrics_com:   list_metrics_head   list_metrics_arg
  	    	|   list_metrics_head error {set_error(yylval.string,command_name[CMD_LIST_METRICS]);} 
      	    	;
list_metrics_head:  LIST_METRICS_HEAD   
    	    	;
list_metrics_arg:   /* empty */
		|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** LIST_PARAMS **/
list_params_com:    list_params_head   list_params_arg
  	    	|   list_params_head error {set_error(yylval.string,command_name[CMD_LIST_PARAMS]);} 
      	    	;
list_params_head:   LIST_PARAMS_HEAD   
    	    	;
list_params_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** LIST_REPORTS **/
list_reports_com:   list_reports_head   list_reports_arg
  	    	|   list_reports_head error {set_error(yylval.string,command_name[CMD_LIST_REPORTS]);} 
      	    	;
list_reports_head:  LIST_REPORTS_HEAD   
    	    	;
list_reports_arg:   /* empty */
		|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** LIST_BREAKS **/
list_breaks_com:    list_breaks_head   list_breaks_arg
  	    	|   list_breaks_head error {set_error(yylval.string,command_name[CMD_LIST_BREAKS]);} 
      	    	;
list_breaks_head:   LIST_BREAKS_HEAD   
    	    	;
list_breaks_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{push_string(general_name[H_GEN_ALL],NAME_DUNNO);}
    	    	;

    	    /** LIST_TYPES **/
list_types_com:     list_types_head   list_types_arg
  	    	|   list_types_head error {set_error(yylval.string,command_name[CMD_LIST_TYPES]);} 
      	    	;
list_types_head:   LIST_TYPES_HEAD   
    	    	;
list_types_arg:    /* empty */
    	    	;

    	    /** GEN_CLEAR_BREAK **/
gen_clear_break_com:	gen_clear_break_head   gen_clear_break_args
  	    	|   	gen_clear_break_head error {set_error(yylval.string,command_name[CMD_CLEAR_BREAK]);} 
      	    	;
gen_clear_break_head:	GEN_CLEAR_BREAK_HEAD   
    	    	;
gen_clear_break_args:	/* empty */
    	    	|   	gen_clear_break_arg {push_32bit_value($1,TABLE_BREAK);}
    	    	|   	gen_clear_break_args gen_clear_break_arg {push_32bit_value($2,TABLE_BREAK);} 
    	    	;
gen_clear_break_arg:	NUMBER 
    	    	;

    	    /** GEN_EXIT **/
gen_exit_com:	    gen_exit_head   gen_exit_arg
  	    	|   gen_exit_head error {set_error(yylval.string,command_name[CMD_EXIT]);} 
      	    	;
gen_exit_head:	    GEN_EXIT_HEAD   
    	    	;
gen_exit_arg:	    /* empty */
    	    	;

    	    /** GEN_OPEN_GUI **/
gen_open_gui_com:   gen_open_gui_head   gen_open_gui_arg
  	    	|   gen_open_gui_head error {set_error(yylval.string,command_name[CMD_OPEN_GUI]);} 
      	    	;
gen_open_gui_head:  GEN_OPEN_GUI_HEAD   
    	    	;
gen_open_gui_arg:   /* empty */
    	    	;

    	    /** GEN_HELP **/
gen_help_com:	    gen_help_head
    		|   gen_help_head   gen_help_args 
    	    	;
gen_help_head:	    GEN_HELP_HEAD   
    	    	;
gen_help_args:	    gen_help_arg 
    	    	|   gen_help_args gen_help_arg  
    	    	;
gen_help_arg:	    NAME {push_help($1);}
    	    	|   ATTACH_HEAD     	    {push_help(command_name[CMD_EXP_ATTACH]);}
    	    	|   CLOSE_HEAD	    	    {push_help(command_name[CMD_EXP_CLOSE]);}
    	    	|   CREATE_HEAD     	    {push_help(command_name[CMD_EXP_CREATE]);}
    	    	|   DETACH_HEAD     	    {push_help(command_name[CMD_EXP_DETACH]);}
    	    	|   FOCUS_HEAD	    	    {push_help(command_name[CMD_EXP_FOCUS]);}
    	    	|   PAUSE_HEAD	    	    {push_help(command_name[CMD_EXP_PAUSE]);}
    	    	|   RESTORE_HEAD    	    {push_help(command_name[CMD_EXP_RESTORE]);}
    	    	|   GO_HEAD	    	    {push_help(command_name[CMD_EXP_GO]);}
    	    	|   SAVE_HEAD   	    {push_help(command_name[CMD_EXP_SAVE]);}
   	    	|   SETPARAM_HEAD   	    {push_help(command_name[CMD_EXP_SETPARAM]);}
    	    	|   VIEW_HEAD	    	    {push_help(command_name[CMD_EXP_VIEW]);}
    	    	|   LIST_EXP_HEAD   	    {push_help(command_name[CMD_LIST_EXP]);}
    	    	|   LIST_HOSTS_HEAD 	    {push_help(command_name[CMD_LIST_HOSTS]);}
    	    	|   LIST_OBJ_HEAD   	    {push_help(command_name[CMD_LIST_OBJ]);}
    	    	|   LIST_PIDS_HEAD  	    {push_help(command_name[CMD_LIST_PIDS]);}
    	    	|   LIST_SRC_HEAD   	    {push_help(command_name[CMD_LIST_SRC]);}
    	    	|   LIST_METRICS_HEAD	    {push_help(command_name[CMD_LIST_METRICS]);}
    	    	|   LIST_PARAMS_HEAD	    {push_help(command_name[CMD_LIST_PARAMS]);}
    	    	|   LIST_REPORTS_HEAD	    {push_help(command_name[CMD_LIST_REPORTS]);}
    	    	|   LIST_BREAKS_HEAD	    {push_help(command_name[CMD_LIST_BREAKS]);}
    	    	|   LIST_TYPES_HEAD	    {push_help(command_name[CMD_LIST_TYPES]);}
    	    	|   GEN_CLEAR_BREAK_HEAD    {push_help(command_name[CMD_CLEAR_BREAK]);}
    	    	|   GEN_EXIT_HEAD   	    {push_help(command_name[CMD_EXIT]);}
    	    	|   GEN_OPEN_GUI_HEAD	    {push_help(command_name[CMD_OPEN_GUI]);}
    	    	|   GEN_HELP_HEAD   	    {push_help(command_name[CMD_HELP]);}
    	    	|   GEN_HISTORY_HEAD	    {push_help(command_name[CMD_HISTORY]);}
    	    	|   GEN_LOG_HEAD    	    {push_help(command_name[CMD_LOG]);}
    	    	|   GEN_PLAYBACK_HEAD	    {push_help(command_name[CMD_PLAYBACK]);}
    	    	|   GEN_RECORD_HEAD 	    {push_help(command_name[CMD_RECORD]);}
    	    	|   GEN_SETBREAK_HEAD	    {push_help(command_name[CMD_SETBREAK]);}

    	    	|   FOCUS   {push_help(general_name[H_GEN_FOCUS]);}
    	    	|   ALL	    {push_help(general_name[H_GEN_ALL]);}
    	    	|   COPY    {push_help(general_name[H_GEN_COPY]);}
    	    	|   GUI	    {push_help(general_name[H_GEN_GUI]);}
    	    	|   KILL    {push_help(general_name[H_GEN_KILL]);}
    	    	|   MPI	    {push_help("-mpi");}

    	    	|   EXP_PCSAMP	    {push_help(experiment_name[H_EXP_PCSAMP]);}
    	    	|   EXP_USERTIME    {push_help(experiment_name[H_EXP_USERTIME]);}
    	    	|   EXP_MPI	    {push_help(experiment_name[H_EXP_MPI]);}
    	    	|   EXP_FPE	    {push_help(experiment_name[H_EXP_FPE]);}
    	    	|   EXP_HWC	    {push_help(experiment_name[H_EXP_HWC]);}
    	    	|   EXP_IO	    {push_help(experiment_name[H_EXP_IO]);}

    	    	|   VIEW_TOPN	    {push_help(viewtype_name[H_VIEW_TOPN]);}
    	    	|   VIEW_EXCLTIME   {push_help(viewtype_name[H_VIEW_EXCLTIME]);}
    	    	|   VIEW_IO	    {push_help(viewtype_name[H_VIEW_IO]);}
    	    	|   VIEW_FPE	    {push_help(viewtype_name[H_VIEW_FPE]);}
    	    	|   VIEW_HWC	    {push_help(viewtype_name[H_VIEW_HWC]);}

    	    	|   PARAM_DISPLAY_MODE	    {push_help(paramtype_name[H_PARAM_DMODE]);}
    	    	|   PARAM_DISPLAY_MEDIA	    {push_help(paramtype_name[H_PARAM_DMEDIA]);}
    	    	|   PARAM_DISPLAY_REFRESH_RATE {push_help(paramtype_name[H_PARAM_DREFRESH]);}
    	    	|   PARAM_EXP_SAVE_FILE	    {push_help(paramtype_name[H_PARAM_XSAVEFILE]);}
    	    	|   PARAM_SAMPLING_RATE	    {push_help(paramtype_name[H_PARAM_SAMPRATE]);}

    	    	|   RANK_ID	    {push_help("-r");}
    	    	|   HOST_ID	    {push_help("-h");}
    	    	|   PROCESS_ID	    {push_help("-p");}
    	    	|   THREAD_ID	    {push_help("-t");}
    	    	|   FILE_ID	    {push_help("-f");}
    	    	|   EXP_ID	    {push_help("-x");}
    	    	|   LINE_ID	    {push_help("-l");}
    	    	;

    	    /** GEN_HISTORY **/
gen_history_com:    gen_history_head   gen_history_arg
  	    	|   gen_history_head error {set_error(yylval.string,command_name[CMD_HISTORY]);} 
      	    	;
gen_history_head:   GEN_HISTORY_HEAD   
    	    	;
gen_history_arg:    /* empty */
    	    	|   host_file
    	    	;

    	    /** GEN_LOG **/
gen_log_com:	    gen_log_head   gen_log_arg
  	    	|   gen_log_head error {set_error(yylval.string,command_name[CMD_LOG]);} 
      	    	;
gen_log_head:	    GEN_LOG_HEAD   
    	    	;
gen_log_arg:	    /* empty */
    	    	|   host_file
    	    	;

    	    /** GEN_PLAYBACK **/
gen_playback_com:   gen_playback_head   gen_playback_arg
  	    	|   gen_playback_head error {set_error(yylval.string,command_name[CMD_PLAYBACK]);} 
      	    	;
gen_playback_head:  GEN_PLAYBACK_HEAD   
    	    	;
gen_playback_arg:   host_file
    	    	;

    	    /** GEN_RECORD **/
gen_record_com:     gen_record_head   gen_record_arg
  	    	|   gen_record_head error {set_error(yylval.string,command_name[CMD_RECORD]);} 
      	    	;
gen_record_head:    GEN_RECORD_HEAD   
    	    	;
gen_record_arg:     host_file
    	    	;

    	    /** GEN_SETBREAK **/
gen_setbreak_com:   gen_setbreak_head   gen_setbreak_args 
  	    	|   gen_setbreak_head error {set_error(yylval.string,command_name[CMD_SETBREAK]);} 
      	    	;
gen_setbreak_head:  /* empty */
    	    	|   GEN_SETBREAK_HEAD   
    	    	;
gen_setbreak_args:  gen_setbreak_arg
    	    	|   gen_setbreak_args gen_setbreak_arg
    	    	;
gen_setbreak_arg:   /* empty */
    	    	|   expId_spec
    	    	|   host_file_rpt
		|   address_description {push_32bit_value($1,TABLE_ADDRESS);}
    	    	;

    	    /** JUST_QUIT **/
just_quit_com:     JUST_QUIT_HEAD
    	    	;


    /**************************************************/
    

expId_spec:	    EXP_ID  NUMBER {set_exp_id($2);}
    	    	;

host_file_rpt_list: host_file_rpt
    	    	|   host_file_rpt_list host_file_rpt
    	    	;

host_file_rpt:	    host_list_spec
    	    	|   file_list_spec
		|   pid_list_spec
		|   thread_list_spec
		|   rank_list_spec
    	    	;

host_list_spec:     HOST_ID host_list
		;

host_list:  	    host_name
    	    	|   host_list COMMA host_name
		;

host_name:  	    NAME {push_host_name($1);}
    	    	|   ip_address {push_host_ip($1);}
		;

host_file:  	    host_spec file_spec
    	    	|   file_spec

host_spec:  	    HOST_ID host_list
    	    	;

file_spec:  	    FILE_ID file_name
    	    	;

file_list_spec:     FILE_ID file_list
		;

file_list:  	    file_name 
    	    	|   file_list COMMA file_name 
		;

file_name:  	    NAME {push_file($1);}

pid_list_spec:	    PROCESS_ID pid_list
		;

pid_list:  	    pid_range
    	    	|   pid_list COMMA pid_range
		;

pid_range:  	    NUMBER {push_32bit_range($1,$1,TABLE_PID);}
    	    	|   NUMBER COLON NUMBER {push_32bit_range($1,$3,TABLE_PID);}
		;

thread_list_spec:   THREAD_ID thread_list
		;

thread_list:  	    thread_range
    	    	|   thread_list COMMA thread_range
		;

thread_range:  	    NUMBER {push_32bit_range($1,$1,TABLE_THREAD);}
    	    	|   NUMBER COLON NUMBER {push_32bit_range($1,$3,TABLE_THREAD);}
		;

rank_list_spec:     RANK_ID rank_list
		;

rank_list:  	    rank_range
    	    	|   rank_list COMMA rank_range
		;

rank_range:  	    NUMBER {push_32bit_range($1,$1,TABLE_RANK);}
    	    	|   NUMBER COLON NUMBER {push_32bit_range($1,$3,TABLE_RANK);}
		;

ip_address: 	    NUMBER
    	    	|   NUMBER DOT NUMBER DOT NUMBER DOT NUMBER 
    	    	    { $$ = ($1<<24) | ($3<<16) | ($5<<8) | $7;}
    	    	;

expParam:   	    expType DOUBLE_COLON param_name
    	    	|   param_name
    	    	;

param_name: 	    PARAM_DISPLAY_MODE	    	{push_string($1,NAME_PARAM);}
    	    	|   PARAM_DISPLAY_MEDIA     	{push_string($1,NAME_PARAM);}
    	    	|   PARAM_DISPLAY_REFRESH_RATE	{push_string($1,NAME_PARAM);}
    	    	|   PARAM_EXP_SAVE_FILE     	{push_string($1,NAME_PARAM);}
    	    	|   PARAM_SAMPLING_RATE     	{push_string($1,NAME_PARAM);}
		;

expType_list:  	    expType
    	    	|   expType_list COMMA expType
		;

expType:	    EXP_PCSAMP	    	    	{push_string($1,NAME_EXPERIMENT);}
    	    	|   EXP_USERTIME    	    	{push_string($1,NAME_EXPERIMENT);}
    	    	|   EXP_MPI 	    	    	{push_string($1,NAME_EXPERIMENT);}
    	    	|   EXP_FPE 	    	    	{push_string($1,NAME_EXPERIMENT);}
    	    	|   EXP_HWC 	    	    	{push_string($1,NAME_EXPERIMENT);}
    	    	|   EXP_IO  	    	    	{push_string($1,NAME_EXPERIMENT);}
		|   NAME  	    	    	{push_string($1,NAME_EXPERIMENT);}
		;

viewType:   	    VIEW_TOPN	    	    	{push_string($1,NAME_VIEW_TYPE);}
    	    	|   VIEW_EXCLTIME   	    	{push_string($1,NAME_VIEW_TYPE);}
    	    	|   VIEW_IO 	    	    	{push_string($1,NAME_VIEW_TYPE);}
    	    	|   VIEW_FPE	    	    	{push_string($1,NAME_VIEW_TYPE);}
    	    	|   VIEW_HWC	    	    	{push_string($1,NAME_VIEW_TYPE);}
    	    	|   NAME	    	    	{push_string($1,NAME_VIEW_TYPE);}
		;

param_list: 	    expParam EQUAL NUMBER
    	    	|   param_list expParam EQUAL NUMBER
		;

/* expParamValue:	    NUMBER */
/*    	    	; */

/* breakId:    	    NUMBER */
/*    	    	; */

/* lineno_list_spec:   LINE_ID lineno_list */
/*    	    	; */

lineno_spec:	    LINE_ID lineno
    	    	;

/* lineno_list:	    lineno_range */
/*     	    	|   lineno_list COMMA lineno_range */
/* 		; */

/* lineno_range:	    lineno {push_32bit_value($1,TABLE_LINENO);} */
/*     	    	|   lineno COLON lineno {push_32bit_range($1,$3,TABLE_LINENO);} */
/* 		; */

lineno: 	    NUMBER
    	    	;

address_description: NUMBER {push_32bit_value($1,TABLE_ADDRESS);}
		|   NAME {push_string($1,NAME_ADDR);}
    	    	;


%%


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

