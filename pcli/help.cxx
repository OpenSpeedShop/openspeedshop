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

#define MAX_INDENT 11
typedef enum {
    HOST_NAME,
    HOST_NUM,
    HOST_MAX
} oss_host_enum;

typedef enum {
    HELP_DUNNO,
    HELP_CMD,
    HELP_PARM,
    HELP_EXPERIMENT,
    HELP_VIEW,
    HELP_GENERAL,
    HELP_MAX
} oss_help_enum_t;

typedef enum {
    H_PARAM_DMODE,
    H_PARAM_DMEDIA,
    H_PARAM_DREFRESH,
    H_PARAM_XSAVEFILE,
    H_PARAM_SAMPRATE,
    H_PARAM_MAX
} help_param_enum;

typedef enum {
    H_EXP_PCSAMP,
    H_EXP_USERTIME,
    H_EXP_MPI,
    H_EXP_FPE,
    H_EXP_HWC,
    H_EXP_IO,
    H_EXP_MAX
} help_exp_enum;

typedef enum {
    H_VIEW_TOPN,
    H_VIEW_EXCLTIME,
    H_VIEW_IO,
    H_VIEW_FPE,
    H_VIEW_HWC,
    H_VIEW_MAX
} help_view_enum;

typedef enum {
    H_GEN_FOCUS,
    H_GEN_ALL,
    H_GEN_COPY,
    H_GEN_KILL,
    H_GEN_GUI,
    H_GEN_MPI,
    H_GEN_MAX,
} help_gen_enum;

typedef struct help_desc {
    char *name;
    oss_help_enum_t tag;
    union {
    	oss_cmd_enum cmd_ndx;
	help_param_enum param_ndx;
	help_exp_enum exp_ndx;
	help_view_enum view_ndx;
	help_gen_enum gen_ndx;
    } u;
} help_desc_t;

/** A host can be either a number or a string */
typedef struct host_id {
    union {
    	char *name;
	unsigned ip_num;
    }u;
    oss_host_enum tag;
} host_id_t;

/** Should match all the tables in command struct */
typedef enum {
    TABLE_DUNNO,
    TABLE_NAME,
    TABLE_PID,
    TABLE_BREAK,
    TABLE_ADDRESS,
    TABLE_RANK,
    TABLE_THREAD,
    TABLE_HOST,
    TABLE_FILE,
    TABLE_HELP,
    TABLE_LINENO,
    TABLE_MAX
} oss_table_enum;

typedef enum {
    NAME_DUNNO,
    NAME_HELP,
    NAME_ADDR,
    OSS_NAME_MAX
} oss_name_enum;

/** Which suboption is this string associated with? */
typedef struct name_tab {
    char *name;
    oss_name_enum tag;
} name_tab_t;

/** range of int values */
typedef struct {
    int first;
    int second;
} int_range_t;

/** range of addresses which can be 64 bit */
typedef struct {
    int first;
    int second;
} addr_range_t;

/** host/name pairs */
typedef struct {
    char *host;
    char *file;
} name_tuple_t;

/** tag values for arg_desc_t */
typedef enum {
    ARG_INT_VAL,
    ARG_ADDR_VAL,
    ARG_INT_RANGE,
    ARG_ADDR_RANGE,
    ARG_STRING,
    ARG_HOST_FILE,
    OSS_ARG_MAX
} oss_arg_enum;

/** Storage of argument values */
typedef struct {
    oss_arg_enum tag;
    union {
    	int int_value;
	int addr_value; /* needs to be 64 bit! */
	int_range_t int_range;
	addr_range_t addr_range;
	char *string;
	name_tuple_t hostfile;
    } u;
} arg_desc_t;

/** Generic table database. */
typedef struct cmd_array_desc cmd_array_desc_t;
struct cmd_array_desc {
    void *  table;  	/**< Table of entry_size sized structures */
    
    int cur_node;	/**< Current available node in table. */
    int max_node;	/**< Max entry possible in table. */
    int entry_size; 	/**< Size in bytes for each entry in table. */
};

