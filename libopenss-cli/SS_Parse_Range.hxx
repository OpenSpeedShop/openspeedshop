/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006 Krell Institute  All Rights Reserved.
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
    VAL_STRING,
    VAL_NUMBER
} val_enum_t;

typedef struct {
    // union is not possible with type string
    string name;
    int64_t num;
    val_enum_t tag; /** Determines with field to be used */
} parse_val_t;

typedef struct {
    parse_val_t start_range;	/** Begin of range or point */
    parse_val_t end_range;  	/** End of Range */
    bool   is_range;	    	/** Is this a range or single point */
} parse_range_t;

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

//	/** Constructor. */
	ParseRange(int64_t num);
	ParseRange(char *);
	ParseRange(char * name, int64_t num);
	ParseRange(char * name1, char * name2);
	ParseRange(int64_t num, char * name);
	ParseRange(int64_t num1, int64_t num2);

//	/** Destructor. */
//	~ParseRange();

    	parse_range_t *getRange()
	{
	    return &dm_range;
	}

    private:
    	/** range struct to fill */
    	parse_range_t dm_range;

};

} }

#endif // __OpenSpeedShop_Parse_Range_HXX__
