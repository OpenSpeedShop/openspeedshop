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

%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

void yyerror(const char *);
extern char *yytext;
extern int yylex();
extern int yywrap();
extern ParseResult *p_parse_result;

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
%token LIST_SRC_HEAD LIST_METRICS_HEAD LIST_PARAMS_HEAD LIST_VIEWS_HEAD
%token LIST_BREAKS_HEAD LIST_TYPES_HEAD
%token LIST_RANKS_HEAD LIST_STATUS_HEAD LIST_THREADS_HEAD

%token GEN_CLEAR_BREAK_HEAD GEN_EXIT_HEAD GEN_OPEN_GUI_HEAD GEN_HELP_HEAD
%token GEN_HISTORY_HEAD GEN_LOG_HEAD GEN_PLAYBACK_HEAD GEN_RECORD_HEAD
%token GEN_SETBREAK_HEAD

%token JUST_QUIT_HEAD

%token RANK_ID HOST_ID THREAD_ID FILE_ID EXP_ID PROCESS_ID
%token LINE_ID CLUSTER_ID

%token COMMA SEMICOLON DOUBLE_COLON COLON END_LINE EQUAL

%token <val> IPNUMBER_32
%token <val> HEXNUMBER
%token <val> NUMBER
%token <string> NAME

%type <val> lineno
%type <val> ip_address
%type <val> expId_spec
%type <string> file_name gen_help_arg param_name viewType expType

%%

/*
ss_line:    
    	|  ss_line command_line
	;
*/
command_line:  END_LINE {/* printf("OSS > "); */ }
    	|   command_desc END_LINE {p_parse_result->PushParseTarget(); }
    	;

command_desc: exp_attach_com 	{p_parse_result->SetCommandType(CMD_EXP_ATTACH);}
    	    | exp_close_com 	{p_parse_result->SetCommandType(CMD_EXP_CLOSE);}
	    | exp_create_com 	{p_parse_result->SetCommandType(CMD_EXP_CREATE);}
	    | exp_detach_com 	{p_parse_result->SetCommandType(CMD_EXP_DETACH);}
	    | exp_disable_com 	{p_parse_result->SetCommandType(CMD_EXP_DISABLE);}
	    | exp_enable_com 	{p_parse_result->SetCommandType(CMD_EXP_ENABLE);}
	    | exp_focus_com 	{p_parse_result->SetCommandType(CMD_EXP_FOCUS);}
	    | exp_pause_com 	{p_parse_result->SetCommandType(CMD_EXP_PAUSE);}
	    | exp_restore_com 	{p_parse_result->SetCommandType(CMD_EXP_RESTORE);}
	    | exp_go_com   	{p_parse_result->SetCommandType(CMD_EXP_GO);}
	    | exp_save_com  	{p_parse_result->SetCommandType(CMD_EXP_SAVE);}
	    | exp_setparam_com  {p_parse_result->SetCommandType(CMD_EXP_SETPARAM);}
	    | exp_view_com  	{p_parse_result->SetCommandType(CMD_EXP_VIEW);}
	    | list_breaks_com 	{p_parse_result->SetCommandType(CMD_LIST_BREAKS);}
	    | list_exp_com  	{p_parse_result->SetCommandType(CMD_LIST_EXP);}
	    | list_hosts_com 	{p_parse_result->SetCommandType(CMD_LIST_HOSTS);}
	    | list_metrics_com  {p_parse_result->SetCommandType(CMD_LIST_METRICS);}
	    | list_obj_com  	{p_parse_result->SetCommandType(CMD_LIST_OBJ);}
	    | list_params_com 	{p_parse_result->SetCommandType(CMD_LIST_PARAMS);}
	    | list_pids_com 	{p_parse_result->SetCommandType(CMD_LIST_PIDS);}
	    | list_ranks_com	{p_parse_result->SetCommandType(CMD_LIST_RANKS);}
	    | list_views_com	{p_parse_result->SetCommandType(CMD_LIST_VIEWS);}
	    | list_src_com  	{p_parse_result->SetCommandType(CMD_LIST_SRC);}
	    | list_status_com  	{p_parse_result->SetCommandType(CMD_LIST_STATUS);}
	    | list_threads_com 	{p_parse_result->SetCommandType(CMD_LIST_THREADS);}
	    | list_types_com 	{p_parse_result->SetCommandType(CMD_LIST_TYPES);}
	    | gen_clear_break_com 	{p_parse_result->SetCommandType(CMD_CLEAR_BREAK);}
	    | gen_exit_com      	{p_parse_result->SetCommandType(CMD_EXIT);}
	    | gen_open_gui_com  	{p_parse_result->SetCommandType(CMD_OPEN_GUI);}
	    | gen_help_com      	{p_parse_result->SetCommandType(CMD_HELP);}
	    | gen_history_com   	{p_parse_result->SetCommandType(CMD_HISTORY);}
	    | gen_log_com 	    	{p_parse_result->SetCommandType(CMD_LOG);}
	    | gen_playback_com  	{p_parse_result->SetCommandType(CMD_PLAYBACK);}
	    | gen_record_com    	{p_parse_result->SetCommandType(CMD_RECORD);}
	    | gen_setbreak_com  	{p_parse_result->SetCommandType(CMD_SETBREAK);}
	    | just_quit_com 	 {exit(0);}
    	    ; 

    	    /** EXP_ATTACH **/	    
