/*******************************************************************************
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
 * Definition of the OpenSS_EncodeParameters() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <stdlib.h>



/**
 * Encode a function's parameters.
 *
 * Encodes separate parameters into the XDR encoded blob argument and string
 * passed to collector runtime functions.
 *
 * @param data        Parameter data structure to be encoded.
 * @param xdrproc     XDR procedure for the parameter data structure.
 * @param argument    Pointer to the encoded blob argument.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_EncodeParameters(const void* data,
			     const xdrproc_t xdrproc, char* argument)
{
    const size_t EncodingBufferSize = 15 * 1024;
    unsigned i, j, size;
    char* buffer = NULL;
    XDR xdrs;
    
    /* Check assertions */
    Assert(argument != NULL);
    Assert(xdrproc != NULL);
    Assert(data != NULL);

    /* Allocate the encoding buffer */
    buffer = alloca(EncodingBufferSize);

    /* Create an XDR stream using the encoding buffer */
    xdrmem_create(&xdrs, buffer, EncodingBufferSize, XDR_ENCODE);
    
    /* Encode the parameter data structure to this stream */
    Assert((*xdrproc)(&xdrs, (void*)data) == TRUE);

    /* Get the encoded size */
    size = xdr_getpos(&xdrs);

    /* Close the XDR stream */
    xdr_destroy(&xdrs);

    /* Iterate over each byte in the XDR-encoded blob argument */
    for(i = 0; i < size; ++i) {

	/* Encode the most-significant four bits */
	argument[i * 2] = ((buffer[i] >> 4) & 0xF) + '0';
		 
	/* Encode the least-significant four bits */
	argument[(i * 2) + 1] = (buffer[i] & 0xF) + '0';

    }
    argument[size * 2] = '\0';
}
