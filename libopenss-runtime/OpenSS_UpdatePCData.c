/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the OpenSS_UpdatePCData() function.
 *
 */

#include "RuntimeAPI.h"



/**
 * Update PC data.
 *
 * Updates the specified program counter (PC) sampling data buffer with the
 * passed PC address.
 *
 * @note    Multiple samples at the same PC address within a given buffer are
 *          indicated by incrementing a sample count rather than repeating the
 *          PC address. This helps compress the data in the common case where
 *          a degree of spatial locality is present. A hash table is used to
 *          accelerate the determination of whether an addresss is already in 
 *          the buffer. This concept is losely based on the technique employed
 *          by Digital/Compaq/HP's DCPI.
 *
 * @sa    http://h30097.www3.hp.com/dcpi/src-tn-1997-016a.html
 *
 * @param pc        PC address to be added.
 * @param buffer    PC sampling data buffer to be updated.
 * @return          Boolean "true" if the buffer is now full, "false" otherwise.
 *
 * @ingroup RuntimeAPI
 */
bool_t OpenSS_UpdatePCData(uint64_t pc, OpenSS_PCData* buffer)
{
    unsigned bucket, entry;

    /*
     * Search the sample buffer for an existing entry corresponding to this
     * PC address. Use the hash table and a simple linear probe to accelerate
     * the search.
     */
    bucket = (pc >> 4) % OpenSS_PCHashTableSize;
    while((buffer->hash_table[bucket] > 0) &&
          (buffer->pc[buffer->hash_table[bucket] - 1] != pc))
        bucket = (bucket + 1) % OpenSS_PCHashTableSize;

    /* Increment count for existing entry if found and not already maxed */
    if((buffer->hash_table[bucket] > 0) &&
       (buffer->pc[buffer->hash_table[bucket] - 1] == pc) &&
       (buffer->count[buffer->hash_table[bucket] - 1] < UINT8_MAX)) {
        buffer->count[buffer->hash_table[bucket] - 1]++;
	return FALSE;
    }

    /* Otherwise add a new entry for this PC address to the sample buffer */
    entry = buffer->length;
    buffer->pc[entry] = pc;
    buffer->count[entry] = 1;
    buffer->length++;
    
    /* Update the address interval in the sample buffer */
    if(pc < buffer->addr_begin)
        buffer->addr_begin = pc;
    if(pc > buffer->addr_end)
        buffer->addr_end = pc;
    
    /* Update the hash table with this new entry */
    buffer->hash_table[bucket] = entry + 1;
    
    /* Indicate to the caller if the sample buffer is full */
    return (buffer->length == OpenSS_PCBufferSize);
}
