/** @file
 *
 * Parser result description.
 *
 */

#ifndef __OpenSpeedShop_Parse_Result_HXX__
#define __OpenSpeedShop_Parse_Result_HXX__

namespace OpenSpeedShop { namespace cli {

typedef enum {
    VAL_STRING,
    VAL_NUMBER
} val_enum_t;

typedef struct {
    // union is not possible with type string
    string name;
    int num;
    val_enum_t tag; /** Determines with field to be used */
} parse_val_t;

typedef struct {
    parse_val_t start_range;
    parse_val_t end_range;
} parse_range_t;

/** Simple static info about a command */
typedef struct {
    char *name;     	/** formal name of the command */
    bool ret_list;  	/** does this return a list or single instance */
    oss_cmd_enum ndx;	/** for sanity checking */
} command_type_t;

class Target;

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
	ParseResult()
	{
	    dm_experiment_set = false;
	}

//	/** Destructor. */
//	~ParseResult();

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

    	/** Handle list of break ids. */
    	vector<int> * getBreakList()
	{
	    return &dm_break_id_list;
	}

    	void pushBreakId(int break_id) {
    	    dm_break_id_list.push_back(break_id);
	}

    	/** Debugging code */
    	void dumpInfo();

    protected:
    	/** What command are we representing */
    	oss_cmd_enum dm_command_type;

    	/** What command are we representing */
    	int dm_experiment_id;
    	bool dm_experiment_set;

    	/** Container of break Ids as integers */
    	vector<int> dm_break_id_list;
    	/** Container of experiment types as strings */
    	vector<string> dm_exp_type_list;
    	/** Container of view types as strings */
    	vector<string> dm_view_type_list;
    	/** Container of general modifiers as strings */
    	vector<string> dm_modifier_list;
//    	/** Container of parameter info as class param_tuple */
//    	vector<param_tuple> dm_parameter_list;
    	/** Container of host/file/rpt as class HostFileRPT */
    	vector<Target> dm_target_list;



//    protected:
//    	/** Things I don't want to happen so don't define!*/
//	Base& operator=(const Base& rhs);
	
};

} }

#endif // __OpenSpeedShop_Parse_Result_HXX__
