/** @file
 *
 * Parser result description.
 *
 */

#include "SS_Parse_Param.hxx"

#ifndef __OpenSpeedShop_Parse_Result_HXX__
#define __OpenSpeedShop_Parse_Result_HXX__

typedef enum {
    CMD_HEAD_ERROR,	    /* used for error processing */
    CMD_EXP_ATTACH,
    CMD_EXP_CLOSE,
    CMD_EXP_CREATE,
    CMD_EXP_DETACH,
    CMD_EXP_DISABLE,
    CMD_EXP_ENABLE,
    CMD_EXP_FOCUS,
    CMD_EXP_PAUSE,
    CMD_EXP_RESTORE,
    CMD_EXP_GO,
    CMD_EXP_SAVE,
    CMD_EXP_SETPARAM,
    CMD_EXP_VIEW,
    CMD_LIST_EXP,
    CMD_LIST_HOSTS,
    CMD_LIST_OBJ,
    CMD_LIST_PIDS,
    CMD_LIST_SRC,
    CMD_LIST_METRICS,
    CMD_LIST_PARAMS,
    CMD_LIST_REPORTS,
    CMD_LIST_BREAKS,
    CMD_LIST_TYPES,
    CMD_CLEAR_BREAK,
    CMD_EXIT,
    CMD_OPEN_GUI,
    CMD_HELP,
    CMD_HISTORY,
    CMD_LOG,
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
	void PushParseTarget();

    	/** Return pointer to current ParseTarget object */
	ParseTarget * CurrentTarget()
	{
	    return dm_p_cur_target;
	}

    	/** Return pointer to ParseTarget object list */
	vector<ParseTarget> * GetTargetList()
	{
	    return &dm_target_list;
	}

    	/** Handle command type */
	void SetCommandType(oss_cmd_enum type)
	{
	    dm_command_type = type;
	}
	oss_cmd_enum GetCommandType()
	{
	    return dm_command_type;
	}
	char * GetCommandname();
	bool isRetList();
 
    	/** Handle experiment id */
	void SetExpId(int id)
	{
	    dm_experiment_id = id;
	    dm_experiment_set = true;
	}
	int GetExpId()
	{
	    return dm_experiment_id;
	}
	bool IsExpId()
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

    	void push_modifiers(char * name) {
    	    dm_modifier_list.push_back(name);
	}

    	void push_modifiers(help_gen_enum ndx) {
    	    dm_modifier_list.push_back(general_name[ndx]);
	}

    	/** Handle list of help requests. */
    	vector<string> * getHelpList()
	{
	    return &dm_help_list;
	}

    	void push_help(char * name) {
    	    dm_help_list.push_back(name);
	}

    	/** Syntax error handling. */
    	void set_error(char * name1, char * name2) {
    	    ParseRange range(name1,name2);
    	    dm_error_list.push_back(range);
	}
    	vector<ParseRange> * getErrorList() {return &dm_error_list;}

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
	bool IsParam();
    	vector<ParseParam> *getParmList();
    	void pushParm(char *etype, char *ptype, int num);
    	void pushParm(char *etype, char *ptype, char *name);

    	/** Debugging code */
    	void dumpInfo();

    private:
    	/** What command are we representing */
    	oss_cmd_enum dm_command_type;

    	/** What command are we representing */
    	int dm_experiment_id;
    	bool dm_experiment_set;
    	bool dm_param_set;

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
    	/** Container of addresses */
    	vector<ParseRange> dm_lineno_list;
    	/** Container of parameter info as class param_tuple */
    	vector<ParseParam> dm_param_list;
    	/** Container of syntax error symbols */
    	vector<ParseRange> dm_error_list;

    	/** Container of host/file/rpt as class HostFileRPT */
    	vector<ParseTarget> dm_target_list;
	ParseTarget *dm_p_cur_target;
	


//    protected:
//    	/** Things I don't want to happen so don't define!*/
//	ParseResult& operator=(const ParseResult& rhs);
	
};

} }

#endif // __OpenSpeedShop_Parse_Result_HXX__
