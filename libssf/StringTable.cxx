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
 * Definition of the StringTable class.
 *
 */

#include "Assert.hxx"
#include "StringTable.hxx"

#include <stdexcept>
#include <string.h>

using namespace OpenSpeedShop::Framework;



/**
 * Default constructor.
 *
 * Constructs an empty string table. An initial amount of memory is allocated
 * for the hash table and strings. Those structures are the expanded on-the-fly
 * as needed.
 */
StringTable::StringTable()
{
    // Allocate and zero the hash table
    dm_hashtable.blocking = 1024;
    dm_hashtable.num = dm_hashtable.blocking;
    dm_hashtable.data = new key_type[dm_hashtable.num];
    memset(dm_hashtable.data, 0, dm_hashtable.num * sizeof(key_type));
    dm_hashtable.used = 0;
    
    // Allocate and zero the strings
    dm_strings.blocking = 4096;
    dm_strings.size = dm_strings.blocking;
    dm_strings.data = new uint8_t[dm_strings.size];
    memset(dm_strings.data, 0, dm_strings.size);
    dm_strings.next = 1;
}



/**
 * Constructor from a strings buffer.
 *
 * Member functions getStringsSize() and getStrings() can be used to access and
 * make persistent the strings. That data contains all the necessary information
 * to recreate the entire string table. This constructor performs the recreation
 * by making a local, internal, copy of the strings and then rebuilding the hash
 * table.
 *
 * @param size    Number of bytes in the strings buffer.
 * @param data    Strings buffer.
 */
StringTable::StringTable(const unsigned& size, const uint8_t* data)
{
    // Check assertions
    Assert(size > 0);
    Assert(data != NULL);
    
    // Allocate and zero the hash table
    dm_hashtable.blocking = 1024;
    dm_hashtable.num = dm_hashtable.blocking;
    dm_hashtable.data = new key_type[dm_hashtable.num];
    memset(dm_hashtable.data, 0, dm_hashtable.num * sizeof(key_type));
    dm_hashtable.used = 0;
    
    // Allocate and zero the strings
    dm_strings.blocking = 4096;
    dm_strings.size = size;
    dm_strings.data = new uint8_t[dm_strings.size];
    memset(dm_strings.data, 0, dm_strings.size);
    dm_strings.next = 1;

    // Add each individual string from the strings buffer to the string table
    for(key_type key = 1;
	key < size;
	key += strlen(reinterpret_cast<const char*>(&(data[key]))))
	addString(reinterpret_cast<const char*>(&(data[key])));
}



/**
 * Destructor
 *
 * Destroys the string table by releasing the memory allocated for the hash
 * table and strings.
 */
StringTable::~StringTable()
{
    // Destroy the strings
    delete dm_strings.data;
    
    // Destroy the hash table
    delete dm_hashtable.data;    
}



/**
 * Add a C++ string to the table.
 *
 * Searches for the passed standard C++ string in the string table and returns
 * that string's key. If the string isn't currently in the table it is, of
 * course, added to the table before the key is returned.
 *
 * @todo    Currently we use simple linear probing as our collision resolution
 *          strategy for the hash table. Since it suffers from primary cluster-
 *          ing, something a lot smarter (maybe double hashing?) should be
 *          implemented in the future.
 *
 * @param value    Standard C++ string to be added to table.
 * @return         Key for this string.
 */
