/** @file
 *
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */

#include <vector>
#include <string>
#include <iostream>
#include <cctype>

#include "SS_Input_Manager.hxx"
#include "SS_Cmd_Execution.hxx"

using namespace std;

#include "SS_Parse_Param.hxx"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

extern void dump_help_cmd(oss_cmd_enum, int, bool, CommandObject *);
extern void dump_help_brief(CommandObject *);

command_type_t OpenSpeedShop::cli::cmd_desc[CMD_MAX] = {
    "Syntax_Error", false,  CMD_HEAD_ERROR, /* used in error reporting */
    "expAttach",    false,  CMD_EXP_ATTACH,
    "expClose",     false,  CMD_EXP_CLOSE,
    "expCreate",    false,  CMD_EXP_CREATE,
    "expDetach",    false,  CMD_EXP_DETACH,
    "expDisable",   false,  CMD_EXP_DISABLE,
    "expEnable",    false,  CMD_EXP_ENABLE,
    "expFocus",     false,  CMD_EXP_FOCUS,
    "expPause",     false,  CMD_EXP_PAUSE,
    "expRestore",   false,  CMD_EXP_RESTORE,
    "expGo",	    false,  CMD_EXP_GO,
    "expSave",	    false,  CMD_EXP_SAVE,
    "expSetparm",   false,  CMD_EXP_SETPARAM,
    "expView",	    false,  CMD_EXP_VIEW,

    "listBreaks",   true,   CMD_LIST_BREAKS,
    "listExp",	    true,   CMD_LIST_EXP,
    "listHosts",    true,   CMD_LIST_HOSTS,
    "listMetrics",  true,   CMD_LIST_METRICS,
    "listObj",	    true,   CMD_LIST_OBJ,
    "listParams",   true,   CMD_LIST_PARAMS,
    "listPids",     true,   CMD_LIST_PIDS,
    "listRanks",    true,   CMD_LIST_RANKS,
    "listSrc",	    true,   CMD_LIST_SRC,
    "listStatus",   true,   CMD_LIST_STATUS,
    "listThreads",  true,   CMD_LIST_THREADS,
    "listTypes",    true,   CMD_LIST_TYPES,
    "listViews",    true,   CMD_LIST_VIEWS,

    "clearBreak",   false,  CMD_CLEAR_BREAK,
    "exit", 	    false,  CMD_EXIT,
    "help", 	    false,  CMD_HELP,
    "history",	    false,  CMD_HISTORY,
    "log",  	    false,  CMD_LOG,
    "openGui",	    false,  CMD_OPEN_GUI,
    "playback",     false,  CMD_PLAYBACK,
    "record",	    false,  CMD_RECORD,
    "setBreak",     false,  CMD_SETBREAK
};
 
/* This will eventually give way to an experiment registry. */
char *OpenSpeedShop::cli::experiment_name[H_EXP_MAX] = {
    "pcsamp",
    "usertime",
    "mpi",
    "fpe",
    "hwc",
    "io"
};

/* This will eventually give way to a viewtype registry. */
char *OpenSpeedShop::cli::viewtype_name[H_VIEW_MAX] = {
    "vTopN",
    "vExclTime",
    "vIO",
    "VFpe",
    "vHwc"
};

/* This will eventually give way to a paramtype registry. */
char *OpenSpeedShop::cli::paramtype_name[H_PARAM_MAX] = {
    "displayMode",
    "displayMedia",
    "displayRefreshRate",
    "expSaveFile",
    "samplingRate"
};

/* General hodgepodge of names. */
char *OpenSpeedShop::cli::general_name[H_GEN_MAX] = {
    "focus",
    "all",
    "copy",
    "kill",
    "gui",
    "mpi"
};

/**
 * Constructor: ParseResult::ParseResult()
 * 
 *     
 * @param   void
 *
 * @todo    Error handling.
 *
 */
ParseResult::
ParseResult() :
    dm_command_type(CMD_HEAD_ERROR),
    dm_experiment_id(-1),
    dm_experiment_set(false),
    dm_help_set(false),
    dm_param_set(false),
    dm_error_set(false),
    dm_p_param(NULL)
    
{
    // Create first ParseTarget object.
    this->dm_p_cur_target = new ParseTarget();

}
 
/**
 * Destructor: ParseResult::~ParseResult()
 * 
 *     
 * @param   void
 *
 * @todo    Error handling.
 *
 */
ParseResult::
~ParseResult()
{
    if (this->dm_p_cur_target)
    	delete this->dm_p_cur_target;
    if (this->dm_p_param)
    	delete this->dm_p_param;
}
 
