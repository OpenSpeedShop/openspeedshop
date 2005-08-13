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

/** @file
 *
 * Parser result description.
 *
 */

#ifndef __OpenSpeedShop_Parse_Result_HXX__
#define __OpenSpeedShop_Parse_Result_HXX__

#include "SS_Parse_Param.hxx"

typedef enum {
    CMD_HEAD_ERROR,	    /* used for error processing */
    CMD_EXP_ATTACH,
    CMD_EXP_CLOSE,
    CMD_EXP_CREATE,
    CMD_EXP_DETACH,
    CMD_EXP_DISABLE,
    CMD_EXP_ENABLE,
    CMD_EXP_FOCUS,
    CMD_EXP_GO,
    CMD_EXP_PAUSE,
    CMD_EXP_RESTORE,
    CMD_EXP_SAVE,
    CMD_EXP_SETPARAM,
    CMD_EXP_STATUS,
    CMD_EXP_VIEW,

    CMD_LIST_BREAKS,
    CMD_LIST_EXP,
    CMD_LIST_HOSTS,
    CMD_LIST_METRICS,
    CMD_LIST_OBJ,
    CMD_LIST_PARAMS,
    CMD_LIST_PIDS,
    CMD_LIST_RANKS,
    CMD_LIST_SRC,
    CMD_LIST_STATUS,
    CMD_LIST_THREADS,
    CMD_LIST_TYPES,
    CMD_LIST_VIEWS,

    CMD_CLEAR_BREAK,
    CMD_EXIT,
    CMD_HELP,
    CMD_HISTORY,
    CMD_LOG,
    CMD_OPEN_GUI,
    CMD_PLAYBACK,
    CMD_RECORD,
    CMD_SETBREAK,
    CMD_MAX
} oss_cmd_enum;

namespace OpenSpeedShop { namespace cli {


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

/** Simple static info about a command */
typedef struct {
    char *name;     	/** formal name of the command */
    bool ret_list;  	/** does this return a list or single instance */
    oss_cmd_enum ndx;	/** for sanity checking */
} command_type_t;

/** Decribe Parm information */
typedef struct {
    vector<ParseParam> parm_list;   /** List of parameters and values */
} parm_type_t;

////////////////////////////////////////////////////
#define MAX_INDENT 11

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
////////////////////////////////////////////////////

// External references.
extern command_type_t cmd_desc[];
extern char *experiment_name[];
extern char *viewtype_name[];
extern char *paramtype_name[];
extern char *general_name[];

/**
 * Parser result class.
 *
 *  Describes the results from parsing one OSS command.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseResult {

    public:

	/** Constructor. */
	ParseResult();

	/** Destructor. */
	~ParseResult();

    	/** ParseTarget object handling */
	void pushParseTarget();

    	/** Return pointer to current ParseTarget object */
	ParseTarget * currentTarget()
	{
	    return dm_p_cur_target;
	}

    	/** Return pointer to ParseTarget object list */
	vector<ParseTarget> * getTargetList()
	{
	    return &dm_target_list;
	}

    	/** Handle command type */
	void setCommandType(oss_cmd_enum type)
	{
	    dm_command_type = type;
	}
	oss_cmd_enum getCommandType()
	{
	    return dm_command_type;
	}
	char * getCommandname();
	bool isRetList();
 
    	/** Handle experiment id */
	void setExpId(int id)
	{
	    dm_experiment_id = id;
	    dm_experiment_set = true;
	}
	int getExpId()
	{
	    return dm_experiment_id;
	}
	bool isExpId()
	{
	    return dm_experiment_set;
	}

    	/** Handle list of experiment types. */
//    	const vector<string> * getExpList() const
    	vector<string> * getExpList()
	{
	    return &dm_exp_type_list;
	}

    	void pushExpType(char * name) {
    	    dm_exp_type_list.push_back(name);
	}

    	/** Handle list of view types. */
    	vector<string> * getViewList()
	{
	    return &dm_view_type_list;
	}

    	void pushViewType(char * name) {
    	    dm_view_type_list.push_back(name);
	}

    	/** Handle list of general modifiers. */
    	vector<string> * getModifierList()
	{
	    return &dm_modifier_list;
	}

    	void pushModifiers(char * name) {
    	    dm_modifier_list.push_back(name);
	}

    	void pushModifiers(help_gen_enum ndx) {
    	    dm_modifier_list.push_back(general_name[ndx]);
	}

    	/** Handle list of help requests. */
    	vector<string> * getHelpList()
	{
	    return &dm_help_list;
	}

    	void pushHelp(char * name);
    	void pushHelp();

    	/** Syntax error handling. */
    	void setError(char * name1, char * name2);
    	void setError(char * name);
	bool syntaxError( ) { return dm_error_set;}
    	vector<ParseRange> * getErrorList() {return &dm_error_list;}

    	/** Handle list of expMetrics. */
    	void pushExpMetric(char * name1, char * name2);
    	void pushExpMetric(char * name);
    	vector<ParseRange> * getexpMetricList() {return &dm_exp_metric_list;}

