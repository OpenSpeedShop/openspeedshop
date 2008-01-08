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
 * Declaration of the DataQueues namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_DataQueues_
#define _OpenSpeedShop_Framework_DataQueues_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Database;
    class DataCache;
    template <typename> class SmartPtr;

    /**
     * Performance data queues.
     *
     * Namespace containing the procedural functions to write performance data
     * to the appropriate experiment database. Locking overhead results in very
     * low efficiency when writing each blob of performance data to the database
     * individually. Performance testing indicated write rates that were as low
     * as 10 blobs/second when each blob was stored within its own transaction.
     * Aggregating multiple blobs into a single transaction yielded write rates
     * as high as 2500 blobs/second. As a result, this namespace contains the
     * necessary code to queue up incoming performance data so that it can be
     * aggregated into larger transactions, significantly improving performance.
     *
     * @note    When sending a blob of performance data to the tool, collector
     *          runtimes need to specify into which experiment the data should
     *          be stored. The only unique identifier an Experiment normally has
     *          is the name of its experiment database. But we don't want to be
     *          shuttling a full database name across the network every time we
     *          send performance data. In order to provide an efficient id, this
     *          namespace maintains a mapping between unique integers and every
     *          Experiment that has been created.
     *
     * @ingroup Implementation
     */
    namespace DataQueues
    {

	extern DataCache TheCache;

	void addDatabase(const SmartPtr<Database>&);
	void removeDatabase(const SmartPtr<Database>&);
	SmartPtr<Database> getDatabase(const int&);
	int getDatabaseIdentifier(const SmartPtr<Database>&);
	void flushDatabase(const SmartPtr<Database>&);
	
	void enqueuePerformanceData(const Blob&);
	void flushPerformanceData();

    }

} }



#endif
