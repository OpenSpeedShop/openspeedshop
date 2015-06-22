////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2012 The Krell Institute. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Declaration of the Callbacks namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Callbacks_
#define _OpenSpeedShop_Framework_Callbacks_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <set>
#include <boost/shared_ptr.hpp>
#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/OfflineEvents.h"
#include "KrellInstitute/Messages/PCSamp_data.h"
#include "KrellInstitute/Messages/Stats.h"
#include "KrellInstitute/Messages/Symbol.h"
#include "KrellInstitute/Messages/Time.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "Address.hxx"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"



namespace OpenSpeedShop { namespace Framework {

    /**
     * Message callbacks.
     *
     * Namespace containing the message callback functions. These callbacks
     * are registered with the frontend message pump by the instrumentor and
     * will handle all incoming requests from the backends.
     *
     * @ingroup Implementation
     */
    namespace Callbacks
    {
	void attachedToThreads(const boost::shared_ptr<CBTF_Protocol_AttachedToThreads> &);
	void addressBuffer(const KrellInstitute::Core::AddressBuffer&);
	void createdProcess(const boost::shared_ptr<CBTF_Protocol_CreatedProcess> &);
	void performanceData(const boost::shared_ptr<CBTF_Protocol_Blob> & );
	void loadedLinkedObject(const boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject> & );
	void linkedObjectGroup(const boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup> & );
	void linkedObjectEntryVec(const KrellInstitute::Core::LinkedObjectEntryVec& );
	void symbolTable(const boost::shared_ptr<CBTF_Protocol_SymbolTable> & in);
	void maxFunctionValues(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> & in);
	void minFunctionValues(const boost::shared_ptr<CBTF_Protocol_FunctionThreadValues> & in);
	void avgFunctionValues(const boost::shared_ptr<CBTF_Protocol_FunctionAvgValues> & in);
	void process_addressspace(const std::set<Address> addresses);
	void finalize();
    }
    
} }



#endif
