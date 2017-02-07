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
 * Definition of the Blob class.
 *
 */

#include "Assert.hxx"
#include "Blob.hxx"

#include <iostream>
#include <string.h>

using namespace OpenSpeedShop::Framework;



/**
 * Default constructor.
 *
 * Constructs an empty Blob whose size is zero and contents is null.
 */
Blob::Blob() :
    dm_size(0),
    dm_contents(NULL)
{
}



/**
 * Copy constructor.
 *
 * Constructs a new Blob by copying the specified blob. The compiler provided
 * default is insufficient here because we need a deep, rather than shallow,
 * copy of the contents for correct object destruction.
 *
 * @param other    Blob to be copied.
 */
Blob::Blob(const Blob& other) :
    dm_size(0),
    dm_contents(NULL)
{
    // Only do an actual copy if the other blob is non-empty
    if((other.dm_size > 0) && (other.dm_contents != NULL)) {
	
	// Make a copy of the blob
	dm_size = other.dm_size;
	dm_contents = new char[dm_size];
	memcpy(dm_contents, other.dm_contents, dm_size);
	
    }
}



/**
 * Constructor from size and contents.
 *
 * Constructs a new Blob from the specified size and contents. A copy of the
 * contents is made and is automatically release upon object destruction.
 *
 * @param size        Size of the blob (in bytes).
 * @param contents    Pointer to the blob's contents.
 */
Blob::Blob(const unsigned& size, const void* contents) :
    dm_size(0),
    dm_contents(NULL)
{
    // Only do initialization if the size and pointer are valid
    if((size > 0) && (contents != NULL)) {
    
	// Make a copy of the blob's contents
	dm_size = size;
	dm_contents = new char[dm_size];
	memcpy(dm_contents, contents, dm_size);

    }
}



/**
 * Constructor from XDR encoding.
 *
 * Constructs a new Blob containing the XDR encoding of the specified data
 * structure. The XDR procedure for the specified data structure's type must
 * also be passed as a parameter.
 *
 * @note    The size of the XDR encoding for the data structure cannot be
 *          determined apriori. As a result, we end up trying to make some
 *          semi-intelligent guesses at appropriate encoding buffer sizes.
 *          Basically we keep increasing the size until it is large enough.
 *          This is not pretty and maybe could be improved in the future.
 *
 * @param xdrproc    XDR procedure for the passed data type.
 * @param data       Pointer to the data structure to be encoded.
 */
Blob::Blob(const xdrproc_t xdrproc, const void* data) :
    dm_size(0),
    dm_contents(NULL)
{
    // Check assertions
    Assert(xdrproc != NULL);
    Assert(data != NULL);
    
    // Iteratively allocate increasingly large buffers to hold the XDR encoding
    for(unsigned size = 1024; dm_contents == NULL; size *= 2) {
	
	// Allocate the encoding buffer
	char* buffer = new char[size];
	
	// Create an XDR stream using the encoding buffer
	XDR xdrs;
	xdrmem_create(&xdrs, buffer, size, XDR_ENCODE);

	// Attempt to encode the data structure to this stream
	if((*xdrproc)(&xdrs, const_cast<void*>(data)) == TRUE) {
	    
	    // Success! Make this encoding the blob's contents
	    dm_size = xdr_getpos(&xdrs);
	    dm_contents = buffer;
	    
	}
	
	// Close the XDR stream
	xdr_destroy(&xdrs);
	
	// Destroy the encoding buffer if encoding failed
	if(dm_contents == NULL)
	    delete [] buffer;
	
    }    
}



/**
 * Destructor.
 *
 * Destroys the blob's contents if it had any.
 */
Blob::~Blob()
{    
    // Destroy our contents (if any)
    delete [] reinterpret_cast<char*>(dm_contents);
}



/**
 * Assignment operator.
 *
 * Operator "=" defined for a Blob object. The compiler provided default is
 * insufficient here because we need a deep, rather than shallow, copy of the
 * contents for correct object destruction.
 *
 * @param other    Blob to be copied.
 */
Blob& Blob::operator=(const Blob& other)
{
    // Only do an assignment if the LHS and RHS differ
    if((dm_size != other.dm_size) || (dm_contents != other.dm_contents)) {
	
	// Destroy our current contents (if any)
	delete [] reinterpret_cast<char*>(dm_contents);

	// Only do an actual copy if the RHS is a non-empty blob
	if((other.dm_size > 0) && (other.dm_contents != NULL)) {

	    // Copy the RHS blob
	    dm_size = other.dm_size;
	    dm_contents = new char[dm_size];
	    memcpy(dm_contents, other.dm_contents, dm_size);
	
	}

    }

    // Return ourselves to the caller
    return *this;
}



