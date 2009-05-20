/*******************************************************************************
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007,2008,2009 The Krell Institue. All Rights Reserved.
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
 * Definition of the OpenSS_GetExecutablePath() function.
 *
 */

#include "RuntimeAPI.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>



/** Path of the executable. */
static char executable_path[PATH_MAX] = "";



/**
 * Executable path initialization.
 *
 * Performs one-time initialization which actually gets the executable path.
 *
 * @note    One-time execution of this initialization is accomplished by
 *          applying the "constructor" attribute to this function. Doing
 *          so causes GCC to force it to be executed upon loading into a
 *          process. This attribute appears to be supported back to GCC
 *          2.95.3 or further.
 *
 * @sa    http://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Function-Attributes.html
 *
 * @ingroup Implementation
 */
static void __attribute__ ((constructor)) initialize()
{
    size_t length = 0;

    /*
     * Simply dereferencing the symbolic link at /proc/self/exe will give the
     * full path of the executable 99% of the time on Linux. Check there first.
     * The funky business with memset() and strlen() here is to deal with early
     * Linux kernels that returned incorrect length values from readlink(). 
     */
    memset(executable_path, 0, sizeof(executable_path));
    readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
    length = strlen(executable_path);
    if((length > 0) && (executable_path[length - 1] == '*'))
        executable_path[length - 1] = 0;
    if(strlen(executable_path) == 0)
        sprintf(executable_path, "UnknownExecutable");
}



/**
 * Get the executable path.
 *
 * Returns the executable path of the calling thread.
 *
 * @return    Executable path.
 *
  * @ingroup RuntimeAPI
 */
const char* OpenSS_GetExecutablePath()
{
    if (strlen(executable_path) > 1) {
	return executable_path;
    } else {
	/* One-time initialization may not succeed with mpi.
	 * In that case we need to try at again.  This is
	 * based on results from a SUSE cluster at NASA running
	 * MPT where the executable path for all the ranks was
	 * found to be empty.
	 */
	size_t length = 0;
	memset(executable_path, 0, sizeof(executable_path));
	readlink("/proc/self/exe", executable_path, sizeof(executable_path) - 1);
	length = strlen(executable_path);
	if((length > 0) && (executable_path[length - 1] == '*'))
            executable_path[length - 1] = 0;
	if(strlen(executable_path) == 0)
            sprintf(executable_path, "UnknownExecutable");
    }
}
