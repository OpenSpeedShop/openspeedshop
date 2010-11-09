/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Parser interval description and ParseInterval class.
 *
 */

#ifndef __OpenSpeedShop_Parse_Interval_HXX__
#define __OpenSpeedShop_Parse_Interval_HXX__

namespace OpenSpeedShop { namespace cli {

/**
 * Parser time interval class.
 *
 *  Describes the results from parsing a time
 *  interval for viewing.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseInterval {

    public:

//	/** Constructor. */
	ParseInterval(int64_t begin, int64_t end);
	ParseInterval(int64_t begin, double end);
	ParseInterval(double begin, int64_t end);
	ParseInterval(double begin, double end);

//	/** Destructor. */
//	~TimeInterval();

	bool   isStartInt()  	    {return dm_start_is_int;}
	int64_t getStartInt() 	    {return dm_start_int;}
	double getStartdouble()     {return dm_start_double;}

	bool   isEndInt()  	    {return dm_end_is_int;}
	int64_t getEndInt() 	    {return dm_end_int;}
	double getEndDouble() 	    {return dm_end_double;}

    private:
    	/** range struct to fill */
    	bool	dm_start_is_int;
    	int64_t dm_start_int;
    	double  dm_start_double;

    	bool	dm_end_is_int;
    	int64_t dm_end_int;
    	double  dm_end_double;
};

} }

#endif // __OpenSpeedShop_Parse_Interval_HXX__
