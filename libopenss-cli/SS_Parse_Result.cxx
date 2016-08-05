/* //////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2015 Krell Institute  All Rights Reserved.
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
//#define DEBUG_CLI 0

/** @file
 *
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */

// This must be first include due to warnings regarding redefinition
// of certain definitions (POSIX_*)
#include "SS_Input_Manager.hxx"

#include <vector>
#include <string>
#include <iostream>
#include <cctype>



#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

#include "SS_Parse_Param.hxx"
#include "SS_Parse_Interval.hxx"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"
#include "SS_Settings.hxx"

using namespace OpenSpeedShop::cli;
using namespace OpenSpeedShop;

extern SS_Message_Czar& theMessageCzar();

//////////////////////////////////////////////////////////////////
//
//  If you are going to edit this array you will
//  probably need to change a number of other files.
//
//  SS_Parse_result.hxx -> the original enum for commands.
//  ATTENTION! The enumerations and this array need to be
//  in the same order. We now have a sanity check
//  at startup time for this. The table is already setup for
//  the check because it has the enum value embedded in each
//  struct entry of the array.
//
//  SS_Parse_yacc.yxx -> The grammar for the commands.
//
//  SS_Parse_Lex.lxx -> The lexical analizer for the commands.
//  The name needs to be recognized as being special and tokenized.
//
//  SS_Preparse.py -> The preparser. You need to register the
//  command name and return type.
//
//  help_cmd_msg.dxx -> Database for HELP entries for the known
//  commands.
//
//////////////////////////////////////////////////////////////////
command_type_t OpenSpeedShop::cli::cmd_desc[CMD_MAX] = {
    "Syntax_Error", false,  CMD_HEAD_ERROR, /* used in error reporting */
    "expAttach",    false,  CMD_EXP_ATTACH,
    "expClone",     false,  CMD_EXP_CLONE,
    "expClose",     false,  CMD_EXP_CLOSE,
    "expCompare",   true,   CMD_EXP_COMPARE,
    "expCont",	    false,  CMD_EXP_CONT,
    "expCreate",    false,  CMD_EXP_CREATE,
    "expData",	    true,   CMD_EXP_DATA,
    "expDetach",    false,  CMD_EXP_DETACH,
    "expDisable",   false,  CMD_EXP_DISABLE,
    "expEnable",    false,  CMD_EXP_ENABLE,
    "expFocus",     false,  CMD_EXP_FOCUS,
    "expGo",	    false,  CMD_EXP_GO,
    "expPause",     false,  CMD_EXP_PAUSE,
    "expRestore",   false,  CMD_EXP_RESTORE,
    "expSave",	    false,  CMD_EXP_SAVE,
    "expSetargs",   false,  CMD_EXP_SETARGS,
    "expSetparam",  false,  CMD_EXP_SETPARAM,
    "expStatus",    true,   CMD_EXP_STATUS,
    "expView",	    true,   CMD_EXP_VIEW,

    "View",	    true,   CMD_VIEW,

    "cViewCluster", true,   CMD_C_VIEW_CLUSTERS,
    "cViewCreate",  false,  CMD_C_VIEW_CREATE,
    "cViewDelete",  false,  CMD_C_VIEW_DELETE,
    "cViewInfo",    true,   CMD_C_VIEW_INFO,
    "cView",	    true,   CMD_C_VIEW,

    "list", 	    true,   CMD_LIST_GENERIC,

    "clearBreak",   false,  CMD_CLEAR_BREAK,
    "echo", 	    false,  CMD_ECHO,
    "exit", 	    false,  CMD_EXIT,
    "help", 	    true,   CMD_HELP,
    "history",	    true,   CMD_HISTORY,
    "info",	    true,   CMD_INFO,
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
    dm_help_set(false),
    dm_param_set(false),
    dm_error_set(false),
    dm_redirect(),
    dm_append(),
    dm_interval_attribute_set(false),
    dm_interval_attribute()
    
{
    // Create first ParseTarget object.
    this->dm_p_cur_target = new ParseTarget();
}
 

/**
 * Copy constructor: ParseResult::ParseResult()
 *
 * Copy the contents of one ParseResult object into a new one.
 * Allocate a new ParseTarget object for dm_p_cur_target in
 * the new ParseResult object and clear any other poitner fields
 * in the new object.
 *
 * @param   the ParseResult to copy
 *
 * @todo    Error handling.
 *
 */
ParseResult::
ParseResult(const ParseResult& other) :
    dm_command_type(CMD_HEAD_ERROR),
    dm_help_set(false),
    dm_param_set(false),
    dm_error_set(false),
    dm_redirect(),
    dm_append(),
    dm_interval_attribute_set(false),
    dm_interval_attribute()
{
    *this = other;
    this->dm_p_cur_target = new ParseTarget();

#if DEBUG_CLI
    printf("ParseResult::ParseResult, this=%d\n", this);
#endif
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

    // Most commands only allow a single expId.
    // Check here.
#if DEBUG_CLI
    printf("ParseResult::pushParseTarget, this->dm_command_type=%d\n", this->dm_command_type);
#endif
    switch(this->dm_command_type) {
    	case CMD_EXP_COMPARE:
	    break;
	default:
	    if ((this->expIdCount()) > 1) {
	    	this->setError("Only one expId allowed");
	    }
    }
    
    return ;
}
 
/**
 * Method: s_dumpInterval()
 * 
 * Dump time interval information
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void 
s_dumpInterval(ParseResult* p_result, std::vector<ParseInterval> *p_list, const char *label)
{
    std::vector<ParseInterval>::iterator iter;

#if DEBUG_CLI
    printf("ParseResult::s_dumpInterval, label=%s\n", label);
#endif
    if (p_list->begin() != p_list->end())
    	std::cout << "\t" << label << ": " << std::endl;

    if (p_result->isIntervalAttribute()) {
    	std::cout << "\t\t" << "Attribute: " << *p_result->getIntervalAttribute() << std::endl;
    }

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	    std::cout << "\t\t\t";
    
    	if (iter->isStartInt()) 
	    std::cout << iter->getStartInt() << " : ";
	else
	    std::cout << iter->getStartdouble() << " : ";

    	if (iter->isEndInt()) 
	    std::cout << iter->getEndInt() << std::endl;
	else
	    std::cout << iter->getEndDouble() << std::endl;
    }
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
s_dumpParam(ParseParam *p_parm, const char *label)
{
    std::cout << "\t\t" << label << ": " << std::endl;

#if DEBUG_CLI
    printf("ParseResult::s_dumpParam, label=%s\n", label);
#endif
    std::cout << "\t\t";
    if (p_parm->getExpType()) {
    	std::cout << p_parm->getExpType() << "::";
    }
	
    std::cout << p_parm->getParamType() << " = " << std::endl;
	
    std::vector<ParamVal>::iterator iter;

    std::vector<ParamVal> * p_list = p_parm->getValList();
    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	std::cout << "\t\t\t";
    
    	switch(iter->getValType()) {
	    case PARAM_VAL_STRING:
	    	std::cout << iter->getSVal() << std::endl;
	    	break;
	    case PARAM_VAL_INT:
	    	std::cout << iter->getIVal() << std::endl;
	    	break;
	    case PARAM_VAL_DOUBLE:
	    	std::cout << iter->getDVal() << std::endl;
	    	break;
	    default :
	    	std::cout << "UNKNOWN VALUE TYPE: " << iter->getValType() << std::endl;
	    	break; }
    }

}

/**
 * Method: ParseResult::dumpError()
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
#if DEBUG_CLI
    printf("ParseResult::dumpError, cmd=%s\n", cmd);
#endif

    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();
    
    // Local string for storing up complex message.
    std::string err_string;

    // Get list of error strings
    std::vector<ParseRange> *p_list = this->getErrorList();

    std::vector<ParseRange>::iterator iter;
    
    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	parse_range_t *p_range = iter->getRange();

    	parse_val_t *p_val1 = &p_range->start_range;
    	if (p_val1->tag == VAL_STRING) {
	    std::string s("Invalid argument: ");
	    
	    s.append(p_val1->name);

	    err_string.append(s);
	    err_string.append("\n");
    	}
    }

    // Print out help for command in question
    std::vector <SS_Message_Element *> element;
    czar.Find_By_Keyword(this->getCommandname(), &element);
    
    std::vector <SS_Message_Element*>:: iterator k;
    for (k = element.begin();
    	 k != element.end();
    	 ++k) {

    	SS_Message_Element *p_el = *k;

    	// Syntax list
    	std::vector<std::string> * const p_string = p_el->get_syntax_list();
    	for (std::vector <std::string> :: iterator i=p_string->begin();
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
s_dumpRange(std::vector<ParseRange> *p_list, const char *label, bool is_hex)
{
    std::vector<ParseRange>::iterator iter;
#if DEBUG_CLI
    printf("ParseResult::s_dumpRange, label=%s\n", label);
#endif
    
    if (is_hex){
    	std::cout.setf(std::ios_base::hex,std::ios_base::basefield);
    	std::cout.setf(std::ios_base::showbase);
    }
    else {
    	std::cout.setf(std::ios_base::dec,std::ios_base::basefield);
    	std::cout.unsetf(std::ios_base::showbase);
    }
    
    if (p_list->begin() != p_list->end())
    	    std::cout << "\t\t" << label << ": " ;

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	parse_range_t *p_range = iter->getRange();
    	if (p_range->is_range) {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    parse_val_t *p_val2 = &p_range->end_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	std::cout << p_val1->name << ":";
    	    }
    	    else {
    	    	std::cout << p_val1->num << ":";
    	    }
    	    if (p_val2->tag == VAL_STRING) {
    	    	std::cout << p_val2->name << " ";
    	    }
    	    else {
    	    	std::cout << p_val2->num << " ";
    	    }
    	}
    	else {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	std::cout << p_val1->name << " ";
    	    }
    	    else {
    	    	std::cout << p_val1->num << " ";
    	    }
    	}
    }
    if (p_list->begin() != p_list->end())
    	    std::cout << std::endl ;

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
s_nocase_compare(const char c1, const char c2)
{
    return toupper(c1) == toupper(c2);
}

/**
 * Method: s_dump_help_topic()
 * 
 * Dump help topics based on topic std::std::string
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void
s_dump_help_topic(CommandObject *cmd, 
    	    	SS_Message_Czar& czar, 
		std::string *p_topic_str) 
{
    std::vector <SS_Message_Element *> sub_element;
    czar.Find_By_Topic(*p_topic_str, &sub_element);
    
    if (sub_element.begin() == sub_element.end()) {
    	;
    }
    else {
    	std::vector <SS_Message_Element*>:: iterator l;
    	for (l = sub_element.begin();
    	    l != sub_element.end();
    	    ++l) {
    	    	    SS_Message_Element *p_el = *l;
    	    	    std::string * const p_keyword = p_el->get_keyword();
		    std::string newstr(*p_keyword);
		    if ((newstr.length()) < 15)
		    	newstr.resize(15,' ');
    	    	    std::string * const p_brief = p_el->get_brief();
		    newstr.append(" -> " + *p_brief); 
		    //std::string newstr(*p_keyword + " -> " + *p_brief);
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
    std::vector<std::string> *p_slist = this->getHelpList();

    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();

    // Standalone help message and list
    if (p_slist->begin() == p_slist->end()) {
    
    	// Beginning blurb
	cmd->Result_String("Open|SpeedShop is a performance tool brought");
	cmd->Result_String("to you by The Krell Institute and sponsors.");
	cmd->Result_String(" ");
	cmd->Result_String("Here are the available help topics:");
	cmd->Result_String(" ");
	
    	std::string topic_str("topic");
    	s_dump_help_topic(cmd, czar, &topic_str);

    	return;
    }

    for (std::vector<std::string>::iterator j=p_slist->begin();
    	 j != p_slist->end() && !found_match; 
	 j++) {
	std::string name(*j);
    	std::vector <SS_Message_Element *> element;
	    
    	czar.Find_By_Keyword(name.c_str(), &element);
    
    	if (element.begin() == element.end()) {
	    cmd->Result_String( "No help for " + name);
	    continue;
    	}
	else {
	    found_match = true;
	}

    	std::vector <SS_Message_Element*>:: iterator k;
    	for (k = element.begin();
    	    k != element.end();
	    ++k) {
	    SS_Message_Element *p_el = *k;
	    
	    cmd->Result_String ("    *********");

    	    // Normal list
	    std::vector<std::string> * const p_string = p_el->get_normal_list();
    	    for (std::vector <std::string> :: iterator i=p_string->begin();
    	     	 i!= p_string->end();
	     	 ++i) {
		cmd->Result_String (*i);
	    }

    	    // Example list
	    std::vector<std::string> * const p_string_2 = p_el->get_example_list();
    	    if (p_string_2->begin() != p_string_2->end()) {
    	    	cmd->Result_String( "Examples:\n\n");
    	    }
    	    for (std::vector <std::string> :: iterator i=p_string_2->begin();
    	     	 i!= p_string_2->end();
	     	 ++i) {
		cmd->Result_String (*i);
	    }

    	    // Related list
	    std::vector<std::string> * const p_string_3 = p_el->get_related_list();
    	    if (p_string_3->begin() != p_string_3->end()) {
    	    	cmd->Result_String( "See also:\n\n");
    	    }
    	    for (std::vector <std::string> :: iterator i=p_string_3->begin();
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
    std::cout << "\nCommand: " << this->getCommandname() << std::endl;

    // Experimant id list.
    s_dumpRange(this->getExpIdList(), "Experiment Id",false /* is_hex */);


    // Experiment types.
    std::vector<std::string> *p_slist = this->getExpList();
    std::vector<std::string>::iterator j;
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tExperiment Types: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // Generic strings. 
    // Currently used by echo.
    p_slist = this->getStringList();
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tGeneric strings: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // view types.
    p_slist = this->getViewList();
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tView Types: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // help keywords.
    p_slist = this->getHelpList();
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tHelp Keywords: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // help modifiers.
    p_slist = this->getHelpModifierList();
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tHelp Modifiers: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // general modifier types.
    p_slist = this->getModifierList();
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tGeneral Modifier Types: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // general instrumentor types.
    p_slist = this->getInstrumentor();
	
    if (p_slist->begin() != p_slist->end())
    	std::cout << "\tGeneral Instrumentor Type: " ;
    for (j=p_slist->begin();j != p_slist->end(); j++) {
    	std::cout << *j << " " ;
    }
    if (p_slist->begin() != p_slist->end())
    	std::cout << std::endl ;

    // Break id list.
    std::vector<int>::iterator i;
    std::vector<int> *p_ilist = this->getBreakList();
	
    if (p_ilist->begin() != p_ilist->end())
    	std::cout << "\tBreak ids: " ;
    for (i=p_ilist->begin();i != p_ilist->end(); i++) {
    	std::cout << *i << " " ;
    }
    if (p_ilist->begin() != p_ilist->end())
    	std::cout << std::endl ;

    // View set list.
    s_dumpRange(this->getViewSet(), "VIEW SETS",false /* is_hex */);

    // Address list.
    s_dumpRange(this->getAddressList(), "ADDRESSES",true /* is_hex */);

    // History list. Should be only one entry.
    s_dumpRange(this->getHistoryList(), "HISTORY",false /* is_hex */);

    // Metric list.
    s_dumpRange(this->getexpMetricList(), "METRICS",false /* is_hex */);

    // Metric list.
    s_dumpRange(this->getexpFormatList(), "METRICS",false /* is_hex */);

    // Interval list.
    s_dumpInterval(this, this->getParseIntervalList(), "INTERVALS");

    // Param list.
    if (this->isParam())
    	s_dumpParam(this->getParam(), "PARAMS");

    // Syntax error.
    s_dumpRange(this->getErrorList(), "ERROR", false /* is_hex */);

    // target list.
    std::vector<ParseTarget>::iterator t_iter;
    std::vector<ParseTarget> *p_tlist = this->getTargetList();

    int64_t count = 1;
    for (t_iter=p_tlist->begin() ;t_iter != p_tlist->end(); t_iter++) {
    	std::cout << "\tTarget #" << count++ << " : " << std::endl;
	
	// various lists
	s_dumpRange(t_iter->getHostList(), "HOST",true /* is_hex */);
	s_dumpRange(t_iter->getFileList(), "FILE",false /* is_hex */);
	s_dumpRange(t_iter->getRankList(), "RANK",false /* is_hex */);
	s_dumpRange(t_iter->getPidList(),  "PID",false /* is_hex */);
	s_dumpRange(t_iter->getThreadList(), "THREAD",false /* is_hex */);
	s_dumpRange(t_iter->getClusterList(), "CLUSTER",false /* is_hex */);
    }
    
    std::string *p_string = this->getRedirectTarget();
    if (p_string->length() > 0) {
    	
    	std::cout << "\tCopy Redirect target: " << *p_string << std::endl;
    }
    
    p_string = this->getAppendTarget();
    if (p_string->length() > 0) {
    	
    	std::cout << "\tAppend Redirect target: " << *p_string << std::endl;
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
const char * 
ParseResult::
getCommandname()
{
#if DEBUG_CLI
    printf("ParseResult::getCommandname, cmd_desc[dm_command_type].name=%s\n", cmd_desc[dm_command_type].name);
#endif
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
#if DEBUG_CLI
    printf("ParseResult::isRetList, cmd_desc[dm_command_type].ret_list=%d\n", cmd_desc[dm_command_type].ret_list);
#endif
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
#if DEBUG_CLI
    printf("ParseResult::isParam, this->dm_param_set=%d\n", this->dm_param_set);
#endif
    return this->dm_param_set;
}
 
/**
 * Method: ParseResult::getParam()
 * 
 *     
 * @return  <ParseParam> *
 *
 * @todo    Error handling.
 *
 */
ParseParam *
ParseResult::
getParam()
{
    return &this->dm_param;
}
 
/**
 * Method: ParseResult::setParam(char *etype, char * ptype)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
setParam(const char *etype, const char * ptype)
{
    dm_param.setExpType(etype);
    dm_param.setParamType(ptype);
    dm_param_set = true;
    
    return ;
}
 
/**
 * Method: ParseResult::pushParmVal(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushParamVal(const char * sval)
{
    dm_param.pushVal(sval);
    return ;
}

/**
 * Method: ParseResult::pushParmVal(int64_t ival)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushParamVal(int64_t ival)
{
    dm_param.pushVal(ival);
    return ;
}

/**
 * Method: ParseResult::pushParmVal(double dval)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushParamVal(double dval)
{
    dm_param.pushVal(dval);
    return ;
}

/////////////////////////////////////////////////

/**
 * Method: ParseResult::getExpId(void)
 * 
 * Used for generic help topic dump
 *     
 * @return  int.
 *
 * @todo    help facility.
 *
 */
int
ParseResult::
getExpId()
{
    std::vector<ParseRange> *p_list = this->getExpIdList();
    int the_id = 0;
    
    if (p_list->begin() != p_list->end()) {
    	std::vector<ParseRange>::iterator iter;
    	iter=p_list->begin();
    	parse_range_t *p_range = iter->getRange();
    	the_id = p_range->start_range.num;
    }

#if DEBUG_CLI
    printf("ParseResult::getExpId, the_id=%d\n", the_id);
#endif
    return the_id;
}

/**
 * Method: ParseResult::isExpId()
 * 
 * Was there an expId request?
 *     
 * @return  true/false.
 *
 * @todo    help facility.
 *
 */
bool
ParseResult::
isExpId()
{

    std::vector<ParseRange> *p_slist = this->getExpIdList();

    if (p_slist->begin() != p_slist->end())
    	return true;

    return false;
}

/**
 * Method: ParseResult::expIdCount()
 * 
 * How many experiment IDs were specified?
 *     
 * @return  Number of IDs.
 *
 * @todo    dunno.
 *
 */
int
ParseResult::
expIdCount()
{

    std::vector<ParseRange> *p_list = this->getExpIdList();
    std::vector<ParseRange>::iterator iter;
    int count = 0;
    
    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	parse_range_t *p_range = iter->getRange();
    	if (p_range->is_range) {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    parse_val_t *p_val2 = &p_range->end_range;
	    
	    count += (p_val2->num-p_val1->num)+1;
    	}
	else {
	    ++count;
	}
    }

#if DEBUG_CLI
    printf("ParseResult::expIdCount, count=%d\n", count);
#endif
    return count;
}

