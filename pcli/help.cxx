/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <vector>
#include <iostream>

#include "SS_Input_Manager.hxx"
#include "SS_Cmd_Execution.hxx"

using namespace std;

#include "SS_Parse_Param.hxx"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

#define MAX_STRING 1024

char *indent_table[MAX_INDENT] = {
    "",
    "    ",
    "        ",
    "            ",
    "                ",
    "                    ",
    "                        ",
    "                            ",
    "                                ",
/* Note: This dummy comment is used to eliminate a false-positive found by
   the SGI source scanner when comparing against AT&T Unix SVR4.1ES source.
*/
    "                                    ",
    "                                        "
};

/** Decribe message information */
typedef struct {
    char *brief_string; /**  */
    char **normal;  	/**  */
    int normal_count;   /**  */
    char **verbose; 	/**  */
    int verbose_count;	/**  */
    char **examples;	/**  */
    int example_count;	/**  */
} message_type_t;

// **************************************************************



// **************************************************************

extern ParseResult *p_parse_result;
extern CommandObject *p_cmdobj;

static void help_exptype(int indent_ndx);
static void help_host_list(int indent_ndx);
static command_t command;
static char *p_buf;

#if 0
/**
 * Function: xxx
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
xxx(int indent_ndx)
{

}
#endif

/**
 * Function: s_output_to_CO
 * 
 * @param   input_str string to push to command object.
 *
 * @return  void.
 *
 */
static void
s_output_to_CO(char *input_str)
{

//    int len = strlen(input_str)+1;
//    char *t_str = (char *)malloc(len);

//    strcpy(t_str,input_str);
//    printf("%s ",t_str);
//    p_cmdobj->Result_String (t_str);
//    p_cmdobj->Result_String ("STUPID STUFF!!");
   p_cmdobj->Result_String (p_buf);

}
/**
 * Function: is_type
 * 
 * 
 *     
 * @param   table   string table.
 * @param   count   count of string table.
 * @param   name    string to be searched.
 *
 * @return  void.
 *
 */
static int
is_type(char **table, int count, char *name)
{

    int i;
    
    for (i=0;i<count;++i) {
    	if ((strcmp(name,table[i])) == 0) {
	    return i;
	}
    }
    
    return count;
}
  
/******************************************************************/
/**
 * Function: help_name
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_name(char *name, char *format, int indent_ndx)
{
    sprintf(p_buf,"%s<%sname> ::= <%s> ",
    	    indent_table[indent_ndx],
	    name,
	    format);

    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_num_name
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_spec(char *icon, char *name, char *format, int indent_ndx)
{
    sprintf(p_buf,"%s<%s_spec> ::= %s <%sname>  ",
    	    indent_table[indent_ndx],
	    name,
	    icon,
	    name);
    s_output_to_CO(p_buf);

    help_name(name, format, indent_ndx+1);
}
 
/**
 * Function: help_range
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_range(char *name, char *format, int indent_ndx)
{
    sprintf(p_buf,"%s<%s_range> ::= <%sname> [: <%sname>]... ",
    	    indent_table[indent_ndx],
	    name,
	    name,
	    name);
    s_output_to_CO(p_buf);
    help_name(name, format, indent_ndx+1);
}
 
/**
 * Function: help_list
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_list(char *name, char *format, bool has_range, int indent_ndx)
{
    if (has_range) {
    	sprintf(p_buf,"%s<%s_list> ::= <%s_range>[,<%s_range>]... ",
    	    	indent_table[indent_ndx],
	    	name,
	    	name,
	    	name);
    	s_output_to_CO(p_buf);
    	help_range(name, format, indent_ndx+1);
    }
    else {
    	sprintf(p_buf,"%s<%s_list> ::= <%s_name>[,<%s_name>]... ",
    	    	indent_table[indent_ndx],
	    	name,
	    	name,
	    	name);
    	s_output_to_CO(p_buf);
    	help_name(name, format, indent_ndx+1);
    }
}
 
/**
 * Function: help_list_spec
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_list_spec(
    	    	char *icon, 
    	    	char *name, 
		char *format, 
		bool has_range, 
		int indent_ndx)
{
    if (has_range) {
    	sprintf(p_buf,"%s<%s_list_spec> ::= %s <%s_list>  ",
    	    	indent_table[indent_ndx],
	    	name,
	    	icon,
	    	name);
    	s_output_to_CO(p_buf);
    	help_list(name, format, has_range, indent_ndx+1);
    }
    else {
    	sprintf(p_buf,"%s<%s_list_spec> ::= <%s_name>[,<%s_name>]... ",
    	    	indent_table[indent_ndx],
	    	name,
	    	icon,
	    	name);
    	s_output_to_CO(p_buf);
    	help_name(name, format, indent_ndx+1);
    }
}
 
/******************************************************************/

