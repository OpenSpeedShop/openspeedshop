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
 * Declaration of the Collector class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Collector_
#define _OpenSpeedShop_Framework_Collector_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Protocol.h"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Collector name.
     *
     * Names a specific instance of a collector. To uniquely identify a
     * collector, the experiment and collector's unique identifiers must
     * be specified.
     *
     * @note    This class has intentionally been given an identical name to
     *          the similar class in the framework library itself. In the near
     *          future, the experiment database functionality may be split into
     *          a separate library and utilized by the daemon. If this happens,
     *          this class will be removed in favor of the existing framework
     *          class.
     *
     * @ingroup Implementation
     */
    class Collector
    {

    public:

	/** Constructor from a OpenSS_Protocol_Collector object. */
	Collector(const OpenSS_Protocol_Collector& object) :
	    dm_experiment(object.experiment),
	    dm_collector(object.collector)
	{
	}

	/** Type conversion to a OpenSS_Protocol_Collector object. */
	operator OpenSS_Protocol_Collector() const
	{
	    OpenSS_Protocol_Collector object;
	    object.experiment = dm_experiment;
	    object.collector = dm_collector;
	    return object;
	}

	/** Operator "<" defined for two Collector objects. */
	bool operator<(const Collector& other) const
	{
	    if(dm_experiment < other.dm_experiment)
		return true;
	    if(dm_experiment > other.dm_experiment)
		return false;
	    return dm_collector < other.dm_collector;
	}

	/** Read-only data member accessor function. */
	const int& getExperiment() const
	{
	    return dm_experiment;
	}

	/** Read-only data member accessor function. */
	const int& getCollector() const
	{
	    return dm_collector;
	}

    private:

	/** Unique identifier for the experiment containing this collector. */
	int dm_experiment;

	/** Identifier for this collector within that experiment. */
	int dm_collector;
	
    };
    
} }



#endif
