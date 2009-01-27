/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Definition of the OpenSS_DecodeParameters() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <alloca.h>



/**
 * Decode a function's parameters.
 *
 * Decodes the string and XDR encoded blob argument passed to collector runtime
 * functions into separate parameters.
 *
 * @note    It is crucial that the parameter data structure be initialized
 *          to zero by the caller before being passed into this function. In
 *          several places where decoding variable-length data, such as in
 *          xdr_string(), the XDR library assumes that if it finds a non-null
 *          pointer it can simply use the memory at that location. If that
 *          pointer is a bunch of uninitialized values, a crash is almost
 *          certain. The memory cannot be zeroed here because we don't know
 *          the type (or even the size) of the parameter data structure.
 *
 * @param argument    Encoded blob argument to be decoded.
 * @param xdrproc     XDR procedure for the parameter data structure.
 * @retval data       Pointer to the decoded parameter data structure.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_DecodeParameters(const char* argument,
			     const xdrproc_t xdrproc, void* data)
{
    unsigned i, size;
    char* buffer = NULL;
    XDR xdrs;
    
    /* Check assertions */
    Assert(argument != NULL);
    Assert(xdrproc != NULL);
    Assert(data != NULL);
    
    /* Allocate the decoding buffer */
    Assert(strlen(argument) % 2 == 0);    
    size = strlen(argument) / 2;
    buffer = alloca(size);
    
    /* Iterate over each byte in the string-encoded blob argument */
    for(i = 0; i < strlen(argument); i += 2) {
	
	/* Decode the most-significant four bits */
	buffer[i / 2] = (argument[i] - '0') << 4;
	
	/* Decode the least-significant four bits */
	buffer[i / 2] |= argument[i + 1] - '0';

    }
    
    /* Create an XDR stream using the decoding buffer */
    xdrmem_create(&xdrs, buffer, size, XDR_DECODE);
    
    /* Decode the parameter data structure from this stream */
    Assert((*xdrproc)(&xdrs, data) == TRUE);
    
    /* Close the XDR stream */
    xdr_destroy(&xdrs);
}
