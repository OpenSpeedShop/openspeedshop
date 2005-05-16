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
 * Load messages pertaining to this module.
 *
 */

#include <vector>
#include <string>
#include <iostream>

using namespace std;

#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

#include "SS_Input_Manager.hxx"
#include "SS_Cmd_Execution.hxx"

using namespace OpenSpeedShop::cli;

#include "SS_Parse_Param.hxx"
#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop;

#include "help_cmd_msg.dxx"

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
    for (int i=1;i<CMD_MAX;++i) {
    	SS_Message_Element element;
	
	element.set_element(&cmd_msg[i]);
	czar.Add_Help(element);
    }
    
    // Load grammar help messages.
    // Table ends when topic is NULL.
    int i = 0;
    while (grammar_msg[i].topic != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&grammar_msg[i]);
	czar.Add_Help(element);
	++i;
    }
    
}

