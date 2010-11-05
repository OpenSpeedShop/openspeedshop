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

////////////////////////////////////////////////
// Case insensitive compare function:
int 
stringCmpi(const std::string& s1, const std::string& s2) {
    // Select the first element of each std::string:
    std::string::const_iterator 
    p1 = s1.begin(), p2 = s2.begin();

    // Don't run past the end:
    while(p1 != s1.end() && p2 != s2.end()) {
    	// Compare upper-cased chars:
    	if(toupper(*p1) != toupper(*p2))
    	    // Report which was lexically  greater:
    	    return (toupper(*p1)<toupper(*p2))? -1 : 1;
    	p1++;
    	p2++;
    }
    // If they match up to the detected eos, say 
    // which was longer. Return 0 if the same.
    return(s2.size() - s1.size());
}
////////////////////////////////////////////////

#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

using namespace OpenSpeedShop;

/**
 * Method: SS_Message_Czar::Add_Help()
 * 
 * Add an SS_Message_Element to the help container
 * for a particular help message.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 * @todo    Example.
 *
 */
void
SS_Message_Czar::
Add_Help(const SS_Message_Element& element) 
{
   dm_help_list.push_back(element);
}

/**
 * Method: SS_Message_Czar::Add_Error()
 * 
 * Add an SS_Message_Element to the error container
 * for a particular error message.
 * 
 * It needs to be decided how to key into error messages.
 * Do we require error numbers or are keywords good enough?
 *     
 * @return  void.
 *
 * @todo    Error handling.
 * @todo    Example.
 *
 */
void
SS_Message_Czar::
Add_Error(const SS_Message_Element& element) 
{

   dm_error_list.push_back(element);

}

/**
 * Method: SS_Message_Czar::Find_By_Keyword()
 * 
 * Given a keyword, do a case sensitive search in
 * the help list. If no hits, do a case insensitive
 * search.
 * 
 * We go through the full message database gathering
 * multiple hits.
 *     
 * @return  void, but std::vector of SS_Message_Element * passed in.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Find_By_Keyword(const std::string keyword,std::vector <SS_Message_Element *> *p_element) 
{
    std::vector <SS_Message_Element>:: iterator k;
    bool found = false;

    // Case sensitive search
    for (k=dm_help_list.begin();
    	 k!= dm_help_list.end();
	 ++k) {
	if ((keyword.compare(*k->get_keyword())) == 0) {
	    SS_Message_Element& el = *k;
	    p_element->push_back(&el);
	    found = true;
	}
    }
    
    if (found)
    	return;

    // Case insensitive search
    for (k=dm_help_list.begin();
    	 k!= dm_help_list.end();
	 ++k) {
    	if ((stringCmpi(keyword,*k->get_keyword())) == 0) {
	    SS_Message_Element& el = *k;
	    p_element->push_back(&el);
	}
    }
}

/**
 * Method: SS_Message_Czar::Find_By_Err_No()
 * 
 * Given an integer find the message associated
 * with it.
 * 
 *     
 * @return  void, but std::vector of SS_Message_Element * passed in.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Find_By_Err_No(const int id_num, std::vector <SS_Message_Element *> *p_element) 
{

    std::vector <SS_Message_Element>:: iterator k;
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
 * Method: SS_Message_Czar::Find_By_Topic()
 * 
 * Not done yet!
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
SS_Message_Czar::
Find_By_Topic(const std::string related, std::vector <SS_Message_Element *> *p_element) 
{

#if 1
    std::vector <SS_Message_Element>:: iterator k;
    for (k=dm_help_list.begin();
    	 k!= dm_help_list.end();
	 ++k) {
	if ((related.compare(*k->get_topic())) == 0) {
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

