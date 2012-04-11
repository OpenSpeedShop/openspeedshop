/*******************************************************************************
 * ** Copyright (c) 2006-2012 The Krell Institue. All Rights Reserved.
 * **
 * ** This library is free software; you can redistribute it and/or modify it under
 * ** the terms of the GNU Lesser General Public License as published by the Free
 * ** Software Foundation; either version 2.1 of the License, or (at your option)
 * ** any later version.
 * **
 * ** This library is distributed in the hope that it will be useful, but WITHOUT
 * ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * ** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * ** details.
 * **
 * ** You should have received a copy of the GNU Lesser General Public License
 * ** along with this library; if not, write to the Free Software Foundation, Inc.,
 * ** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * *******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <dlfcn.h>
#include <pthread.h>

extern void offline_record_dso(const char* dsoname, uint64_t begin, uint64_t end, uint8_t is_dlopen);
#if defined(TARGET_OS_BGP)
#include <link.h>
extern etext;
extern edata;
#elif defined(TARGET_OS_BGQ)
#include <link.h>
extern etext;
extern edata;
#endif

int OpenSS_GetDLInfo(pid_t pid, char *path)
{
#if defined(TARGET_OS_BGP)
    offline_record_dso(OpenSS_GetExecutablePath(),(uint64_t)0x01000000,(uint64_t)&etext,0);
#elif defined(TARGET_OS_BGQ)
    offline_record_dso(OpenSS_GetExecutablePath(),(uint64_t)0x01000000,(uint64_t)&etext,0);
#else
    char mapfile_name[PATH_MAX];
    FILE *mapfile;

    sprintf(mapfile_name, "/proc/%ld/maps", (long)pid);
    mapfile = fopen(mapfile_name, "r");

    if(!mapfile) {
	fprintf(stderr,"Error opening%s: %s\n", mapfile_name, strerror(errno));
	return(1);
    }

#if 0
#ifndef NDEBUG
    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {

	pthread_t (*f_pthread_self)();
        f_pthread_self = (pthread_t (*)())dlsym(RTLD_DEFAULT, "pthread_self");
	fprintf(stderr,"OpenSS_GetDLInfo called for %s in %d,%lu\n",
		path ? path : "EMPTY PATH", getpid(),
		(f_pthread_self != NULL) ? (*f_pthread_self)() : 0);
    }
#endif
#endif

    while(!feof(mapfile)) {
	char buf[PATH_MAX+100], perm[5], dev[6], mappedpath[PATH_MAX];
	unsigned long begin, end, inode, offset;
	int n;

	/* read in one line from the /proc maps file for this pid. */
	if(fgets(buf, sizeof(buf), mapfile) == 0) {
	    break;
	}

	char *permstring = strchr(buf, (int) ' ');
	if (!(*(permstring+3) == 'x' && strchr(buf, (int) '/'))) {
	    continue;
	}

	mappedpath[0] = '\0';

	/* Read in the /proc/<pid>/maps file as it is formatted. */
	/* All fields are strings. The fields are as follows. */
	/* address  perms offset  dev  inode  pathname */
	/* The address field is begin-end in hex. */
	/* We record these as uint64_t. */
	/* perms are at least one of rwxp - we want the begin and end */
	/* address of the text section marked as "x". */
	/* We record the mappedpath as is and ignore the rest of the fields. */
        sscanf(buf, "%lx-%lx %s %lx %s %ld %s", &begin, &end, perm,
                &offset, dev, &inode, mappedpath);

	/* If a dso is passed in the path argument we only want to record */
	/* this particular dso into the openss-raw file. This happens when */
	/* the victim application has performed a dlopen. */
	if (path != NULL &&
	    mappedpath != NULL &&
	    (strncmp(path, mappedpath, strlen(path)) == 0) ) {
#ifndef NDEBUG
	    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
		fprintf(stderr,"OpenSS_GetDLInfo DLOPEN RECORD: %s [%08lx, %08lx]\n",
		    mappedpath, begin, end);
	    }
#endif
	    offline_record_dso(mappedpath, begin, end, 1);
	    break;
	}

	// DPM: added test for path 4-15-08
	else if (perm[2] == 'x' && path == NULL) {
#ifndef NDEBUG
	    if ( (getenv("OPENSS_DEBUG_COLLECTOR") != NULL)) {
		fprintf(stderr,"OpenSS_GetDLInfo LD RECORD %s\n", mappedpath);
	    }
#endif
	    offline_record_dso(mappedpath, begin, end, 0);
	}
    }
    fclose(mapfile);
#endif
    return(0);
}
