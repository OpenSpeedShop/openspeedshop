////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Declaration of the ThreadNameGroup class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ThreadNameGroup_
#define _OpenSpeedShop_Framework_ThreadNameGroup_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"
#include "Protocol.h"
#include "ThreadName.hxx"
#include "Utility.hxx"

#include <algorithm>
#include <BPatch.h>
#include <set>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Arbitrary group of thread names.
     *
     * Container holding an arbitrary group of thread names. No specific
     * relationship is implied between the thread names within a given thread
     * name group.
     *
     * @ingroup Implementation
     */
    class ThreadNameGroup :
	public std::set<ThreadName>
    {

    public:

	/** Default constructor. */
	ThreadNameGroup() :
	    std::set<ThreadName>()
	{
	}

	/** Copy constructor. */
	ThreadNameGroup(const ThreadNameGroup& other) :
	    std::set<ThreadName>(other)
	{
	}

	/** Constructor from a single ThreadName object. */
	ThreadNameGroup(const ThreadName& thread) :
	    std::set<ThreadName>()
	{
	    insert(thread);
	}

	/** Constructor from an experiment and BPatch_process object. */
	ThreadNameGroup(const int& experiment,
			/* const */ BPatch_process& process) :
	    std::set<ThreadName>()
	{
	    BPatch_Vector<BPatch_thread*> threads;
	    process.getThreads(threads);
	    for(int i = 0; i < threads.size(); ++i) {
		Assert(threads[i] != NULL);
		insert(ThreadName(experiment, *(threads[i])));
	    }
	}

	/** Constructor from a OpenSS_Protocol_ThreadNameGroup object. */
	ThreadNameGroup(const OpenSS_Protocol_ThreadNameGroup& object) :
	    std::set<ThreadName>()
	{
	    std::string local_host = getCanonicalName(getLocalHost());
	    for(int i = 0; i < object.names.names_len; ++i)
		if(getCanonicalName(object.names.names_val[i].host) ==
		   local_host)
		    insert(ThreadName(object.names.names_val[i]));
	}

	/** Type conversion to a OpenSS_Protocol_ThreadNameGroup object. */
	operator OpenSS_Protocol_ThreadNameGroup() const
	{
	    OpenSS_Protocol_ThreadNameGroup object;
	    object.names.names_len = size();
	    object.names.names_val =
		reinterpret_cast<OpenSS_Protocol_ThreadName*>(malloc(
                    std::max(static_cast<size_type>(1), size()) *
		    sizeof(OpenSS_Protocol_ThreadName)
		    ));
	    int idx = 0;
	    for(ThreadNameGroup::const_iterator
		    i = begin(); i != end(); ++i, ++idx)
		object.names.names_val[idx] = *i;
	    return object;
	}
	
    };

} }



#endif