exp_attach_com:     ATTACH_HEAD   exp_attach_args 
    	    	|   ATTACH_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_ATTACH].name);} 
    	    	;
exp_attach_args:    /* empty */
    	    	|   expId_spec
    	    	|   target_list
    	    	|   expType_list
    	    	|   expId_spec target_list
    	    	|   expId_spec target_list expType_list
    	    	|   expId_spec expType_list
    	    	|   target_list expType_list
    	    	;

    	    /** EXP_CLOSE **/
exp_close_com:	    CLOSE_HEAD   exp_close_args 
    	    	|   CLOSE_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_CLOSE].name);} 
    	    	;
exp_close_args:     exp_close_arg
    	    	;
exp_close_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   FOCUS	    	{p_parse_result->push_modifiers(general_name[H_GEN_FOCUS]);}
    	    	|   KILL expId_spec 	{p_parse_result->push_modifiers(general_name[H_GEN_KILL]);}
		|   KILL ALL	    	{p_parse_result->push_modifiers(general_name[H_GEN_KILL]);
		    	    	    	 p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   KILL FOCUS	    	{p_parse_result->push_modifiers(general_name[H_GEN_KILL]);
		    	    	    	 p_parse_result->push_modifiers(general_name[H_GEN_FOCUS]);}
    	    	|   expId_spec KILL 	{p_parse_result->push_modifiers(general_name[H_GEN_KILL]);}
		|   ALL KILL	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);
		    	    	    	 p_parse_result->push_modifiers(general_name[H_GEN_KILL]);}
		|   FOCUS KILL	    	{p_parse_result->push_modifiers(general_name[H_GEN_FOCUS]);
		    	    	    	 p_parse_result->push_modifiers(general_name[H_GEN_KILL]);}
 		|   KILL	    	{p_parse_result->push_modifiers(general_name[H_GEN_KILL]);}
   	    	;

    	    /** EXP_CREATE **/
exp_create_com:     CREATE_HEAD   exp_create_args 
    	    	|   CREATE_HEAD {p_parse_result->SetCommandType(CMD_EXP_CREATE);} error 
    	    	;
exp_create_args:    /* empty */
    	    	|   target_list
    	    	|   expType_list target_list
    	    	|   target_list expType_list
    	    	|   expType_list
    	    	;

    	    /** EXP_DETACH **/
exp_detach_com:     DETACH_HEAD   exp_detach_args 
  	    	|   DETACH_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_DETACH].name);} 
      	    	;
