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
 * Parser param description.
 *
 */

#ifndef __OpenSpeedShop_Parse_Param_HXX__
#define __OpenSpeedShop_Parse_Param_HXX__

namespace OpenSpeedShop { namespace cli {

/**
 * Parser result class.
 *
 *  Describes the results from parsing one OSS command.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseParam {

    public:

//	/** Constructor. */
	ParseParam(char * exp_type, char * parm_type, int val);
	ParseParam(char * exp_type, char * parm_type, char * name);

//	/** Destructor. */
//	~ParseRange();

    	char *getParmExpType() {return dm_exptype;}
    	char *getParmParamType() {return dm_param_type;}
	bool isValString() {return dm_val_is_string;}
	char *getStingVal() {return dm_param_val_string;}
	int getnumVal() {return dm_param_val_num







;}

    private:
    	/** range struct to fill */
    	bool dm_has_exptype;
    	char * dm_exptype;
    	char * dm_param_type;

    	bool dm_val_is_string;
    	int dm_param_val_num;
    	char * dm_param_val_string;

};

} }

#endif // __OpenSpeedShop_Parse_Param_HXX__
