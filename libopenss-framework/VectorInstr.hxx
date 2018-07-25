////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2018 Krell Institute. All Rights Reserved.
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
 * Declaration of the VectorInstr class.
 *
 */

#ifndef _OpenSpeedShop_Framework_VectorInstr_
#define _OpenSpeedShop_Framework_VectorInstr_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Entry.hxx"
#include <set>

namespace OpenSpeedShop { namespace Framework {

    class Database;
    class Experiment;
    class ExtentGroup;
    class Function;
    class Loop;
    class LoopCache;
    class LinkedObject;
    template <typename> class SmartPtr;
    class Statement;
    class Thread;
    class ThreadGroup;
    
    /**
     * Source code vector instruction augmentation.
     *
     * Representation of a source code statement that is executing vector instructions. 
     * Provides member functions for requesting information about this statement, 
     * where it is located, and what it contains.
     *
     * @ingroup CollectorAPI ToolAPI
     */
    class VectorInstr :
	public Entry
    {
	friend class Experiment;
	friend class Function;
	friend class Loop;
	friend class LoopCache;
	friend class LinkedObject;
	friend class Statement;
	friend class Thread;
	friend class ThreadGroup;
	friend class VectorInstrCache;
        
    public:
	
	std::set<Thread> getThreads() const;
	ExtentGroup getExtentIn(const Thread&) const;
	LinkedObject getLinkedObject() const;

	std::set<Statement> getDefinitions() const;
	int getMaxOperandSize() const;
	std::string getOpCode() const;

	std::set<Function> getFunctions() const;
	std::set<Statement> getStatements() const;
	std::set<Loop> getLoops() const;
                
    private:
        
	static VectorInstrCache TheCache;
        
	VectorInstr();
	VectorInstr(const SmartPtr<Database>&, const int&);

	void getLinkedObjectAndExtent(LinkedObject&, ExtentGroup&) const;
        
    };
        
} }

#endif
