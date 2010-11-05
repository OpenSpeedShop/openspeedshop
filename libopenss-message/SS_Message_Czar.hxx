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

#ifndef __OpenSpeedShop_Message_Czar_HXX__
#define __OpenSpeedShop_Message_Czar_HXX__

namespace OpenSpeedShop {

/**
 * OSS message czar class.
 *
 *  This class is used exclusively from within this
 *  file. There should be exactly one (1) instance
 *  of this class object created per invocation of 
 *  SpeedShop.
 *
 *  It should be the vessel from which all messages
 *  are stored at runtime and from which the other
 *  elements of the tool retrieve these messages, probably
 *  in the form of a pointer to one or more SS_Message_Element
 *  objects.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class SS_Message_Czar {

    public:
    
    	void Add_Help(const SS_Message_Element&);
	void Add_Error(const SS_Message_Element&);
    	void Find_By_Keyword(const std::string, std::vector <SS_Message_Element *> *);
    	void Find_By_Topic(const std::string, std::vector <SS_Message_Element *> *);
	// void Find_By_Related(const std::string, std::vector <SS_Message_Element *> *);
	void Find_By_Err_No(const int, std::vector <SS_Message_Element *> *);

    private:
    
    	std::vector <SS_Message_Element> dm_error_list;
	std::vector <SS_Message_Element> dm_help_list;

};  // class SS_Message_Czar
	

}  	// namespace OpenSpeedShop
#endif // __OpenSpeedShop_Message_Czar_HXX__
