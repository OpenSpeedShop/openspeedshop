/** @file
 *
 * Parser result description.
 *
 */

#include "SS_Parse_Param.hxx"

#ifndef __OpenSpeedShop_Parse_Result_HXX__
#define __OpenSpeedShop_Parse_Result_HXX__

namespace OpenSpeedShop { namespace cli {

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