/**
 * Function: help_expmetric
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_expmetric(int indent_ndx)
{
    sprintf(p_buf,"%s<expMetric> ::= [<expType>::] <metricname ", 
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_exptype(indent_ndx+1);
    help_name("metricname", "string", indent_ndx+1);
}
 
/**
 * Function: help_paramname
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_paramname(int indent_ndx)
{
    int i;

    sprintf(p_buf,"%s<paramname> ::= {",indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    for (i=0;i<H_PARAM_MAX;++i) {
    	sprintf(p_buf,"%s %s ", 
	    	paramtype_name[i],
		i==H_PARAM_MAX-1 ? " " : "||");
    	s_output_to_CO(p_buf);
    }
    sprintf(p_buf,"} ");
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_expparam
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_expparam(int indent_ndx)
{
    sprintf(p_buf,"%s<expParam> ::= [<expType>::] <paramname> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_predefined_exp
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_predefined_exp(int indent_ndx)
{
    int i;

    sprintf(p_buf,"%s<predefined_exp> ::= ",indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    for (i=0;i<H_EXP_MAX;++i) {
    	sprintf(p_buf,"%s%s %s ", 
	    	indent_table[indent_ndx+1],
	    	experiment_name[i],
		i==H_EXP_MAX-1 ? " " : "||");
    	s_output_to_CO(p_buf);
    }
//    sprintf(p_buf," ");
//    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_user_defined_exp
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_user_defined_exp(int indent_ndx)
{
    sprintf(p_buf,"%s<user_define_exp> ::= string ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_exptype
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_exptype(int indent_ndx)
{
    sprintf(p_buf,"%s<expType = <predefined_exp> || <user_define_exp> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_expType_list
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_expType_list(int indent_ndx)
{

    sprintf(p_buf,"%s<expType_list> ::= <expType>[,expType>] ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_exptype(indent_ndx+1);
    help_user_defined_exp(indent_ndx+1);
    help_predefined_exp(indent_ndx+1);
}
 
/******************************************************************/
/**
 * Function: help_ip_address
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_ip_address(int indent_ndx)
{
    sprintf(p_buf,"%s<IP_address> ::= <int>.<int>.<int>.<int> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_filename
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_filename(int indent_ndx)
{
    sprintf(p_buf,"%s<filename> ::= <string> || \"<string>\" ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_hostname
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_hostname(int indent_ndx)
{
    sprintf(p_buf,"%s<hostname> ::= <string> | IP_address* ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_ip_address(indent_ndx+1);
}
 
/**
 * Function: help_file_spec
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_file_spec(int indent_ndx)
{
    sprintf(p_buf,"%s<file_spec> ::= -f <filename> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_filename(indent_ndx+1);
}
 
/**
 * Function: help_host_spec
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_host_spec(int indent_ndx)
{
    sprintf(p_buf,"%s<host_spec> ::= -h <host_list> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_host_list(indent_ndx+1);
}
 
/**
 * Function: help_host_file
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_host_file(int indent_ndx)
{
    sprintf(p_buf,"%s<host_file> ::= [<host_spec>] <file_spec> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_host_spec(indent_ndx+1);
    help_file_spec(indent_ndx+1);
}

/**
 * Function: help_file_list
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_file_list(int indent_ndx)
{
    sprintf(p_buf,"%s<file_list> ::= <filename>[,<filename>]* ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_filename(indent_ndx+1);
}
 
/**
 * Function: help_host_list
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_host_list(int indent_ndx)
{
    sprintf(p_buf,"%s<host_list> ::= <hostname>[,<hostname>]* ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_hostname(indent_ndx+1);
}
 
/**
 * Function: xxx
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_target(int indent_ndx)
{
    sprintf(p_buf,"%s<target> ::= "
    	    "\n\t\t[<host_list_spec>]"
	    "\n\t\t[<file_list_spec>]"
    	    "\n\t\t[<pid_list_spec>]"
	    "\n\t\t[<thread_list_spec>]"
	    "\n\t\t[<cluster_list_spec>]"
	    "\n\t\t[<rank_list_spec>]\n",
	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);

    help_list_spec("-h","host",     "<string> || <IP_Address>",true,indent_ndx+1);
    help_list_spec("-f","file",     "<string>",true,indent_ndx+1);
    help_list_spec("-p","pid",	    "<int>",true,indent_ndx+1);
    help_list_spec("-t","thread",   "<int>",true,indent_ndx+1);
    help_list_spec("-c","cluster",   "<int> || <string>",true,indent_ndx+1);
    help_list_spec("-r","rank",     "<int>",true,indent_ndx+1);
}

/**
 * Function: help_target_list
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_target_list(int indent_ndx)
{
    sprintf(p_buf,"%s<target_list> ::= <target>[,target>]... ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_target(indent_ndx+1);
}
 
/******************************************************************/

