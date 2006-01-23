/* //////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////// */

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

using namespace std;

#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

#include "SS_Parse_Param.hxx"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
using namespace OpenSpeedShop;

extern SS_Message_Czar& theMessageCzar();

command_type_t OpenSpeedShop::cli::cmd_desc[CMD_MAX] = {
    "Syntax_Error", false,  CMD_HEAD_ERROR, /* used in error reporting */
    "expAttach",    false,  CMD_EXP_ATTACH,
    "expClose",     false,  CMD_EXP_CLOSE,
    "expCreate",    false,  CMD_EXP_CREATE,
    "expCompare",   true,   CMD_EXP_COMPARE,
    "expDetach",    false,  CMD_EXP_DETACH,
    "expDisable",   false,  CMD_EXP_DISABLE,
    "expEnable",    false,  CMD_EXP_ENABLE,
    "expFocus",     false,  CMD_EXP_FOCUS,
    "expGo",	    false,  CMD_EXP_GO,
    "expPause",     false,  CMD_EXP_PAUSE,
    "expRestore",   false,  CMD_EXP_RESTORE,
    "expSave",	    false,  CMD_EXP_SAVE,
    "expSetparam",  false,  CMD_EXP_SETPARAM,
    "expStatus",    true,   CMD_EXP_STATUS,
    "expView",	    true,   CMD_EXP_VIEW,

    "list", 	    true,   CMD_LIST_GENERIC,

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
    "help", 	    true,   CMD_HELP,
    "history",	    true,   CMD_HISTORY,
    "log",  	    false,  CMD_LOG,
    "openGui",	    false,  CMD_OPEN_GUI,
    "playBack",     false,  CMD_PLAYBACK,
    "record",	    false,  CMD_RECORD,
    "setBreak",     false,  CMD_SETBREAK,
    "wait", 	    false,  CMD_WAIT
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
    dm_p_param(NULL),
    dm_redirect(),
    dm_append()
    
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
 * Method: ParseResult::pushParseTarget()
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
pushParseTarget()
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
	    cout << iter->getParmExpType() << " :: ";
	}
	
	cout << iter->getParmParamType() << " = ";
	
    	if (iter->isValString()) 
	    cout << iter->getStingVal() << endl;
	else
	    cout << iter->getnumVal() << endl;
    }
}

/**
 * Method: s_dumpError()
 * 
 * Dump error lists. Basically a variation on s_dumpRange
 * but talored for syntax error reporting.
 * 
 * The assumption here is that there is no real range and
 * that the arguments are strings and probably just one
 * at that.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
dumpError(CommandObject *cmd)
{
    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();
    
    // Local string for storing up complex message.
    string err_string;

    // Get list of error strings
    vector<ParseRange> *p_list = this->getErrorList();

    vector<ParseRange>::iterator iter;
    
    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	parse_range_t *p_range = iter->getRange();

    	parse_val_t *p_val1 = &p_range->start_range;
    	if (p_val1->tag == VAL_STRING) {
	    string s("Invalid argument: ");
	    
	    s.append(p_val1->name);

	    err_string.append(s);
	    err_string.append("\n");
    	}
    }

    // Print out help for command in question
    vector <SS_Message_Element *> element;
    czar.Find_By_Keyword(this->getCommandname(), &element);
    
    vector <SS_Message_Element*>:: iterator k;
    for (k = element.begin();
    	 k != element.end();
    	 ++k) {

    	SS_Message_Element *p_el = *k;

    	// Syntax list
    	vector<string> * const p_string = p_el->get_syntax_list();
    	for (vector <string> :: iterator i=p_string->begin();
    	     i!= p_string->end();
    	     ++i) {
    	    err_string.append(*i);
	    err_string.append("\n");
    	}
    }

    // This sets the message in whatever message 
    // delivery system
    Mark_Cmd_With_Soft_Error(cmd,err_string);

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
 * Method: s_nocase_compare()
 * 
 * case insensitive compare
 *     
 * @return  bool true if same.
 *
 * @todo    Error handling.
 *
 */
