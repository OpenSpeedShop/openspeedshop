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
 * Definition of the GetMetricByStatement functions.
 *
 */

#include "ToolAPI.hxx"
#include "Queries.hxx"

using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;



/**
 * Get metric values by statement in a file.
 *
 * Gets the current list of statements in the specified file and evalutes
 * the specified metric, in the specified collector, over the entire time
 * interval. The results are returned in a map of statements to metric values.
 * Only those statements with non-zero metric values will be present in the
 * returned map.
 *
 * @pre    The specified collector and thread must be in the same experiment
 *         database. An assertion failure occurs if the collector and thread
 *         are in different databases.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param file         File for which to get metric values.
 * @param Thread       Thread for which to query.
 * @retval result      Smart pointer to a map of the file's statements to
 *                     their metric values.
 */
void Queries::GetMetricByStatementInFileForThread(
    const Collector& collector,
    const std::string& metric,
    const std::string& path,
    const Thread& thread,
    SmartPtr<std::map<int, double> >& result)
{
    // Check preconditions
    Assert(collector.inSameDatabase(thread));

    // Lock the appropriate database
    collector.lockDatabase();
    
    // Time interval covering earliest to latest possible time
    const TimeInterval Forever = 
	TimeInterval(Time::TheBeginning(), Time::TheEnd());
    
    // Allocate a new map of statements to doubles
    result = SmartPtr<std::map<int, double> >(
	new std::map<int, double>()
	);
    Assert(!result.isNull());

    // Get the list of statements in this file
    std::set<Statement> statements = thread.getStatementsBySourceFile(path);
    
    // Iterate over each statement
    double value = 0.0;
    for(std::set<Statement>::const_iterator
	    i = statements.begin(); i != statements.end(); ++i) 
    {	
	value = 0.0;
	Statement s = *i;
	int statement_line = (int)s.getLine();
	
	// Get the address ranges of this statement
	std::set<AddressRange> ranges = i->getAddressRanges();
	
	// Iterate over each address range
	for(std::set<AddressRange>::const_iterator
		j = ranges.begin(); j != ranges.end(); ++j )
	{
	    // Evalute the metric over this address range
	    double tmp = 0.0;
	    collector.getMetricValue(metric, thread, *j, Forever, tmp);
	    value += tmp;
	}
	
	// Add this statement and its metric value to the map
	if(value != 0.0)
	{
	    result->insert(std::make_pair(statement_line, value));
	}
	
    }
    
    // Unlock the appropriate database
    collector.unlockDatabase();
}



/**
 * Get metric values by statement in a function.
 *
 * Gets the current list of statements in the specified function and evalutes
 * the specified metric, in the specified collector, over the entire time
 * interval. The results are returned in a map of statements to metric values.
 * Only those statements with non-zero metric values will be present in the
 * returned map.
 *
 * @pre    The specified collector and function must be in the same experiment
 *         database. An assertion failure occurs if the collector and function
 *         are in different databases.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param function     Function for which to get metric values.
 * @retval result      Smart pointer to a map of the function's statements to
 *                     their metric values.
 */
void Queries::GetMetricByStatementInFunction(
    const Collector& collector, 
    const std::string& metric,
    const Function& function,
    SmartPtr<std::map<Statement, double> >& result)
{
    // Check preconditions
    Assert(collector.inSameDatabase(function));

    // Lock the appropriate database
    collector.lockDatabase();

    // Time interval covering earliest to latest possible time
    const TimeInterval Forever = 
	TimeInterval(Time::TheBeginning(), Time::TheEnd());
    
    // Allocate a new map of statements to doubles
    result = SmartPtr<std::map<Statement, double> >(
	new std::map<Statement, double>()
	);
    Assert(!result.isNull());

    // Get the thread containing this function
    Thread thread = function.getThread();

    // Get the list of statements in this function
    std::set<Statement> statements = function.getStatements();
    
    // Iterate over each statement
    for(std::set<Statement>::const_iterator
	    i = statements.begin(); i != statements.end(); ++i) {
	
	// Get the address ranges of this statement
	std::set<AddressRange> ranges = i->getAddressRanges();
	
	// Iterate over each address range
	double value = 0.0;
	for(std::set<AddressRange>::const_iterator
		j = ranges.begin(); j != ranges.end(); ++j ) {
	    
	    // Evalute the metric over this address range
	    double tmp = 0.0;
	    collector.getMetricValue(metric, thread, *j, Forever, value);
	    value += tmp;
	    
	}
	
	// Add this function and its metric value to the map
	if(value != 0.0)
	    result->insert(std::make_pair(*i, value));
	
    }

    // Unlock the appropriate database
    collector.unlockDatabase();
}