exp_detach_args:    /* empty */
    	    	|   expId_spec
    	    	|   target_list
    	    	|   expType_list
    	    	|   expId_spec target_list
    	    	|   expId_spec target_list expType_list
    	    	|   expId_spec expType_list
    	    	|   target_list expType_list
    	    	;    	    	;

    	    /** EXP_DISABLE **/
exp_disable_com:    DISABLE_HEAD   exp_disable_arg
  	    	|   DISABLE_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_DISABLE].name);} 
      	    	;
exp_disable_arg:    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** EXP_ENABLE **/
exp_enable_com:	    ENABLE_HEAD   exp_enable_arg
  	    	|   ENABLE_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_ENABLE].name);} 
      	    	;
exp_enable_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** EXP_FOCUS **/
exp_focus_com:	    FOCUS_HEAD   
  	    	|   FOCUS_HEAD expId_spec
  	    	|   FOCUS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_FOCUS].name);} 
      	    	;

    	    /** EXP_GO **/
exp_go_com:	    GO_HEAD   exp_go_arg
  	    	|   GO_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_GO].name);} 
      	    	;
exp_go_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** EXP_PAUSE **/
exp_pause_com:	    PAUSE_HEAD   exp_pause_arg
  	    	|   PAUSE_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_PAUSE].name);} 
      	    	;
exp_pause_arg:	    /* empty */
    	    	|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** EXP_RESTORE **/
exp_restore_com:    RESTORE_HEAD   exp_restore_arg
  	    	|   RESTORE_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_RESTORE].name);} 
      	    	;
exp_restore_arg:    file_spec
    	    	;

    	    /** EXP_SAVE **/
exp_save_com:	    SAVE_HEAD   exp_save_arg
  	    	|   SAVE_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_SAVE].name);} 
      	    	;
exp_save_arg:	    file_spec
    	    	|   expId_spec COPY file_spec {p_parse_result->push_modifiers(general_name[H_GEN_COPY]);}
    	    	|   expId_spec file_spec
		|   COPY file_spec {p_parse_result->push_modifiers(general_name[H_GEN_COPY]);}
    	    	;

    	    /** EXP_SETPARAM **/
exp_setparam_com:   SETPARAM_HEAD   exp_setparam_arg
  	    	|   SETPARAM_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_SETPARAM].name);} 
      	    	;
exp_setparam_arg:   /* empty */
    	    	|   expId_spec param_list
    	    	|   param_list
    	    	;

    	    /** EXP_VIEW **/
exp_view_com:	    VIEW_HEAD   exp_view_arg
  	    	|   VIEW_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXP_VIEW].name);} 
      	    	;
exp_view_arg:	    expId_spec GUI viewType {p_parse_result->push_modifiers(general_name[H_GEN_GUI]);}
    	    	|   GUI viewType {p_parse_result->push_modifiers(general_name[H_GEN_GUI]);}
    	    	|   expId_spec viewType
    	    	|   viewType 
    	    	;

    	    /** LIST_BREAKS **/
list_breaks_com:    LIST_BREAKS_HEAD   list_breaks_arg
  	    	|   LIST_BREAKS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_BREAKS].name);} 
      	    	;
list_breaks_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** LIST_EXP **/
list_exp_com:	    LIST_EXP_HEAD   list_exp_arg 
  	    	|   LIST_EXP_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_EXP].name);} 
      	    	;
list_exp_arg:	    /* empty */
    	    	;

    	    /** LIST_HOSTS **/
list_hosts_com:     LIST_HOSTS_HEAD   list_hosts_arg
  	    	|   LIST_HOSTS_HEAD  
      	    	;
list_hosts_arg:  
    	    	    expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	|   cluster_list_spec
    	    	|   error END_LINE {p_parse_result->set_error("<list_hosts_arg>");} 
    	    	;

    	    /** LIST_METRICS **/
list_metrics_com:   LIST_METRICS_HEAD   list_metrics_arg
  	    	|   LIST_METRICS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_METRICS].name);} 
      	    	;
