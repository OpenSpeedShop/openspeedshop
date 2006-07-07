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
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */

#include <vector>
#include <iostream>

#include "SS_Input_Manager.hxx"

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(int64_t num)
{
    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num;
	    
    dm_range.is_range = false;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */

ParseRange::
ParseRange(char * name)
{
    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name;
	    
    dm_range.is_range = false;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(char * name, int64_t num)
{
    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name;
	    
    dm_range.end_range.tag = VAL_NUMBER;
    dm_range.end_range.num = num;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(char * name1, char * name2)
{
    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name1;
	    
    dm_range.end_range.tag = VAL_STRING;
    dm_range.end_range.name = name2;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(int64_t num, char * name)
{
    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num;
	    
    dm_range.end_range.tag = VAL_STRING;
    dm_range.end_range.name = name;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(int64_t num1, int64_t num2)
{
    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num1;
	    
    dm_range.end_range.tag = VAL_NUMBER;
    dm_range.end_range.num = num2;
	    
    dm_range.is_range = true;
}
 
