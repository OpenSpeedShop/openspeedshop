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
 * Definition of the AddressBitmap class.
 *
 */

#include "AddressBitmap.hxx"
#include "Blob.hxx"

#include <string.h>

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from address range.
 *
 * Constructs a new address bitmap covering the specified address range. Each
 * entry in the bitmap is initially set to "false".
 *
 * @param range    Address range covered by this bitmap.
 */
AddressBitmap::AddressBitmap(const AddressRange& range) :
    dm_range(range),
    dm_bitmap(range.getWidth(), false)
{
}



/**
 * Constructor from address range and blob.
 *
 * Constructs a new address bitmap covering the specified range and with its
 * contents specified by a blob.
 *
 * @param range    Address range covered by this bitmap.
 * @param blob     Blob containing the bitmap's contents.
 */
AddressBitmap::AddressBitmap(const AddressRange& range, const Blob& blob) :
    dm_range(range),
    dm_bitmap(range.getWidth(), false)
{
    // Check assertions
    Assert(blob.getSize() >= (dm_range.getWidth() / 8));

    // Transfer bitmap from the blob's contents
    const char* contents = reinterpret_cast<const char*>(blob.getContents());
    for(unsigned i = 0; i < dm_range.getWidth(); ++i)
	dm_bitmap[i] = contents[i / 8] & (1 << (i % 8));
}



/**
 * Set a value.
 *
 * Sets the value in this bitmap corresponding to the specified address.
 *
 * @param address    Address to be set.
 * @param value      Value to set for this address.
 */
void AddressBitmap::setValue(const Address& address, const bool& value)
{
    // Check assertions
    Assert(dm_range.doesContain(address));

    // Set the value
    dm_bitmap[address - dm_range.getBegin()] = value;
}



/**
 * Get a value.
 *
 * Returns the value in this bitmap corresponding to the specified address.
 *
 * @param address    Address to get.
 * @return           Value at that address.
 */
bool AddressBitmap::getValue(const Address& address) const
{
    // Check assertions
    Assert(dm_range.doesContain(address));

    // Return the value to the caller
    return dm_bitmap[address - dm_range.getBegin()];
}



/**
 * Get bitmap as a blob.
 *
 * Returns a blob containing the bitmaps as its contents.
 * 
 * @return    Blob containing this bitmap.
 */
Blob AddressBitmap::getBlob() const
{
    // Calculate the size of the blob to cover this address range
    unsigned size = ((dm_range.getWidth() - 1) / 8) + 1;

    // Allocate and zero the contents of the blob
    char* contents = new char[size];
    memset(contents, 0, size);

    // Transfer bitmap into the blob's contents
    for(unsigned i = 0; i < dm_range.getWidth(); ++i)
	if(dm_bitmap[i])
	    contents[i / 8] |= 1 << (i % 8);

    // Create the blob
    Blob blob(size, contents);

    // Destroy the contents (the blob keeps a private copy)
    delete [] contents;

    // Return the blob to the caller
    return blob;
}
