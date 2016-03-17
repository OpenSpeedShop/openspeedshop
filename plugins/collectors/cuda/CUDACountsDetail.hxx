////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014-2016 Argo Navis Technologies. All Rights Reserved.
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

/** @file Declaration and definition of the CUDACountsDetail class. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/cstdint.hpp>
#include <string>
#include <vector>

#include "Assert.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA hardware performance counter details.
     *
     * Encapsulate the detail metric (exclusive only) for hardware performance
     * counter data recorded by the CUDA collector.
     */
    class CUDACountsDetail
    {

    public:

        /** Default constructor. */
        CUDACountsDetail() : dm_event_names(), dm_event_counts() { }
       
        /** Constructor from event names and counts. */
        CUDACountsDetail(const std::vector<std::string>& event_names,
                         const std::vector<boost::uint64_t>& event_counts) :
            dm_event_names(event_names),
            dm_event_counts(event_counts)
        {
        }

        /** Number of sampled events. */
        unsigned int getEventCount() const
        {
            Assert(dm_event_names.size() == dm_event_counts.size());
            return dm_event_names.size();
        }
        
        /** Name of the specified event. */
        std::string getEventName(unsigned int event) const
        {
            Assert(event < dm_event_names.size());
            return dm_event_names[event];
        }
        
        /** Count for the specified event. */
        boost::uint64_t getEventCount(unsigned int event) const
        {
            Assert(event < dm_event_counts.size());
            return dm_event_counts[event];
        }
        
    private:
        
        /** Name of all events. */
        std::vector<std::string> dm_event_names;
        
        /** Counts for all events. */
        std::vector<boost::uint64_t> dm_event_counts;
        
    }; // class CUDACountsDetail
        
} } // namespace OpenSpeedShop::Framework
