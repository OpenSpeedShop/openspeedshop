////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 Krell Institute All Rights Reserved.
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
 * Declaration and definition of the MPIOTFDetail structure.
 *
 */

#ifndef _MPIOTFDetail_
#define _MPIOTFDetail_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TotallyOrdered.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * MPI extended event tracing collector details. 
     *
     * Encapsulate the details metric (inclusive or exclusive) for the MPI
     * extended event tracing collector.
     */
    struct MPIOTFDetail :
	public TotallyOrdered<MPIOTFDetail>
    {
	TimeInterval dm_interval;  /**< Begin/End time of the call. */
	double dm_time;            /**< Time spent in the call. */	

        int dm_source;             /**< Source rank (in MPI_COMM_WORLD). */
        int dm_destination;        /**< Destination rank (in MPI_COMM_WORLD). */
        uint64_t dm_size;          /**< Number of bytes sent. */
        int dm_tag;                /**< Tag of the message (if any). */
        int dm_communicator;       /**< Communicator used. */
        int dm_datatype;           /**< Data type of the message. */
        int dm_retval;             /**< Enumerated return value. */

	/** Operator "<" defined for two MPIOTFDetail objects. */
	bool operator<(const MPIOTFDetail& other) const
	{
	    if(dm_interval < other.dm_interval)
                return true;
            if(dm_interval > other.dm_interval)
                return false;
	    if(dm_time < other.dm_time)
                return true;
            if(dm_time > other.dm_time)
                return false;
	    if(dm_source < other.dm_source)
                return true;
            if(dm_source > other.dm_source)
                return false;
	    if(dm_destination < other.dm_destination)
                return true;
            if(dm_destination > other.dm_destination)
                return false;
	    if(dm_size < other.dm_size)
                return true;
            if(dm_size > other.dm_size)
                return false;
	    if(dm_tag < other.dm_tag)
                return true;
            if(dm_tag > other.dm_tag)
                return false;
	    if(dm_communicator < other.dm_communicator)
                return true;
            if(dm_communicator > other.dm_communicator)
                return false;
	    if(dm_datatype < other.dm_datatype)
                return true;
            if(dm_datatype > other.dm_datatype)
                return false;
	    return dm_retval < other.dm_retval;
        }

    };
    
} }



#endif
