////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration and definition of the Assert function.
 *
 */

#ifndef _OpenSpeedShop_Framework_Assert_
#define _OpenSpeedShop_Framework_Assert_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>



/**
 * Check a runtime assertion.
 *
 * Asserts (i.e. checks at runtime) that a particular expression is true. Does
 * so in a manner almost identical to that of the standard C assert() function.
 * The only real difference is that this function is NOT disabled by defining
 * the NDEBUG macro.
 *
 * @param assertion    Assertion expression to be checked.
 *
 * @ingroup Utility
 */
#define Assert(assertion)			    			    \
    if(!(assertion)) {							    \
	fprintf(stderr, "Assertion \"%s\" failed in file %s at line %d.\n", \
		# assertion, __FILE__, __LINE__);			    \
	fflush(stderr);                                                     \
	abort();							    \
    }



#endif
