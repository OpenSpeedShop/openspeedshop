/*******************************************************************************
** Copyright (c) 2007 The Krell Institute. All Rights Reserved.
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
 * Declaration for the offline PC sampling collector's runtime.
 *
 */

#include <stdio.h>
#include <signal.h>
#include "monitor.h"
#include "OpenSS_DataHeader.h"
#include "offline/offline.h"
#include <dlfcn.h>

char *OpenSS_rawdata;

static __thread  char *rawprefix;
static __thread  char *OpenSS_exepath;
static __thread  pthread_t tid = 0;

char *OpenSS_dsopath;

/** Thread-local storage. */
static __thread struct {

    OpenSS_DataHeader header;  /**< Header for following object blob. */

    openss_objects objs;
    
} tlsobj;

static __thread struct {

    OpenSS_DataHeader header;  /**< Header for following object blob. */

    openss_expinfo info;
    
} tlsinfo;

void offline_record_dso(const char* dsoname, uint64_t begin, uint64_t end);
