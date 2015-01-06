/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2015 Krell Institute  All Rights Reserved.
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
 * Load messages pertaining to this module.
 *
 */

// This must be first include due to warnings regarding redefinition
// of certain definitions (POSIX_*)
#include "SS_Input_Manager.hxx"

#include <vector>
#include <string>
#include <iostream>


#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"


using namespace OpenSpeedShop::cli;

#include "SS_Parse_Interval.hxx"
#include "SS_Parse_Param.hxx"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop;

// CLI messages
#include "help_general_msg.dxx"
#include "help_cmd_msg.dxx"
#include "help_grammar_msg.dxx"
#include "help_topic_msg.dxx"

// Scripting messages
#include "help_scripting_msg.dxx"

extern SS_Message_Czar& theMessageCzar();

/**
 * Function:  pcli_load_messages()
 * 
 * Load messages for the pcli module into the
 * message czar.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
pcli_load_messages() 
{

    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();
    
    // Load command help messages.
    // First command entry is always dummy error so skip.
    int i = 1;
    while (cmd_msg[i].topic != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&cmd_msg[i],false /* is_topic */);
	czar.Add_Help(element);
	++i;
    }
    
    // Load grammar help messages.
    // Table ends when topic is NULL.
    i = 0;
    while (grammar_msg[i].topic != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&grammar_msg[i],false /* is_topic */);
	czar.Add_Help(element);
	++i;
    }
    
    // Load general help messages.
    // Table ends when topic is NULL.
    i = 0;
    while (general_msg[i].topic != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&general_msg[i],false /* is_topic */);
	czar.Add_Help(element);
	++i;
    }

    // Load topic messages.
    // Table ends when topic is NULL.
    i = 0;
    while (topic_msg[i].topic != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&topic_msg[i],true /* is_topic */);
	czar.Add_Help(element);
	++i;
    }
    
    
}

/**
 * Function:  pcli_load_scripting_messages()
 * 
 * Load messages for python scripting into the
 * message czar.
 *
 * This will initially only contain parsing syntax
 * information since the python help facility should
 * suffice for anything else.
 *     
 * @return  void.
 *
 * @todo    We may  have to figure out how to extend
 *  	    this for plugin information.
 *
 */
void
pcli_load_scripting_messages() 
{

    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();
    
    // Load python scripting api grammar help messages.
    // Table ends when topic is NULL.
    int i = 1;
    while (script_msg[i].keyword != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&script_msg[i],false /* is_topic */);
	czar.Add_Help(element);
	++i;
    }
        
}

