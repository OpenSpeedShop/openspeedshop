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
 * Declaration of the Experiment class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Experiment_
#define _OpenSpeedShop_Framework_Experiment_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Collector.hxx"
#include "Protocol.h"
#include "ThreadName.hxx"
#include "TotallyOrdered.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Experiment name.
     *
     * Names a specific experiment. To uniquely identify an experiment, only
     * the unique identifier of the experiment itself must be specified.
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
    class Experiment :
	public TotallyOrdered<Experiment>
    {

    public:

	/** Constructor from a Collector object. */
	Experiment(const Collector& collector) :
	    dm_experiment(collector.getExperiment())
	{
	}

	/** Constructor from a ThreadName object. */
	Experiment(const ThreadName& thread) :
	    dm_experiment(thread.getExperiment())
	{
	}

	/** Constructor from a OpenSS_Protocol_Experiment object. */
	Experiment(const OpenSS_Protocol_Experiment& object) :
	    dm_experiment(object.experiment)
	{
	}

	/** Type conversion to a OpenSS_Protocol_Experiment object. */
	operator OpenSS_Protocol_Experiment() const
	{
	    OpenSS_Protocol_Experiment object;
	    object.experiment = dm_experiment;
	    return object;
	}

	/** Operator "<" defined for two Experiment objects. */
	bool operator<(const Experiment& other) const
	{
	    return dm_experiment < other.dm_experiment;
	}

	/** Read-only data member accessor function. */
	const int& getExperiment() const
	{
	    return dm_experiment;
	}

    private:

	/** Unique identifier for this experiment. */
	int dm_experiment;
	
    };
    
} }



#endif
