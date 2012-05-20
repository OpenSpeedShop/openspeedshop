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
#ifndef _OpenSpeedShop_Framework_PCBuffer_
#define _OpenSpeedShop_Framework_PCBuffer_

#ifdef HAVE_CONFIG_H
//#include "config.h"
#endif

#define UINT8_MAX (255)
#define PCBufferSize 1024*1024

/** Number of entries in the hash table. */
#define PCHashTableSize (PCBufferSize + (PCBufferSize / 4))

/** Type representing PC sampling data (PCs and their respective counts). */
typedef struct {

    uint16_t length;  /**< Actual used length of the PC and count arrays. */

    uint64_t pc[PCBufferSize];    /**< Program counter (PC) addresses. */

    /** Hash table mapping PC addresses to their array index. */
    unsigned hash_table[PCHashTableSize];

} PCBuffer;

bool UpdatePCBuffer(uint64_t, PCBuffer*);
#endif
