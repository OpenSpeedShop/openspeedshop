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

/** @file Declaration and definition of the CUDAXferRate structure. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/cstdint.hpp>

namespace OpenSpeedShop { namespace Queries {

    /**
     * Structure containing metrics for evaluating the CUDA data transfer rate.
     * The data transfer rate is calculated simply as (size / time).
     */
    struct CUDAXferRate
    {
        /** Size (in bytes) of the CUDA data transfers. */
        boost::uint64_t size;
        
        /** Time (in seconds) spent in CUDA data transfers. */
        double time;
        
    }; // struct CUDAXferRate

} } // namespace OpenSpeedShop::Queries
