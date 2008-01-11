/*******************************************************************************
** Copyright (c) The Krell Institute (2008). All Rights Reserved.
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
 * Declaration and definition of the Watcher_OpenSS_FileIO funstions for
 * the offline and mrnet collector runtimes.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "Watcher_RuntimeAPI.hxx"
#include "Watcher_FileIO.hxx"
#include <iostream>
#include <ltdl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <unistd.h>



pid_t create_Watcher_OpenSS_exepath () {
    pid_t pid = getpid();
    char tmpname[PATH_MAX];

    /* find our exe path (read symlink for /proc/pid/exe) */
    Watcher_OpenSS_Path_From_Pid(tmpname);
    Watcher_OpenSS_exepath = strdup(tmpname);
    return pid;
}

pid_t create_Watcher_OpenSS_exepath (pid_t pidID) {
    char tmpname[PATH_MAX];

    /* find our exe path (read symlink for /proc/pid/exe) */
    Watcher_OpenSS_Path_From_Pid(tmpname, pidID);
    Watcher_OpenSS_exepath = strdup(tmpname);
    return pidID;
}


char * Watcher_OpenSS_GetFilePrefix (const char *collectorname, pid_t pidID) {

    if (collectorname == NULL) {
	fprintf(stderr,"Error: invalid prefix name\n");
	abort();
    }

    pid_t pid = create_Watcher_OpenSS_exepath(pidID);
    char tmpname[PATH_MAX], dirname[PATH_MAX],*bname;

    bname = basename(Watcher_OpenSS_exepath);
    sprintf(dirname,"/tmp/%s-%s-%d",collectorname,bname,pid);

    return(strdup(dirname));
}

char * Watcher_OpenSS_GetFilePrefix (const char *collectorname) {

    if (collectorname == NULL) {
	fprintf(stderr,"Error: invalid prefix name\n");
	abort();
    }

    pid_t pid = create_Watcher_OpenSS_exepath();
    char tmpname[PATH_MAX], dirname[PATH_MAX],*bname;

    bname = basename(Watcher_OpenSS_exepath);
    sprintf(dirname,"/tmp/%s-%s-%d",collectorname,bname,pid);

    return(strdup(dirname));
}

void Watcher_OpenSS_CreateFilePrefix (char *collectorname) {
    if (collectorname == NULL) {
	fprintf(stderr,"Error: invalid prefix name\n");
	abort();
    }

    pid_t pid = create_Watcher_OpenSS_exepath();
    char tmpname[PATH_MAX], dirname[PATH_MAX],*bname;

    bname = basename(Watcher_OpenSS_exepath);
    sprintf(dirname,"/tmp/%s-%s-%d",collectorname,bname,pid);

    /* create a subdirectory in /tmp (TODO: Allow overriding /tmp)
     * to hold resulting raw output files. */
    int rval = mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sprintf(tmpname,"%s/%s-%d",dirname,bname,pid);
    Watcher_OpenSS_rawprefix = strdup(tmpname);
}

#if 0
void Watcher_OpenSS_CreateOutfile (char *suffix) {
    uint64_t (*realfunc)() = dlsym (RTLD_NEXT, "pthread_self");
    if ( (*realfunc) != NULL) {
	Watcher_OpenSS_rawtid = (*realfunc)();
    }

    char tmpname[PATH_MAX];

    /* create pathname to write raw data for specific thread */
    if (Watcher_OpenSS_rawtid > 0) {
        sprintf(tmpname,"%s-%lu.%s",Watcher_OpenSS_rawprefix,Watcher_OpenSS_rawtid,suffix);
    }
    else {
        sprintf(tmpname,"%s.%s",Watcher_OpenSS_rawprefix,suffix);
    }

    /* create our openss-raw output filename */
    Watcher_OpenSS_outfile = strdup(tmpname);
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_FILEIO") != NULL) {
	fprintf(stderr,"Watcher_OpenSS_CreateOutfile: Watcher_OpenSS_outfile = %s\n",Watcher_OpenSS_outfile);
    }
#endif
}
#endif
