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

#include "Blob.hxx"

#include <string.h>

using namespace OpenSpeedShop::Framework;



/**
 * Default constructor.
 *
 * Constructs an empty Blob whose size is zero and contents is NULL.
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
    dm_size(other.dm_size),
    dm_contents(NULL)
{
    dm_contents = new char[dm_size];
    memcpy(dm_contents, other.dm_contents, dm_size);
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
    dm_size(size),
    dm_contents(NULL)
{
    dm_contents = new char[dm_size];
    memcpy(dm_contents, contents, dm_size);
}



/**
 * Destructor.
 *
 * Destroys the blob's contents if it had any.
 */
Blob::~Blob()
{    
    // Destroy our contents (if any)
    delete reinterpret_cast<char*>(dm_contents);
}



/**
 * Assignment operator.
 *
 * Operator "=" defined for a Blob object. The compiler provided default is
 * insufficient here because we need a deep, rather than shallow, copy of the
 * contents for correct object destruction.
 */
Blob& Blob::operator=(const Blob& other)
{
    // Only do an assignment if the LHS and RHS differ
    if((dm_size != other.dm_size) || (dm_contents != other.dm_contents)) {
	
	// Destroy our current contents (if any)
	delete reinterpret_cast<char*>(dm_contents);
	
	// Replace with the new blob
	dm_size = other.dm_size;
	dm_contents = new char[dm_size];
	memcpy(dm_contents, other.dm_contents, dm_size);

    }

    // Return ourselves to the caller
    return *this;
}