typedef struct a_command_struct {
    cmd_array_desc_t name_table;    /**< General strings */
    cmd_array_desc_t pid_table;     /**< Process IDs */
    cmd_array_desc_t break_table;   /**< Break ids */
    cmd_array_desc_t address_table; /**< Addresses */
    cmd_array_desc_t rank_table;    /**< Rank values */
    cmd_array_desc_t thread_table;  /**< Thread IDs */
    cmd_array_desc_t host_tablenedit ;    /**< Host names */
    cmd_array_desc_t file_table;    /**< File names */
    cmd_array_desc_t experiment_table;    /**< Experiment names */
    cmd_array_desc_t param_table;   /**< Parameter names */
    cmd_array_desc_t view_table;    /**< View type names */
    cmd_array_desc_t help_table;    /**< Help requests */
    cmd_array_desc_t lineno_table;  /**< Line numbers */
    
    int 	    exp_id; 	    /**< experiment ID */
    oss_cmd_enum    type;   	    /**< type of command */
    bool 	    error_found;    /**< This command has been invalidated */
} command_t;
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
    "gui",
    "mpi"
};

static void help_exptype(int indent_ndx);
static void help_host_list(int indent_ndx);
static command_t command;

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
 * @return  true/false.
 *
 */
static bool
help_get_type(help_desc_t *p_help)
{

    int ret;
   
    ret = is_type(command_name,CMD_MAX,p_help->name);
    if (ret != CMD_MAX) {
    	p_help->tag = HELP_CMD;
	p_help->u.cmd_ndx = (oss_cmd_enum)ret;
	return true;
    }
    ret = is_type(paramtype_name,H_PARAM_MAX,p_help->name);
    if (ret != H_PARAM_MAX) {
    	p_help->tag = HELP_PARM;
	p_help->u.param_ndx = (help_param_enum)ret;
	return true;
    }
    ret = is_type(experiment_name,H_EXP_MAX,p_help->name);
    if (ret != H_EXP_MAX) {
    	p_help->tag = HELP_EXPERIMENT;
	p_help->u.exp_ndx = (help_exp_enum)ret;
	return true;
    }
    ret = is_type(viewtype_name,H_VIEW_MAX,p_help->name);
    if (ret != H_VIEW_MAX) {
    	p_help->tag = HELP_VIEW;
	p_help->u.view_ndx = (help_view_enum)ret;
	return true;
    }
    ret = is_type(general_name,H_GEN_MAX,p_help->name);
    if (ret != H_GEN_MAX) {
    	p_help->tag = HELP_GENERAL;
	p_help->u.gen_ndx = (help_gen_enum)ret;
	return true;
    }
    
    return false;
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
help_list(char *name, char *format, bool has_range, int indent_ndx)
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
		bool has_range, 
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
    printf("%s<user_define_exp> = string\n",
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
    printf("%s<file_spec> = -f <filename>\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<host_spec> = -h <host_list>\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<host_file> = [<host_spec>] <file_spec>\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<file_list> = <filename>[,<filename>]*\n",
    	    indent_table[indent_ndx]);
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
    printf("%s<host_list> = <hostname>[,<hostname>]*\n",
    	    indent_table[indent_ndx]);
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
help_host_file_rpd(int indent_ndx)
{
    printf("%s<host_file_rpd> = "
    	    "\t[<host_list_spec>]"
	    "\n\t\t\t\t[<file_list_spec>]"
    	    "\n\t\t\t\t[<pid_list_spec>]"
	    "\n\t\t\t\t[<thread_list_spec>]"
	    "\n\t\t\t\t[<rank_list_spec>]\n",
	    indent_table[indent_ndx]);
    help_list_spec("-h","host",     "int",true,indent_ndx+1);
    help_list_spec("-f","file",     "int",true,indent_ndx+1);
    help_list_spec("-p","pid",	    "int",true,indent_ndx+1);
    help_list_spec("-t","thread",   "int",true,indent_ndx+1);
    help_list_spec("-r","rank",     "int",true,indent_ndx+1);
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
    printf("%s<address_description> = <64bit value> || <string>\n",
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

/**
 * Function: cmd_init
 * 
 * 
 *     
 * @param   void
 *
 * @return  void.
 *
 */
void
cmd_init()
{
    command.lineno_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.address_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.pid_table.entry_size    	= sizeof(arg_desc_t);
    command.break_table.entry_size  	= sizeof(arg_desc_t);
    command.rank_table.entry_size   	= sizeof(arg_desc_t);
    command.thread_table.entry_size 	= sizeof(arg_desc_t);
    command.file_table.entry_size   	= sizeof(arg_desc_t);
    command.experiment_table.entry_size	= sizeof(arg_desc_t);	/*  */
    command.name_table.entry_size	= sizeof(name_tab_t);	/*  */
    command.host_table.entry_size   	= sizeof(host_id_t);	/*  */
    command.help_table.entry_size   	= sizeof(help_desc_t);
    command.param_table.entry_size   	= sizeof(arg_desc_t);
    command.view_table.entry_size   	= sizeof(arg_desc_t);

}
