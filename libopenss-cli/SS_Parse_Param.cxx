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

using namespace std;

#include "SS_Parse_Param.hxx"

using namespace OpenSpeedShop::cli;

/**
 * Method: ParseParam::ParseParam()
 * 
 *     
 * @param   exp_type    	
 * @param   parm_type 	.
 * @param   val	 	.
 *
 * @todo    Error handling.
 *
 */
ParseParam::
ParseParam(char * exp_type, char * parm_type, int val) :
    dm_has_exptype(false),
    dm_exptype(exp_type),
    dm_param_type(parm_type),
    dm_val_is_string(false),
    dm_param_val_num(val),
    dm_param_val_string(NULL)
{

    /* I decided this is less complicated than 
       making more constructors */
    if (exp_type)
    	dm_has_exptype = true;

    /* I should allocate and copy the strings */
}

/**
 * Method: ParseParam::ParseParam()
 * 
 *     
 * @param   exp_type    	
 * @param   parm_type 	.
 * @param   val	 	.
 *
 * @todo    Error handling.
 *
 */
ParseParam::
ParseParam(char * exp_type, char * parm_type, char * val) :
    dm_has_exptype(false),
    dm_exptype(exp_type),
    dm_param_type(parm_type),
    dm_val_is_string(true),
    dm_param_val_num(0),
    dm_param_val_string(val)
{
    /* I decided this is less complicated than 
       making more constructors */
    if (exp_type)
    	dm_has_exptype = true;

    /* I should allocate and copy the strings */
}

