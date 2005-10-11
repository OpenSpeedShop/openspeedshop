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