/////////////////////////////////////////////////

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
pushHelp(const char *name)
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

/////////////////////////////////////////////////

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
pushHelpModifier(const char *name)
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
setError(const char * name1, const char * name2)
{
    ParseRange range(name1,name2);

#if DEBUG_CLI
    std::cout << "ParseResult::in setError(" << name1 << "," << name2 << ")" << std::endl;
#endif

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
setError(const char * name)
{
    ParseRange range(name);

#if DEBUG_CLI
    std::cout << "in ParseResult::setError(" << name << ")" << std::endl;
#endif

    dm_error_set = true;
    dm_error_list.push_back(range);

    return ;
}
 
/**
 * Method: ParseResult::pushInterval(int64_t begin, int64_t end, char *attribute)
 * 
 * @param   begin    	integer begin point.
 * @param   end 	integer end point.
 * @param   attribute	What the begin/end numbers represent.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushInterval(int64_t begin, int64_t end)
{
    ParseInterval interval(begin,end);

    dm_interval_list.push_back(interval);

    return ;
}
 
/**
 * Method: ParseResult::pushInterval(int64_t begin, int64_t end, char *attribute)
 * 
 * @param   begin    	integer begin point.
 * @param   end 	double end point.
 * @param   attribute	What the begin/end numbers represent.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushInterval(int64_t begin, double end)
{
    ParseInterval interval(begin,end);

    dm_interval_list.push_back(interval);

    return ;
}
 
/**
 * Method: ParseResult::pushInterval(int64_t begin, int64_t end, char *attribute)
 * 
 * @param   begin    	double begin point.
 * @param   end 	integer end point.
 * @param   attribute	What the begin/end numbers represent.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushInterval(double begin, int64_t end)
{
    ParseInterval interval(begin,end);

    dm_interval_list.push_back(interval);

    return ;
}
 
/**
 * Method: ParseResult::pushInterval(int64_t begin, int64_t end, char *attribute)
 * 
 * @param   begin    	double begin point.
 * @param   end 	double end point.
 * @param   attribute	What the begin/end numbers represent.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
pushInterval(double begin, double end)
{
    ParseInterval interval(begin,end);

    dm_interval_list.push_back(interval);

    return ;
}
 
/**
 * Method: ParseResult::pushInterval(int64_t begin, int64_t end, char *attribute)
 * 
 * @param   begin    	double begin point.
 * @param   end 	double end point.
 * @param   attribute	What the begin/end numbers represent.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
ParseResult::
setIntervalAttribute(const char *attribute)
{
    dm_interval_attribute = attribute;
    dm_interval_attribute_set = true;

    return ;
}
 
/**
 * Method: ParseResult::pushInterval(int64_t begin, int64_t end, char *attribute)
 * 
 * @param   begin    	double begin point.
 * @param   end 	double end point.
 * @param   attribute	What the begin/end numbers represent.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
const std::string *
ParseResult::
getIntervalAttribute()
{
    if (dm_interval_attribute_set)
    	return &dm_interval_attribute;
    else
    	return (const std::string *)NULL;
}

void Dump_ParseResult(ParseRange *p) {
}
 
/**
 * * Method: ParseResult::ExpMetric(char * name1, char * name2, char * name3)
 * *
 * *     Recombine name2 and name3 strings with a ":" in between to allow
 * *     colons to be used as part of a valid name in certain contexts.
 * *    * @return  void.
 * *
 * * @todo    Error handling.
 * *
 * */
ParseRange *
ParseResult::
ExpMetric(const char * name1, const char * name2, const char * name3)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetric, name1=%s, name2=%s, name3=%s\n", name1, name2, name3);
#endif

   int name2_len = strlen(name2);
   int name3_len = strlen(name3);
   char *name23 = (char *)malloc(name2_len + name3_len + 2);
   name23 = strcpy(name23, name2);
   name23 = strcat(name23, ":");
   name23 = strcat(name23, name3);

   ParseRange *pr;
   if (name1 != NULL) {
     pr = new ParseRange(name1,name23);
   } else {
     pr = new ParseRange(name23);
   }

   return pr;
}
 