    	/** Handle list of break ids. */
    	vector<int> * getBreakList(){return &dm_break_id_list;}

    	void pushBreakId(int break_id) {dm_break_id_list.push_back(break_id);}

    	/** Handle list of file names. */
    	vector<ParseRange> * getAddressList(){return &dm_address_list;}

    	void pushAddressPoint(char * name) {
	    ParseRange range(name);
    	    dm_address_list.push_back(range);
	}
    	void pushAddressPoint(int num) {
	    ParseRange range(num);
    	    dm_address_list.push_back(range);
	}
    	void pushAddressRange(char * name, int num) {
	    ParseRange range(name,num);
    	    dm_address_list.push_back(range);
	}
    	void pushAddressRange(char * name1, char * name2) {
	    ParseRange range(name1,name2);
    	    dm_address_list.push_back(range);
	}
    	void pushAddressRange(int num, char * name) {
	    ParseRange range(num,name);
    	    dm_address_list.push_back(range);
	}
    	void pushAddressRange(int num1, int num2) {
	    ParseRange range(num1,num2);
    	    dm_address_list.push_back(range);
	}

    	/** Handle list of history numbers. */
	// semantically there is only one number,
	// but it is easy to use the ParseRange container.
    	vector<ParseRange> * getHistoryList()
	{
	    return &dm_history_list;
	}

    	void pushHistoryPoint(int num) {
	    ParseRange range(num);
    	    dm_history_list.push_back(range);
	}

    	/** Handle list of line numbers. */
    	vector<ParseRange> * getLineNoList()
	{
	    return &dm_lineno_list;
	}

    	void pushLineNoPoint(char * name) {
	    ParseRange range(name);
    	    dm_lineno_list.push_back(range);
	}
    	void pushLineNoPoint(int num) {
	    ParseRange range(num);
    	    dm_lineno_list.push_back(range);
	}
    	void pushLineNoRange(char * name, int num) {
	    ParseRange range(name,num);
    	    dm_lineno_list.push_back(range);
	}
    	void pushLineNoRange(char * name1, char * name2) {
	    ParseRange range(name1,name2);
    	    dm_lineno_list.push_back(range);
	}
    	void pushLineNoRange(int num, char * name) {
	    ParseRange range(num,name);
    	    dm_lineno_list.push_back(range);
	}
    	void pushLineNoRange(int num1, int num2) {
	    ParseRange range(num1,num2);
    	    dm_lineno_list.push_back(range);
	}

    	/** Handle Params. */
	bool isParam();
    	vector<ParseParam> *getParmList();
    	void pushParm(char *etype, char *ptype, int num);
    	void pushParm(char *etype, char *ptype, char *name);

    	/** Set/Get redirect target string. */
	bool setRedirectTarget(char *name) {
	    dm_redirect = name;
	}
	string * getRedirectTarget() {
	    return &dm_redirect;
	}

    	/** Syntax error reporting */
    	void dumpError(CommandObject *cmd);
	
    	/** Debugging code */
    	void dumpInfo();
	
	/**  Temporary help facility */
	void dumpHelp(CommandObject *);

    private:
    	/** What command are we representing */
    	oss_cmd_enum dm_command_type;

    	/** What command are we representing */
    	int dm_experiment_id;
    	bool dm_experiment_set;

    	/** Was help called? */
    	bool dm_help_set;

    	/** Are there any param values? */
    	bool dm_param_set;

    	/** Were there any parsing errors? */
    	bool dm_error_set;

	/** Param values */
	ParseParam *dm_p_param;

    	/** Container of break Ids as integers */
    	vector<int> dm_break_id_list;
    	/** Container of experiment types as strings */
    	vector<string> dm_exp_type_list;
    	/** Container of view types as strings */
    	vector<string> dm_view_type_list;
    	/** Container of general modifiers as strings */
    	vector<string> dm_modifier_list;
    	/** Container of help requests as strings */
    	vector<string> dm_help_list;
    	/** Container of addresses */
    	vector<ParseRange> dm_address_list;
    	/** Container of history ranges */
    	vector<ParseRange> dm_history_list;
    	/** Container of line numbers */
    	vector<ParseRange> dm_lineno_list;
    	/** Container of parameter info as class param_tuple */
    	vector<ParseParam> dm_param_list;
    	/** Container of syntax error symbols */
    	vector<ParseRange> dm_error_list;
    	/** Container of expMetric tuples */
    	vector<ParseRange> dm_exp_metric_list;

    	/** Container of host/file/rpt as class HostFileRPT */
    	vector<ParseTarget> dm_target_list;
	ParseTarget *dm_p_cur_target;
	
    	/** Redirect target string */
	string dm_redirect;

//    protected:
//    	/** Things I don't want to happen so don't define!*/
//	ParseResult& operator=(const ParseResult& rhs);
	
};

} }

#endif // __OpenSpeedShop_Parse_Result_HXX__
