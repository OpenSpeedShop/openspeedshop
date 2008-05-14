////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 The Krell Institue. All Rights Reserved.
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
 * Definition of the PCHash functions.
 *
 */

#include "ToolAPI.hxx"
#include "PCBuffer.hxx"

//#define UINT8_MAX (255)

bool UpdatePCBuffer(uint64_t pc, PCBuffer* buffer)
{
    unsigned bucket, entry;

    /*
     * Search the sample buffer for an existing entry corresponding to this
     * PC address. Use the hash table and a simple linear probe to accelerate
     * the search.
     */
    bucket = (pc >> 4) % PCHashTableSize;
    while((buffer->hash_table[bucket] > 0) &&
          (buffer->pc[buffer->hash_table[bucket] - 1] != pc))
        bucket = (bucket + 1) % PCHashTableSize;

    /* Return if entry found */
    if((buffer->hash_table[bucket] > 0) &&
       (buffer->pc[buffer->hash_table[bucket] - 1] == pc) ) {
        return FALSE;
    }

    /* Otherwise add a new entry for this PC address to the sample buffer */
    entry = buffer->length;
    buffer->pc[entry] = pc;
    buffer->length++;

    /* Update the hash table with this new entry */
    buffer->hash_table[bucket] = entry + 1;

    /* Indicate to the caller if the sample buffer is full */
    return (buffer->length == PCBufferSize);
}
