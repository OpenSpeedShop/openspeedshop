////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2008 Krell Institute. All Rights Reserved.
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
 * Declaration of the WatcherThreadTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_WatcherThreadTable_
#define _OpenSpeedShop_Framework_WatcherThreadTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <string>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif



namespace OpenSpeedShop { namespace Framework {

    /**
     * Thread table.
     *
     * ...
     *
     * @ingroup Implementation
     */
    class WatcherThreadTable :
	private Lockable
    {

    public:

	typedef std::pair<pid_t, pthread_t> ProcessThreadId;
        typedef std::vector<ProcessThreadId> VProcessThreadId;

	struct FileInfoEntry {
	    std::string fileName;
	    FILE* filePtr;
	    unsigned long readPosition;
	    unsigned long prevSize;

	    friend std::ostream& operator<<(std::ostream& stream,
				     const struct FileInfoEntry& entry)
	    {
		stream << "{ \"" << entry.fileName << "\", " << entry.filePtr
		       << ", " << entry.readPosition << ", " << entry.prevSize
		       << " }" << std::endl;
		return stream;
	    }

	};

	static WatcherThreadTable TheTable;
	
	WatcherThreadTable();

	void addThread(const ProcessThreadId&, const FileInfoEntry&);
	void removeThread(const ProcessThreadId&);
	FileInfoEntry getEntry(const ProcessThreadId&) const;
        VProcessThreadId getAllThreads() const;
	bool getThreadAlreadyPresent(const ProcessThreadId&) const;


	bool getThreadAlreadyPresent(const pid_t& pid, const pthread_t& tid) const
        {
           return getThreadAlreadyPresent(std::make_pair(pid, tid));
        }

	void setEntry(const ProcessThreadId&, const FileInfoEntry&);

	void addThread(const pid_t& pid, const pthread_t& tid,
		       const FileInfoEntry& entry)
	{
	    addThread(std::make_pair(pid, tid), entry);
	}

	void removeThread(const pid_t& pid, const pthread_t& tid)
	{
	    removeThread(std::make_pair(pid, tid));
	}

	FileInfoEntry getEntry(const pid_t& pid, const pthread_t& tid) const
	{
	    return getEntry(std::make_pair(pid, tid));
	}

	void setEntry(const pid_t& pid, const pthread_t& tid,
		      const FileInfoEntry& entry)
	{
	    setEntry(std::make_pair(pid, tid), entry);
	}

#ifndef NDEBUG
        bool isDebugEnabled() const;
#endif
    private:

	// Map process/thread identifier to file information
	std::map<ProcessThreadId, FileInfoEntry> dm_thread_to_entries;

	
    };

} }

#endif
