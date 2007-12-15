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
 * Definition of the InstrumentationTable class.
 *
 */

#include "Guard.hxx"
#include "ThreadNameGroup.hxx"
#include "InstrumentationTable.hxx"
#include "Utility.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton thread table. */
InstrumentationTable InstrumentationTable::TheTable;



/**
 * Default constructor.
 *
 * Constructs an empty instrumentation table.
 */
InstrumentationTable::InstrumentationTable() :
    Lockable()
{
    // TODO: implement!
}



/**
 * Execute a library function now.
 *
 * ...
 *
 * @param thread              Thread which the function should be executed.
 * @param collector           Collector requesting the execution.
 * @param disable_save_fpr    Boolean "true" if the floating-point registers
 *                            should NOT be saved before executing the library
 *                            function, or "false" if they should be saved.
 * @param callee              Name of the library function to be executed.
 * @param argument            Blob argument to the function.
 */
void InstrumentationTable::addExecuteNow(const ThreadName& thread,
					 const Collector& collector,
					 const bool& disable_save_fpr,
					 const std::string& callee,
					 const Blob& argument)
{
    // TODO: implement!
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * ...
 *
 * @param thread       Thread in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function at whose entry/exit the library
 *                     function should be executed.
 * @param at_entry     Boolean "true" if instrumenting function's entry point,
 *                     or "false" if function's exit point.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void InstrumentationTable::addExecuteAtEntryOrExit(const ThreadName& thread,
						   const Collector& collector,
						   const std::string& where,
						   const bool& at_entry,
						   const std::string& callee,
						   const Blob& argument)
{
    // TODO: implement!
}



/**
 * Execute a library function in place of another function.
 *
 * ...
 *
 * @param thread       Thread in which the function should be executed.
 * @param collector    Collector requesting the execution.
 * @param where        Name of the function to be replaced with the
 *                     library function.
 * @param callee       Name of the library function to be executed.
 */
void InstrumentationTable::addExecuteInPlaceOf(const ThreadName& thread,
					       const Collector& collector,
					       const std::string& where,
					       const std::string& callee)
{
    // TODO: implement!
}



/**
 * Stop at a function's entry or exit.
 *
 * ...
 *
 * @param thread      Thread which should be stopped.
 * @param where       Name of the function at whose entry/exit the stop
 *                    should occur.
 * @param at_entry    Boolean "true" if instrumenting function's entry
 *                    point, or "false" if function's exit point.
 */
void InstrumentationTable::addStopAtEntryOrExit(const ThreadName& thread,
						const std::string& where,
						const bool& at_entry)
{
    // TODO: implement!
}



/**
 * Remove instrumentation from a thread.
 *
 * ...
 *
 * @param thread    Thread from which instrumentation should be removed.
 */
void InstrumentationTable::removeInstrumentation(const ThreadName& thread)
{
    // TODO: implement!
}



/**
 * Remove instrumentation from a thread for a collector.
 *
 * ...
 *
 * @param thread       Thread from which instrumentation should be removed.
 * @param collector    Collector which is removing instrumentation.
 */
void InstrumentationTable::removeInstrumentation(const ThreadName& thread,
						 const Collector& collector)
{
    // TODO: implement!
}



/**
 * Copy instrumentation from one thread to another.
 *
 * ...
 *
 * @param source         Thread from which instrumentation should be copied.
 * @param destination    Thread to which instrumentation should be copied.
 */
void InstrumentationTable::copyInstrumentation(const ThreadName& source,
					       const ThreadName& destination)
{
    // TODO: implement!
}
