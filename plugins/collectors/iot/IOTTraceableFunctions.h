////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Declaration of the IOTCollector TraceableFunctions
 *
 */

#ifndef _IOTTraceableFunctions_
#define _IOTTraceableFunctions_
 
    static const char* TraceableFunctions[] = {

        "close",
        "creat",
        "creat64",
        "dup",
        "dup2",
        "lseek",
        "lseek64",
        "open",
        "open64",
        "pipe",
        "pread",
        "pread64",
        "pwrite",
        "pwrite64",
        "read",
        "readv",
#ifndef DEBUG
        "write",
#endif
        "writev",

	/* End Of Table Entry */
	NULL
    };

#if defined(OPENSS_OFFLINE)
        static const char * traceable = \
	"close,creat,creat64,dup,dup2,lseek,lseek64,open,open64,pipe,pread,pread64,pwrite,pwrite64,read,readv,write,writev";

#endif

#endif
