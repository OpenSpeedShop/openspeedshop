////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2008-2009 Krell Institute. All Rights Reserved.
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
 * Definition of the WatcherThreadTable class.
 *
 */

#include "Guard.hxx"
#include "WatcherThreadTable.hxx"

using namespace OpenSpeedShop::Framework;

/** Singleton thread table. */
WatcherThreadTable WatcherThreadTable::TheTable;



WatcherThreadTable::WatcherThreadTable() :
    Lockable(),
    dm_thread_to_entries()
{
}

bool WatcherThreadTable::getThreadAlreadyPresent(const ProcessThreadId& thread) const
{
    Guard guard_myself(this);
    std::vector<ProcessThreadId> threads;
    
    for(std::map<ProcessThreadId, FileInfoEntry>::const_iterator
		i = dm_thread_to_entries.begin();
	  i != dm_thread_to_entries.end();
        ++i) {

#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
        std::cout << "WatcherThreadTable::getThreadAlreadyPresent("
   	          << "{ " << thread.first << ", " << thread.second << " }, " << ")" 
   	          << " i->first.first (pid)= " << i->first.first  
   	          << " i->first.second (tid)= " << i->first.second  << std::endl;
    }
#endif

        if (i->first == thread) {
#ifndef NDEBUG
           if(WatcherThreadTable::isDebugEnabled()) {
              std::cout << "WatcherThreadTable::getThreadAlreadyPresent( return true)"
   	                << std::endl;
           }
#endif
           return true;
        }
    }

#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
      std::cout << "WatcherThreadTable::getThreadAlreadyPresent(return false)"
                << std::endl;
    }
#endif
    return false;
}


WatcherThreadTable::VProcessThreadId WatcherThreadTable::getAllThreads() const
{
    Guard guard_myself(this);
    std::vector<ProcessThreadId> threads;
    
#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
      std::cout << "WatcherThreadTable::getAllThreads entered."
                << std::endl;
    }
#endif

    for(std::map<ProcessThreadId, FileInfoEntry>::const_iterator
		i = dm_thread_to_entries.begin();
		i != dm_thread_to_entries.end();
		++i)
        threads.push_back(i->first);
    
#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
      std::cout << "WatcherThreadTable::getAllThreads exited."
                << std::endl;
    }
#endif

    return threads;
}


void WatcherThreadTable::addThread(const ProcessThreadId& thread,
				   const FileInfoEntry& entry)
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
      std::cout << "WatcherThreadTable::addThread("
	        << "{ " << thread.first << ", " << thread.second << " }, " << entry
	        << ")" << std::endl;
    }
#endif

    std::map<ProcessThreadId, FileInfoEntry>::const_iterator i = dm_thread_to_entries.find(thread);
    Assert(i == dm_thread_to_entries.end());
    dm_thread_to_entries.insert(std::make_pair(thread, entry));
}



void WatcherThreadTable::removeThread(const ProcessThreadId& thread)
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
       std::cout << "WatcherThreadTable::removeThread(" 
	         << "{ " << thread.first << ", " << thread.second << " }"
	         << ")" << std::endl;
    }
#endif

    std::map<ProcessThreadId, FileInfoEntry>::iterator i =
	dm_thread_to_entries.find(thread);
    Assert(i != dm_thread_to_entries.end());
    dm_thread_to_entries.erase(i);
}


WatcherThreadTable::FileInfoEntry WatcherThreadTable::getEntry(const ProcessThreadId& thread) const
{
    Guard guard_myself(this);
    FileInfoEntry entry;

    std::map<ProcessThreadId, FileInfoEntry>::const_iterator i = dm_thread_to_entries.find(thread);
    Assert(i != dm_thread_to_entries.end());
    entry = i->second;

#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
       std::cout << "WatcherThreadTable::getEntry(" 
 	         << "{ " << thread.first << ", " << thread.second << " }"
	         << ") = " << entry << std::endl;
    }
#endif

    return entry;
}



void WatcherThreadTable::setEntry(const ProcessThreadId& thread, 
				  const FileInfoEntry& entry)
{
    Guard guard_myself(this);

#ifndef NDEBUG
    if(WatcherThreadTable::isDebugEnabled()) {
       std::cout << "WatcherThreadTable::setEntry(" 
  	         << "{ " << thread.first << ", " << thread.second << " }, " << entry
	         << ")" << std::endl;
    }
#endif

    std::map<ProcessThreadId, FileInfoEntry>::iterator i = dm_thread_to_entries.find(thread);
    Assert(i != dm_thread_to_entries.end());
    i->second = entry;
}

#ifndef NDEBUG
/**
 * Get backend debugging flag.
 *
 * Returns a flag indicating if debugging for the backend is enabled.
 *
 * @return    Boolean "true" if debugging for the backend is enabled,
 *            "false" otherwise.
 */
bool WatcherThreadTable::isDebugEnabled() const
{
    bool is_backend_debug_enabled = (getenv("OPENSS_DEBUG_MRNET_WATCHER") != NULL) ;
    return is_backend_debug_enabled;

}
#endif


				   
