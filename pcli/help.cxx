#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "support.h"

#define MAX_INDENT 11
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
    "                                    ",
    "                                        "
};

/* This must match oss_cmd_enum in support.h */
char *command_name[CMD_MAX] = {
    "", /* used in error reporting */
    "expAttach",
    "expClose",
    "expCreate",
    "expDetach",
    "expEnable",
    "expDisable",
    "expFocus",
    "expPause",
    "expRestore",
    "expGo",
    "expSave",
    "expSetparm",
    "expStop",
    "expView",
    "listExp",
    "listHosts",
    "listObj",
    "listPids",
    "listSrc",
    "listMetrics",
    "listParams",
    "listReports",
    "listBreaks",
    "listTypes",
    "clearBreak",
    "exit",
    "openGui",
    "help",
    "history",
    "log",
    "playback",
    "record",
    "setBreak"
};

/* This will eventually give way to an experiment registry. */
char *experiment_name[H_EXP_MAX] = {
    "pcsamp",
    "usertime",
    "mpi",
    "fpe",
    "hwc",
    "io"
};

/* This will eventually give way to a viewtype registry. */
char *viewtype_name[H_VIEW_MAX] = {
    "vTopN",
    "vExclTime",
    "vIO",
    "VFpe",
    "vHwc"
};

/* This will eventually give way to a paramtype registry. */
char *paramtype_name[H_PARAM_MAX] = {
    "displayMode",
    "displayMedia",
    "displayRefreshRate",
    "expSaveFile",
    "samplingRate"
};

/* General hodgepodge of names. */
char *general_name[H_GEN_MAX] = {
    "focus",
    "all",
    "copy",
    "kill",
    "gui"
};

static void help_exptype(int indent_ndx);

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
 
/**
 * Function: help_get_type
 * 
 * 
 *     
 * @param   p_help  pointer to help descriptor.
 *
 * @return  TRUE/FALSE.
 *
 */
