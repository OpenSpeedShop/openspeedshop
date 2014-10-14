////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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

/** @file Declaration of the CUDACollector class. */

#pragma once
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/function.hpp>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "KrellInstitute/Messages/CUDA_data.h"

#include "Address.hxx"
#include "Blob.hxx"
#include "Collector.hxx"
#include "CollectorImpl.hxx"
#include "Collector.hxx"
#include "Extent.hxx"
#include "ExtentGroup.hxx"
#include "PCBuffer.hxx"
#include "SmartPtr.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

#include "CUDADeviceDetail.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA collector.
     *
     * Intercepts all calls to CUDA memory copy/set and kernel executions and
     * records, for each call, the current stack trace and start/end time, as
     * well as additional relevant information depending on the operation. In
     * addition, has the ability to periodically sample hardware event counts
     * via PAPI for both the CPU and GPU.
     */
    class CUDACollector :
        public CollectorImpl
    {
        
    public:

        CUDACollector();    
        
        virtual Blob getDefaultParameterValues() const;
        virtual void getParameterValue(const std::string&,
                                       const Blob&, void*) const;
        virtual void setParameterValue(const std::string&,
                                       const void*, Blob&) const;
        
        virtual void startCollecting(const Collector&,
                                     const ThreadGroup&) const;
        virtual void stopCollecting(const Collector&,
                                    const ThreadGroup&) const;
        
        virtual void getMetricValues(const std::string&,
                                     const Collector&, const Thread&,
                                     const Extent&, const Blob&, 
                                     const ExtentGroup&, void*) const;
        
        virtual void getUniquePCValues(const Thread&, const Blob&,
                                       PCBuffer*) const;
        
        virtual void getUniquePCValues(const Thread&, const Blob&,
                                       std::set<Address>&) const;

    private:

        /** Type of function invoked to process a completed kernel execution. */
        typedef boost::function<
            void (const CUDA_EnqueueRequest&,
                  const CUDA_ExecutedKernel&,
                  const std::vector<Address>&,
                  const SmartPtr<CUDADeviceDetail>&)
            > ExecutedKernelVisitor;

        /** Type of function invoked to process a completed memory copy. */
        typedef boost::function<
            void (const CUDA_EnqueueRequest&,
                  const CUDA_CopiedMemory&,
                  const std::vector<Address>&,
                  const SmartPtr<CUDADeviceDetail>&)
            > MemoryCopyVisitor;

        /** Type of function invoked to process a completed memory set. */
        typedef boost::function<
            void (const CUDA_EnqueueRequest&,
                  const CUDA_SetMemory&,
                  const std::vector<Address>&,
                  const SmartPtr<CUDADeviceDetail>&)
            > MemorySetVisitor;

        /**
         * Plain old data (POD) structure describing a single pending request.
         *
         * @sa http://en.wikipedia.org/wiki/Plain_old_data_structure
         */
        struct Request
        {
            /** Original CUDA message describing the enqueued request. */
            CUDA_EnqueueRequest message;
            
            /** Call site of the enqueued request. */
            std::vector<Address> call_site;
        };
        
        /**
         * Plain old data (POD) structure holding thread-specific data.
         *
         * @sa http://en.wikipedia.org/wiki/Plain_old_data_structure
         */
        struct ThreadSpecificData
        {
            /** Set of previously-seen extents. */
            std::set<Extent> extents;

            /** Table of pending requests. */
            std::list<Request> requests;
        };
        
        void handleRequests(const CBTF_cuda_data&,
                            const CBTF_cuda_message&,
                            ThreadSpecificData&,
                            ExecutedKernelVisitor&,
                            MemoryCopyVisitor&,
                            MemorySetVisitor&) const;
        
        SmartPtr<CUDADeviceDetail> getDeviceDetail(const Address&) const;
        
        /** Device for all known contexts. */
        mutable std::map<Address, unsigned int> dm_contexts;
        
        /** Device details for each known device. */
        mutable std::map<unsigned int, SmartPtr<CUDADeviceDetail> > dm_devices;
        
        /** Thread-specific data for all known threads. */
        mutable std::map<Thread, ThreadSpecificData> dm_threads;
        
    }; // class CUDACollector
        
} } // namespace OpenSpeedShop::Framework