/**
 * * Method: ParseResult::pushExpMetricExpr(char *name1, ParseRange *arg1)
 * *
 * *     Create an expression out of the arguments: name1 indicates the requrired operation.
 * *     This entry point is used for unary functions.
 * *    * @return  void.
 * *
 * * @todo    Error handling.
 * *
 * */
ParseRange *
ParseResult::
ExpMetricExpr1(const char * name, ParseRange *arg1)
{
#if DEBUG_CLI
   printf("ParseResult::pushExpMetricExpr1, name=%s\n",name);
#endif

   ParseRange *arg2 = NULL;
   expression_operation_t arith_op = EXPRESSION_OP_ERROR;
   if (name != NULL) {
     if (!strcasecmp(name, "uminus")) {
       arith_op = EXPRESSION_OP_UMINUS;
     } else if (!strcasecmp(name, "abs")) {
       arith_op = EXPRESSION_OP_ABS;
     } else if (!strcasecmp(name, "sqrt")) {
       arith_op = EXPRESSION_OP_SQRT;
     } else if (!strcasecmp(name, "a_add")) {
       arith_op = EXPRESSION_OP_A_ADD;
     } else if (!strcasecmp(name, "a_mult")) {
       arith_op = EXPRESSION_OP_A_MULT;
     } else if (!strcasecmp(name, "a_min")) {
       arith_op = EXPRESSION_OP_A_MIN;
     } else if (!strcasecmp(name, "a_max")) {
       arith_op = EXPRESSION_OP_A_MAX;
     } else if (!strcasecmp(name, "uint")) {
       arith_op = EXPRESSION_OP_UINT;
     } else if (!strcasecmp(name, "int")) {
       arith_op = EXPRESSION_OP_INT;
     } else if (!strcasecmp(name, "float")) {
       arith_op = EXPRESSION_OP_FLOAT;
     } else {
      // This is probably an error.
       printf("Error: unrecognized operation in Metric Expression (1 argument) '%s'\n",name);
     }
   }

   return new ParseRange(arith_op,arg1);
}
 
