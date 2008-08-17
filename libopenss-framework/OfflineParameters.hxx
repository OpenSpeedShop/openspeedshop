/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008 Krell Institute  All Rights Reserved.
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
 * Parser param description and definition of the OfflineParameters and OfflineParamVal classes.
 *
 */

#ifndef __OfflineParameters_HXX__
#define __OfflineParameters_HXX__

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
class OfflineParamVal {

    public:

//	/** Constructor. */
	OfflineParamVal(char * sval) :
	    dm_sval(sval),
	    dm_val_type(PARAM_VAL_STRING) {
	};

	OfflineParamVal(int64_t ival) :
	    dm_ival(ival),
	    dm_val_type(PARAM_VAL_INT) {
	};

	OfflineParamVal(double dval) :
	    dm_dval(dval),
	    dm_val_type(PARAM_VAL_DOUBLE) {
	};

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
class OfflineParameters {

    public:

//	/** Constructors. */
	OfflineParameters():
	    dm_has_exptype(false),
	    dm_exptype(NULL),
	    dm_param_type(NULL) {
	};

	OfflineParameters(char *exp_type,char *parm_type) :
	    dm_has_exptype(false),
	    dm_exptype(exp_type),
	    dm_param_type(parm_type) {

	    if (exp_type)
		dm_has_exptype = true;

	};

//	/** Destructor. */
//	~ParseRange();

    	void setExpType(char * name) {
	    dm_exptype = name;
	}
    	void setParamType(char * name) {
	    dm_param_type = name;
	}
    	char *getExpType() {return dm_exptype;}
    	char *getParamType() {return dm_param_type;}

    	/** Handle list of line numbers. */
    	std::vector<OfflineParamVal> * getValList()
	{
	    return &dm_param_val_list;
	}

    	void pushVal(char * sval) {
	    OfflineParamVal value(sval);
    	    dm_param_val_list.push_back(value);
	}
    	void pushVal(int64_t ival) {
	    OfflineParamVal value(ival);
    	    dm_param_val_list.push_back(value);
	}
    	void pushVal(double dval) {
	    OfflineParamVal value(dval);
    	    dm_param_val_list.push_back(value);
	}

    private:
    	/** range struct to fill */
    	bool dm_has_exptype;
    	char * dm_exptype;
    	char * dm_param_type;

    	/** Container of param values */
    	std::vector<OfflineParamVal> dm_param_val_list;
};

#endif // __OfflineParameters_HXX__
