////////////////////////////////////////////////////////////////////////////////
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
 * Definition of the PCSampCollector class.
 *
 */
 
#include "PCSampCollector.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* pcsamp_LTX_CollectorFactory()
{
    return new PCSampCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new PC sampling collector with the proper metadata.
 */
PCSampCollector::PCSampCollector() :
    CollectorImpl("pcsamp",
		  "PC Sampling",
		  "Periodically interupts the running process, obtains the "
		  "current program counter (PC) value, increments a running "
		  "counter associated with that value, and allows the process "
		  "to continue execution.")
{
}