/**
 * Method: ParseResult::PushParseTarget()
 * 
 *     
 * @param   void
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
PushParseTarget()
{
    if (this->dm_p_cur_target->used()) {
    	this->dm_target_list.push_back(*this->dm_p_cur_target);
    	delete this->dm_p_cur_target;
    	this->dm_p_cur_target = new ParseTarget();
    }
    return ;
}
 
/**
 * Method: s_dumpParam()
 * 
 * Dump param information
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void 
s_dumpParam(vector<ParseParam> *p_list, char *label)
{
    vector<ParseParam>::iterator iter;

    if (p_list->begin() != p_list->end())
    	cout << "\t\t" << label << ": " << endl;

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	    cout << "\t\t\t";
    
    	if (iter->getParmExpType()) {
	    cout << iter->getParmExpType() << "::";
	}
	
	cout << iter->getParmParamType() << " = ";
	
    	if (iter->isValString()) 
	    cout << iter->getStingVal() << endl;
	else
	    cout << iter->getnumVal() << endl;
    }
}

/**
 * Method: s_dumpRange()
 * 
 * Dump range and point lists
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void 
s_dumpRange(vector<ParseRange> *p_list, char *label, bool is_hex)
{
    vector<ParseRange>::iterator iter;
    
    if (is_hex){
    	cout.setf(ios_base::hex,ios_base::basefield);
    	cout.setf(ios_base::showbase);
    }
    else {
    	cout.setf(ios_base::dec,ios_base::basefield);
    	cout.unsetf(ios_base::showbase);
    }
    
    if (p_list->begin() != p_list->end())
    	    cout << "\t\t" << label << ": " ;

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	parse_range_t *p_range = iter->getRange();
    	if (p_range->is_range) {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    parse_val_t *p_val2 = &p_range->end_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	cout << p_val1->name << ":";
    	    }
    	    else {
    	    	cout << p_val1->num << ":";
    	    }
    	    if (p_val2->tag == VAL_STRING) {
    	    	cout << p_val2->name << " ";
    	    }
    	    else {
    	    	cout << p_val2->num << " ";
    	    }
    	}
    	else {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	cout << p_val1->name << " ";
    	    }
    	    else {
    	    	cout << p_val1->num << " ";
    	    }
    	}
    }
    if (p_list->begin() != p_list->end())
    	    cout << endl ;

}

/**
 * Method: nocase_compare()
 * 
 * case insensitive compare
 *     
 * @return  bool true if same.
 *
 * @todo    Error handling.
 *
 */
static bool
nocase_compare(char c1, char c2)
{
    return toupper(c1) == toupper(c2);
}

/**
 * Method: ParseResult::dumpHelp()
 * 
 * Dump help requests
 * This is not meant to be used by dumpInfo().
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
dumpHelp(CommandObject *cmd) 
{

    bool found_match = false;
    char input[512]; // until I can figure out how to lowercase in C++
//    extern int tolower();

    if (!dm_help_set)
    	return;

    // general modifier types.
    vector<string> *p_slist = this->getHelpList();

    // Dump out command oneliners
    if (p_slist->begin() == p_slist->end()) {
    	dump_help_brief(cmd);

    	return;
    }

    for (vector<string>::iterator j=p_slist->begin();
    	 j != p_slist->end() && !found_match; 
	 j++) {
	string name(*j);
	
	// lower case the input string
//	transform(*j->begin(),
//	    	  *j->end(),
//		  *j->begin(),
//		  tolower);
	
	// Look for commands
	for (int i=0;i<CMD_MAX && !found_match;++i) {
	    string table(cmd_desc[(oss_cmd_enum)i].name);

   	    if ((strcasecmp(name.c_str(),table.c_str())) == 0) {
	    	dump_help_cmd((oss_cmd_enum)i,0,false /* is_brief */,cmd);
		found_match = true;
		break;
	    }
	}
	// Look for param types
	for (int i=0;i<H_PARAM_MAX && !found_match;++i) {
	}
	// Look for experiment types
	for (int i=0;i<H_EXP_MAX && !found_match;++i) {
	}
	// Look for view types
	for (int i=0;i<H_VIEW_MAX && !found_match;++i) {
	}
	// Look for general modifiers
	for (int i=0;i<H_GEN_MAX && !found_match;++i) {
	}
    }
}