static bool
s_nocase_compare(char c1, char c2)
{
    return toupper(c1) == toupper(c2);
}

/**
 * Method: s_dump_help_topic()
 * 
 * Dump help topics based on topic string
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void
s_dump_help_topic(CommandObject *cmd, 
    	    	SS_Message_Czar& czar, 
		string *p_topic_str) 
{
    vector <SS_Message_Element *> sub_element;
    czar.Find_By_Topic(*p_topic_str, &sub_element);
    
    if (sub_element.begin() == sub_element.end()) {
    	;
    }
    else {
    	vector <SS_Message_Element*>:: iterator l;
    	for (l = sub_element.begin();
    	    l != sub_element.end();
    	    ++l) {
    	    	    SS_Message_Element *p_el = *l;
    	    	    string * const p_keyword = p_el->get_keyword();
    	    	    string * const p_brief = p_el->get_brief();
		    string newstr(*p_keyword + " -> " + *p_brief);
    	    	    cmd->Result_String (newstr);
    	}
    }
    cmd->Result_String(" ");
    cmd->Result_String("To get details for a particular topic,");
    cmd->Result_String("type \"help <topic>\".");
    cmd->Result_String(" ");
    cmd->Result_String("For instance, to get help list of the topic \"commands\"");
    cmd->Result_String("\topenss>>help commands");
    cmd->Result_String("To get help on an individual command type, such as \"expCreate\"");
    cmd->Result_String("\topenss>>help expCreate");
    cmd->Result_String(" ");

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

    if (!dm_help_set)
    	return;

    // List of help strings to look up.
    vector<string> *p_slist = this->getHelpList();

    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();

    // Standalone help message and list
    if (p_slist->begin() == p_slist->end()) {
    
    	// Beginning blurb
	cmd->Result_String("Open|SpeedShop is a performance tool brought");
	cmd->Result_String("to you by SGI.");
	cmd->Result_String(" ");
	cmd->Result_String("Here are the available help topics:");
	cmd->Result_String(" ");
	
    	string topic_str("topic");
    	s_dump_help_topic(cmd, czar, &topic_str);

    	return;
    }

    for (vector<string>::iterator j=p_slist->begin();
    	 j != p_slist->end() && !found_match; 
	 j++) {
	string name(*j);
    	vector <SS_Message_Element *> element;
	    
    	czar.Find_By_Keyword(name.c_str(), &element);
    
    	if (element.begin() == element.end()) {
	    cmd->Result_String( "No help for " + name);
	    continue;
    	}
	else {
	    found_match = true;
	}

    	vector <SS_Message_Element*>:: iterator k;
    	for (k = element.begin();
    	    k != element.end();
	    ++k) {
	    SS_Message_Element *p_el = *k;
	    
	    cmd->Result_String ("    *********");

    	    // Normal list
	    vector<string> * const p_string = p_el->get_normal_list();
    	    for (vector <string> :: iterator i=p_string->begin();
    	     	 i!= p_string->end();
	     	 ++i) {
		cmd->Result_String (*i);
	    }

    	    // Example list
	    vector<string> * const p_string_2 = p_el->get_example_list();
    	    if (p_string_2->begin() != p_string_2->end()) {
    	    	cmd->Result_String( "Examples:\n\n");
    	    }
    	    for (vector <string> :: iterator i=p_string_2->begin();
    	     	 i!= p_string_2->end();
	     	 ++i) {
		cmd->Result_String (*i);
	    }

    	    // Related list
	    vector<string> * const p_string_3 = p_el->get_related_list();
    	    if (p_string_3->begin() != p_string_3->end()) {
    	    	cmd->Result_String( "See also:\n\n");
    	    }
    	    for (vector <string> :: iterator i=p_string_3->begin();
    	     	 i!= p_string_3->end();
	     	 ++i) {
		cmd->Result_String ("\t" + *i);
	    }
    	    cmd->Result_String (" ");

	    // Check for topics (nodes of the tree
	    if (p_el->is_topic()) {
    	    	s_dump_help_topic(cmd, czar, p_el->get_keyword());
	    }
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
    cout << "\nCommand: " << this->getCommandname() << endl;

    // Experimant id.
    if (this->isExpId())
    	cout << "\tExperiment Id: " << this->getExpId() << endl;

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

    // help keywords.
    p_slist = this->getHelpList();
	
    if (p_slist->begin() != p_slist->end())
    	cout << "\tHelp Keywords: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	cout << endl ;

    // help modifiers.
    p_slist = this->getHelpModifierList();
	
    if (p_slist->begin() != p_slist->end())
    	cout << "\tHelp Modifiers: " ;
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

    // History list. Should be only one entry.
    s_dumpRange(this->getHistoryList(), "HISTORY",false /* is_hex */);

    // Metric list.
    s_dumpRange(this->getexpMetricList(), "METRICS",false /* is_hex */);

    // Param list.
    s_dumpParam(this->getParmList(), "PARAMS");

    // Syntax error.
    s_dumpRange(this->getErrorList(), "ERROR", false /* is_hex */);

    // target list.
    vector<ParseTarget>::iterator t_iter;
    vector<ParseTarget> *p_tlist = this->getTargetList();

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
    
    string *p_string = this->getRedirectTarget();
    if (p_string->length() > 0) {
    	
    	cout << "\tCopy Redirect target: " << *p_string << endl;
    }
    
    p_string = this->getAppendTarget();
    if (p_string->length() > 0) {
    	
    	cout << "\tAppend Redirect target: " << *p_string << endl;
    }
}