/**
 * * Method: ParseResult::ExpMetricExpr(char *name1, ParseRange *arg1, ParseRange *arg2)
 * *
 * *     Create an expression out of the arguments: name1 indicates the requrired operation.
 * *     This entry point is used for binary expressions.
 * *    * @return  void.
 * *
 * * @todo    Error handling.
 * *
 * */
ParseRange *
ParseResult::
ExpMetricExpr2(const char * name, ParseRange *arg1, ParseRange *arg2)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetricExpr2, name=%s\n",name);
#endif

   expression_operation_t arith_op = EXPRESSION_OP_ERROR;
   if (name != NULL) {
     if (!strcasecmp(name, "header")) {
       arith_op = EXPRESSION_OP_HEADER;
     } else if (!strcasecmp(name, "+") ||
                !strcasecmp(name, "add")) {
       arith_op = EXPRESSION_OP_ADD;
     } else if (!strcasecmp(name, "-") ||
                !strcasecmp(name, "sub")) {
       arith_op = EXPRESSION_OP_SUB;
     } else if (!strcasecmp(name, "*") ||
                !strcasecmp(name, "mult")) {
       arith_op = EXPRESSION_OP_MULT;
     } else if (!strcasecmp(name, "/") ||
                !strcasecmp(name, "div")) {
       arith_op = EXPRESSION_OP_DIV;
     } else if (!strcasecmp(name, "%") ||
                !strcasecmp(name, "mod")) {
       arith_op = EXPRESSION_OP_MOD;
     } else if (!strcasecmp(name, "min")) {
       arith_op = EXPRESSION_OP_MIN;
     } else if (!strcasecmp(name, "max")) {
       arith_op = EXPRESSION_OP_MAX;
     } else if (!strcasecmp(name, "percent")) {
       arith_op = EXPRESSION_OP_PERCENT;
     } else if (!strcasecmp(name, "ratio")) {
       arith_op = EXPRESSION_OP_RATIO;
     } else {
      // This is probably an error.
       printf("Error: unrecognized operation in Metric Expression (2 arguments) '%s'\n",name);
     }
   }

   return new ParseRange(arith_op,arg1,arg2);
}
 