list_metrics_arg:   /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   expType_list
    	    	;

    	    /** LIST_OBJ **/
list_obj_com:	    LIST_OBJ_HEAD   list_obj_arg
  	    	|   LIST_OBJ_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_OBJ].name);} 
      	    	;
list_obj_arg:	    /* empty */
    	    	|   expId_spec
		|   target
 		|   expId_spec target
    	    	;

    	    /** LIST_PARAMS **/
list_params_com:    LIST_PARAMS_HEAD   list_params_arg
  	    	|   LIST_PARAMS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_PARAMS].name);} 
      	    	;
list_params_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   expType_list
    	    	;

    	    /** LIST_PIDS **/
list_pids_com:	    LIST_PIDS_HEAD   list_pids_arg
  	    	|   LIST_PIDS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_PIDS].name);} 
      	    	;
list_pids_arg:	    /* empty */
    	    	|   host_file
    	    	|   expId_spec 
    	    	|   ALL     	    	    {p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	|   expId_spec host_file
    	    	|   ALL host_file   	    {p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** LIST_RANKS **/
list_ranks_com:    LIST_RANKS_HEAD   list_ranks_arg
  	    	|   LIST_RANKS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_RANKS].name);} 
      	    	;
list_ranks_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   target
		|   expId_spec target
		|   ALL target	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** LIST_SRC **/
list_src_com:	    LIST_SRC_HEAD   list_src_arg
  	    	|   LIST_SRC_HEAD  error END_LINE {p_parse_result->set_error("<list_src_arg>");} 
      	    	;
list_src_arg:	    /* empty */
    	    	|   expId_spec 
    	    	|   expId_spec target_list
    	    	|   expId_spec lineno_range_spec
    	    	|   expId_spec target_list  lineno_range_spec
		|   target_list
		|   target_list lineno_range_spec
 		|   lineno_range_spec
    	    	;

    	    /** LIST_STATUS **/
list_status_com:    LIST_STATUS_HEAD   list_status_arg
  	    	|   LIST_STATUS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_STATUS].name);} 
      	    	;
list_status_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** LIST_THREADS **/
list_threads_com:    LIST_THREADS_HEAD   list_threads_arg
  	    	|   LIST_THREADS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_THREADS].name);} 
      	    	;
list_threads_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   target
		|   expId_spec target
		|   ALL target	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
    	    	;

    	    /** LIST_TYPES **/
list_types_com:     LIST_TYPES_HEAD list_types_arg
  	    	|   LIST_TYPES_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LIST_TYPES].name);} 
      	    	;

list_types_arg:    /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
   	    	;

    	    /** LIST_VIEWS **/
list_views_com:   LIST_VIEWS_HEAD   list_views_arg
  	    	|   LIST_VIEWS_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[LIST_VIEWS_HEAD].name);} 
      	    	;
list_views_arg:   /* empty */
		|   expId_spec
		|   ALL     	    	{p_parse_result->push_modifiers(general_name[H_GEN_ALL]);}
		|   expType_list
    	    	;

    	    /** GEN_CLEAR_BREAK **/
gen_clear_break_com:	GEN_CLEAR_BREAK_HEAD   gen_clear_break_arg
  	    	|   	GEN_CLEAR_BREAK_HEAD END_LINE error {p_parse_result->set_error(cmd_desc[CMD_CLEAR_BREAK].name);} 
      	    	;
gen_clear_break_arg:	NUMBER {p_parse_result->pushBreakId($1);}
    	    	;

    	    /** GEN_EXIT **/
gen_exit_com:	    GEN_EXIT_HEAD
  	    	|   GEN_EXIT_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_EXIT].name);} 
      	    	;

    	    /** GEN_HELP **/
gen_help_com:	    GEN_HELP_HEAD
    		|   GEN_HELP_HEAD   gen_help_args 
    	    	;
gen_help_args:	    gen_help_arg 
    	    	|   gen_help_args gen_help_arg  
    	    	;