static boolean
help_get_type(help_desc_t *p_help)
{

    int ret;
   
    ret = is_type(command_name,CMD_MAX,p_help->name);
    if (ret != CMD_MAX) {
    	p_help->tag = HELP_CMD;
	p_help->u.cmd_ndx = (oss_cmd_enum)ret;
	return TRUE;
    }
    ret = is_type(paramtype_name,H_PARAM_MAX,p_help->name);
    if (ret != H_PARAM_MAX) {
    	p_help->tag = HELP_PARM;
	p_help->u.param_ndx = (help_param_enum)ret;
	return TRUE;
    }
    ret = is_type(experiment_name,H_EXP_MAX,p_help->name);
    if (ret != H_EXP_MAX) {
    	p_help->tag = HELP_EXPERIMENT;
	p_help->u.exp_ndx = (help_exp_enum)ret;
	return TRUE;
    }
    ret = is_type(viewtype_name,H_VIEW_MAX,p_help->name);
    if (ret != H_VIEW_MAX) {
    	p_help->tag = HELP_VIEW;
	p_help->u.view_ndx = (help_view_enum)ret;
	return TRUE;
    }
    ret = is_type(general_name,H_GEN_MAX,p_help->name);
    if (ret != H_GEN_MAX) {
    	p_help->tag = HELP_GENERAL;
	p_help->u.gen_ndx = (help_gen_enum)ret;
	return TRUE;
    }
    
    return FALSE;
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
    printf("%s<%sname> = <%s>\n",
    	    indent_table[indent_ndx],
	    name,
	    format);
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
    printf("%s<%s_spec> = %s <%sname> \n",
    	    indent_table[indent_ndx],
	    name,
	    icon,
	    name);

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
    printf("%s<%s_range> = <%sname> [: <%sname>]...\n",
    	    indent_table[indent_ndx],
	    name,
	    name,
	    name);
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
help_list(char *name, char *format, boolean has_range, int indent_ndx)
{
    if (has_range) {
    	printf("%s<%s_list> = <%s_range>[,<%s_range>]...\n",
    	    	indent_table[indent_ndx],
	    	name,
	    	name,
	    	name);
    	help_range(name, format, indent_ndx+1);
    }
    else {
    	printf("%s<%s_list> = <%s_name>[,<%s_name>]...\n",
    	    	indent_table[indent_ndx],
	    	name,
	    	name,
	    	name);
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
		boolean has_range, 
		int indent_ndx)
{
    if (has_range) {
    	printf("%s<%s_list_spec> = %s <%s_list> \n",
    	    	indent_table[indent_ndx],
	    	name,
	    	icon,
	    	name);
    	help_list(name, format, has_range, indent_ndx+1);
    }
    else {
    	printf("%s<%s_list_spec> = <%s_name>[,<%s_name>]...\n",
    	    	indent_table[indent_ndx],
	    	name,
	    	icon,
	    	name);
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
    printf("%s<expMetric> = [<expType>::] <metricname\n", 
    	    indent_table[indent_ndx]);
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

    printf("%s<paramname> = {",indent_table[indent_ndx]);
    for (i=0;i<H_PARAM_MAX;++i) {
    	printf("%s %s ", 
	    	paramtype_name[i],
		i==H_PARAM_MAX-1 ? " " : "||");
    }
    printf("}\n");
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
    printf("%s<expParam> = [<expType>::] <paramname>\n",
    	    indent_table[indent_ndx]);
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

    printf("%s<predefined_exp> = {",indent_table[indent_ndx]);
    for (i=0;i<H_EXP_MAX;++i) {
    	printf("%s %s ", 
	    	experiment_name[i],
		i==H_EXP_MAX-1 ? " " : "||");
    }
    printf("\n");
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
    printf("%s<user_define_exp = string\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<expType = <predefined_exp> || <user_define_exp>\n",
    	    indent_table[indent_ndx]);
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

    printf("%s<expType_list> = <expType>[,expType>]\n",
    	    indent_table[indent_ndx]);
    help_exptype(indent_ndx);
    help_user_defined_exp(indent_ndx);
    help_predefined_exp(indent_ndx);
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
    printf("%s<IP_address> = <int>.<int>.<int>.<int>\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<filename> = <string> || \"<string>\"\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<hostname> = <string> | IP_address*\n",
    	    indent_table[indent_ndx]);
    help_ip_address(indent_ndx);
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
    printf("%s<file_spec> = -f <filename>\n",
    	    indent_table[indent_ndx]);
    help_filename(indent_ndx);
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
    printf("%s<host_spec> = -h <hostname>\n",
    	    indent_table[indent_ndx]);
    help_hostname(indent_ndx);
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
    printf("%s<host_file> = [<host_spec>] <file_spec>\n",
    	    indent_table[indent_ndx]);
    help_host_spec(indent_ndx);
    help_file_spec(indent_ndx);
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
    printf("%s<file_list> = <filename>[,<filename>]*\n",
    	    indent_table[indent_ndx]);
    help_filename(indent_ndx);
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
    printf("%s<host_list> = <hostname>[,<hostname>]*\n",
    	    indent_table[indent_ndx]);
    help_hostname(indent_ndx);
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
help_host_file_rpd(int indent_ndx)
{
    printf("%s<host_file_rpd> = "
    	    "\t[<host_list_spec>]"
	    "\n\t\t\t\t[<file_list_spec>]"
    	    "\n\t\t\t\t[<pid_list_spec>]"
	    "\n\t\t\t\t[<thread_list_spec>]"
	    "\n\t\t\t\t[<rank_list_spec>]\n",
	    indent_table[indent_ndx]);
    help_list_spec("-h","host",     "int",TRUE,indent_ndx+1);
    help_list_spec("-f","file",     "int",TRUE,indent_ndx+1);
    help_list_spec("-p","pid",	    "int",TRUE,indent_ndx+1);
    help_list_spec("-t","thread",   "int",TRUE,indent_ndx+1);
    help_list_spec("-r","rank",     "int",TRUE,indent_ndx+1);
}

/**
 * Function: help_host_file_rpd_list
 * 
 * 
 *     
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
help_host_file_rpd_list(int indent_ndx)
{
    printf("%s<host_file_rpd_list> = <host_file_rpd>[,host_file_rpd>]...\n",
    	    indent_table[indent_ndx]);
    help_host_file_rpd(indent_ndx+1);
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
    printf("%s<address_description> = <64bit value>\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<linenumber> = <int>\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<linenumber_range> = <linenumber> [:<linenumber>]...\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<linenumber_spec> = -l <linenumber_range>\n",
    	    indent_table[indent_ndx]);
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

    printf("%s<viewType> = ",indent_table[indent_ndx]);
    for (i=0;i<H_VIEW_MAX;++i) {
    	printf("%s %s ", 
	    	viewtype_name[i],
		i==H_VIEW_MAX-1 ? " " : "||");
    }
    printf("\n");
}
  
/**
 * Function: dump_help_cmd
 * 
 * 
 *     
 * @param   p_help  	pointer to help descriptor.
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
dump_help_cmd(help_desc_t *p_help, int indent_ndx)
{

    printf("\n");
    switch(p_help->u.cmd_ndx) {
    	case CMD_EXP_ATTACH:
    	case CMD_EXP_DETACH:
    	    printf("%s  [<expId_spec>] [<host_file_rpd_list>] [<expType_list>]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_host_file_rpd_list(indent_ndx+1 /* number of indents */);
    	    help_expType_list(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_EXP_CLOSE:
    	    printf("%s  [focus || <expId_spec> || all] [kill] \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
   	case CMD_EXP_DISABLE:
   	case CMD_EXP_ENABLE:
    	case CMD_EXP_PAUSE:
    	case CMD_EXP_GO:
    	case CMD_EXP_STOP:
    	    printf("%s  [<expId_spec>] || all \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
    	case CMD_EXP_CREATE:
    	    printf("%s  [<host_file_rpd_list>] [<expType_list>]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_host_file_rpd_list(indent_ndx+1 /* number of indents */);
    	    help_expType_list(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_EXP_RESTORE:
    	case CMD_HISTORY:
    	case CMD_LOG:
    	case CMD_PLAYBACK:
    	case CMD_RECORD:
    	    printf("%s  <host_file> \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_host_file(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_EXP_SAVE:
    	    printf("%s  [<expId_spec>] [copy] <host_file> \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_host_file(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_EXP_SETPARAM:
    	    printf("%s  [<expId_spec>] <expParam>=<expParamValue> [,<expParam>=<expParamValue>] \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
	    help_expparam(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_EXP_VIEW:
    	    printf("%s  [<expId_spec>] [-gui] <viewType> \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
	    help_viewtype(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_LIST_EXP:
    	    printf("%s  <expId_spec> || all \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
     	    break;
    	case CMD_LIST_HOSTS:
    	    printf("%s  <host_spec> \n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_host_spec(indent_ndx+1 /* number of indents */);
     	    break;
    	case CMD_LIST_OBJ:
    	    printf("%s  [<expId_spec>] [<host_file_rpd_list>]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_host_file_rpd_list(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_LIST_PIDS:
    	    printf("%s  <host_file> [-mpi]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_host_file(indent_ndx+1 /* number of indents */);
    	    break;
    	case CMD_LIST_SRC:
    	    printf("%s  [<expId_spec>] [<host_file_rpd_list>] [<linenumber_spec>]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_host_file_rpd_list(indent_ndx+1 /* number of indents */);
	    help_linenumber_spec(indent_ndx+1);
    	    break;
    	case CMD_LIST_METRICS:
    	case CMD_LIST_PARAMS:
    	case CMD_LIST_REPORTS:
    	    printf("%s  [<expId_spec> || all]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
    	case CMD_LIST_BREAKS:
    	    printf("%s  [<expId_spec> || all]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    break;
    	case CMD_CLEAR_BREAK:
    	    printf("%s  <breakId>\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_name("breakId", "int", indent_ndx+1);
    	    break;
    	case CMD_EXIT:
	case CMD_LIST_TYPES:
    	case CMD_OPEN_GUI:
    	    printf("%s <no args>\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    break;
    	case CMD_HELP:
    	    printf("%s <string>\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    break;
    	case CMD_SETBREAK:
    	    printf("%s  [<expId_spec>] [<host_file_rpd_list>] [<address_description>]\n",
    	    	    command_name[p_help->u.cmd_ndx]);
    	    printf("%sWhere:\n", indent_table[indent_ndx]);
    	    help_spec("-x","expId",     "int",indent_ndx+1);
    	    help_host_file_rpd_list(indent_ndx+1 /* number of indents */);
	    help_address_description(indent_ndx+1);
    	    break;
    	default :
    	    printf("No help for %s\n",p_help->name);
    	    break;
    }
}
 
/**
 * Function: dump_help_param
 * 
 * 
 *     
 * @param   p_help  	pointer to help descriptor.
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
dump_help_param(help_desc_t *p_help, int indent_ndx)
{
    switch(p_help->u.param_ndx) {
    	case H_PARAM_DMODE:
    	case H_PARAM_DMEDIA:
    	case H_PARAM_DREFRESH:
    	case H_PARAM_XSAVEFILE:
    	case H_PARAM_SAMPRATE:
    	default :
    	    printf("No help for %s\n",p_help->name);
    	    break;
    }
}
 
/**
 * Function: dump_help_exp
 * 
 * 
 *     
 * @param   p_help  	pointer to help descriptor.
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
dump_help_exp(help_desc_t *p_help, int indent_ndx)
{
    switch(p_help->u.param_ndx) {
    	case H_EXP_PCSAMP:
    	case H_EXP_USERTIME:
    	case H_EXP_MPI:
    	case H_EXP_FPE:
    	case H_EXP_HWC:
    	case H_EXP_IO:
    	default :
    	    printf("No help for %s\n",p_help->name);
    	    break;
    }
}
 
/**
 * Function: dump_help_view
 * 
 * 
 *     
 * @param   p_help  	pointer to help descriptor.
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
dump_help_view(help_desc_t *p_help, int indent_ndx)
{
    switch(p_help->u.param_ndx) {
    	case H_VIEW_TOPN:
    	case H_VIEW_EXCLTIME:
    	case H_VIEW_IO:
    	case H_VIEW_FPE:
    	case H_VIEW_HWC:
    	default :
    	    printf("No help for %s\n",p_help->name);
    	    break;
    }
}
 
/**
 * Function: dump_help_gen
 * 
 * 
 *     
 * @param   p_help  	pointer to help descriptor.
 * @param   indent_ndx	indentation index.
 *
 * @return  void.
 *
 */
static void
dump_help_gen(help_desc_t *p_help, int indent_ndx)
{
    switch(p_help->u.param_ndx) {
    	case H_GEN_FOCUS:
    	    printf("\n%s: Alters which experiment is the default focus.\n",
	    	    p_help->name);
    	    break;
    	case H_GEN_ALL:
    	    printf("\n%s: Apply action to all targets.\n",
	    	    p_help->name);
    	    break;
    	case H_GEN_COPY:
    	    printf("\n%s: Copy state to database.\n",
	    	    p_help->name);
    	    break;
     	case H_GEN_KILL:
    	    printf("\n%s: Force applications to terminate.\n",
	    	    p_help->name);
    	    break;
    	case H_GEN_GUI:
    	    printf("\n%s: Lauch the gui for display.\n",
	    	    p_help->name);
    	    break;
    	default :
    	    printf("\nNo help for %s\n",p_help->name);
    	    break;
    }
}
 
/**
 * Function: dump_help
 * 
 * 
 *     
 * @param   xxx    	xxx.
 *
 * @return  void.
 *
 */
void 
dump_help(command_t *p_cmd)
{
    if (command.help_table.cur_node) {
    	int i;
	help_desc_t *help_tab = (help_desc_t *)command.help_table.table;
	int count = command.help_table.cur_node;
	int indent = 1;

	for (i=0;i<count;++i) {
	    help_desc_t *p_help = &help_tab[i];
	    
	    if  (help_get_type(p_help)) {
	    	switch (help_tab[i].tag) {
		    case HELP_CMD:
		    	dump_help_cmd(p_help, indent);
		    	break;
		    case HELP_PARM:
		    	dump_help_param(p_help, indent);
		    	break;
		    case HELP_EXPERIMENT:
		    	dump_help_exp(p_help, indent);
		    	break;
		    case HELP_VIEW:
		    	dump_help_view(p_help, indent);
		    	break;
		    case HELP_GENERAL:
		    	dump_help_gen(p_help, indent);
		    	break;
		    default:
	    	    	printf("No help for %s",help_tab[i].name);
			break;
		} /* switch */
	    }	/* if */

	    else
	    	printf("No help for %s",help_tab[i].name);
	}   /* for */
	printf("\n");
    }	/* if */

    fflush(stdout);
}

