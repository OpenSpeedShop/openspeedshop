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
 * Message related objects and routines.
 *
 */

#include <vector>
#include <string>
#include <iostream>

using namespace std;

#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

using namespace OpenSpeedShop;

/**
 * Method: SS_Message_Czar::Add_Help()
 * 
 * 
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Add_Help(SS_Message_Element& element) 
{
   dm_help_list.push_back(element);
}

/**
 * Method: SS_Message_Czar::Add_Error()
 * 
 * 
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Add_Error(SS_Message_Element& element) 
{

   dm_error_list.push_back(element);

}

/**
 * Method: SS_Message_Czar::Find_By_Keyword()
 * 
 * 
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Find_By_Keyword(string keyword,vector <SS_Message_Element *> *p_element) 
{
    vector <SS_Message_Element>:: iterator k;
    for (k=dm_help_list.begin();
    	 k!= dm_help_list.end();
	 ++k) {
	if ((keyword.compare(*k->get_keyword())) == 0) {
	    SS_Message_Element& el = *k;
	    p_element->push_back(&el);
	}
    }
}

/**
 * Method: SS_Message_Czar::Find_By_Err_No()
 * 
 * 
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Find_By_Err_No(int id_num, vector <SS_Message_Element *> *p_element) 
{

    vector <SS_Message_Element>:: iterator k;
    for (k=dm_help_list.begin();
    	 k!= dm_help_list.end();
	 ++k) {
	SS_Message_Element *p = &(*k);
	int ndx = p->get_id();
	if (ndx == id_num) {
	    p_element->push_back(&(*k));
	}
    }
}

/**
 * Method: SS_Message_Czar::Find_By_Related()
 * 
 * 
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Find_By_Related(string related, vector <SS_Message_Element *> *p_element) 
{

#if 0
    vector <SS_Message_Element>:: iterator k;
    for (k=dm_help_list.begin();
    	 k!= dm_help_list.end();
	 ++k) {
	if ((related.compare(*k->get_related())) == 0) {
	    p_element->push_back(&(*k));
	}
    }
#endif
}

/**
 * Method: theMessageCzar()
 * 
 * This is where one references the message czar with
 * confidence that it has been constructed and without
 * the cost of extra instanciations.
 *     
 * @return  SS_Message_Czar&.
 *
 * @todo    Error handling.
 *
 */
SS_Message_Czar&
theMessageCzar() 
{

    static SS_Message_Czar msg_czar;
    
    return msg_czar;

}

#if 0
/**
 * Method: SS_Message_Czar::xx()
 * 
 * 
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
xx() 
{

}


#endif