gen_help_arg:	    NAME {p_parse_result->push_help($1);}
    	    	|   ATTACH_HEAD     	    {p_parse_result->push_help(cmd_desc[CMD_EXP_ATTACH].name);}
    	    	|   CLOSE_HEAD	    	    {p_parse_result->push_help(cmd_desc[CMD_EXP_CLOSE].name);}
    	    	|   CREATE_HEAD     	    {p_parse_result->push_help(cmd_desc[CMD_EXP_CREATE].name);}
    	    	|   DETACH_HEAD     	    {p_parse_result->push_help(cmd_desc[CMD_EXP_DETACH].name);}
    	    	|   FOCUS_HEAD	    	    {p_parse_result->push_help(cmd_desc[CMD_EXP_FOCUS].name);}
    	    	|   PAUSE_HEAD	    	    {p_parse_result->push_help(cmd_desc[CMD_EXP_PAUSE].name);}
    	    	|   RESTORE_HEAD    	    {p_parse_result->push_help(cmd_desc[CMD_EXP_RESTORE].name);}
    	    	|   GO_HEAD	    	    {p_parse_result->push_help(cmd_desc[CMD_EXP_GO].name);}
    	    	|   SAVE_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_EXP_SAVE].name);}
   	    	|   SETPARAM_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_EXP_SETPARAM].name);}
    	    	|   VIEW_HEAD	    	    {p_parse_result->push_help(cmd_desc[CMD_EXP_VIEW].name);}
    	    	|   LIST_EXP_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_LIST_EXP].name);}
    	    	|   LIST_HOSTS_HEAD 	    {p_parse_result->push_help(cmd_desc[CMD_LIST_HOSTS].name);}
    	    	|   LIST_OBJ_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_LIST_OBJ].name);}
    	    	|   LIST_PIDS_HEAD  	    {p_parse_result->push_help(cmd_desc[CMD_LIST_PIDS].name);}
    	    	|   LIST_SRC_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_LIST_SRC].name);}
    	    	|   LIST_METRICS_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_LIST_METRICS].name);}
    	    	|   LIST_PARAMS_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_LIST_PARAMS].name);}
    	    	|   LIST_VIEWS_HEAD	    {p_parse_result->push_help(cmd_desc[LIST_VIEWS_HEAD].name);}
    	    	|   LIST_BREAKS_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_LIST_BREAKS].name);}
    	    	|   LIST_TYPES_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_LIST_TYPES].name);}
    	    	|   GEN_CLEAR_BREAK_HEAD    {p_parse_result->push_help(cmd_desc[CMD_CLEAR_BREAK].name);}
    	    	|   GEN_EXIT_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_EXIT].name);}
    	    	|   GEN_OPEN_GUI_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_OPEN_GUI].name);}
    	    	|   GEN_HELP_HEAD   	    {p_parse_result->push_help(cmd_desc[CMD_HELP].name);}
    	    	|   GEN_HISTORY_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_HISTORY].name);}
    	    	|   GEN_LOG_HEAD    	    {p_parse_result->push_help(cmd_desc[CMD_LOG].name);}
    	    	|   GEN_PLAYBACK_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_PLAYBACK].name);}
    	    	|   GEN_RECORD_HEAD 	    {p_parse_result->push_help(cmd_desc[CMD_RECORD].name);}
    	    	|   GEN_SETBREAK_HEAD	    {p_parse_result->push_help(cmd_desc[CMD_SETBREAK].name);}

    	    	|   FOCUS   {p_parse_result->push_help(general_name[H_GEN_FOCUS]);}
    	    	|   ALL	    {p_parse_result->push_help(general_name[H_GEN_ALL]);}
    	    	|   COPY    {p_parse_result->push_help(general_name[H_GEN_COPY]);}
    	    	|   GUI	    {p_parse_result->push_help(general_name[H_GEN_GUI]);}
    	    	|   KILL    {p_parse_result->push_help(general_name[H_GEN_KILL]);}
    	    	|   MPI	    {p_parse_result->push_help("-mpi");}

    	    	|   EXP_PCSAMP	    {p_parse_result->push_help(experiment_name[H_EXP_PCSAMP]);}
    	    	|   EXP_USERTIME    {p_parse_result->push_help(experiment_name[H_EXP_USERTIME]);}
    	    	|   EXP_MPI	    {p_parse_result->push_help(experiment_name[H_EXP_MPI]);}
    	    	|   EXP_FPE	    {p_parse_result->push_help(experiment_name[H_EXP_FPE]);}
    	    	|   EXP_HWC	    {p_parse_result->push_help(experiment_name[H_EXP_HWC]);}
    	    	|   EXP_IO	    {p_parse_result->push_help(experiment_name[H_EXP_IO]);}

    	    	|   VIEW_TOPN	    {p_parse_result->push_help(viewtype_name[H_VIEW_TOPN]);}
    	    	|   VIEW_EXCLTIME   {p_parse_result->push_help(viewtype_name[H_VIEW_EXCLTIME]);}
    	    	|   VIEW_IO	    {p_parse_result->push_help(viewtype_name[H_VIEW_IO]);}
    	    	|   VIEW_FPE	    {p_parse_result->push_help(viewtype_name[H_VIEW_FPE]);}
    	    	|   VIEW_HWC	    {p_parse_result->push_help(viewtype_name[H_VIEW_HWC]);}

    	    	|   PARAM_DISPLAY_MODE	    {p_parse_result->push_help(paramtype_name[H_PARAM_DMODE]);}
    	    	|   PARAM_DISPLAY_MEDIA	    {p_parse_result->push_help(paramtype_name[H_PARAM_DMEDIA]);}
    	    	|   PARAM_DISPLAY_REFRESH_RATE {p_parse_result->push_help(paramtype_name[H_PARAM_DREFRESH]);}
    	    	|   PARAM_EXP_SAVE_FILE	    {p_parse_result->push_help(paramtype_name[H_PARAM_XSAVEFILE]);}
    	    	|   PARAM_SAMPLING_RATE	    {p_parse_result->push_help(paramtype_name[H_PARAM_SAMPRATE]);}

    	    	|   RANK_ID	    {p_parse_result->push_help("-r");}
    	    	|   HOST_ID	    {p_parse_result->push_help("-h");}
    	    	|   PROCESS_ID	    {p_parse_result->push_help("-p");}
    	    	|   THREAD_ID	    {p_parse_result->push_help("-t");}
    	    	|   FILE_ID	    {p_parse_result->push_help("-f");}
    	    	|   EXP_ID	    {p_parse_result->push_help("-x");}
    	    	|   LINE_ID	    {p_parse_result->push_help("-l");}
    	    	;

    	    /** GEN_HISTORY **/