/**
 * * Method: ParseResult::ExpMetricExpr3(char *name1, ParseRange *arg1, ParseRange *arg2, ParseRange *arg3)
 * *
 * *     Create an expression out of the arguments: name1 indicates the requrired operation.
 * *     This entry point is used for conditional expressions.
 * *    * @return  void.
 * *
 * * @todo    Error handling.
 * *
 * */
ParseRange *
ParseResult::
ExpMetricExpr3(const char * name, ParseRange *arg1, ParseRange *arg2, ParseRange *arg3)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetricExpr3, name=%s\n",name);
#endif

   expression_operation_t arith_op = EXPRESSION_OP_ERROR;
   if (name != NULL) {
     if (!strcasecmp(name, "condexp")) {
       arith_op = EXPRESSION_OP_CONDEXP;
     } else if (!strcasecmp(name, "stdev")) {
       arith_op = EXPRESSION_OP_STDEV;
     } else {
      // This is probably an error.
       printf("Error: unrecognized operation in Metric Expression (3 arguments) '%s'\n",name);
     }
   }

   return new ParseRange(arith_op,arg1,arg2,arg3);
}
 
/**
  * Method: ParseResult::ExpMetricExpr(int64_t ival)
  * 
  *     
  * @return  void.
  *
  * @todo    Error handling.
  *
  */
