/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Storage for a single time interval for a view.
 *
 */

#include <vector>
#include <iostream>
#include <stdint.h>


#include "SS_Parse_Interval.hxx"

using namespace OpenSpeedShop::cli;

/**
 * Constructer: ParseInterval::ParseInterval()
 * 
 *     
 * @param   begin    	integer begin point.
 * @param   end 	integer end point.
 *
 * @todo    Error handling.
 *
 */
ParseInterval::
ParseInterval(int64_t begin, int64_t end) :
    dm_start_is_int(true),
    dm_start_int(begin),
    dm_start_double(0.0),

    dm_end_is_int(true),
    dm_end_int(end),
    dm_end_double(0.0)
{

    /* So far, nothing more to do. */
}

/**
 * Constructer: ParseInterval::ParseInterval()
 * 
 *     
 * @param   begin    	integer begin point.
 * @param   end 	double end point.
 *
 * @todo    Error handling.
 *
 */
ParseInterval::
ParseInterval(int64_t begin, double end) :
    dm_start_is_int(true),
    dm_start_int(begin),
    dm_start_double(0.0),

    dm_end_is_int(false),
    dm_end_int(0),
    dm_end_double(end)
{

    /* So far, nothing more to do. */
}

/**
 * Constructer: ParseInterval::ParseInterval()
 * 
 *     
 * @param   begin    	double begin point.
 * @param   end 	integer end point.
 *
 * @todo    Error handling.
 *
 */
ParseInterval::
ParseInterval(double begin, int64_t end) :
    dm_start_is_int(false),
    dm_start_int(0),
    dm_start_double(begin),

    dm_end_is_int(true),
    dm_end_int(end),
    dm_end_double(0.0)
{

    /* So far, nothing more to do. */
}

/**
 * Constructer: ParseInterval::ParseInterval()
 * 
 *     
 * @param   begin    	double begin point.
 * @param   end 	double end point.
 *
 * @todo    Error handling.
 *
 */
ParseInterval::
ParseInterval(double begin, double end) :
    dm_start_is_int(false),
    dm_start_int(0),
    dm_start_double(begin),

    dm_end_is_int(false),
    dm_end_int(0),
    dm_end_double(end)
{

    /* So far, nothing more to do. */
}

