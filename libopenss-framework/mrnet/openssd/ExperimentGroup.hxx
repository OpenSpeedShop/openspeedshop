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
 * Declaration of the ExperimentGroup class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExperimentGroup_
#define _OpenSpeedShop_Framework_ExperimentGroup_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Experiment.hxx"
#include "Protocol.h"
#include "ThreadNameGroup.hxx"

#include <algorithm>
#include <set>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Arbitrary group of experiment names.
     *
     * Container holding an arbitrary group of experiment names. No specific
     * relationship is implied between the experiment names within a given
     * experiment group.
     *
     * @ingroup Implementation
     */
    class ExperimentGroup :
	public std::set<Experiment>
    {

    public:

	/** Default constructor. */
	ExperimentGroup() :
	    std::set<Experiment>()
	{
	}

	/** Copy constructor. */
	ExperimentGroup(const ExperimentGroup& other) :
	    std::set<Experiment>(other)
	{
	}

	/** Constructor from a single Experiment object. */
	ExperimentGroup(const Experiment& experiment) :
	    std::set<Experiment>()
	{
	    insert(experiment);
	}

	/** Constructor from a ThreadNameGroup object. */
	ExperimentGroup(const ThreadNameGroup& threads) :
	    std::set<Experiment>()
	{
	    for(ThreadNameGroup::const_iterator
		    i = threads.begin(); i != threads.end(); ++i)
		insert(Experiment(*i));
	}

	/** Constructor from a OpenSS_Protocol_ExperimentGroup object. */
	ExperimentGroup(const OpenSS_Protocol_ExperimentGroup& object) :
	    std::set<Experiment>()
	{
	    for(int i = 0; i < object.experiments.experiments_len; ++i)
		insert(Experiment(object.experiments.experiments_val[i]));
	}

	/** Type conversion to a OpenSS_Protocol_ExperimentGroup object. */
	operator OpenSS_Protocol_ExperimentGroup() const
	{
	    OpenSS_Protocol_ExperimentGroup object;
	    object.experiments.experiments_len = size();
	    object.experiments.experiments_val = 
		reinterpret_cast<OpenSS_Protocol_Experiment*>(malloc(
		    std::max(static_cast<size_type>(1), size()) *
		    sizeof(OpenSS_Protocol_Experiment)
		    ));
	    int idx = 0;
	    for(ExperimentGroup::const_iterator
		    i = begin(); i != end(); ++i, ++idx)
		object.experiments.experiments_val[idx] = *i;
	    return object;
	}
	
    };

} }



#endif
