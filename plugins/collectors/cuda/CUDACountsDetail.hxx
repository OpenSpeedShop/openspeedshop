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

#include <ArgoNavis/Base/Time.hpp>

#include "Assert.hxx"
#include "TotallyOrdered.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA hardware performance counter details.
     *
     * Encapsulate the detail metric (exclusive only) for hardware performance
     * counter data recorded by the CUDA collector.
     */
    class CUDACountsDetail :
        public TotallyOrdered<CUDACountsDetail>
    {

    public:

        /** Default constructor. */
        CUDACountsDetail() : dm_time(), dm_counts() { }
       
        /** Constructor from event names and counts. */
        CUDACountsDetail(const ArgoNavis::Base::Time& time,
                         const std::vector<boost::uint64_t>& counts) :
            dm_time(time),
            dm_counts(counts)
        {
        }
        
        /** Operator "<" defined for two CUDACountsDetail objects. */
        bool operator<(const CUDACountsDetail& other) const
        {
            return dm_time < other.dm_time;
        }

        /** Time at which this sample was taken. */
        const ArgoNavis::Base::Time& getTime() const
        {
            return dm_time;
        }

        /** Counts for all events. */
        const std::vector<boost::uint64_t>& getCounts() const
        {
            return dm_counts;
        }
        
    private:

        /** Time at which this sample was taken. */
        ArgoNavis::Base::Time dm_time;
        
        /** Counts for all events. */
        std::vector<boost::uint64_t> dm_counts;
        
    }; // class CUDACountsDetail
        
} } // namespace OpenSpeedShop::Framework
