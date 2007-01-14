////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the ThreadName class.
 *
 */

#include "EntrySpy.hxx"
#include "LinkedObject.hxx"
#include "Path.hxx"
#include "Thread.hxx"
#include "ThreadName.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from thread.
 *
 * Constructs a new ThreadName for the specified thread.
 *
 * @param thread    Thread for which to construct a repeatable name.
 */
ThreadName::ThreadName(const Thread& thread) :
    dm_command(false, ""),
    dm_host(""),
    dm_tid(false, 0),
    dm_rank(false, 0),
    dm_executable(false, "")
{
    SmartPtr<Database> database = EntrySpy(thread).getDatabase();

    // Get the thread's repeatable attributes
    BEGIN_TRANSACTION(database);
    EntrySpy(thread).validate();
    database->prepareStatement(
	"SELECT command, host, openmp_tid, mpi_rank FROM Threads WHERE id = ?;"
	);
    database->bindArgument(1, EntrySpy(thread).getEntry());
    while(database->executeStatement()) {
	if(!database->getResultIsNull(1))
	    dm_command = std::make_pair(true, database->getResultAsString(1));
	dm_host = database->getResultAsString(2);
	if(!database->getResultIsNull(3))
	    dm_tid = std::make_pair(true, database->getResultAsInteger(3));
	if(!database->getResultIsNull(4))
	    dm_rank = std::make_pair(true, database->getResultAsInteger(4));
    }
    std::pair<bool, LinkedObject> executable =
	thread.getExecutable(Time::TheBeginning());
    if(executable.first)
	dm_executable = std::make_pair(true, executable.second.getPath());
    END_TRANSACTION(database);
}



/**
 * Equality operator.
 *
 * Operator "==" defined for two ThreadName objects. Allows data collectors
 * applied to the new threads to mimic those applied to the original threads.
 *
 * @param other    Thread name to compare against.
 * @return         Boolean "true" if we are equal to the other entry, "false"
 *                 otherwise.
 */
bool ThreadName::operator==(const ThreadName& other) const
{
    // Equal if the host name and command are identical
    if(dm_command.first && other.dm_command.first &&
       (dm_host == other.dm_host) &&
       (dm_command.second == other.dm_command.second))
	return true;

    // Equal if the executable and MPI rank are identical
    if(dm_executable.first && other.dm_executable.first &&
       dm_rank.first && other.dm_rank.first &&
       (dm_executable.second == other.dm_executable.second) &&
       (dm_rank.second == other.dm_rank.second))
	return true;

    // Equal if the host name and executable are identical
    if(dm_executable.first && other.dm_executable.first &&
       (dm_host == other.dm_host) &&
       (dm_executable.second == other.dm_executable.second))
	return true;

    // Otherwise the two names are not equal
    return false;
}
