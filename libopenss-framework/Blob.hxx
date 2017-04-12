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
 * Declaration of the Blob class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Blob_
#define _OpenSpeedShop_Framework_Blob_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/rpc.h>
#include <string>



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Binary large object.
     *
     * Encapsulates a buffer of raw, untyped, binary data. Relational database
     * management systems typically use such "blobs" to store data which has a
     * unknown structure. Mechanisms are also provided here for performing XDR
     * encoding/decoding of typed data structures to/from such blobs.
     *
     * @sa    http://www.hyperdictionary.com/computing/binary+large+object
     *
     * @ingroup Utility
     */
    class Blob
    {

    public:

	Blob();
	Blob(const Blob&);
	Blob(const unsigned&, const void*);
	Blob(const xdrproc_t, const void*);	
	~Blob();
	
	Blob& operator=(const Blob&);
	
	/** Read-only data member accessor function. */
	const unsigned& getSize() const
	{
	    return dm_size;
	}

	/** Read-only data member accessor function. */
	const void* getContents() const
	{
	    return dm_contents;
	}

	unsigned getXDRDecoding(const xdrproc_t, void*) const;
	unsigned getAndVerifyXDRDecoding(const xdrproc_t, void*) const;
	std::string getStringEncoding() const;

	bool isEmpty() const;

    private:

	/** Size of the blob (in bytes). */
	unsigned dm_size;

	/** Pointer to the blob's contents. */
	void* dm_contents;
	
    };
    
} }



#endif