/**
 * Method: ParseResult::dumpInfo()
 * 
 * Dump the data for this command
 * This is for debugging only and not meant
 * for regular use in the tool.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
dumpInfo() 
{

    // Command name.
    cout << "\nCommand: " << this->GetCommandname() << endl;

    // Experimant id.
    if (this->IsExpId())
    	cout << "\tExperiment Id: " << this->GetExpId() << endl;

    // Experiment types.
    vector<string> *p_slist = this->getExpList();
    vector<string>::iterator j;
	
    if (p_slist->begin() != p_slist->end())
    	cout << "\tExperiment Types: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	cout << endl ;

    // view types.
    p_slist = this->getViewList();
	
    if (p_slist->begin() != p_slist->end())
    	cout << "\tView Types: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	cout << endl ;

    // general modifier types.
    p_slist = this->getModifierList();
	
    if (p_slist->begin() != p_slist->end())
    	cout << "\tGeneral Modifier Types: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	cout << endl ;

    // Break id list.
    vector<int>::iterator i;
    vector<int> *p_ilist = this->getBreakList();
	
    if (p_ilist->begin() != p_ilist->end())
    	cout << "\tBreak ids: " ;
    for (i=p_ilist->begin();i != p_ilist->end(); i++) {
    	cout << *i << " " ;
    }
    if (p_ilist->begin() != p_ilist->end())
    	cout << endl ;

    // Address list.
    s_dumpRange(this->getAddressList(), "ADDRESSES",true /* is_hex */);

    // Param list.
    s_dumpParam(this->getParmList(), "PARAMS");

    // Syntax error.
    s_dumpRange(this->getErrorList(), "ERROR", false /* is_hex */);

    // target list.
    vector<ParseTarget>::iterator t_iter;
    vector<ParseTarget> *p_tlist = this->GetTargetList();

    int count = 1;
    for (t_iter=p_tlist->begin() ;t_iter != p_tlist->end(); t_iter++) {
    	cout << "\tTarget #" << count++ << " : " << endl;
	
	// various lists
	s_dumpRange(t_iter->getHostList(), "HOST",true /* is_hex */);
	s_dumpRange(t_iter->getFileList(), "FILE",false /* is_hex */);
	s_dumpRange(t_iter->getRankList(), "RANK",false /* is_hex */);
	s_dumpRange(t_iter->getPidList(),  "PID",false /* is_hex */);
	s_dumpRange(t_iter->getThreadList(), "THREAD",false /* is_hex */);
	s_dumpRange(t_iter->getClusterList(), "CLUSTER",false /* is_hex */);
    }
}

/**
 * Method: ParseResult::GetCommandname()
 * 
 * Retrieve the formal command name.
 * 
 *     
 * @return  a char * to the name of this command.
 *
 * @todo    Error handling.
 *
 */
char * 
ParseResult::
GetCommandname()
{
    return cmd_desc[dm_command_type].name;
}
 
/**
 * Method: ParseResult::isRetList()
 * 
 * This should change to return a command return
 * type. Until then we will just return a boolean
 * indicating that the return type is in the form
 * of a list or not.
 *     
 * @return  true/false.
 *
 * @todo    Error handling.
 *
 */
bool
ParseResult::
isRetList()
{
    return cmd_desc[dm_command_type].ret_list;
}
 
/**
 * Method: ParseResult::IsParam()
 * 
 *     
 * @return  true/false.
 *
 * @todo    Error handling.
 *
 */
bool
ParseResult::
IsParam()
{
    return this->dm_param_set;
}
 
/**
 * Method: ParseResult::getParmList()
 * 
 *     
 * @return  vector<ParseParam> *
 *
 * @todo    Error handling.
 *
 */
vector<ParseParam> *
ParseResult::
getParmList()
{
    return &this->dm_param_list;
}
 
/**
 * Method: ParseResult::pushParm(char *etype, char * ptype, int num)
 * 
 *     
 * @return  true/false.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushParm(char *etype, char * ptype, int num)
{
    ParseParam param(etype,ptype,num);
    
    dm_param_list.push_back(param);

    return ;
}
 
/**
 * Method: ParseResult::pushParm(char *etype, char * ptype, char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushParm(char *etype, char * ptype, char * name)
{
    ParseParam param(etype,ptype,name);

    dm_param_list.push_back(param);

    return ;
}
 
/**
 * Method: ParseResult::push_help(void)
 * 
 * Used for generic help topic dump
 *     
 * @return  void.
 *
 * @todo    help facility.
 *
 */
void
ParseResult::
push_help()
{
    dm_help_set = true;
}

/**
 * Method: ParseResult::push_help(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    help facility.
 *
 */
void
ParseResult::
push_help(char *name)
{
    dm_help_set = true;
    dm_help_list.push_back(name);
    
    // This is temporary during development.
//    int i;
//    for (i=1;i<CMD_MAX;++i) {
//    	if ((strcmp(name,cmd_desc[(oss_cmd_enum)i].name)) == 0) {
//	    dump_help_cmd((oss_cmd_enum)i,0,false /* is_brief */);
//	    break;
//	}
//    }
}

/**
 * Method: ParseResult::set_error(char * name1, char * name2)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
set_error(char * name1, char * name2)
{
    ParseRange range(name1,name2);

    dm_error_set = true;
    dm_error_list.push_back(range);

    return ;
}
 
/**
 * Method: ParseResult::set_error(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
set_error(char * name)
{
    ParseRange range(name);

    dm_error_set = true;
    dm_error_list.push_back(range);

    return ;
}
 
