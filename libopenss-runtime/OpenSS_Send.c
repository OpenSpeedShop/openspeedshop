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
 * Definition of the OpenSS_Send() function.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <alloca.h>

/* External declaration (defined by the instrumentor) */
int openss_send(const unsigned, const void*);



/**
 * Send performance data.
 *
 * Sends performance data to the framework instance which loaded this runtime
 * library into the target process. The necessary data header is prepended to
 * the actual data and both are XDR encoded before being sent.
 *
 * @todo    DPCL currently imposes a maximum (around 16Kb) on the length of
 *          individual messages sent using Ais_send(). In the future this limit
 *          should be removed. For now an assertion failure occurs if the 
 *          encoded message exceeds this limit.
 *
 * @param header     Performance data header to apply to this data.
 * @param xdrproc    XDR procedure for the passed data structure.
 * @param data       Pointer to the data structure to be sent.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_Send(const OpenSS_DataHeader* header,
		 const xdrproc_t xdrproc, const void* data)
{
    const size_t EncodingBufferSize = (OpenSS_BlobSizeFactor * 15 * 1024);
    unsigned size;
    char* buffer = NULL;
    XDR xdrs;
    
    /* Check preconditions */
    Assert(header != NULL);
    Assert(xdrproc != NULL);
    Assert(data != NULL);
    
    /* Allocate the encoding buffer */
    buffer = alloca(EncodingBufferSize);
    
    /* Create an XDR stream using the encoding buffer */
    xdrmem_create(&xdrs, buffer, EncodingBufferSize, XDR_ENCODE);

    /* Encode the performance data header to this stream */
    Assert(xdr_OpenSS_DataHeader(&xdrs, (void*)header) == TRUE);

    /* Encode the data structure to this stream */
    Assert((*xdrproc)(&xdrs, (void*)data) == TRUE);
    
    /* Get the encoded size */
    size = xdr_getpos(&xdrs);
    
    /* Close the XDR stream */
    xdr_destroy(&xdrs);
    
    /* Send the data */
    Assert(openss_send(size, buffer) == 1);
}