/**
 * Function: help_address_description
 * 
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_address_description(int indent_ndx)
{
    sprintf(p_buf,"%s<address_description> ::= <64bit value> || <string> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}

/**
 * Function: help_linenumber
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_linenumber(int indent_ndx)
{
    sprintf(p_buf,"%s<linenumber> ::= <int> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
}
 
/**
 * Function: help_linenumber_range
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_linenumber_range(int indent_ndx)
{
    sprintf(p_buf,"%s<linenumber_range> ::= <linenumber> [:<linenumber>]... ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_linenumber(indent_ndx+1 /* number of indents */);
}
 
/**
 * Function: help_linenumber_spec
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_linenumber_spec(int indent_ndx)
{
    sprintf(p_buf,"%s<linenumber_spec> ::= -l <linenumber_range> ",
    	    indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    help_linenumber_range(indent_ndx+1 /* number of indents */);
}
 
/**
 * Function: help_viewtype
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_viewtype(int indent_ndx)
{
    int i;

    sprintf(p_buf,"%s<viewType> ::= ",indent_table[indent_ndx]);
    s_output_to_CO(p_buf);
    for (i=0;i<H_VIEW_MAX;++i) {
    	sprintf(p_buf,"%s %s ", 
	    	viewtype_name[i],
		i==H_VIEW_MAX-1 ? " " : "||");
    	s_output_to_CO(p_buf);
    }
    sprintf(p_buf," ");
    s_output_to_CO(p_buf);
}
  
/**
 * Function: dump_help_cmd
 * 
 * 
 *     
 * @param   cmd_ndx  	index to command string.
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
void
dump_help_cmd(oss_cmd_enum cmd_ndx, int indent_ndx, bool is_brief,CommandObject *cmd)
{
    char buf[MAX_STRING];
    
    p_buf = buf; // Make it used by all the help routines.
    p_cmdobj = cmd;

    switch(cmd_ndx) {
    	case CMD_EXP_ATTACH:
    	case CMD_EXP_DETACH:
    	    sprintf(p_buf,"%s  [<expId_spec>] [<target_list>] [<expType_list>]",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_target_list(indent_ndx+1 /* number of indents */);
    	    help_expType_list(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_EXP_CLOSE:
    	    sprintf(p_buf,"%s  [focus || <expId_spec> || all] [kill]  ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
    	case CMD_LIST_STATUS:
    	case CMD_LIST_TYPES:
   	case CMD_EXP_DISABLE:
   	case CMD_EXP_ENABLE:
    	case CMD_EXP_PAUSE:
    	case CMD_EXP_GO:
    	    sprintf(p_buf,"%s  [<expId_spec> || all]  ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
    	case CMD_EXP_FOCUS:
    	    sprintf(p_buf,"%s  [<expId_spec>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
    	case CMD_EXP_CREATE:
    	    sprintf(p_buf,"%s  [<target_list>] [<expType_list>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_target_list(indent_ndx+1 /* number of indents */);
    	    help_expType_list(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_EXP_RESTORE:
    	case CMD_HISTORY:
    	case CMD_LOG:
    	case CMD_PLAYBACK:
    	case CMD_RECORD:
    	    sprintf(p_buf,"%s  <file_spec>  ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_file_spec(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_EXP_SAVE:
    	    sprintf(p_buf,"%s  [<expId_spec>] [copy] <file_spec>  ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_file_spec(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_EXP_SETPARAM:
    	    sprintf(p_buf,"%s  [<expId_spec>] <expParam>=<expParamValue> [,<expParam>=<expParamValue>]  ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
	    help_expparam(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_EXP_VIEW:
    	    sprintf(p_buf,"%s  [<expId_spec>] [-gui] <viewType>  ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
	    help_viewtype(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_LIST_HOSTS:
    	    sprintf(p_buf,"%s  [<expId_spec> || all || <cluster_spec>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_spec("-c","cluster",	"string || int",indent_ndx+1);
     	    break;
    	case CMD_LIST_OBJ:
    	    sprintf(p_buf,"%s  [<expId_spec>] [<target>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_target(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_LIST_PIDS:
    	    sprintf(p_buf,"%s [<expId_spec> || all] [<host_file>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_host_file(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_LIST_SRC:
    	    sprintf(p_buf,"%s  [<expId_spec>] [<target>] [<linenumber_spec>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_target(indent_ndx+1 /* number of indents */);
	    help_linenumber_spec(indent_ndx+1);
    	    break;
    	case CMD_LIST_METRICS:
    	case CMD_LIST_PARAMS:
    	case CMD_LIST_RANKS:
    	case CMD_LIST_BREAKS:
    	case CMD_LIST_VIEWS:
    	    sprintf(p_buf,"%s  [<expId_spec> || all || <expType>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_exptype(indent_ndx+1);
    	    break;
    	case CMD_LIST_THREADS:
    	    sprintf(p_buf,"%s  [<expId_spec> || all || <target>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_target(indent_ndx+1);
    	    break;
    	case CMD_CLEAR_BREAK:
    	    sprintf(p_buf,"%s  <breakId> ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_name("breakId", "int", indent_ndx+1);
    	    break;
    	case CMD_LIST_EXP:
    	case CMD_EXIT:
    	case CMD_OPEN_GUI:
    	    sprintf(p_buf,"%s <no args> ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    break;
    	case CMD_HELP:
    	    sprintf(p_buf,"%s [<string>] ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    break;
    	case CMD_SETBREAK:
    	    sprintf(p_buf,"%s  [<expId_spec>] [<target>] <address_description> ",
    	    	    OpenSpeedShop::cli::cmd_desc[cmd_ndx].name);
    	    s_output_to_CO(p_buf);
    	    if (is_brief)
	    	break;
	    
    	    sprintf(p_buf,"%sWhere: ", indent_table[indent_ndx]);
    	    s_output_to_CO(p_buf);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_target(indent_ndx+1 /* number of indents */);
	    help_address_description(indent_ndx+1);
    	    break;
    	default :
    	    sprintf(p_buf,"No help for requested string ");
    	    s_output_to_CO(p_buf);
    	    break;
    }
}
 
/**
 * Function: dump_help_brief
 * 
 * Dump a one line per command description.
 *     
 *
 * @return  void.
 *
 */
void
dump_help_brief(CommandObject *cmd)
{

    for (int i=1;i<CMD_MAX;++i) {
    	dump_help_cmd((oss_cmd_enum)i,
	    	      0 /* indentation level */,
		      true /* is_brief */,
		      cmd);
    }
}

/**
 * Function: dump_help_param
 * 
 * 
 *     
 * @param   name  	param string.
 * @param   param_ndx  	index to param type.
 * @param   is_brief	is this a short or verbose message.
 * @param   cmd     	pointer to command object.
 *
 * @return  void.
 *
 */
void
dump_help_param(char *name, int param_ndx, bool is_brief, CommandObject *cmd)
{
    char buf[MAX_STRING];
    
    p_buf = buf; // Make it used by all the help routines.
    p_cmdobj = cmd;

    switch(param_ndx) {
    	case H_PARAM_DMODE:
    	case H_PARAM_DMEDIA:
    	case H_PARAM_DREFRESH:
    	case H_PARAM_XSAVEFILE:
    	case H_PARAM_SAMPRATE:
    	default :
    	    sprintf(p_buf,"No help for %s ", name);
    	    s_output_to_CO(p_buf);
    	    break;
    }
}
 
/**
 * Function: dump_help_exp
 * 
 * 
 *     
 * @param   name  	param string.
 * @param   param_ndx  	index to param type.
 * @param   is_brief	is this a short or verbose message.
 * @param   cmd     	pointer to command object.
 *
 * @return  void.
 *
 */
void
dump_help_exp(char *name, int param_ndx, bool is_brief, CommandObject *cmd)
{
    char buf[MAX_STRING];
    
    p_buf = buf; // Make it used by all the help routines.
    p_cmdobj = cmd;

    switch(param_ndx) {
    	case H_EXP_PCSAMP:
    	case H_EXP_USERTIME:
    	case H_EXP_MPI:
    	case H_EXP_FPE:
    	case H_EXP_HWC:
    	case H_EXP_IO:
    	default :
    	    sprintf(p_buf,"No help for %s ", name);
    	    s_output_to_CO(p_buf);
    	    break;
    }
}
 
/**
 * Function: dump_help_view
 * 
 * 
 *     
 * @param   name  	param string.
 * @param   param_ndx  	index to param type.
 * @param   is_brief	is this a short or verbose message.
 * @param   cmd     	pointer to command object.
 *
 * @return  void.
 *
 */
void
dump_help_view(char *name, int view_ndx, bool is_brief, CommandObject *cmd)
{
    char buf[MAX_STRING];
    
    p_buf = buf; // Make it used by all the help routines.
    p_cmdobj = cmd;

    switch(view_ndx) {
    	case H_VIEW_TOPN:
    	case H_VIEW_EXCLTIME:
    	case H_VIEW_IO:
    	case H_VIEW_FPE:
    	case H_VIEW_HWC:
    	default :
    	    sprintf(p_buf,"No help for %s ", name);
    	    s_output_to_CO(p_buf);
    	    break;
    }
}
 
/**
 * Function: dump_help_gen
 * 
 * 
 *     
 * @param   name  	param string.
 * @param   param_ndx  	index to param type.
 * @param   is_brief	is this a short or verbose message.
 * @param   cmd     	pointer to command object.
 *
 * @return  void.
 *
 */
void
dump_help_gen(char *name, int gen_ndx, bool is_brief, CommandObject *cmd)
{
    char buf[MAX_STRING];
    
    p_buf = buf; // Make it used by all the help routines.
    p_cmdobj = cmd;

    switch(gen_ndx) {
    	case H_GEN_FOCUS:
    	    sprintf(p_buf,"\n%s: Alters which experiment is the default focus. ",
	    	    name);
    	    break;
    	case H_GEN_ALL:
    	    sprintf(p_buf,"\n%s: Apply action to all targets. ",
	    	    name);
    	    break;
    	case H_GEN_COPY:
    	    sprintf(p_buf,"\n%s: Copy state to database. ",
	    	    name);
    	    break;
     	case H_GEN_KILL:
    	    sprintf(p_buf,"\n%s: Force applications to terminate. ",
	    	    name);
    	    break;
    	case H_GEN_GUI:
    	    sprintf(p_buf,"\n%s: Lauch the gui for display. ",
	    	    name);
    	    break;
    	default :
    	    sprintf(p_buf,"\nNo help for %s ", name);
    	    break;
    }
    s_output_to_CO(p_buf);
}
 