/**
 * Method: ParseResult::getCommandname()
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
getCommandname()
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
 * Method: ParseResult::isParam()
 * 
 *     
 * @return  true/false.
 *
 * @todo    Error handling.
 *
 */
bool
ParseResult::
isParam()
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
 * Method: ParseResult::pushHelp(void)
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
pushHelp()
{
    dm_help_set = true;
}

/**
 * Method: ParseResult::pushHelp(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    help facility.
 *
 */
void
ParseResult::
pushHelp(char *name)
{
    int len = strlen(name);
    char *tname = (char *)malloc(len +1);
    
    if (!tname) {
    	return; // ERROR!
    }
    
    // Lower case and strip off any surrounding 
    // grammar symbolic characters.
    int i = 0;
    int k = 0;
    if ((name[0] == '<') && (name[len-1] == '>')){
    	i++;
    	len--;
    }

    for (k=0;i<len;++i,++k) {
    	tname[k] = name[i];
    }
    tname[k] = '\0';
      
    dm_help_set = true;
    dm_help_list.push_back(tname);
    
    free(tname);
    
}

/**
 * Method: ParseResult::pushHelp(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    help facility modifiers.
 *
 */
void
ParseResult::
pushHelpModifier(char *name)
{
    
    dm_help_set = true;
    dm_help_modifier_list.push_back(name);   
}

/**
 * Method: ParseResult::setError(char * name1, char * name2)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
setError(char * name1, char * name2)
{
    ParseRange range(name1,name2);

    dm_error_set = true;
    dm_error_list.push_back(range);

    return ;
}
 
/**
 * Method: ParseResult::setError(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
setError(char * name)
{
    ParseRange range(name);

    dm_error_set = true;
    dm_error_list.push_back(range);

    return ;
}
 
/**
 * Method: ParseResult::pushExpMetric(char * name1, char * name2)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushExpMetric(char * name1, char * name2)
{
    ParseRange range(name1,name2);

    dm_exp_metric_list.push_back(range);

    return ;
}
 
/**
 * Method: ParseResult::pushExpMetric(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushExpMetric(char * name)
{
    ParseRange range(name);

   dm_exp_metric_list.push_back(range);

    return ;
}
 