gen_history_com:    GEN_HISTORY_HEAD   gen_history_arg
  	    	|   GEN_HISTORY_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_HISTORY].name);} 
      	    	;
gen_history_arg:    /* empty */
    	    	|   file_spec
    	    	;

    	    /** GEN_LOG **/
gen_log_com:	    GEN_LOG_HEAD   gen_log_arg
  	    	|   GEN_LOG_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_LOG].name);} 
      	    	;
gen_log_arg:	    /* empty */
    	    	|   file_spec
    	    	;

    	    /** GEN_OPEN_GUI **/
gen_open_gui_com:   GEN_OPEN_GUI_HEAD
  	    	|   GEN_OPEN_GUI_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_OPEN_GUI].name);} 
      	    	;

    	    /** GEN_PLAYBACK **/
gen_playback_com:   GEN_PLAYBACK_HEAD   gen_playback_arg
  	    	|   GEN_PLAYBACK_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_PLAYBACK].name);} 
      	    	;
gen_playback_arg:   file_spec
    	    	;

    	    /** GEN_RECORD **/
gen_record_com:     GEN_RECORD_HEAD   gen_record_arg
  	    	|   GEN_RECORD_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_RECORD].name);} 
      	    	;
gen_record_arg:     /* empty */
    	    	|   file_spec
    	    	;

    	    /** GEN_SETBREAK **/
