////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
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
 * Declaration of the LinkedObject class.
 *
 */

#ifndef _OpenSpeedShop_Framework_LinkedObject_
#define _OpenSpeedShop_Framework_LinkedObject_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Entry.hxx"
#include "AddressRange.hxx"

#include <set>



namespace OpenSpeedShop { namespace Framework {

    class AddressSpace;
    class Database;
    class Experiment;
    class ExtentGroup;
    class Function;
    class FunctionCache;
    class Loop;
    class LoopCache;
    class Path;
    template <typename> class SmartPtr;
    class Statement;
    class StatementCache;
    class Thread;
    class ThreadGroup;
    
    /**
     * Linked object.
     *
     * Representation of a single executable or library (a "linked object").
     * Provides member functions for requesting information about this linked
     * object, where it is contained, and what it contains.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class LinkedObject :
        public Entry
    {
        friend class AddressSpace;
        friend class Experiment;
        friend class Function;
        friend class FunctionCache;
        friend class Loop;
        friend class LoopCache;
        friend class Statement;
        friend class StatementCache;
        friend class Thread;
        friend class ThreadGroup;
	
    public:

        std::set<Thread> getThreads() const;
        ExtentGroup getExtentIn(const Thread&) const;
	
        Path getPath() const;
        bool isExecutable() const;
	
        std::set<Function> getFunctions() const;
        std::set<Loop> getLoops() const;
        std::set<Statement> getStatements() const;

        std::set<AddressRange> getAddressRange() const;
        
    private:

        LinkedObject();
        LinkedObject(const SmartPtr<Database>&, const int&);
        
    };
    
} }



#endif
