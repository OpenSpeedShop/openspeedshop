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
 * utility functions for libunwind
 *
 */

#ifndef _runtime_libunwind_
#define _runtime_libunwind_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <errno.h>
#define UNW_LOCAL_ONLY
#include "libunwind.h"

#if UNW_TARGET_X86
# define STACK_SIZE     (128*1024)      /* On x86, SIGSTKSZ is too small */
#else
# define STACK_SIZE     SIGSTKSZ
#endif

void OpenSS_GetStackTraceFromContext (const ucontext_t* context,
                        int overhead,
                        int maxframes,
                        int *framecount,
                        uint64_t *framebuf);

#endif
