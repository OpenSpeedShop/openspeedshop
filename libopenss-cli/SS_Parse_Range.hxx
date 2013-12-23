/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2012 Krell Institute  All Rights Reserved.
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
 * Parser range description and definition of the ParseRange class.
 *
 */

#ifndef __OpenSpeedShop_Parse_Range_HXX__
#define __OpenSpeedShop_Parse_Range_HXX__

namespace OpenSpeedShop { namespace cli {

typedef enum {
    PARSE_RANGE_ERROR,
    PARSE_RANGE_VALUE,
    PARSE_EXPRESSION_VALUE
} parse_type_enum_t;

typedef enum {
    VAL_STRING,
    VAL_NUMBER,
    VAL_DOUBLE
} val_enum_t;

typedef struct {
    // union is not possible with type string
    std::string name;
    int64_t num;
    double val;
    val_enum_t tag; /** Determines with field to be used */
} parse_val_t;

typedef struct {
    parse_val_t start_range;	/** Begin of range or point */
    parse_val_t end_range;  	/** End of Range */
    bool   is_range;	    	/** Is this a range or single point */
} parse_range_t;

class ParseRange;
typedef struct {
    expression_operation_t exp_op;
    std::vector<ParseRange *> exp_operands;
} parse_expression_t;

/**
 * Parser result class.
 *
 *  Describes the results from parsing one OSS command.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseRange {

    public:

	/** Constructor. */
	ParseRange(int64_t num);
	ParseRange(const char *name);
	ParseRange(const char * name, int64_t num);
	ParseRange(const char * name, double dval);
	ParseRange(const char * name1, const char * name2);
	ParseRange(int64_t num, const char * name);
	ParseRange(int64_t num1, int64_t num2);
	ParseRange(expression_operation_t op, char *c);
	ParseRange(expression_operation_t op, int64_t ival);
	ParseRange(expression_operation_t op, double dval);
	ParseRange(expression_operation_t op, ParseRange *operand0);
	ParseRange(expression_operation_t op, ParseRange *operand0, ParseRange *operand1);
	ParseRange(expression_operation_t op, ParseRange *operand0,
                                         ParseRange *operand1, ParseRange *operand2);
	void Dump();

//	/** Destructor. */
//	~ParseRange();

    	parse_range_t *getRange()
	{
	    return &dm_range;
	}

    	ParseRange *getNext()
	{
	    return next;
	}

    	void setNext(ParseRange *newNext)
	{
	    next = newNext;
	}

    	void setParseTypeError()
	{
	    dm_parse_type = PARSE_RANGE_ERROR;
	}

    	parse_type_enum_t getParseType()
	{
	    return dm_parse_type;
	}

    	expression_operation_t getOperation()
	{
	    return dm_exp.exp_op;
	}

    	parse_expression_t *getExpression()
	{
	    return &dm_exp;
	}

    private:
    	/** range struct to fill */
    	parse_type_enum_t dm_parse_type;
    	parse_expression_t dm_exp;
    	parse_range_t dm_range;
    	ParseRange *next;

};

// Utility used to find format options after parsing.
ParseRange *Look_For_Format_Specification (std::vector<ParseRange> *f_list, std::string Key);

} }

#endif // __OpenSpeedShop_Parse_Range_HXX__