gen_setbreak_com:   GEN_SETBREAK_HEAD   gen_setbreak_arg
  	    	|   GEN_SETBREAK_HEAD error END_LINE {p_parse_result->set_error(cmd_desc[CMD_SETBREAK].name);} 
      	    	;
gen_setbreak_arg:   expId_spec address_description 
    	    	|   target address_description 
 		|   expId_spec target address_description 
 		|   address_description
    	    	;

    	    /** JUST_QUIT **/
just_quit_com:     JUST_QUIT_HEAD
    	    	;


    /**************************************************/
    

address_description: NUMBER {p_parse_result->pushAddressPoint($1);}
		|   HEXNUMBER {p_parse_result->pushAddressPoint($1);}
		|   NAME {p_parse_result->pushAddressPoint($1);}
    	    	;

expId_spec:	    EXP_ID  NUMBER {p_parse_result->SetExpId($2);}
    	    	;

target_list:	    target
    	    	|   target_list SEMICOLON {p_parse_result->PushParseTarget();} target
    	    	;

target:     	    target_element
    	    	|   target target_element
    	    	;

target_element:	    host_list_spec
    	    	|   file_list_spec
		|   pid_list_spec
		|   thread_list_spec
		|   rank_list_spec
		|   cluster_list_spec
    	    	;

host_list_spec:     HOST_ID host_list
		|   HOST_ID {p_parse_result->set_error("Bad host list entry.");} error  
		;

host_list:  	    host_name
    	    	|   host_list COMMA host_name
		;

host_name:  	    NAME {p_parse_result->CurrentTarget()->pushHostPoint($1);}
    	    	|   ip_address {p_parse_result->CurrentTarget()->pushHostPoint($1);}
		;

host_file:  	    host_spec file_spec
    	    	|   file_spec

host_spec:  	    HOST_ID host_list
    	    	;

file_spec:  	    FILE_ID file_name
    	    	;

file_list_spec:     FILE_ID file_list
		|   FILE_ID {p_parse_result->set_error("Bad file list entry.");} error  
		;

file_list:  	    file_name 
    	    	|   file_list COMMA file_name 
		;

file_name:  	    NAME {p_parse_result->CurrentTarget()->pushFilePoint($1);}

cluster_list_spec:  CLUSTER_ID cluster_list
		|   CLUSTER_ID {p_parse_result->set_error("Bad cluster list entry.");} error  
		;

cluster_list:  	    cluster_name 
    	    	|   cluster_list COMMA cluster_name 
		;

cluster_name:  	    NAME {p_parse_result->CurrentTarget()->pushClusterPoint($1);}

pid_list_spec:	    PROCESS_ID pid_list
		|   PROCESS_ID {p_parse_result->set_error("Bad pid list entry.");} error  
		;

pid_list:  	    pid_range
    	    	|   pid_list COMMA pid_range
		;

pid_range:  	    NUMBER {p_parse_result->CurrentTarget()->pushPidRange($1,$1);}
    	    	|   NUMBER COLON NUMBER {p_parse_result->CurrentTarget()->pushPidRange($1,$3);}
		;

thread_list_spec:   THREAD_ID thread_list
		|   THREAD_ID {p_parse_result->set_error("Bad thread list entry.");} error  
		;

thread_list:  	    thread_range
    	    	|   thread_list COMMA thread_range
		;

thread_range:  	    NUMBER {p_parse_result->CurrentTarget()->pushThreadRange($1,$1);}
    	    	|   NUMBER COLON NUMBER {p_parse_result->CurrentTarget()->pushThreadRange($1,$3);}
		;

rank_list_spec:     RANK_ID rank_list
		|   RANK_ID {p_parse_result->set_error("Bad rank list entry.");} error  
		;

rank_list:  	    rank_range
    	    	|   rank_list COMMA rank_range
		;

