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
 * Definition of the Queries namespace and related code.
 *
 */

#include "Queries.hxx"
#include "ToolAPI.hxx"

using namespace OpenSpeedShop;



/**
 * Get a thread's linked objects.
 * 
 * Returns the linked objects contained within the specified thread. An empty
 * set is returned if no linked objects are found.
 *
 * @param thread     Thread for which to get linked objects.
 * @param objects    Linked objects contained within this thread.
 */
void Queries::GetSourceObjects(const Framework::Thread& thread,
			       std::set<Framework::LinkedObject>& objects)
{
    objects = thread.getLinkedObjects();
}



/**
 * Get a thread's functions.
 * 
 * Returns the functions contained within the specified thread. An empty set is
 * returned if no functions are found.
 *
 * @param thread     Thread for which to get functions.
 * @param objects    Functions contained within this thread.
 */
void Queries::GetSourceObjects(const Framework::Thread& thread,
			       std::set<Framework::Function>& objects)
{
    objects = thread.getFunctions();
}



/**
 * Get a thread's statements.
 * 
 * Returns the statements contained within the specified thread. An empty set is
 * returned if no statements are found.
 *
 * @param thread     Thread for which to get statements.
 * @param objects    Statements contained within this thread.
 */
void Queries::GetSourceObjects(const Framework::Thread& thread,
			       std::set<Framework::Statement>& objects)
{
    objects = thread.getStatements();
}



/**
 * Strict weak ordering predicate evaluator for linked objects.
 *
 * Implements a strict weak ordering predicate for linked objects that works
 * properly even when the two linked objects are in different experiment
 * databases.
 *
 * @param lhs    Linked object on left hand side of comparison.
 * @param rhs    Linked object on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareLinkedObjects::operator()(
    const Framework::LinkedObject& lhs,
    const Framework::LinkedObject& rhs) const
{
    return lhs.getPath() < rhs.getPath();
}



/**
 * Strict weak ordering predicate evaluator for functions.
 *
 * Implements a strict weak ordering predicate for functions that works
 * properly even when the two functions are in different experiment databases.
 *
 * @param lhs    Function on left hand side of comparison.
 * @param rhs    Function on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareFunctions::operator()(
    const Framework::Function& lhs,
    const Framework::Function& rhs) const
{
    if(lhs.getName() < rhs.getName())
	return true;
    else if(lhs.getName() > rhs.getName())
	return false;
    return lhs.getLinkedObject().getPath() < rhs.getLinkedObject().getPath();
}



/**
 * Strict weak ordering predicate evaluator for statements.
 *
 * Implements a strict weak ordering predicate for statements that works
 * properly even when the two statements are in different experiment databases.
 *
 * @param lhs    Statement on left hand side of comparison.
 * @param rhs    Statement on right hand side of comparison.
 * @return       Boolean "true" if (lhs < rhs), "false" otherwise.
 */
bool Queries::CompareStatements::operator()(
    const Framework::Statement& lhs,
    const Framework::Statement& rhs) const
{
    if(lhs.getPath() < rhs.getPath())
	return true;
    else if(lhs.getPath() > rhs.getPath())
	return false;
    else if(lhs.getLine() < rhs.getLine())
	return true;
    else if(lhs.getLine() > rhs.getLine())
	return false;
    else if(lhs.getColumn() < rhs.getColumn())
	return true;
    else if(lhs.getColumn() > rhs.getColumn())
	return false;
    return lhs.getLinkedObject().getPath() < rhs.getLinkedObject().getPath();
}



/**
 * Addition-assignment operator.
 *
 * Operator "+=" defined for a map of stack traces to a list of doubles. Combine
 * the two maps such that the result contains the superset of stack traces from
 * both of the original two maps. Stack traces contained in the intersection of
 * the two maps will have a value that is the concatentation of the lists of
 * doubles from each map.
 *
 * @param lhs    Resulting, combined, map.
 * @param rhs    Map to be added to this map.
 */
void operator+=(
    std::map<Framework::StackTrace, std::vector<double> >& lhs,
    const std::map<Framework::StackTrace, std::vector<double> >& rhs
    )
{
    // Iterate over each key/value pair in the RHS
    for(std::map<Framework::StackTrace, std::vector<double> >::const_iterator
	    i = rhs.begin(); i != rhs.end(); ++i) {
	
	// Simply add this pair if the key isn't already found in the LHS
	if(lhs.find(i->first) == lhs.end())
	    lhs.insert(*i);
	
	// Otherwise insert the RHS's value onto the end of the LHS' value
	else
	    lhs[i->first].insert(lhs[i->first].end(),
				 i->second.begin(), i->second.end());
	
    }
}
