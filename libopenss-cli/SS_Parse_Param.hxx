/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.
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
 * Parser param description and definition of the ParseParam and ParseVal classes.
 *
 */

#ifndef __OpenSpeedShop_Parse_Param_HXX__
#define __OpenSpeedShop_Parse_Param_HXX__

namespace OpenSpeedShop { namespace cli {

typedef enum {
    PARAM_VAL_STRING,
    PARAM_VAL_INT,
    PARAM_VAL_DOUBLE
    
} oss_param_val_enum;

/**
 * Parser Param value class.
 *
 *  Describes the values assigned to a param.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParamVal {

    public:

//	/** Constructor. */
	ParamVal(const char * name);
	ParamVal(int64_t ival);
	ParamVal(double dval);

//	/** Destructor. */
//	~ParseRange();

	char	*getSVal() {return dm_sval;}
	int 	 getIVal() {return dm_ival;}
	double	 getDVal() {return dm_dval;}
	oss_param_val_enum getValType()
	    	    {return dm_val_type;}

    private:
    	/** type of value */
    	oss_param_val_enum dm_val_type;

    	char * dm_sval;
    	int    dm_ival;
    	double dm_dval;
};

/**
 * Parser Param class.
 *
 *  Describes the results from parsing an openss param.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseParam {

    public:

//	/** Constructors. */
	ParseParam();
	ParseParam(const char *s1,const char *s2);

//	/** Destructor. */
//	~ParseRange();

    	void setExpType(const char * name) {
	    dm_exptype = (char *) name;
	}
    	void setParamType(const char * name) {
	    dm_param_type = (char *) name;
	}
    	char *getExpType() {return dm_exptype;}
    	char *getParamType() {return dm_param_type;}

    	/** Handle list of line numbers. */
    	std::vector<ParamVal> * getValList()
	{
	    return &dm_param_val_list;
	}

    	void pushVal(const char * sval) {
	    ParamVal value(sval);
    	    dm_param_val_list.push_back(value);
	}
    	void pushVal(int64_t ival) {
	    ParamVal value(ival);
    	    dm_param_val_list.push_back(value);
	}
    	void pushVal(double dval) {
	    ParamVal value(dval);
    	    dm_param_val_list.push_back(value);
	}

    private:
    	/** range struct to fill */
    	bool dm_has_exptype;
    	char * dm_exptype;
    	char * dm_param_type;

    	/** Container of param values */
    	std::vector<ParamVal> dm_param_val_list;
};

} }

#endif // __OpenSpeedShop_Parse_Param_HXX__
