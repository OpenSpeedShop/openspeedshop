///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration of the ExperimentTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ExperimentTable_
#define _OpenSpeedShop_Framework_ExperimentTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"
#include "SmartPtr.hxx"

#include <map>



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Collector;
    class Database;
    class Experiment;
    class Thread;

    /**
     * Experiment table.
     *
     * When sending a blob of performance data to the tool, a collector runtime
     * needs to specify into which experiment the data should be stored. The
     * only unique identifier an Experiment normally has is the name of its
     * experiment database. But we don't want to be shuttling a full database
     * name across the network every time we send performance data. In order to
     * provide an efficient identifier, this class maintains a mapping between
     * unique integers and every Experiment that has been created. Instead of
     * providing direct access to this mapping, this class provides the wrapping
     * functionality around that mapping to get an experiment's identifier and
     * to store a blob of performance data.
     *
     * @ingroup Implementation
     */
    class ExperimentTable :
	private Lockable
    {

    public:

	static ExperimentTable TheTable;

	void addExperiment(const Experiment*);
	void removeExperiment(const Experiment*);

	int getIdentifier(const SmartPtr<Database>&) const;
	void storePerformanceData(const Blob&) const;
	
    private:
	
	/** Next available identifier value. */
	int dm_next_identifier;
	
	/** Map identifiers to their experiment. */
	std::map<int, const Experiment*> dm_identifier_to_experiment;

	/** Map experiments to their identifier. */
	std::map<const Experiment*, int> dm_experiment_to_identifier;
	
    };

} }



#endif