rank_range:  	    NUMBER {p_parse_result->CurrentTarget()->pushRankRange($1,$1);}
    	    	|   NUMBER COLON NUMBER {p_parse_result->CurrentTarget()->pushRankRange($1,$3);}
		;

ip_address: 	    NUMBER
    	    	|   IPNUMBER_32
    	    	;

param_list:  	    param
    	    	|   param_list COMMA param
		;

param:	    	    expType DOUBLE_COLON param_name EQUAL NUMBER
		     {p_parse_result->pushParm($1,$3,$5);}
    	    	    expType DOUBLE_COLON param_name EQUAL NAME
		     {p_parse_result->pushParm($1,$3,$5);}
    	    	|   param_name EQUAL NUMBER
		     {p_parse_result->pushParm(NULL,$1,$3);}
    	    	|   param_name EQUAL NAME
		     {p_parse_result->pushParm(NULL,$1,$3);}
		;

param_name: 	    PARAM_DISPLAY_MODE	    	    	
    	    	|   PARAM_DISPLAY_MEDIA	    	    	
    	    	|   PARAM_DISPLAY_REFRESH_RATE	      	
    	    	|   PARAM_EXP_SAVE_FILE	    	    	
    	    	|   PARAM_SAMPLING_RATE	    	    	
    	    	|   NAME	    	    	    	 
		;

expType_list:  	    expType 
    	    	|   expType_list COMMA expType
		;

expType:	    EXP_PCSAMP
		     {p_parse_result->pushExpType($1);}
    	    	|   EXP_USERTIME
		     {p_parse_result->pushExpType($1);}
    	    	|   EXP_MPI
		     {p_parse_result->pushExpType($1);}
    	    	|   EXP_FPE
		     {p_parse_result->pushExpType($1);}
    	    	|   EXP_HWC
		     {p_parse_result->pushExpType($1);}
    	    	|   EXP_IO
		     {p_parse_result->pushExpType($1);}
		|   NAME
		     {p_parse_result->pushExpType($1);}
		;

viewType:   	    VIEW_TOPN	    	    	{p_parse_result->pushViewType($1);}
    	    	|   VIEW_EXCLTIME   	    	{p_parse_result->pushViewType($1);}
    	    	|   VIEW_IO 	    	    	{p_parse_result->pushViewType($1);}
    	    	|   VIEW_FPE	    	    	{p_parse_result->pushViewType($1);}
    	    	|   VIEW_HWC	    	    	{p_parse_result->pushViewType($1);}
    	    	|   NAME	    	    	{p_parse_result->pushViewType($1);}
		;

lineno_range_spec:  LINE_ID lineno_range
		|   LINE_ID {p_parse_result->set_error("Bad line number entry.");} error  
    	    	;

lineno_range:	    lineno {p_parse_result->pushLineNoPoint($1);} 
     	    	|   lineno COLON lineno {p_parse_result->pushLineNoRange($1,$3);}
 		; 

lineno: 	    NUMBER
    	    	|   HEXNUMBER
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
/*
    Count will be 4 for 32 bit IP numbers and
    8 for 64 bit IP numbers.
    
    Lex will have weeded out most syntactical problems.
    
    The return value will need to change to long long
    soon.
    
 */
int
ip_number(char *input_str,int count)
{
    int i;
    char *ptr1 = input_str;
    unsigned long num = 0;
    unsigned long t_num = 0;
    
    for (i=0;i<count;++i) {
    	num = num << 8;
	t_num = strtoul(ptr1,&ptr1,10);
	/* Check for invalid value */
	if (t_num > 255) {
	    p_parse_result->set_error(input_str,"ip_number");
	}
    	num += t_num;
	ptr1++;
   }
   
    return (int)num;
}

/***************************************************/
/*
    This is a memory leak. I need to keep track
    of the allocations of memory and free them
    up periodically.
 */
char *
copy_string(char *p_old)
{
    char *p_buf = (char *)malloc(strlen(p_old)+1);
    strcpy(p_buf,p_old);
    return p_buf;
}
/***************************************************/



