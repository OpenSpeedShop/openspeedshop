////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Declaration of the GlobalTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_GlobalTable_
#define _OpenSpeedShop_Framework_GlobalTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "DataBucket.hxx"
#include "Guard.hxx"
#include "Lockable.hxx"
#include "Thread.hxx"

#include <map>
#include <string>
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Global table.
     *
     * Table used to keep track of the incoming values for global variables.
     * Built on top of the DataBucket class, this table provides the means for
     * a thread to wait until the value of a global variable has been obtained,
     * and the means by which another thread can provide that value.
     *
     * @ingroup Implementation
     */
    class GlobalTable :
	private Lockable,
	private std::multimap<std::pair<Thread, std::string>,
	                      std::pair<bool, void*> >
    {

    public:

	static GlobalTable TheTable;

	GlobalTable();

	template <typename T>
	bool waitForValue(const Thread&, const std::string&, T&);

	template <typename T>
	void provideValue(const Thread&, const std::string&,
			  const std::pair<bool, T>&);
	
    };



    /**
     * Wait for a value.
     *
     * Wait for the value of the specified global variable. Creates a new data
     * bucket for retrieving the value, adds it to this table, and waits for the
     * value to arrive. Once it has arrived, the data bucket is destroyed and
     * the value is returned.
     *
     * @param thread    Thread from which the global variable value is being
     *                  retrieved.
     * @param global    Name of the global variable whose value is being
     *                  retrieved.
     * @param value     Current value of that variable.
     * @return          Boolean "true" if the variable's value was successfully
     *                  retrieved, "false" otherwise.
     */
    template <typename T>
    bool GlobalTable::waitForValue(const Thread& thread, 
				   const std::string& global,
				   T& value)
    {
	Guard guard_myself(this);

	// Create a new data bucket for this value and add it to this table
	DataBucket<T>* bucket = new DataBucket<T>();
	std::multimap<std::pair<Thread, std::string>,
	              std::pair<bool, void*> >::iterator i = 
	    insert(std::make_pair(std::make_pair(thread, global),
				  std::make_pair(false, bucket)));

	// Wait until the incoming value arrives in the data bucket
	releaseLock();
	value = bucket->getValue();
	bool retval = i->second.first;
	acquireLock();
	
	// Remove the data bucket from this table and destroy it
	erase(i);
	delete bucket;
	
	// Return the value to the caller
	return retval;
    }



    /**
     * Provide a value.
     *
     * Provides the value of the specifed global variable. Finds the data
     * bucket for retrieving the value and sets it to the provided value.
     *
     * @note    An assertion failure occurs if an attempt is made to provide
     *          a value for a global variable for which nobody is waiting.
     *
     * @param thread    Thread containing the global variable being retrieved.
     * @param global    Global variable being retrieved.
     * @param value     Pair containing the current value of that variable.
     */
    template <typename T>
    void GlobalTable::provideValue(const Thread& thread,
				   const std::string& global,
				   const std::pair<bool, T>& value)
    {
	Guard guard_myself(this);
	
	// Find the data bucket for this value
	std::multimap<std::pair<Thread, std::string>,
	              std::pair<bool, void*> >::iterator i = 
	    find(std::make_pair(thread, global));
	Assert(i != end());
	DataBucket<T>* bucket = 
	    reinterpret_cast<DataBucket<T>*>(i->second.second);
	Assert(bucket != NULL);
	
	// Set the value of this global's data bucket
	i->second.first = value.first;
	bucket->setValue(value.second);
    }
    


} }



#endif
