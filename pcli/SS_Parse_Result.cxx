/** @file
 *
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */

#include <vector>
#include <iostream>

using namespace std;

#include "support.h"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

command_type_t cmd_desc[CMD_MAX] = {
    "",false,CMD_HEAD_ERROR, /* used in error reporting */
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
    "listExp",	    true,   CMD_LIST_EXP,
    "listHosts",    true,   CMD_LIST_HOSTS,
    "listObj",	    true,   CMD_LIST_OBJ,
    "listPids",     true,   CMD_LIST_PIDS,
    "listSrc",	    true,   CMD_LIST_SRC,
    "listMetrics",  true,   CMD_LIST_METRICS,
    "listParams",   true,   CMD_LIST_PARAMS,
    "listReports",  true,   CMD_LIST_REPORTS,
    "listBreaks",   true,   CMD_LIST_BREAKS,
    "listTypes",    true,   CMD_LIST_TYPES,
    "clearBreak",   false,  CMD_CLEAR_BREAK,
    "exit", 	    false,  CMD_EXIT,
    "openGui",	    false,  CMD_OPEN_GUI,
    "help", 	    false,  CMD_HELP,
    "history",	    false,  CMD_HISTORY,
    "log",  	    false,  CMD_LOG,
    "playback",     false,  CMD_PLAYBACK,
    "record",	    false,  CMD_RECORD,
    "setBreak",     false,  CMD_SETBREAK
};

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
s_dumpRange(vector<ParseRange> *p_range_list, char *label)
{
    vector<ParseRange>::iterator r_iter;
    if (p_range_list->begin() != p_range_list->end())
    	    cout << "\t" << label << ": " ;
    for (r_iter=p_range_list->begin();r_iter != p_range_list->end(); r_iter++) {
    	parse_range_t *p_range = r_iter->getRange();
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
    if (p_range_list->begin() != p_range_list->end())
    	    cout << endl ;

}

/**
 * Method: ParseResult::dumpInfo()
 * 
 * Dump the data for this command
 * 
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

    // target list.
    vector<ParseTarget>::iterator t_iter;
    vector<ParseTarget> *p_tlist = this->GetTargetList();

    int count = 0;
    for (t_iter=p_tlist->begin() ;t_iter != p_tlist->end(); t_iter++) {
    	cout << "\tTarget #" << count++ << " : " << endl;
	
	// rank list
	s_dumpRange(t_iter->getHostList(), "HOST");
	s_dumpRange(t_iter->getFileList(), "FILE");
	s_dumpRange(t_iter->getRankList(), "RANK");
	s_dumpRange(t_iter->getPidList(),  "PID");
	s_dumpRange(t_iter->getThreadList(), "THREAD");
	s_dumpRange(t_iter->getClusterList(), "CLUSTER");
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
 
