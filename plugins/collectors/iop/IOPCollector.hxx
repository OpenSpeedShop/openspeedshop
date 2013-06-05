////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 The Krell Institute. All Rights Reserved.
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
 * Declaration of the IOPCollector class.
 *
 */

#ifndef _IOPCollector_
#define _IOPCollector_
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CollectorAPI.hxx"



namespace OpenSpeedShop { namespace Framework {

    /**
     * IOP profiling collector.
     *
     * Intercepts all calls to IOP functions that perform any significant amount
     * of work and records, for each call, the current stack trace,
     * and time spent in the call.
     *
     */
    class IOPCollector :
	public CollectorImpl
    {
	
    public:
	
	IOPCollector();    

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
	
	virtual void getUniquePCValues( const Thread& thread,
					const Blob& blob,
					PCBuffer *buf) const;

	virtual void getUniquePCValues( const Thread& thread, const Blob& blob,
                                        std::set<Address>& uaddresses) const;
    };
    
} }



#endif
