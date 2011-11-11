/*******************************************************************************
** Copyright (c) 2010 The Krell Institute. All Rights Reserved.
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
 * utility functions for unwinding
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

void OpenSS_GetStackTraceFromContext(const ucontext_t*,
                                     bool_t, unsigned, unsigned,
                                     unsigned*, uint64_t*);

#if defined(__linux) && defined(__x86_64)
void OpenSS_GetStackTrace( bool_t, unsigned, unsigned,
			   unsigned*, uint64_t*);
#endif