ParseRange *
ParseResult::
ExpMetricConstant(char* c)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetricConstant, char=%s\n",c);
#endif

  ParseRange *pr = new ParseRange(EXPRESSION_OP_CONST, c);
  return pr;
}
 
/**
  * Method: ParseResult::ExpMetricExpr(int64_t ival)
  * 
  *     
  * @return  void.
  *
  * @todo    Error handling.
  *
  */
ParseRange *
ParseResult::
ExpMetricConstant(int64_t ival)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetricConstant, int64_t=%lld\n",ival);
#endif

  ParseRange *pr = new ParseRange(EXPRESSION_OP_CONST, ival);
  return pr;
}

/**
 * Method: ParseResult::ExpMetricExpr(double dval)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange *
ParseResult::
ExpMetricConstant(double dval)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetricConstant, double=%20.10f\n",dval);
#endif

  ParseRange *pr = new ParseRange(EXPRESSION_OP_CONST, dval);
  return pr;
  
}

/**
 * * Method: ParseResult::ExpMetricExpr(char *name1, ParseRange *arg1, ParseRange *arg2, ParseRange *arg3)
 * *
 * *     Create an expression out of the arguments: name1 indicates the requrired operation.
 * *    * @return  void.
 * *
 * * @todo    Error handling.
 * *
 * */
