////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration and definition of the StringTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_StringTable_
#define _OpenSpeedShop_Framework_StringTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * String table.
     *
     * Container for an arbitrary set of strings that each can be refered to by
     * an unique key value. Member functions are provided for adding a string to
     * the table and getting a string based on its key. Used to insure that the
     * text of strings that are refered to repetitively are only stored once.
     * Also facilitates quick equality comparison by simply comparing keys.
     *
     * @ingroup Utility
     */
    class StringTable
    {
	
    public:
	
	/** Type representing a string key. */
	typedef uint32_t key_type;	
	
	StringTable();
	StringTable(const unsigned&, const uint8_t*);
	~StringTable();
	
	key_type addString(const std::string&);
	std::string getString(const key_type&) const;
	
	/** Read-only data member accessor function. */
	unsigned getStringsSize() const
	{
	    return dm_strings.next;
	}
	
	/** Read-only data member accessor function. */
	const uint8_t* getStrings() const
	{ 
	    return dm_strings.data;
	}

	unsigned getMemoryUsage() const;

    private:

	static uint32_t hashStringUsingFNV(const char* value);
	
	/** Hash table. */
	struct {
	    unsigned blocking;    /**< Number of entries per allocation. */
	    unsigned num;         /**< Number of allocated entries. */
	    key_type* data;       /**< Actual hash table. */
	    unsigned used;        /**< Number of used entries. */
	} dm_hashtable;
	
	/** Strings. */
	struct {
	    unsigned blocking;    /**< Number of bytes per allocation. */
	    unsigned size;        /**< Number of allocated bytes. */
	    uint8_t* data;        /**< Actual strings. */
	    key_type next;        /**< Next available key. */
	} dm_strings;
	
    };
    
} }



#endif