/**
 * Get XDR decoding of contents.
 *
 * Gets an XDR decoding of the blob's contents, placed into a caller-provided
 * data structure. The XDR procedure for the specified data structure's type
 * must also be passed as a parameter.
 *
 * @note    An assertion failure occurs if the XDR decoding of the blob's
 *          contents fails for any reason.
 *
 * @note    It is crucial that the data structure be initialized to zero by
 *          the caller before being passed into this function. In several places
 *          where decoding variable length data, such as in xdr_string(), the
 *          XDR library assumes that if it finds a non-null pointer it can
 *          simply use the memory at that location. If that pointer is a bunch
 *          of uninitialized values, a crash is almost certain. The memory
 *          cannot be zeroed here because we don't know the type (or even the
 *          size) of the data structure.
 *
 * @note    It is the responsibility of the caller to use xdr_free() in order
 *          to free the decoded data structure when it is no longer needed.
 *
 * @param xdrproc    XDR procedure for the returned data type.
 * @retval data      Pointer to the decoded data structure.
 * @return           Decoding size (in bytes).
 */
unsigned Blob::getXDRDecoding(const xdrproc_t xdrproc, void* data) const
{
    // Check assertions
    Assert(xdrproc != NULL);
    Assert(data != NULL);

    // Open an XDR stream using our contents
    XDR xdrs;
    xdrmem_create(&xdrs, reinterpret_cast<char*>(dm_contents),
		  dm_size, XDR_DECODE);
    
    // Decode the data structure from this stream
    Assert((*xdrproc)(&xdrs, data) == TRUE);
    
    // Get the decoding size
    unsigned size = xdr_getpos(&xdrs);
    
    // Close the XDR stream
    xdr_destroy(&xdrs);    
    
    // Return the decoding size to the caller
    return size;
}

/**
 * Get XDR decoding of contents.
 *
 * @note    Same comments apply as for getXDRDecoding.
 *          The exception here is that we relax the xdrproc
 *          assert to allow a caller to decide what to do by
 *          returning 0 for the failed xdrproc. It is required
 *          by the caller to handle the case where 0 is returned.
 *
 * @param xdrproc    XDR procedure for the returned data type.
 * @retval data      Pointer to the decoded data structure.
 * @return           Decoding size (in bytes).
 */
unsigned Blob::getAndVerifyXDRDecoding(const xdrproc_t xdrproc, void* data) const
{
    // Check assertions
    Assert(xdrproc != NULL);
    Assert(data != NULL);

    // Open an XDR stream using our contents
    XDR xdrs;
    xdrmem_create(&xdrs, reinterpret_cast<char*>(dm_contents),
		  dm_size, XDR_DECODE);

    // Decode the data structure from this stream
    // Return 0 if this fails and calling code must decide what to do.
    if((*xdrproc)(&xdrs, data) != TRUE) {
	//std::cerr << "Blob::getXDRDecoding is FALSE" << std::endl;
	return 0;
    }

    // Get the decoding size
    unsigned size = xdr_getpos(&xdrs);

    // Close the XDR stream
    xdr_destroy(&xdrs);

    // Return the decoding size to the caller
    return size;
}



/**
 * Get string encoding of contents.
 *
 * Returns a string encoding of the blob's contents. DPCL only allows passing
 * a restricted subset of types (integers, floats, and strings) as parameters
 * when calling a library function. We get about 95% around this by utilizing
 * blobs containing XDR encodings of arbitrary data structures. These blobs,
 * however, contain 0x00 bytes that prevent them from being directly passed
 * to the library function as a string. This encoding gets us the last 5% of
 * the way.
 *
 * @todo    Currently "base16" encoding is used because it is extremely simple
 *          to implement. It doubles the size of the data, however, making it
 *          a very space-inefficient encoding. A more space-efficient encoding
 *          such as "base64" could be used in the future. An such change must
 *          also be reflected in the runtime's OpenSS_DecodeParameters().
 *
 * @sa    http://www.faqs.org/rfcs/rfc3548.html
 *
 * @return    String encoding of the blob.
 */
std::string Blob::getStringEncoding() const
{
    std::string encoding;

    // Iterate over each byte in the blob
    for(unsigned i = 0; i < dm_size; ++i) {	
	char byte = reinterpret_cast<const char*>(dm_contents)[i];

	// Encode the most-significant four bits
	encoding.append(1, static_cast<char>('0' + (byte >> 4)));

	// Encode the least-significant four bits
	encoding.append(1, static_cast<char>('0' + (byte & 0x0F)));

    }

    // Return the encoding to the caller
    return encoding;
}



/**
 * Test if empty.
 *
 * Returns a boolean value indicating if the blob is empty (has a zero size or
 * null contents).
 *
 * @return    Boolean "true" if the blob is empty, "false" otherwise.
 */
bool Blob::isEmpty() const
{
    return (dm_size == 0) || (dm_contents == NULL);
}