ParseRange *
ParseResult::
ExpMetricExpr(const char * name, ParseRange *arg1, ParseRange *arg2, ParseRange *arg3)
{
#if DEBUG_CLI
   printf("ParseResult::ExpMetricExpr, name=%s %p %p %p\n",name,arg1,arg2,arg3);
   if (arg1 != NULL) {printf("\targ1: "); arg1->Dump(); printf("\n");}
   if (arg2 != NULL) {printf("\targ2: "); arg2->Dump(); printf("\n");}
   if (arg3 != NULL) {printf("\targ3: "); arg3->Dump(); printf("\n");}
#endif

   if (strcasecmp(name, "single") == 0) {
     return arg1;
   }
   if (strcasecmp(name, "parens") == 0) {
     return arg1;
   }
   if (strcasecmp(name, "list") == 0) {
     return arg1;
   }

   if ( (arg1 != NULL) && (arg2 == NULL) && (arg3 == NULL) ) {
    arg2 = arg1->getNext();
    if (arg2 != NULL) {
      arg3 = arg2->getNext();
    }
   }

   expression_operation_t arith_op = EXPRESSION_OP_ERROR;
   if (name != NULL) {
     if ( (arg1 != NULL) && (arg2 == NULL) && (arg3 == NULL) ) {
       return ExpMetricExpr1 (name, arg1);
     } else if ( (arg1 != NULL) && (arg2 != NULL) && (arg3 == NULL) ) {
       return ExpMetricExpr2 (name, arg1, arg2);
     } else if ( (arg1 != NULL) && (arg2 != NULL) && (arg3 != NULL) ) {
       return ExpMetricExpr3 (name, arg1, arg2, arg3);
     } else {
      // This is probably an error.
       printf("Error: unrecognized operation in Metric Expression '%s'\n",name);
     }
   }
}

/**
 * Method: ParseResult::ExpMetric(char * name1, char * name2)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange *
ParseResult::
ExpMetric(const char * name1, const char * name2)
{
#if DEBUG_CLI
    printf("ParseResult::ExpMetric, name1=%s, name2=%s\n", name1, name2);
#endif

    return new ParseRange(name1,name2);
}
 

/**
 * Method: ParseResult::ExpMetric(char * name)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange *
ParseResult::
ExpMetric(const char * name)
{
#if DEBUG_CLI
    printf("ParseResult::ExpMetric, name=%s\n", name);
#endif

ParseRange *pr = new ParseRange(name);
  return pr;
}

ParseRange *
ParseResult::
ExpMetric(const char * name, ParseRange *pr)
{
#if DEBUG_CLI
    printf("ParseResult::ExpMetric(name,pr)), %p name=%s\n", pr, name);
    printf("\t");pr->Dump(); return pr;
#endif
    return pr;
}
 
/**
 * * Method: ExpMetricList(ParseRange *expr1, ParseRange expr2)
 * *
 * *     Connect 2 ParseRange structures in a list by placing
 * *     expr2 at the end of any existing list already attached
 * *     to expr1.
 * *    * @return  ParseRange*
 * *
 * */
ParseRange *
ParseResult::
ExpMetricList(ParseRange *expr1, ParseRange *expr2)
{
#if DEBUG_CLI
  printf("\nExpMetricList, expression=%p->%p\n", expr1, expr2);
#endif

  if (expr1 == NULL) {
    return expr2;
  }

  ParseRange *pr = expr1;
 // Find the end of the list on arg1.
  for ( ParseRange *npr = pr->getNext(); npr != NULL; npr = npr->getNext() ) { pr = npr; }

 // Attach arg2 to the end of arg1.
  pr->setNext(expr2);
  return expr1;
}

