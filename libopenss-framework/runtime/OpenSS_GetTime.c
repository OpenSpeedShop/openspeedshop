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
 * Definition of the OpenSS_GetTime() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <time.h>



/**
 * Get the current time.
 *
 * Returns the current wall-clock time as a single 64-bit unsigned integer.
 * This integer is interpreted as the number of nanoseconds that have passed
 * since midnight (00:00) Coordinated Universal Time (UTC), on January 1, 1970.
 *
 * @return    Current time.
 *
 * @ingroup RuntimeAPI
 */
uint64_t OpenSS_GetTime()
{
    struct timespec now;

    /* Get the current wall-clock time */
    Assert(clock_gettime(CLOCK_REALTIME, &now) == 0);
 
    /* Return the time to the caller */
    return ((uint64_t)(now.tv_sec) * (uint64_t)(1000000000)) +
	(uint64_t)(now.tv_nsec);
}
