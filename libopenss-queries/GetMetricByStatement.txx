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
 * Definition of the GetMetricByStatement template functions.
 *
 */

#ifndef _OpenSpeedShop_Queries_GetMetricByStatement_
#define _OpenSpeedShop_Queries_GetMetricByStatement_

#include "Queries.hxx"
#include "ToolAPI.hxx"



//
// To avoid requiring fully qualified names be used everywhere, something like:
//
//     using namespace OpenSpeedShop;
//     using namespace OpenSpeedShop::Framework;
//
// would normally be placed near the top of this source file. Here, however,
// that particular formulation doesn't work well. This file contains template
// definitions that are usually included directly into other source files. By
// using the above formulation, those source files would be required to have
// the same "using" clauses. And doing so would pretty much negate the whole
// reason for using namespaces in the first place. The solution is to place
// the definitions directly inside the OpenSpeedShop namespace and only
// qualify names with Framework:: where necessary.
//
namespace OpenSpeedShop {



/**
 * Get metric values of statements in a source file.
 *
 * Evalutes the specified collector's metric, over the specified time interval,
 * for all the statements in a source file (in a thread). Results are provided
 * in a map of statement line numbers to metric values. An initially-empty map
 * is allocated if one is not provided. Statement line numberss with non-zero
 * metric values will then be added into the (new or existing) map.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param interval     Time interval over which to get the metric values. 
 * @param thread       Thread for which to get metric values.
 * @param file         Source file for which to get metric values.
 * @retval result      Smart pointer to a map of the statement line numbers to
 *                     their metric values. A new map is allocated if a null
 *                     smart pointer is passed.
 */
template <typename TM>
void Queries::GetMetricByStatementOfFileInThread(
    const Framework::Collector& collector,
    const std::string& metric,
    const Framework::TimeInterval& interval,
    const Framework::Thread& thread,
    const Framework::Path& file,
    Framework::SmartPtr<std::map<int, TM > >& result)
{
    // Allocate (if necessary) a new map of statement line numbers to values
    if(result.isNull())
	result = Framework::SmartPtr<std::map<int, TM > >(
	    new std::map<int, TM >()
	    );
    Assert(!result.isNull());

    // Get the set of statments for the specified file in this thread
    std::set<Framework::Statement> objects =
	thread.getStatementsBySourceFile(file);
    
    // Get the metric values for these statements
    Framework::SmartPtr<std::map<Framework::Statement, TM > > statements;
    GetMetricInThread(collector, metric, interval, thread, objects, statements);
    
    // Strip out everything but the line numbers for the statements
    for(typename std::map<Framework::Statement, TM >::const_iterator
	    i = statements->begin(); i != statements->end(); ++i)
	result->insert(std::make_pair(i->first.getLine(), i->second));    
}



}  // namespace OpenSpeedShop



#endif