/**
 * * Method: pushMetricList(ParseRange *expr)
 * *
 * *     Convert a ParseRange list into an vector.
 * *    * @return  void
 * *
 * */
void
ParseResult::
pushMetricList(ParseRange *expr)
{
#if DEBUG_CLI
    printf("\npushMetricList, expression=%p\n", expr);
#endif
    for (ParseRange *pr = expr; pr != NULL; )
    {
      ParseRange *npr = pr->getNext();
      pr->setNext(NULL);
      dm_exp_metric_list.push_back(*pr);
      pr = npr;
    }
}
 

 
/**
  * Method: ParseResult::ExpFormatConstant(const char *name)
  * 
  *     
  * @return  void.
  *
  * @todo    Error handling.
  *
  */
ParseRange *
ParseResult::
ExpFormatConstant(const char *name)
{
#if DEBUG_CLI
   printf("ParseResult::ExpFormatConstant, name=%s\n",name);
#endif

  ParseRange *pr = new ParseRange(name);
  if (!check_validFormatName(name)) {
    printf("Error: unsupported format specifier '%s'\n",name);
    pr->setParseTypeError();
  }
  return pr;
}
 
/**
  * Method: ParseResult::ExpFormatConstant(const char *name, char *c)
  * 
  *     
  * @return  void.
  *
  * @todo    Error handling.
  *
  */
ParseRange *
ParseResult::
ExpFormatConstant(const char *name, char* c)
{
#if DEBUG_CLI
   printf("ParseResult::ExpFormatConstant, name=%s, char=%s\n",name,c);
#endif

  ParseRange *pr = new ParseRange(name, c);
  if (!check_validFormatName(name)) {
    printf("Error: unsupported format specifier '%s'\n",name);
    pr->setParseTypeError();
  }
  return pr;
}
 
/**
  * Method: ParseResult::ExpFormatConstant(const char *name, int64_t ival)
  * 
  *     
  * @return  void.
  *
  * @todo    Error handling.
  *
  */
ParseRange *
ParseResult::
ExpFormatConstant(const char *name, int64_t ival)
{
#if DEBUG_CLI
   printf("ParseResult::ExpFormatConstant, name=%s, int64_t=%lld\n",name,ival);
#endif

  ParseRange *pr = new ParseRange(name, ival);
  if (!check_validFormatName(name)) {
    printf("Error: unsupported format specifier '%s'\n",name);
    pr->setParseTypeError();
  }
  return pr;
}

/**
 * Method: ParseResult::ExpFormatConstant(const char *name, double dval)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange *
ParseResult::
ExpFormatConstant(const char *name, double dval)
{
#if DEBUG_CLI
   printf("ParseResult::ExpFormatConstant, name=%s, double=%20.10f\n",name,dval);
#endif

  ParseRange *pr = new ParseRange(name, dval);
  if (!check_validFormatName(name)) {
    printf("Error: unsupported format specifier '%s'\n",name);
    pr->setParseTypeError();
  }
  return pr;
  
}

/**
 * Method: ParseResult::ExpFormat(char * name1, ParseRange *pr)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange *
ParseResult::
ExpFormat(const char * name, ParseRange *pr)
{
#if DEBUG_CLI
    printf("ParseResult::ExpFormat(name,pr), %p name=%s\n", pr, name);
    printf("\t");pr->Dump();
#endif
    return pr;
}
 
/**
 * * Method: ExpFormatList(ParseRange *expr1, ParseRange expr2)
 * *
 * *     Connect 2 ParseRange structures in a list by placing
 * *     expr2 at the end of any existing list already attached
 * *     to expr1.
 * *    * @return  ParseRange*
 * *
 * */
ParseRange *
ParseResult::
ExpFormatList(ParseRange *expr1, ParseRange *expr2)
{
#if DEBUG_CLI
  printf("\nExpFormatList, expression=%p->%p\n", expr1, expr2);
#endif

  if (expr1 == NULL) {
    return expr2;
  }
  if (expr2->getParseType() == PARSE_RANGE_ERROR) {
    delete expr2;
    return expr1;
  }

  ParseRange *pr = expr1;
 // Find the end of the list on arg1.
  for ( ParseRange *npr = pr->getNext(); npr != NULL; npr = npr->getNext() ) { pr = npr; }

 // Attach arg2 to the end of arg1.
  pr->setNext(expr2);
  return expr1;
}

/**
 * * Method: pushFormatList(ParseRange *expr)
 * *
 * *     Convert a ParseRange list into an vector.
 * *    * @return  void
 * *
 * */
void
ParseResult::
pushFormatList(ParseRange *expr)
{
#if DEBUG_CLI
    printf("\npushFormatList, expression=%p\n", expr);
#endif
    for (ParseRange *pr = expr; pr != NULL; )
    {
      ParseRange *npr = pr->getNext();
      pr->setNext(NULL);
      dm_exp_format_list.push_back(*pr);
      pr = npr;
    }
}

void ParseRangeTrace(int t) { fprintf(stderr,"ParseRangeTrace: %d\n",t); }
