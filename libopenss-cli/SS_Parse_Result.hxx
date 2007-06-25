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
 * Parser result description and definition of the ParseResult class.
 *
 */

#ifndef __OpenSpeedShop_Parse_Result_HXX__
#define __OpenSpeedShop_Parse_Result_HXX__

#include "SS_Parse_Param.hxx"

//////////////////////////////////////////////////////////////////
//
//  If you are going to edit this array you will
//  probably need to change a number of other files.
//
//  See the comments for the OpenSpeedShop::cli::cmd_desc
//  array in SS_Parse_Result.cxx
//
//////////////////////////////////////////////////////////////////
typedef enum {
    CMD_HEAD_ERROR,	    /* used for error processing */
    CMD_EXP_ATTACH,
    CMD_EXP_CLONE,
    CMD_EXP_CLOSE,
    CMD_EXP_COMPARE,
    CMD_EXP_CONT,
    CMD_EXP_CREATE,
    CMD_EXP_DATA,
    CMD_EXP_DETACH,
    CMD_EXP_DISABLE,
    CMD_EXP_ENABLE,
    CMD_EXP_FOCUS,
    CMD_EXP_GO,
    CMD_EXP_PAUSE,
    CMD_EXP_RESTORE,
    CMD_EXP_SAVE,
    CMD_EXP_SETARGS,
    CMD_EXP_SETPARAM,
    CMD_EXP_STATUS,
    CMD_EXP_VIEW,

    CMD_VIEW,

    CMD_C_VIEW_CLUSTERS,
    CMD_C_VIEW_CREATE,
    CMD_C_VIEW_DELETE,
    CMD_C_VIEW_INFO,
    CMD_C_VIEW,

    CMD_LIST_GENERIC,	// Replaces all the other list commands.

    CMD_CLEAR_BREAK,
    CMD_ECHO,
    CMD_EXIT,
    CMD_HELP,
    CMD_HISTORY,
    CMD_INFO,
    CMD_LOG,
    CMD_OPEN_GUI,
    CMD_PLAYBACK,
    CMD_RECORD,
    CMD_SETBREAK,
    CMD_WAIT,
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
////////////////////////////////////////////////////

// External references.
extern command_type_t cmd_desc[];

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

	/** Copy constructor. */
	ParseResult(const ParseResult& other);

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
	/** Most commands only allow one id */
	int getExpId();
	int expIdCount();
	bool isExpId();

    	vector<ParseRange> * getExpIdList(){return &dm_experiment_id_list;}

    	void pushExpIdPoint(int num) {
	    ParseRange range(num);
    	    dm_experiment_id_list.push_back(range);
	}
    	void pushExpIdRange(int num1, int num2) {
	    ParseRange range(num1,num2);
    	    dm_experiment_id_list.push_back(range);
	}
	
    	/** Handle list of experiment types. */
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

    	/** Handle list of view set id range values. */
    	vector<ParseRange> * getViewSet()
	{
	    return &dm_view_set_list;
	}
    	void pushViewSet(int num) {
	    ParseRange range(num);
    	    dm_view_set_list.push_back(range);
	}
    	void pushViewSet(int num1, int num2) {
	    ParseRange view_set(num1,num2);
    	    dm_view_set_list.push_back(view_set); }
    	/** Handle list of general modifiers. */
    	vector<string> * getModifierList()
	{
	    return &dm_modifier_list;
	}

    	void pushModifiers(char * name) {
    	    dm_modifier_list.push_back(name);
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

    	/** Handle list of view intervals. */
	void setIntervalAttribute(char *attribute);
	bool isIntervalAttribute(){return dm_interval_attribute_set;}
	const string * getIntervalAttribute();
    	void pushInterval(int64_t begin, int64_t end);
    	void pushInterval(int64_t begin, double end);
    	void pushInterval(double begin, int64_t end);
    	void pushInterval(double begin, double end);
    	vector<ParseInterval> * getParseIntervalList() {return &dm_interval_list;}

    	/** Handle list of expMetrics. */
    	void pushExpMetric(char * name1, char * name2);
    	void pushExpMetric(char * name);
    	vector<ParseRange> * getexpMetricList() {return &dm_exp_metric_list;}

    	/** Handle list of break ids. */
    	vector<int> * getBreakList(){return &dm_break_id_list;}

    	void pushBreakId(int break_id) {dm_break_id_list.push_back(break_id);}
	
	/** Handle list of generic strings */
    	vector<string> * getStringList(){return &dm_generic_string_list;}
    	void pushString(string name) {dm_generic_string_list.push_back(name);}
	

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
    	ParseParam *getParam();
    	void setParam(char *etype, char *ptype);
   	void pushParamVal(char   *sval);
    	void pushParamVal(int64_t ival);
    	void pushParamVal(double  dval);

    	/** Handle list of help modifiers. */
    	vector<string> * getHelpModifierList()
	{
	    return &dm_help_modifier_list;
	}

    	void pushHelpModifier(char * name);

    	/** Set/Get copy redirect target string. */
	bool setRedirectTarget(char *name) {
	    dm_redirect = name;
	}
	string * getRedirectTarget() {
	    return &dm_redirect;
	}

    	/** Set/Get append redirect target string. */
	bool setAppendTarget(char *name) {
	    dm_append = name;
	}
	string * getAppendTarget() {
	    return &dm_append;
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

    	/** Container of break Ids as integers */
    	vector<ParseRange> dm_experiment_id_list;

    	/** Was help called? */
    	bool dm_help_set;

    	/** Are there any param values? */
    	bool dm_param_set;

    	/** Were there any parsing errors? */
    	bool dm_error_set;

	/** Param that is being filled */
	ParseParam dm_param;

    	/** Container of break Ids as integers */
    	vector<int> dm_break_id_list;
    	/** Container of experiment types as strings */
    	vector<string> dm_exp_type_list;
    	/** Container of view types as strings */
    	vector<string> dm_view_type_list;
    	/** Container of view set Ids as integers */
    	vector<ParseRange> dm_view_set_list;
    	/** Container of general modifiers as strings */
    	vector<string> dm_modifier_list;
    	/** Container of help requests as strings */
    	vector<string> dm_help_list;
    	/** Container of help modifiers */
    	vector<string> dm_help_modifier_list;
    	/** Container of addresses */
    	vector<ParseRange> dm_address_list;
    	/** Container of history ranges */
    	vector<ParseRange> dm_history_list;
    	/** Container of line numbers */
    	vector<ParseRange> dm_lineno_list;
    	/** Container of strings */
    	vector<string> dm_generic_string_list;
    	/** Container of syntax error symbols */
    	vector<ParseRange> dm_error_list;
    	/** Container of view interval tuples */
    	vector<ParseInterval> dm_interval_list;
    	/** Container of expMetric tuples */
    	vector<ParseRange> dm_exp_metric_list;

    	/** Container of host/file/rpt as class HostFileRPT */
    	vector<ParseTarget> dm_target_list;
	ParseTarget *dm_p_cur_target;
	
    	/** Redirect target strings */
	string dm_redirect;
	string dm_append;

    	/** Is there an interval attribute? */
	bool dm_interval_attribute_set;
    	string dm_interval_attribute;

//    protected:
//    	/** Things I don't want to happen so don't define!*/
//	ParseResult& operator=(const ParseResult& rhs);
	
};

} }

#endif // __OpenSpeedShop_Parse_Result_HXX__