StringTable::key_type StringTable::addString(const std::string& value)
{
    // Do a linear probe of the hash table for this string
    uint32_t bucket;
    for(bucket = hashStringUsingFNV(value.c_str()) % dm_hashtable.num;
	dm_hashtable.data[bucket] != 0;
	bucket = (bucket + 1) % dm_hashtable.num)
	
	// Return this key if we found a matching string
	if(getString(dm_hashtable.data[bucket]) == value)
	    return dm_hashtable.data[bucket];
    
    // Advance to the next available key while saving the current one
    key_type key = dm_strings.next;
    dm_strings.next += value.size() + 1;
    
    // Expand the strings when necessary
    if(dm_strings.next > dm_strings.size) {
	
        // Temporarily squirrel away the existing strings
        unsigned size = dm_strings.size;
        uint8_t* data = dm_strings.data;
        
        // Allocate and zero the larger strings
        dm_strings.size += dm_strings.blocking;
        dm_strings.data = new uint8_t[dm_strings.size];
        memset(&(dm_strings.data[size]), 0, dm_strings.blocking);
        
        // Copy the existing data into the new strings
        memcpy(dm_strings.data, data, size);
        
        // Destroy the old strings
        delete [] data;
        
    }
    
    // Copy the string into place
    strcpy(reinterpret_cast<char*>(&(dm_strings.data[key])), value.c_str());
    
    // Insert an entry into the hash table for this string
    dm_hashtable.data[bucket] = key;
    ++dm_hashtable.used;
    
    // Expand the hash table when its load factor exceeds 75%
    if(((100 * dm_hashtable.used) / dm_hashtable.num) > 75) {
	
	// Temporarily squirrel away the existing hash table
        unsigned num = dm_hashtable.num;
        uint32_t* data = dm_hashtable.data;
                                                                                
        // Allocate and zero the larger hash table
        dm_hashtable.num += dm_hashtable.blocking;
        dm_hashtable.data = new key_type[dm_hashtable.num];
        memset(dm_hashtable.data, 0, dm_hashtable.num * sizeof(key_type));
        dm_hashtable.used = 0;
	
	// Rehash the old hash table into the new one
	for(bucket = 0; bucket < num; bucket++)
	    if(data[bucket] > 0) {

		// Do a linear probe of the hash table for an empty entry
		uint32_t new_bucket;
		for(new_bucket = hashStringUsingFNV(
			reinterpret_cast<char*>
			(&(dm_strings.data[data[bucket]]))) % dm_hashtable.num;
		    dm_hashtable.data[new_bucket] != 0;
		    new_bucket = (new_bucket + 1) % dm_hashtable.num);
		
		// Insert an entry into the hash table for this string
		dm_hashtable.data[new_bucket] = data[bucket];
		++dm_hashtable.used;
		
	    }
	
        // Destroy the old hash table
        delete [] data;

    }
    
    // Return the key for this string to the caller
    return key;
}



/**
 * Get the C++ string for a key.
 *
 * Returns to the caller the standard C++ string for the key previously obtained
 * via addString().
 *
 * @param key    Key for the string.
 * @return       Standard C++ string copy of the contents of this string.
 */
std::string StringTable::getString(const key_type& key) const
{
    // Check assertions
    Assert(key > 0);
    Assert(key < dm_strings.next);
    
    // Return the string to the caller
    return std::string(reinterpret_cast<char*>(&(dm_strings.data[key])));
}



/**
 * Calculate the memory usage of this string table.
 *
 * Calculates and returns to the caller the number of bytes used by this string
 * table. Used primarily for testing purposes to determine the efficiency of the
 * implementation.
 *
 * @note    This size is NOT the same as the number of bytes required to write
 *          the string table to disk. This later size can be found by calling
 *          getStringsSize(). The hash table, which requires additional memory,
 *          is constructed only when the table is loaded.
 *
 * @return    Number of bytes used by this string table.
 */
unsigned StringTable::getMemoryUsage() const
{
    // Size of the StringTable object itself
    unsigned usage = sizeof(StringTable);
     
    // Add the size of the hash table
    usage += dm_hashtable.num * sizeof(key_type);
    
    // Add the size of the strings
    usage += dm_strings.size;
    
    // Return the result to the caller
    return usage;
}



/**
 * Hash a standard C string using FNV-1.
 *
 * Hashes a standard C string to a 32-bit unsigned integer using the FNV-1
 * (Fowler/Noll/Vo) hash algorithm. The result is returned to the caller.
 *
 * @sa    http://www.isthe.com/chongo/tech/comp/fnv/
 *
 * @param value    Standard C string to be hashed.
 * @return         Hash value for the string.
 */
uint32_t StringTable::hashStringUsingFNV(const char* value)
{
    const uint32_t FNV_offset_basis = 2166136261U;
    const uint32_t FNV_prime = 16777619U;
    
    // Check assertions
    Assert(value != NULL);
    
    // Apply the FNV-1 hashing algorithm to the string
    uint32_t hash = FNV_offset_basis;
    for(const char* ptr = value; *ptr != '\0'; ++ptr) {
	hash *= FNV_prime;
	hash ^= (uint32_t)(*ptr);
    }
    
    // Return the hash value to the caller
    return hash;
}
