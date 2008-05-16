/*******************************************************************************
** Copyright (c) 2007 Krell Institute. All Rights Reserved.
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
 * Definition of the OpenSS_Path_From_Pid() function for offline collectors.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <stdio.h>

/**
 *
 * @param data    Pointer to the pathname to be written.
 * @return        Integer "1" if succeeded or "0" if failed.
 */

bool_t
OpenSS_Path_From_Pid(char *exe_path)
{
    char name[PATH_MAX];
    char symlink[PATH_MAX];

    pid_t cur_pid = getpid(); // This had better be our mutatee.

    // Cobble together "/proc/<pid>/exe"
    sprintf(&symlink[0],"%s%d%s","/proc/",cur_pid,"/exe");

    // Get the real name and full path of the executable.
    memset(name,'\0',PATH_MAX);
    readlink("/proc/self/exe",name,PATH_MAX);
    strncpy(exe_path,name,strlen(name));
}
