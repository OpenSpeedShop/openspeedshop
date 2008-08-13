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
 * Declaration and definition of the OpenSS_FileIO funstions for
 * the offline and mrnet collector runtimes.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"

#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void OpenSS_CreateFilePrefix (char *collectorname);
void OpenSS_CreateOutfile (char *suffix);

__thread  char *OpenSS_outfile;
__thread  char *OpenSS_rawprefix;
__thread  char *OpenSS_exepath;
__thread  uint64_t OpenSS_rawtid = 0;

static pid_t create_OpenSS_exepath () {
    pid_t pid = getpid();
    char tmpname[PATH_MAX],buf[PATH_MAX];

    /* find our exe path (read symlink for /proc/pid/exe) */
#if 0
    OpenSS_Path_From_Pid(tmpname);
    OpenSS_exepath = strdup(tmpname);
#else
    memset(tmpname,0x0,sizeof(tmpname));
    memset(buf,0x0,sizeof(buf));
    sprintf(tmpname,"/proc/%d/exe",pid);
    if (readlink(tmpname,buf,PATH_MAX) == -1) {
	fprintf(stderr,"Openss could not determine executable.\n");
	OpenSS_exepath = strdup("Unknown-exe");
    } else {
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_FILEIO") != NULL) {
	fprintf(stderr,"Openss finds executable %s.\n", buf);
    }
#endif
	OpenSS_exepath = strdup(buf);
    }
#endif

#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_FILEIO") != NULL) {
	fprintf(stderr,"create_OpenSS_exepath: OpenSS_exepath = %s, pid = %d\n",OpenSS_exepath,pid);
    }
#endif
    return pid;
}

void OpenSS_CreateFilePrefix (char *collectorname) {
    if (collectorname == NULL) {
	fprintf(stderr,"Error: invalid prefix name\n");
	abort();
    }

    pid_t pid = create_OpenSS_exepath();

    char rawdirname[PATH_MAX], tmpname[PATH_MAX], dirname[PATH_MAX], bname[PATH_MAX];

    if (getenv("OPENSS_RAWDATA_DIR") != NULL) {
	sprintf(rawdirname,"%s",getenv("OPENSS_RAWDATA_DIR"));
    } else {
	sprintf(rawdirname,"%s","/tmp");
    }

    sprintf(bname,"%s",basename(OpenSS_exepath));
    sprintf(dirname,"%s/openss-rawdata-%d",rawdirname,pid);

    /* create a subdirectory in /tmp (TODO: Allow overriding /tmp)
     * to hold resulting raw output files. */
    int rval = mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sprintf(tmpname,"%s/%s-%d",dirname,bname,pid);
    OpenSS_rawprefix = strdup(tmpname);
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_FILEIO") != NULL) {
	fprintf(stderr,"OpenSS_CreateFilePrefix: OpenSS_rawprefix = %s\n",OpenSS_rawprefix);
    }
#endif
}

void OpenSS_CreateOutfile (char *suffix) {
    uint64_t (*realfunc)() = dlsym (RTLD_NEXT, "pthread_self");
    if ( (*realfunc) != NULL) {
	OpenSS_rawtid = (*realfunc)();
    }

    char tmpname[PATH_MAX];

    /* create pathname to write raw data for specific thread */
    if (OpenSS_rawtid > 0) {
        sprintf(tmpname,"%s-%lu.%s",OpenSS_rawprefix,OpenSS_rawtid,suffix);
    }
    else {
        sprintf(tmpname,"%s.%s",OpenSS_rawprefix,suffix);
    }

    /* create our openss-raw output filename */
    OpenSS_outfile = strdup(tmpname);
#ifndef NDEBUG
    if (getenv("OPENSS_DEBUG_FILEIO") != NULL) {
	fprintf(stderr,"OpenSS_CreateOutfile: OpenSS_outfile = %s\n",OpenSS_outfile);
    }
#endif
}
