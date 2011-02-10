/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2011 Krell Institute  All Rights Reserved.
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
#include <stdint.h>


#include "SS_Parse_Param.hxx"

using namespace OpenSpeedShop::cli;

////////////////////////////////////////////////////
// Definition of ParseParam objects
////////////////////////////////////////////////////
/**
 * Method: ParseParam::ParseParam()
 * 
 *     This is how it is invoked from the parser.
 *
 * @todo    Error handling.
 *
 */
ParseParam::
ParseParam() :
    dm_has_exptype(false),
    dm_exptype(NULL),
    dm_param_type(NULL)
 {

}

/**
 * Method: ParseParam::ParseParam()
 * 
 *     Do we need this constructor?
 *
 * @param   exp_type    	
 * @param   parm_type 	.
 *
 * @todo    Error handling.
 *
 */
ParseParam::
ParseParam(const char * exp_type, const char * parm_type) :
    dm_has_exptype(false),
    dm_exptype((char *) exp_type),
    dm_param_type((char *) parm_type)
{
    /* I decided this is less complicated than 
       making more constructors */
    if (exp_type)
    	dm_has_exptype = true;
}

////////////////////////////////////////////////////
// Definition of ParamVal objects
////////////////////////////////////////////////////
/**
 * Method: ParamVal::ParamVal()
 * 
 *     Set a string value
 *
 * @param   sval value string    	
 *
 * @todo    Error handling.
 *
 */
ParamVal::
ParamVal(const char * sval) :
    dm_sval((char *) sval),
    dm_val_type(PARAM_VAL_STRING)
{
}

/**
 * Method: ParamVal::ParamVal()
 * 
 *     Set an integer value
 *
 * @param   ival int value    	
 *
 * @todo    Error handling.
 *
 */
ParamVal::
ParamVal(int64_t ival) :
    dm_ival(ival),
    dm_val_type(PARAM_VAL_INT)
{
}

/**
 * Method: ParamVal::ParamVal()
 * 
 *     Set a double value
 *
 * @param   dval value string    	
 *
 * @todo    Error handling.
 *
 */
ParamVal::
ParamVal(double dval) :
    dm_dval(dval),
    dm_val_type(PARAM_VAL_DOUBLE)
{
}

