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

#include "Assert.hxx"
#include "RuntimeAPI.h"

#include <alloca.h>



/**
 * Decode a library function's parameters.
 *
 * Decodes into separate parameters the XDR-encoded blob argument to a library
 * function.
 *
 * @note    It is crucial that the parameter data structure be initialized
 *          to zero by the caller before being passed into this function. In
 *          several places where decoding variable length data, such as in
 *          xdr_string(), the XDR library assumes that if it finds a non-null
 *          pointer it can simply use the memory at that location. If that
 *          pointer is a bunch of uninitialized values, a crash is almost
 *          certain. The memory cannot be zeroed here because we don't know
 *          the type (or even the size) of the parameter data structure.
 *
 * @param argument    Encoded blob argument to be decoded.
 * @param xdrproc     XDR procedure for the parameter data structure.
 * @retval data       Pointer to the decoded parameter data structure.
 * @return            Integer "1" if succeeded or "0" if failed.
 */
int OpenSS_DecodeParameters(const char* argument,
			    const xdrproc_t xdrproc,
			    void* data)
{
    const char SafeValue = 0xBA;  /* Constant used in decoding */    
    int retval = 1;  /* Assume success unless found otherwise below */
    unsigned i, size;
    char* contents = NULL;
    XDR xdrs;

    /* Check assertions */
    Assert(argument != NULL);
    Assert(xdrproc != NULL);
    Assert(data != NULL);
    
    /* Allocate a string to contain the string-decoded contents */
    contents = alloca(strlen(argument));
    
    /* Iterate over each byte in the encoded blob argument */
    for(i = 0, size = 0; i < strlen(argument); ++i, ++size) {

	/* Replace doubled-up occurences of the safe value with one occurence */
	if((argument[i] == SafeValue) &&
	   (i < (strlen(argument) - 1)) && (argument[i + 1] == SafeValue)) {
	    contents[size] = SafeValue;
	    ++i;
	}

        /* Replace safe values with zero */
	else if(argument[i] == SafeValue)
	    contents[size] = 0x00;
	
	/* Otherwise pass the byte through unchanged */
	else
	    contents[size] = argument[i];
	
    }

    /* Open an XDR stream using the string-decoded contents */    
    xdrmem_create(&xdrs, contents, size, XDR_DECODE);
    
    /* Attempt to decode the parameter data structure from this stream */
    if((*xdrproc)(&xdrs, data) == FALSE)
	retval = 0;
    
    /* Close the XDR stream */
    xdr_destroy(&xdrs);

    /* Inform the caller whether we succeeded or not */
    return retval;    
}
