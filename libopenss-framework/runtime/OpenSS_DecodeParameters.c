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
 * Definition of the OpenSS_DecodeParameters() function.
 *
 */

#include "RuntimeAPI.h"

#include <alloca.h>
#include <assert.h>



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
 * @param argument  Encoded blob argument to be decoded.
 * @param xdrproc   XDR procedure for the parameter data structure.
 * @retval data     Pointer to the decoded parameter data structure.
 * @return          Boolean "TRUE" if succeeded or "FALSE" if failed.
 */
bool_t OpenSS_DecodeParameters(const char* argument,
			       const xdrproc_t xdrproc, void* data)
{
    const char SafeValue = 0xBA;  /* Constant used in decoding */    
    bool_t ok = TRUE;  /* Assume success unless otherwise found below */
    unsigned i, size;
    char* buffer = NULL;
    XDR xdrs;
    
    /* Check assertions */
    assert(argument != NULL);
    assert(xdrproc != NULL);
    assert(data != NULL);

    /* Allocate the decoding buffer */
    buffer = alloca(strlen(argument));
    
    /* Iterate over each byte in the string-encoded blob argument */
    for(i = 0, size = 0; i < strlen(argument); ++i, ++size) {

	/* Replace doubled-up occurences of the safe value with one occurence */
	if((argument[i] == SafeValue) &&
	   (i < (strlen(argument) - 1)) && (argument[i + 1] == SafeValue)) {
	    buffer[size] = SafeValue;
	    ++i;
	}

        /* Replace safe values with zero */
	else if(argument[i] == SafeValue)
	    buffer[size] = 0x00;
	
	/* Otherwise pass the byte through unchanged */
	else
	    buffer[size] = argument[i];
	
    }

    /* Create an XDR stream using the decoding buffer */
    xdrmem_create(&xdrs, buffer, size, XDR_DECODE);
    
    /* Attempt to decode the parameter data structure from this stream */
    ok &= (*xdrproc)(&xdrs, data);
    
    /* Close the XDR stream */
    xdr_destroy(&xdrs);
    
    /* Inform the caller whether we succeeded or not */
    return ok;
}
