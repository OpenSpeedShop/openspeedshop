/*******************************************************************************
** Copyright (c) 2008 The Krell Institute. All Rights Reserved.
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
 * Declaration for the file io routines needed for offline and mrnet collector runtimes.
 *
 */

#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//char *Watcher_OpenSS_outfile;

static __thread  char *Watcher_OpenSS_rawprefix;
static __thread  char *Watcher_OpenSS_exepath;
static __thread  pthread_t Watcher_OpenSS_rawtid = 0;

char * Watcher_OpenSS_GetFilePrefix (const char *collectorname);
char * Watcher_OpenSS_GetFilePrefix (const char *collectorname, pid_t pidID);
void Watcher_OpenSS_CreateFilePrefix (char *collectorname);
void Watcher_OpenSS_CreateFilePrefix (char *collectorname, pid_t pidID);
void Watcher_OpenSS_CreateOutfile (char *suffix);