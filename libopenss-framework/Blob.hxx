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



namespace OpenSpeedShop { namespace Framework {
    
    /**
     * Binary large object.
     *
     * ...
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

    private:

	/** Size of the blob (in bytes). */
	unsigned dm_size;

	/** Pointer to the blob's contents. */
	void* dm_contents;
	
    };
    
} }



#endif
